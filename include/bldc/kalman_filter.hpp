#pragma once

#include <array>
#include <cmath>
#include <cstdint>

namespace bldc::estimation {

// 2상태/1측정 고정 차원으로 행렬 저장 공간과 계산량을 제한한다.
using StateVector = std::array<float, 2U>;
using StateMatrix = std::array<std::array<float, 2U>, 2U>;
using InputMatrix = std::array<std::array<float, 1U>, 2U>;
using MeasurementVector = std::array<float, 2U>;
enum class TimestampPhase : std::uint8_t {
  none = 0U,
  predicted,
  updated,
};


struct LinearKalmanModel {
  StateMatrix A{};
  InputMatrix B{};
  MeasurementVector C{};
  StateMatrix Q{};
  float R{0.0F};
  float sample_period_s{1.0F};
};

struct KalmanState {
  StateVector x{};
  StateMatrix P{};
  std::uint64_t timestamp{0U};
  bool has_timestamp{false};
  TimestampPhase timestamp_phase{TimestampPhase::none};
};

enum class KalmanError : std::uint8_t {
  none = 0U,
  non_finite_input,
  non_symmetric_covariance,
  invalid_covariance,
  invalid_measurement_noise,
  invalid_dt,
  sample_period_mismatch,
  non_monotonic_timestamp,
  timestamp_required,
  singular_innovation,
  callback_failure,
  non_finite_callback_result,
  non_finite_result,
};

struct KalmanResult {
  KalmanError error{KalmanError::non_finite_input};
  KalmanState state{};
  float innovation{0.0F};
  float innovation_variance{0.0F};

