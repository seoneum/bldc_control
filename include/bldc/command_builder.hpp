// 이 코드 줄의 도메인 동작을 수행한다.
#pragma once

// 필요한 헤더와 타입 정의를 포함한다.
#include <array>
// 필요한 헤더와 타입 정의를 포함한다.
#include <cmath>
// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>
// 필요한 헤더와 타입 정의를 포함한다.
#include <optional>

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/can_frame.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/gim_protocol.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/safety_supervisor.hpp"

// 컴파일 조건과 기능 게이트를 제어한다.
#ifndef BLDC_ENABLE_CAN_TRANSMIT
// 컴파일 조건과 기능 게이트를 제어한다.
#define BLDC_ENABLE_CAN_TRANSMIT 0
// 컴파일 조건과 기능 게이트를 제어한다.
#endif

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace bldc::command {

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
inline constexpr bool kCanTransmitCompiled = BLDC_ENABLE_CAN_TRANSMIT != 0;

// 재사용할 자료 또는 동작 계약을 선언한다.
struct MitLimits {
// 값을 계산하거나 상태를 갱신한다.
  float min_position_rad;
// 값을 계산하거나 상태를 갱신한다.
  float max_position_rad;
// 값을 계산하거나 상태를 갱신한다.
  float max_velocity_rad_per_s;
// 값을 계산하거나 상태를 갱신한다.
  float max_torque_nm;
// 값을 계산하거나 상태를 갱신한다.
  float max_kp;
// 값을 계산하거나 상태를 갱신한다.
  float max_kd;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
struct MitCommand {
// 값을 계산하거나 상태를 갱신한다.
  float position_rad;
// 값을 계산하거나 상태를 갱신한다.
  float velocity_rad_per_s;
// 값을 계산하거나 상태를 갱신한다.
  float kp;
// 값을 계산하거나 상태를 갱신한다.
  float kd;
// 값을 계산하거나 상태를 갱신한다.
  float torque_nm;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};
// 재사용할 자료 또는 동작 계약을 선언한다.
class CommandBuilder;

// HAL 송신 경계는 raw CanFrame이 아니라 이 token만 받는다. CommandBuilder만
// clamp와 compile/runtime/safety gate를 통과한 뒤 token을 만들 수 있다.
// 재사용할 자료 또는 동작 계약을 선언한다.
class AuthorizedFrame {
// 멤버의 접근 범위를 지정한다.
 public:
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const CanFrame& frame() const noexcept { return frame_; }

// 멤버의 접근 범위를 지정한다.
 private:
// 이 코드 줄의 도메인 동작을 수행한다.
  explicit AuthorizedFrame(const CanFrame& frame) noexcept : frame_(frame) {}

// 값을 계산하거나 상태를 갱신한다.
  friend class CommandBuilder;

// 값을 계산하거나 상태를 갱신한다.
  CanFrame frame_;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
class CommandBuilder {
// 멤버의 접근 범위를 지정한다.
 public:
// 함수의 입력과 동작 계약을 선언한다.
  explicit constexpr CommandBuilder(const MitLimits& limits) noexcept
// 이 코드 줄의 도메인 동작을 수행한다.
      : limits_(limits) {}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  std::optional<MitCommand> sanitize(const MitCommand& requested) const noexcept {
// 입력과 상태에 따라 실행 경로를 선택한다.
    if (!limits_are_valid() || !std::isfinite(requested.position_rad) ||
// 이 코드 줄의 도메인 동작을 수행한다.
        !std::isfinite(requested.velocity_rad_per_s) ||
// 이 코드 줄의 도메인 동작을 수행한다.
        !std::isfinite(requested.kp) || !std::isfinite(requested.kd) ||
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
        !std::isfinite(requested.torque_nm)) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // protocol wire range와 별도로 설정한 더 좁은 authority 안에서 먼저
    // clamp해 bench 확인 전 과도한 command를 만들지 않는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    return MitCommand{
// 이 코드 줄의 도메인 동작을 수행한다.
        clamp(requested.position_rad, limits_.min_position_rad,
// 이 코드 줄의 도메인 동작을 수행한다.
              limits_.max_position_rad),
// 이 코드 줄의 도메인 동작을 수행한다.
        clamp(requested.velocity_rad_per_s, -limits_.max_velocity_rad_per_s,
// 이 코드 줄의 도메인 동작을 수행한다.
              limits_.max_velocity_rad_per_s),
// 이 코드 줄의 도메인 동작을 수행한다.
        clamp(requested.kp, 0.0F, limits_.max_kp),
// 이 코드 줄의 도메인 동작을 수행한다.
        clamp(requested.kd, 0.0F, limits_.max_kd),
// 이 코드 줄의 도메인 동작을 수행한다.
        clamp(requested.torque_nm, -limits_.max_torque_nm,
// 이 코드 줄의 도메인 동작을 수행한다.
              limits_.max_torque_nm),
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 이 코드 줄의 도메인 동작을 수행한다.
  std::optional<CanFrame> compile_mit(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      const std::uint8_t node_id, const MitCommand& requested) const noexcept {
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto sanitized = sanitize(requested);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto identifier = pack_standard_id(
// 값을 계산하거나 상태를 갱신한다.
        node_id, static_cast<std::uint8_t>(gim::Command::mit_control));
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!sanitized.has_value() || !identifier.has_value()) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // 안전 limit을 full-scale로 늘려 쓰지 않는다. MIT wire 변환은 번역
    // 매뉴얼의 고정 range를 사용해야 실제 물리 단위가 보존된다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::uint16_t position =
// 값을 계산하거나 상태를 갱신한다.
        encode_range(sanitized->position_rad, -12.5F, 12.5F, 0xFFFFU);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::uint16_t velocity =
// 값을 계산하거나 상태를 갱신한다.
        encode_symmetric(sanitized->velocity_rad_per_s, 65.0F, 0x0FFFU);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::uint16_t kp =
// 값을 계산하거나 상태를 갱신한다.
        encode_range(sanitized->kp, 0.0F, 500.0F, 0x0FFFU);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::uint16_t kd =
// 값을 계산하거나 상태를 갱신한다.
        encode_range(sanitized->kd, 0.0F, 5.0F, 0x0FFFU);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::uint16_t torque =
// 값을 계산하거나 상태를 갱신한다.
        encode_symmetric(sanitized->torque_nm, 50.0F, 0x0FFFU);

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    return CanFrame{
// 이 코드 줄의 도메인 동작을 수행한다.
        *identifier,
// 이 코드 줄의 도메인 동작을 수행한다.
        false,
// 이 코드 줄의 도메인 동작을 수행한다.
        false,
// 이 코드 줄의 도메인 동작을 수행한다.
        8U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
        std::array<std::uint8_t, 8U>{
// 이 코드 줄의 도메인 동작을 수행한다.
            static_cast<std::uint8_t>(position >> 8U),
// 이 코드 줄의 도메인 동작을 수행한다.
            static_cast<std::uint8_t>(position & 0xFFU),
// 이 코드 줄의 도메인 동작을 수행한다.
            static_cast<std::uint8_t>(velocity >> 4U),
// 이 코드 줄의 도메인 동작을 수행한다.
            static_cast<std::uint8_t>(((velocity & 0x000FU) << 4U) |
// 이 코드 줄의 도메인 동작을 수행한다.
                                      ((kp >> 8U) & 0x000FU)),
// 이 코드 줄의 도메인 동작을 수행한다.
            static_cast<std::uint8_t>(kp & 0x00FFU),
// 이 코드 줄의 도메인 동작을 수행한다.
            static_cast<std::uint8_t>(kd >> 4U),
// 이 코드 줄의 도메인 동작을 수행한다.
            static_cast<std::uint8_t>(((kd & 0x000FU) << 4U) |
// 이 코드 줄의 도메인 동작을 수행한다.
                                      ((torque >> 8U) & 0x000FU)),
// 이 코드 줄의 도메인 동작을 수행한다.
            static_cast<std::uint8_t>(torque & 0x00FFU),
// 이 코드 줄의 도메인 동작을 수행한다.
        },
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 이 코드 줄의 도메인 동작을 수행한다.
  std::optional<AuthorizedFrame> build_mit(
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
      const std::uint8_t node_id, const MitCommand& requested,
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
      const SafetySupervisor& safety,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      const bool runtime_transmit_enabled) const noexcept {
    // build-time enable, runtime arm, 현재 heartbeat/fault 상태를 모두
    // 만족해야 HAL이 받을 수 있는 AuthorizedFrame을 만든다.
// 입력과 상태에 따라 실행 경로를 선택한다.
    if (!kCanTransmitCompiled || !runtime_transmit_enabled ||
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
        !safety.permits_command()) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto frame = compile_mit(node_id, requested);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!frame.has_value()) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return AuthorizedFrame{*frame};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 멤버의 접근 범위를 지정한다.
 private:
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  static constexpr float clamp(const float value, const float lower,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
                               const float upper) noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return value < lower ? lower : (value > upper ? upper : value);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  static std::uint16_t encode_range(const float value, const float minimum,
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
                                    const float maximum,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
                                    const std::uint16_t maximum_code) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (maximum <= minimum || value <= minimum) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return 0U;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (value >= maximum) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return maximum_code;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const double normalized =
// 이 코드 줄의 도메인 동작을 수행한다.
        (static_cast<double>(value) - static_cast<double>(minimum)) /
// 값을 계산하거나 상태를 갱신한다.
        (static_cast<double>(maximum) - static_cast<double>(minimum));
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return static_cast<std::uint16_t>(normalized * maximum_code);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  static std::uint16_t encode_symmetric(const float value, const float maximum,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
                                        const std::uint16_t maximum_code) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (maximum <= 0.0F) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return 0U;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return encode_range(value, -maximum, maximum, maximum_code);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  bool limits_are_valid() const noexcept {
    // 안전 clamp는 후보 MIT wire range 밖으로 확장하지 않는다.
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::isfinite(limits_.min_position_rad) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(limits_.max_position_rad) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(limits_.max_velocity_rad_per_s) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(limits_.max_torque_nm) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(limits_.max_kp) && std::isfinite(limits_.max_kd) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           limits_.min_position_rad < limits_.max_position_rad &&
// 이 코드 줄의 도메인 동작을 수행한다.
           limits_.min_position_rad >= -12.5F &&
// 이 코드 줄의 도메인 동작을 수행한다.
           limits_.max_position_rad <= 12.5F &&
// 이 코드 줄의 도메인 동작을 수행한다.
           limits_.max_velocity_rad_per_s >= 0.0F &&
// 이 코드 줄의 도메인 동작을 수행한다.
           limits_.max_velocity_rad_per_s <= 65.0F &&
// 이 코드 줄의 도메인 동작을 수행한다.
           limits_.max_torque_nm >= 0.0F && limits_.max_torque_nm <= 50.0F &&
// 이 코드 줄의 도메인 동작을 수행한다.
           limits_.max_kp >= 0.0F && limits_.max_kp <= 500.0F &&
// 값을 계산하거나 상태를 갱신한다.
           limits_.max_kd >= 0.0F && limits_.max_kd <= 5.0F;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 값을 계산하거나 상태를 갱신한다.
  MitLimits limits_;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace bldc::command
