#pragma once

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>

namespace bldc::control {

// 고정 차원으로 도메인 계약을 드러내고 재귀 계산을 무할당으로 유지한다.
using StateVector = std::array<float, 2U>;
using StateMatrix = std::array<std::array<float, 2U>, 2U>;
using InputMatrix = std::array<std::array<float, 1U>, 2U>;

constexpr std::size_t kTvLqrMaxHorizon = 32U;

struct TvLqrProblem {
  StateMatrix A{};
  InputMatrix B{};
  StateMatrix Q{};
  float R{0.0F};
  StateMatrix S{};
  std::size_t horizon{0U};
};

enum class TvLqrError : std::uint8_t {
  none = 0U,
  invalid_horizon,
  non_finite_input,
  non_symmetric_cost,
  invalid_cost_matrix,
  singular_control_cost,
  invalid_generated_p,
  non_finite_result,
};

struct TvLqrSolution {
  // K[i]는 u[i] = -K[i]x[i]에 사용하며 P에는 terminal P[N]을 포함한다.
  std::array<StateVector, kTvLqrMaxHorizon> K{};
  std::array<StateMatrix, kTvLqrMaxHorizon + 1U> P{};
  std::size_t horizon{0U};
};

struct TvLqrResult {
  TvLqrError error{TvLqrError::invalid_horizon};
  TvLqrSolution solution{};

  [[nodiscard]] bool ok() const noexcept { return error == TvLqrError::none; }
};

namespace detail {

inline bool finite(const StateMatrix& matrix) noexcept {
  for (const auto& row : matrix) {
    for (const float value : row) {
      if (!std::isfinite(value)) {
        return false;
      }
    }
  }
  return true;
}

inline bool finite(const InputMatrix& matrix) noexcept {
  for (const auto& row : matrix) {
    if (!std::isfinite(row[0U])) {
      return false;
    }
  }
  return true;
}

inline bool symmetric(const StateMatrix& matrix) noexcept {
  constexpr float kSymmetryTolerance = 1.0e-6F;
  return std::fabs(matrix[0U][1U] - matrix[1U][0U]) <= kSymmetryTolerance;
}

inline bool positive_semidefinite(const StateMatrix& matrix) noexcept {
  const float determinant = matrix[0U][0U] * matrix[1U][1U] -
                            matrix[0U][1U] * matrix[1U][0U];
  return finite(matrix) && std::isfinite(determinant) &&
         matrix[0U][0U] >= 0.0F && matrix[1U][1U] >= 0.0F &&
         determinant >= 0.0F;
}

inline bool finite_and_symmetric(const StateMatrix& matrix) noexcept {
  return finite(matrix) && symmetric(matrix);
}

}  // namespace detail

// P[i] = Q + A'P[i+1]A - A'P[i+1]B(R+B'P[i+1]B)^-1B'P[i+1]A를 계산한다.
[[nodiscard]] inline TvLqrResult solve_tv_lqr(const TvLqrProblem& problem) noexcept {
  TvLqrResult result{};
  if (problem.horizon == 0U || problem.horizon > kTvLqrMaxHorizon) {
    result.error = TvLqrError::invalid_horizon;
    return result;
  }

  if (!detail::finite(problem.A) || !detail::finite(problem.B) ||
      !detail::finite(problem.Q) || !detail::finite(problem.S) ||
      !std::isfinite(problem.R)) {
    result.error = TvLqrError::non_finite_input;
    return result;
  }
  if (!detail::symmetric(problem.Q) || !detail::symmetric(problem.S)) {
    result.error = TvLqrError::non_symmetric_cost;
    return result;
  }
  if (!detail::positive_semidefinite(problem.Q) ||
      !detail::positive_semidefinite(problem.S)) {
    result.error = TvLqrError::invalid_cost_matrix;
    return result;
  }
  if (!(problem.R > 0.0F)) {
    result.error = TvLqrError::singular_control_cost;
    return result;
  }

  result.solution.horizon = problem.horizon;
  result.solution.P[problem.horizon] = problem.S;
  for (std::size_t index = problem.horizon; index-- > 0U;) {
    const StateMatrix& p_next = result.solution.P[index + 1U];
    const float b0 = problem.B[0U][0U];
    const float b1 = problem.B[1U][0U];

    const float p_b0 = p_next[0U][0U] * b0 + p_next[0U][1U] * b1;
    const float p_b1 = p_next[1U][0U] * b0 + p_next[1U][1U] * b1;
    const float control_curvature = problem.R + b0 * p_b0 + b1 * p_b1;
    if (!std::isfinite(control_curvature)) {
      result.error = TvLqrError::non_finite_result;
      return result;
    }
    if (!(control_curvature > 0.0F)) {
      result.error = TvLqrError::singular_control_cost;
      return result;
    }

    const float p_a00 = p_next[0U][0U] * problem.A[0U][0U] +
                        p_next[0U][1U] * problem.A[1U][0U];
    const float p_a01 = p_next[0U][0U] * problem.A[0U][1U] +
                        p_next[0U][1U] * problem.A[1U][1U];
    const float p_a10 = p_next[1U][0U] * problem.A[0U][0U] +
                        p_next[1U][1U] * problem.A[1U][0U];
    const float p_a11 = p_next[1U][0U] * problem.A[0U][1U] +
                        p_next[1U][1U] * problem.A[1U][1U];

    const float btp_a0 = b0 * p_a00 + b1 * p_a10;
    const float btp_a1 = b0 * p_a01 + b1 * p_a11;
    result.solution.K[index] = StateVector{
        btp_a0 / control_curvature,
        btp_a1 / control_curvature,
    };

    const float at_p_a00 = problem.A[0U][0U] * p_a00 +
                           problem.A[1U][0U] * p_a10;
    const float at_p_a01 = problem.A[0U][0U] * p_a01 +
                           problem.A[1U][0U] * p_a11;
    const float at_p_a11 = problem.A[0U][1U] * p_a01 +
                           problem.A[1U][1U] * p_a11;
    const float at_p_b0 = problem.A[0U][0U] * p_b0 +
                          problem.A[1U][0U] * p_b1;
    const float at_p_b1 = problem.A[0U][1U] * p_b0 +
                          problem.A[1U][1U] * p_b1;
    const float correction00 = at_p_b0 * btp_a0 / control_curvature;
    const float correction01 = at_p_b0 * btp_a1 / control_curvature;
    const float correction11 = at_p_b1 * btp_a1 / control_curvature;

    StateMatrix& p_current = result.solution.P[index];
    p_current[0U][0U] = problem.Q[0U][0U] + at_p_a00 - correction00;
    p_current[0U][1U] = problem.Q[0U][1U] + at_p_a01 - correction01;
    p_current[1U][0U] = p_current[0U][1U];
    p_current[1U][1U] = problem.Q[1U][1U] + at_p_a11 - correction11;

    if (!detail::finite_and_symmetric(p_current) ||
        !detail::positive_semidefinite(p_current)) {
      result.error = TvLqrError::invalid_generated_p;
      return result;
    }
    if (!std::isfinite(result.solution.K[index][0U]) ||
        !std::isfinite(result.solution.K[index][1U])) {
      result.error = TvLqrError::non_finite_result;
      return result;
    }
  }

  result.error = TvLqrError::none;
  return result;
}

}  // namespace bldc::control