  [[nodiscard]] bool ok() const noexcept { return error == KalmanError::none; }
};

namespace detail {
[[nodiscard]] inline float wrap_angle(float angle) noexcept {
  if (!std::isfinite(angle)) {
    return angle;
  }
  constexpr float kPi = 3.14159265358979323846F;
  constexpr float kTwoPi = 2.0F * kPi;
  float wrapped = std::fmod(angle + kPi, kTwoPi);
  if (wrapped < 0.0F) {
    wrapped += kTwoPi;
  }
  return wrapped - kPi;
}


inline bool finite(const StateVector& vector) noexcept {
  return std::isfinite(vector[0U]) && std::isfinite(vector[1U]);
}

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
  return std::isfinite(matrix[0U][0U]) && std::isfinite(matrix[1U][0U]);
}

inline bool symmetric(const StateMatrix& matrix) noexcept {
  constexpr float kSymmetryTolerance = 1.0e-6F;
  return std::fabs(matrix[0U][1U] - matrix[1U][0U]) <= kSymmetryTolerance;
}

inline bool positive_semidefinite(const StateMatrix& matrix) noexcept {
  const float determinant = matrix[0U][0U] * matrix[1U][1U] -
                            matrix[0U][1U] * matrix[1U][0U];
  return std::isfinite(determinant) && matrix[0U][0U] >= 0.0F &&
         matrix[1U][1U] >= 0.0F && determinant >= 0.0F;
}

inline bool valid_covariance(const StateMatrix& matrix) noexcept {
  return finite(matrix) && symmetric(matrix) && positive_semidefinite(matrix);
}


inline void symmetrize(StateMatrix& matrix) noexcept {
  const float off_diagonal = 0.5F * (matrix[0U][1U] + matrix[1U][0U]);
  matrix[0U][1U] = off_diagonal;
  matrix[1U][0U] = off_diagonal;
}

inline bool valid_timestamp(const KalmanState& state,
                            std::uint64_t timestamp) noexcept {
  return !state.has_timestamp || timestamp > state.timestamp;
}

inline KalmanError validate_linear_inputs(const LinearKalmanModel& model,
                                          const KalmanState& state) noexcept {
  if (!finite(model.A) || !finite(model.B) || !finite(model.C) ||
      !finite(model.Q) || !std::isfinite(model.R) || !finite(state.x) ||
      !finite(state.P)) {
    return KalmanError::non_finite_input;
  }
  if (!symmetric(model.Q) || !symmetric(state.P)) {
    return KalmanError::non_symmetric_covariance;
  }
  if (!positive_semidefinite(model.Q) ||
      !positive_semidefinite(state.P)) {
    return KalmanError::invalid_covariance;
  }
  if (!(model.R > 0.0F)) {
    return KalmanError::invalid_measurement_noise;
  }
  if (!std::isfinite(model.sample_period_s) || !(model.sample_period_s > 0.0F)) {
    return KalmanError::invalid_dt;
  }
  return KalmanError::none;
}

inline KalmanError validate_ekf_inputs(const StateMatrix& q, float r,
                                      const KalmanState& state) noexcept {
  if (!finite(q) || !std::isfinite(r) || !finite(state.x) || !finite(state.P)) {
    return KalmanError::non_finite_input;
  }
  if (!symmetric(q) || !symmetric(state.P)) {
    return KalmanError::non_symmetric_covariance;
  }
  if (!positive_semidefinite(q) || !positive_semidefinite(state.P)) {
    return KalmanError::invalid_covariance;
  }
  if (!(r > 0.0F)) {
    return KalmanError::invalid_measurement_noise;
  }
  return KalmanError::none;
}

inline KalmanResult failed(KalmanError error, const KalmanState& state) noexcept {
  KalmanResult result{};
  result.error = error;
  result.state = state;
  return result;
}

inline KalmanResult finish(KalmanError error, const KalmanState& state,
                           float innovation = 0.0F,
                           float innovation_variance = 0.0F) noexcept {
  KalmanResult result{};
  result.error = error;
  result.state = state;
  result.innovation = innovation;
  result.innovation_variance = innovation_variance;
  return result;
}

inline KalmanError validate_dt(float dt, float sample_period_s,
                               bool enforce_sample_period) noexcept {
  if (!std::isfinite(dt) || !(dt > 0.0F)) {
    return KalmanError::invalid_dt;
  }
  if (enforce_sample_period &&
      (!std::isfinite(sample_period_s) || !(sample_period_s > 0.0F))) {
    return KalmanError::invalid_dt;
  }
  if (enforce_sample_period) {
    constexpr float kPeriodTolerance = 1.0e-6F;
    const float tolerance =
        kPeriodTolerance * (sample_period_s > 1.0F ? sample_period_s : 1.0F);
    if (std::fabs(dt - sample_period_s) > tolerance) {
      return KalmanError::sample_period_mismatch;
    }
  }
  return KalmanError::none;
}

inline KalmanError validate_predict_timestamp(const KalmanState& state,
                                              bool has_timestamp,
                                              std::uint64_t timestamp) noexcept {
  if (!has_timestamp || !state.has_timestamp) {
    return KalmanError::none;
  }
  if (state.timestamp_phase != TimestampPhase::updated ||
      timestamp <= state.timestamp) {
    return KalmanError::non_monotonic_timestamp;
  }
  return KalmanError::none;
}

inline KalmanError validate_update_timestamp(const KalmanState& state,
                                             bool has_timestamp,
                                             std::uint64_t timestamp) noexcept {
  if (!has_timestamp) {
    return KalmanError::none;
  }
  if (!state.has_timestamp ||
      state.timestamp_phase != TimestampPhase::predicted ||
      timestamp != state.timestamp) {
    return KalmanError::non_monotonic_timestamp;
  }
  return KalmanError::none;
}

inline KalmanError validate_timestamp_mode(const KalmanState& state,
                                           bool has_timestamp) noexcept {
  if (state.has_timestamp && !has_timestamp) {
    return KalmanError::timestamp_required;
  }
  return KalmanError::none;
}

inline KalmanResult predict_impl(const LinearKalmanModel& model,
                                 KalmanState& state, float input, float dt,
                                 bool has_timestamp,
                                 std::uint64_t timestamp) noexcept {
  const KalmanError timestamp_mode_error =
      validate_timestamp_mode(state, has_timestamp);
  if (timestamp_mode_error != KalmanError::none) {
    return failed(timestamp_mode_error, state);
  }
  const KalmanError input_error = validate_linear_inputs(model, state);
  if (input_error != KalmanError::none) {
    return failed(input_error, state);
  }
  if (!std::isfinite(input)) {
    return failed(KalmanError::non_finite_input, state);
  }
  const KalmanError dt_error =
      validate_dt(dt, model.sample_period_s, true);
  if (dt_error != KalmanError::none) {
    return failed(dt_error, state);
  }
  const KalmanError time_error =
      validate_predict_timestamp(state, has_timestamp, timestamp);
  if (time_error != KalmanError::none) {
    return failed(time_error, state);
  }

  KalmanState candidate = state;
  candidate.x[0U] = model.A[0U][0U] * state.x[0U] +
                    model.A[0U][1U] * state.x[1U] + model.B[0U][0U] * input;
  candidate.x[1U] = model.A[1U][0U] * state.x[0U] +
                    model.A[1U][1U] * state.x[1U] + model.B[1U][0U] * input;

  const float ap00 = model.A[0U][0U] * state.P[0U][0U] +
                     model.A[0U][1U] * state.P[1U][0U];
  const float ap01 = model.A[0U][0U] * state.P[0U][1U] +
                     model.A[0U][1U] * state.P[1U][1U];
  const float ap10 = model.A[1U][0U] * state.P[0U][0U] +
                     model.A[1U][1U] * state.P[1U][0U];
  const float ap11 = model.A[1U][0U] * state.P[0U][1U] +
                     model.A[1U][1U] * state.P[1U][1U];
  candidate.P[0U][0U] = ap00 * model.A[0U][0U] +
                        ap01 * model.A[0U][1U] + model.Q[0U][0U];
  candidate.P[0U][1U] = ap00 * model.A[1U][0U] +
                        ap01 * model.A[1U][1U] + model.Q[0U][1U];
  candidate.P[1U][0U] = ap10 * model.A[0U][0U] +
                        ap11 * model.A[0U][1U] + model.Q[1U][0U];
  candidate.P[1U][1U] = ap10 * model.A[1U][0U] +
                        ap11 * model.A[1U][1U] + model.Q[1U][1U];
  detail::symmetrize(candidate.P);
  if (!finite(candidate.x) || !valid_covariance(candidate.P)) {
    return failed(KalmanError::non_finite_result, state);
  }
  if (has_timestamp) {
    candidate.timestamp = timestamp;
    candidate.has_timestamp = true;
    candidate.timestamp_phase = TimestampPhase::predicted;
  }
  state = candidate;
  return finish(KalmanError::none, state);
}

inline KalmanResult update_impl(const MeasurementVector& measurement_matrix,
                                float measurement_noise,
                                float predicted_measurement,
                                KalmanState& state, float measurement,
                                bool has_timestamp,
                                std::uint64_t timestamp,
                                bool wrap_innovation) noexcept {
  const KalmanError timestamp_mode_error =
      validate_timestamp_mode(state, has_timestamp);
  if (timestamp_mode_error != KalmanError::none) {
    return failed(timestamp_mode_error, state);
  }
  if (!std::isfinite(measurement_noise) || !(measurement_noise > 0.0F) ||
      !finite(measurement_matrix) || !std::isfinite(measurement)) {
    return failed(KalmanError::non_finite_input, state);
  }
  if (!finite(state.x) || !finite(state.P)) {
    return failed(KalmanError::non_finite_input, state);
  }
  if (!symmetric(state.P)) {
    return failed(KalmanError::non_symmetric_covariance, state);
  }
  if (!positive_semidefinite(state.P)) {
    return failed(KalmanError::invalid_covariance, state);
  }
  const KalmanError timestamp_error =
      validate_update_timestamp(state, has_timestamp, timestamp);
  if (timestamp_error != KalmanError::none) {
    return failed(timestamp_error, state);
  }

  float innovation = measurement - predicted_measurement;
  if (wrap_innovation) {
    innovation = wrap_angle(innovation);
  }
  const float pc0 = state.P[0U][0U] * measurement_matrix[0U] +
                    state.P[0U][1U] * measurement_matrix[1U];
  const float pc1 = state.P[1U][0U] * measurement_matrix[0U] +
                    state.P[1U][1U] * measurement_matrix[1U];
  const float innovation_variance = measurement_matrix[0U] * pc0 +
                                    measurement_matrix[1U] * pc1 +
                                    measurement_noise;
  if (!std::isfinite(innovation_variance)) {
    return failed(KalmanError::non_finite_result, state);
  }
  if (!(innovation_variance > 0.0F)) {
    return failed(KalmanError::singular_innovation, state);
  }

  const float gain0 = pc0 / innovation_variance;
  const float gain1 = pc1 / innovation_variance;
  KalmanState candidate = state;
  candidate.x[0U] = state.x[0U] + gain0 * innovation;
  candidate.x[1U] = state.x[1U] + gain1 * innovation;

  // Joseph form은 측정 갱신 뒤에도 공분산 대칭·양의 준정부호 조건을 보존한다.
  const float i00 = 1.0F - gain0 * measurement_matrix[0U];
  const float i01 = -gain0 * measurement_matrix[1U];
  const float i10 = -gain1 * measurement_matrix[0U];
  const float i11 = 1.0F - gain1 * measurement_matrix[1U];
  const float ip00 = i00 * state.P[0U][0U] + i01 * state.P[1U][0U];
  const float ip01 = i00 * state.P[0U][1U] + i01 * state.P[1U][1U];
  const float ip10 = i10 * state.P[0U][0U] + i11 * state.P[1U][0U];
  const float ip11 = i10 * state.P[0U][1U] + i11 * state.P[1U][1U];
  candidate.P[0U][0U] = ip00 * i00 + ip01 * i01 +
                        gain0 * measurement_noise * gain0;
  candidate.P[0U][1U] = ip00 * i10 + ip01 * i11 +
                        gain0 * measurement_noise * gain1;
  candidate.P[1U][0U] = ip10 * i00 + ip11 * i01 +
                        gain1 * measurement_noise * gain0;
  candidate.P[1U][1U] = ip10 * i10 + ip11 * i11 +
                        gain1 * measurement_noise * gain1;
  detail::symmetrize(candidate.P);
  if (!finite(candidate.x) || !valid_covariance(candidate.P) ||
      !std::isfinite(gain0) || !std::isfinite(gain1)) {
    return failed(KalmanError::non_finite_result, state);
  }
  if (has_timestamp) {
    candidate.timestamp = timestamp;
    candidate.has_timestamp = true;
    candidate.timestamp_phase = TimestampPhase::updated;
  }
  state = candidate;
  return finish(KalmanError::none, state, innovation, innovation_variance);
}

}  // namespace detail

