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

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace bldc::gim {

// 이 CAN Simple 레이아웃은 커뮤니티 번역에서 온 후보이며, 제조사와 해당
// 펌웨어 문서로 확인되기 전에는 송신 근거로 사용하지 않는다.
// 재사용할 자료 또는 동작 계약을 선언한다.
enum class Command : std::uint8_t {
// 이 코드 줄의 도메인 동작을 수행한다.
  heartbeat = 0x01U,
// 이 코드 줄의 도메인 동작을 수행한다.
  emergency_stop = 0x02U,
// 이 코드 줄의 도메인 동작을 수행한다.
  axis_state = 0x07U,
// 이 코드 줄의 도메인 동작을 수행한다.
  mit_control = 0x08U,
// 이 코드 줄의 도메인 동작을 수행한다.
  encoder_estimates = 0x09U,
// 이 코드 줄의 도메인 동작을 수행한다.
  controller_mode = 0x0BU,
// 이 코드 줄의 도메인 동작을 수행한다.
  input_position = 0x0CU,
// 이 코드 줄의 도메인 동작을 수행한다.
  input_velocity = 0x0DU,
// 이 코드 줄의 도메인 동작을 수행한다.
  input_torque = 0x0EU,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
enum class AxisState : std::uint8_t {
// 이 코드 줄의 도메인 동작을 수행한다.
  undefined = 0x00U,
// 이 코드 줄의 도메인 동작을 수행한다.
  idle = 0x01U,
// 이 코드 줄의 도메인 동작을 수행한다.
  startup_sequence = 0x02U,
// 이 코드 줄의 도메인 동작을 수행한다.
  full_calibration_sequence = 0x03U,
// 이 코드 줄의 도메인 동작을 수행한다.
  motor_calibration = 0x04U,
// 이 코드 줄의 도메인 동작을 수행한다.
  encoder_index_search = 0x06U,
// 이 코드 줄의 도메인 동작을 수행한다.
  encoder_offset_calibration = 0x07U,
// 이 코드 줄의 도메인 동작을 수행한다.
  closed_loop_control = 0x08U,
// 이 코드 줄의 도메인 동작을 수행한다.
  lockin_spin = 0x09U,
// 이 코드 줄의 도메인 동작을 수행한다.
  encoder_direction_find = 0x0AU,
// 이 코드 줄의 도메인 동작을 수행한다.
  homing = 0x0BU,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
struct Heartbeat {
// 값을 계산하거나 상태를 갱신한다.
  std::uint32_t axis_error;
// 값을 계산하거나 상태를 갱신한다.
  AxisState axis_state;
// 값을 계산하거나 상태를 갱신한다.
  std::uint8_t flags;
// 값을 계산하거나 상태를 갱신한다.
  std::uint8_t life_counter;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
struct EncoderEstimates {
// 값을 계산하거나 상태를 갱신한다.
  float position;
// 값을 계산하거나 상태를 갱신한다.
  float velocity;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
struct TelemetryState {
// 값을 계산하거나 상태를 갱신한다.
  std::optional<Heartbeat> heartbeat;
// 값을 계산하거나 상태를 갱신한다.
  std::optional<EncoderEstimates> encoder_estimates;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
class TelemetryCache {
// 멤버의 접근 범위를 지정한다.
 public:
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  void observe_heartbeat(const Heartbeat& heartbeat) noexcept {
// 값을 계산하거나 상태를 갱신한다.
    state_.heartbeat = heartbeat;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 이 코드 줄의 도메인 동작을 수행한다.
  void observe_encoder_estimates(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      const EncoderEstimates& estimates) noexcept {
// 값을 계산하거나 상태를 갱신한다.
    state_.encoder_estimates = estimates;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const TelemetryState& state() const noexcept { return state_; }

// 멤버의 접근 범위를 지정한다.
 private:
// 값을 계산하거나 상태를 갱신한다.
  TelemetryState state_{};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace detail {

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr bool is_expected_telemetry_frame(
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const CanFrame& frame, const std::uint8_t expected_node_id,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const Command expected_command) noexcept {
// 입력과 상태에 따라 실행 경로를 선택한다.
  if (frame.extended || frame.remote || frame.dlc != 8U ||
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      expected_node_id > 0x3FU) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto identifier = unpack_standard_id(frame.identifier);
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return identifier.has_value() && identifier->node_id == expected_node_id &&
// 값을 계산하거나 상태를 갱신한다.
         identifier->command_id == static_cast<std::uint8_t>(expected_command);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::array<std::uint8_t, 4U> first_word(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const CanFrame& frame) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  return {
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[0U],
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[1U],
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[2U],
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[3U],
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::array<std::uint8_t, 4U> second_word(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const CanFrame& frame) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  return {
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[4U],
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[5U],
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[6U],
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[7U],
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace detail

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
inline std::optional<Heartbeat> decode_heartbeat(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const CanFrame& frame, const std::uint8_t expected_node_id) noexcept {
// 입력과 상태에 따라 실행 경로를 선택한다.
  if (!detail::is_expected_telemetry_frame(frame, expected_node_id,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
                                            Command::heartbeat)) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  return Heartbeat{
// 이 코드 줄의 도메인 동작을 수행한다.
      decode_u32_le(detail::first_word(frame)),
// 이 코드 줄의 도메인 동작을 수행한다.
      static_cast<AxisState>(frame.data[4U]),
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[5U],
// 이 코드 줄의 도메인 동작을 수행한다.
      frame.data[7U],
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
inline std::optional<EncoderEstimates> decode_encoder_estimates(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const CanFrame& frame, const std::uint8_t expected_node_id) noexcept {
// 입력과 상태에 따라 실행 경로를 선택한다.
  if (!detail::is_expected_telemetry_frame(frame, expected_node_id,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
                                            Command::encoder_estimates)) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const EncoderEstimates estimates{
// 이 코드 줄의 도메인 동작을 수행한다.
      decode_f32_le(detail::first_word(frame)),
// 이 코드 줄의 도메인 동작을 수행한다.
      decode_f32_le(detail::second_word(frame)),
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!std::isfinite(estimates.position) || !std::isfinite(estimates.velocity)) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return estimates;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace bldc::gim