[[nodiscard]] inline KalmanError validate(const LinearKalmanModel& model,
                                          const KalmanState& state) noexcept {
  return detail::validate_linear_inputs(model, state);
}

[[nodiscard]] inline KalmanResult predict(const LinearKalmanModel& model,
                                          KalmanState& state, float input,
                                          float dt) noexcept {
  return detail::predict_impl(model, state, input, dt, false, 0U);
}

[[nodiscard]] inline KalmanResult predict_at(const LinearKalmanModel& model,
                                             KalmanState& state, float input,
                                             float dt,
                                             std::uint64_t timestamp) noexcept {
  return detail::predict_impl(model, state, input, dt, true, timestamp);
}

[[nodiscard]] inline KalmanResult update(const LinearKalmanModel& model,
                                         KalmanState& state,
                                         float measurement) noexcept {
  const KalmanError timestamp_mode_error =
      detail::validate_timestamp_mode(state, false);
  if (timestamp_mode_error != KalmanError::none) {
    return detail::failed(timestamp_mode_error, state);
  }
  const KalmanError input_error = detail::validate_linear_inputs(model, state);
  if (input_error != KalmanError::none) {
    return detail::failed(input_error, state);
  }
  const float predicted_measurement =
      model.C[0U] * state.x[0U] + model.C[1U] * state.x[1U];
  return detail::update_impl(model.C, model.R, predicted_measurement, state,
                             measurement, false, 0U, false);
}

[[nodiscard]] inline KalmanResult update_at(const LinearKalmanModel& model,
                                            KalmanState& state, float measurement,
                                            std::uint64_t timestamp) noexcept {
  const KalmanError input_error = detail::validate_linear_inputs(model, state);
  if (input_error != KalmanError::none) {
    return detail::failed(input_error, state);
  }
  const float predicted_measurement =
      model.C[0U] * state.x[0U] + model.C[1U] * state.x[1U];
  return detail::update_impl(model.C, model.R, predicted_measurement, state,
                             measurement, true, timestamp, false);
}

using TransitionFunction = bool (*)(const StateVector&, float, float,
                                    StateVector*) noexcept;
using MeasurementFunction = bool (*)(const StateVector&, float*) noexcept;
using TransitionJacobianFunction = bool (*)(const StateVector&, float, float,
                                            StateMatrix*) noexcept;
using MeasurementJacobianFunction = bool (*)(const StateVector&,
                                             MeasurementVector*) noexcept;

struct EkfModel {
  TransitionFunction transition{nullptr};
  MeasurementFunction measurement{nullptr};
  TransitionJacobianFunction transition_jacobian{nullptr};
  MeasurementJacobianFunction measurement_jacobian{nullptr};
  StateMatrix Q{};
  bool wrap_innovation{false};
  float R{0.0F};
  float sample_period_s{1.0F};
};

namespace detail {

inline KalmanResult ekf_predict_impl(const EkfModel& model, KalmanState& state,
                                     float input, float dt, bool has_timestamp,
                                     std::uint64_t timestamp) noexcept {
  const KalmanError timestamp_mode_error =
      validate_timestamp_mode(state, has_timestamp);
  if (timestamp_mode_error != KalmanError::none) {
    return failed(timestamp_mode_error, state);
  }
  const KalmanError input_error = validate_ekf_inputs(model.Q, model.R, state);
  if (input_error != KalmanError::none) {
    return failed(input_error, state);
  }
  if (model.transition == nullptr || model.transition_jacobian == nullptr) {
    return failed(KalmanError::callback_failure, state);
  }
  const KalmanError dt_error =
      validate_dt(dt, model.sample_period_s, true);
  if (dt_error != KalmanError::none) {
    return failed(dt_error, state);
  }
  const KalmanError time_error =
      validate_predict_timestamp(state, has_timestamp, timestamp);
  if (time_error != KalmanError::none || !std::isfinite(input)) {
    return failed(time_error != KalmanError::none ? time_error
                                                   : KalmanError::non_finite_input,
                  state);
  }

  StateVector next_x{};
  StateMatrix jacobian{};
  if (!model.transition(state.x, input, dt, &next_x)) {
    return failed(KalmanError::callback_failure, state);
  }
  if (!finite(next_x)) {
    return failed(KalmanError::non_finite_callback_result, state);
  }
  if (!model.transition_jacobian(state.x, input, dt, &jacobian)) {
    return failed(KalmanError::callback_failure, state);
  }
  if (!finite(jacobian)) {
    return failed(KalmanError::non_finite_callback_result, state);
  }

  KalmanState candidate = state;
  candidate.x = next_x;
  const float ap00 = jacobian[0U][0U] * state.P[0U][0U] +
                     jacobian[0U][1U] * state.P[1U][0U];
  const float ap01 = jacobian[0U][0U] * state.P[0U][1U] +
                     jacobian[0U][1U] * state.P[1U][1U];
  const float ap10 = jacobian[1U][0U] * state.P[0U][0U] +
                     jacobian[1U][1U] * state.P[1U][0U];
  const float ap11 = jacobian[1U][0U] * state.P[0U][1U] +
                     jacobian[1U][1U] * state.P[1U][1U];
  candidate.P[0U][0U] = ap00 * jacobian[0U][0U] +
                        ap01 * jacobian[0U][1U] + model.Q[0U][0U];
  candidate.P[0U][1U] = ap00 * jacobian[1U][0U] +
                        ap01 * jacobian[1U][1U] + model.Q[0U][1U];
  candidate.P[1U][0U] = ap10 * jacobian[0U][0U] +
                        ap11 * jacobian[0U][1U] + model.Q[1U][0U];
  candidate.P[1U][1U] = ap10 * jacobian[1U][0U] +
                        ap11 * jacobian[1U][1U] + model.Q[1U][1U];
  symmetrize(candidate.P);
  if (!finite(candidate.x) || !valid_covariance(candidate.P)) {
    return failed(KalmanError::non_finite_result, state);
  }
  if (has_timestamp) {
    candidate.timestamp = timestamp;
    candidate.has_timestamp = true;
    candidate.timestamp_phase = TimestampPhase::predicted;
  }
  state = candidate;
  return finish(KalmanError::none, state);
}

inline KalmanResult ekf_update_impl(const EkfModel& model, KalmanState& state,
                                    float measurement, bool has_timestamp,
                                    std::uint64_t timestamp) noexcept {
  const KalmanError timestamp_mode_error =
      validate_timestamp_mode(state, has_timestamp);
  if (timestamp_mode_error != KalmanError::none) {
    return failed(timestamp_mode_error, state);
  }
  const KalmanError input_error = validate_ekf_inputs(model.Q, model.R, state);
  if (input_error != KalmanError::none) {
    return failed(input_error, state);
  }
  if (model.measurement == nullptr || model.measurement_jacobian == nullptr) {
    return failed(KalmanError::callback_failure, state);
  }
  if (!std::isfinite(measurement)) {
    return failed(KalmanError::non_finite_input, state);
  }
  const KalmanError timestamp_error =
      validate_update_timestamp(state, has_timestamp, timestamp);
  if (timestamp_error != KalmanError::none) {
    return failed(timestamp_error, state);
  }

  float predicted_measurement = 0.0F;
  MeasurementVector jacobian{};
  if (!model.measurement(state.x, &predicted_measurement)) {
    return failed(KalmanError::callback_failure, state);
  }
  if (!std::isfinite(predicted_measurement)) {
    return failed(KalmanError::non_finite_callback_result, state);
  }
  if (!model.measurement_jacobian(state.x, &jacobian)) {
    return failed(KalmanError::callback_failure, state);
  }
  if (!finite(jacobian)) {
    return failed(KalmanError::non_finite_callback_result, state);
  }
  return update_impl(jacobian, model.R, predicted_measurement, state,
                     measurement, has_timestamp, timestamp,
                     model.wrap_innovation);
}

}  // namespace detail

[[nodiscard]] inline KalmanResult ekf_predict(const EkfModel& model,
                                               KalmanState& state, float input,
                                               float dt) noexcept {
  return detail::ekf_predict_impl(model, state, input, dt, false, 0U);
}

[[nodiscard]] inline KalmanResult ekf_predict_at(const EkfModel& model,
                                                 KalmanState& state, float input,
                                                 float dt,
                                                 std::uint64_t timestamp) noexcept {
  return detail::ekf_predict_impl(model, state, input, dt, true, timestamp);
}

[[nodiscard]] inline KalmanResult ekf_update(const EkfModel& model,
                                              KalmanState& state,
                                              float measurement) noexcept {
  return detail::ekf_update_impl(model, state, measurement, false, 0U);
}

[[nodiscard]] inline KalmanResult ekf_update_at(const EkfModel& model,
                                                KalmanState& state,
                                                float measurement,
                                                std::uint64_t timestamp) noexcept {
  return detail::ekf_update_impl(model, state, measurement, true, timestamp);
}

// 각도 오차를 한 바퀴의 대표 구간 [-pi, pi)로 접어 경계의 부호를 고정한다.
[[nodiscard]] inline float wrap_angle(float angle) noexcept {
  return detail::wrap_angle(angle);
}

[[nodiscard]] inline float angle_innovation(float measured,
                                            float predicted) noexcept {
  return wrap_angle(measured - predicted);
}

}  // namespace bldc::estimation
