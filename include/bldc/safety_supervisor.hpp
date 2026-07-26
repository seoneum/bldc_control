// 이 코드 줄의 도메인 동작을 수행한다.
#pragma once

// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/gim_protocol.hpp"

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace bldc {

// 재사용할 자료 또는 동작 계약을 선언한다.
enum class SafetyState : std::uint8_t {
// 이 코드 줄의 도메인 동작을 수행한다.
  disarmed = 0U,
// 이 코드 줄의 도메인 동작을 수행한다.
  armed = 1U,
// 이 코드 줄의 도메인 동작을 수행한다.
  heartbeat_stale = 2U,
// 이 코드 줄의 도메인 동작을 수행한다.
  fault_latched = 3U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
class SafetySupervisor {
// 멤버의 접근 범위를 지정한다.
 public:
// 이 코드 줄의 도메인 동작을 수행한다.
  explicit constexpr SafetySupervisor(
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
      const std::uint32_t heartbeat_timeout_ms) noexcept
// 이 코드 줄의 도메인 동작을 수행한다.
      : heartbeat_timeout_ms_(heartbeat_timeout_ms) {}

// 이 코드 줄의 도메인 동작을 수행한다.
  void observe_heartbeat(const gim::Heartbeat& heartbeat,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
                         const std::uint32_t received_at_ms) noexcept {
// 값을 계산하거나 상태를 갱신한다.
    has_heartbeat_ = true;
// 값을 계산하거나 상태를 갱신한다.
    heartbeat_fresh_ = true;
// 값을 계산하거나 상태를 갱신한다.
    last_heartbeat_ms_ = received_at_ms;

    // flags bit 0..3만 fault다. bit 7 trajectory_done은 정상 상태이므로
    // fault latch에 포함하지 않는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (heartbeat.axis_error != 0U || (heartbeat.flags & 0x0FU) != 0U) {
// 값을 계산하거나 상태를 갱신한다.
      fault_latched_ = true;
// 값을 계산하거나 상태를 갱신한다.
      armed_ = false;
// 값을 계산하거나 상태를 갱신한다.
      state_ = SafetyState::fault_latched;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // 정상 heartbeat가 돌아와도 fault latch는 자동으로 해제하지 않는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!armed_) {
// 값을 계산하거나 상태를 갱신한다.
      state_ = SafetyState::disarmed;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  void update(const std::uint32_t now_ms) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (fault_latched_) {
// 값을 계산하거나 상태를 갱신한다.
      armed_ = false;
// 값을 계산하거나 상태를 갱신한다.
      state_ = SafetyState::fault_latched;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!has_heartbeat_) {
// 값을 계산하거나 상태를 갱신한다.
      armed_ = false;
// 값을 계산하거나 상태를 갱신한다.
      heartbeat_fresh_ = false;
// 값을 계산하거나 상태를 갱신한다.
      state_ = SafetyState::disarmed;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // unsigned 차이는 HAL tick rollover에서도 age를 보존한다. timeout 경계
    // 자체부터 stale로 판정해 한 주기 더 명령을 허용하지 않는다.
// 입력과 상태에 따라 실행 경로를 선택한다.
    if (static_cast<std::uint32_t>(now_ms - last_heartbeat_ms_) >=
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
        heartbeat_timeout_ms_) {
// 값을 계산하거나 상태를 갱신한다.
      heartbeat_fresh_ = false;
// 값을 계산하거나 상태를 갱신한다.
      armed_ = false;
// 값을 계산하거나 상태를 갱신한다.
      state_ = SafetyState::heartbeat_stale;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 값을 계산하거나 상태를 갱신한다.
    heartbeat_fresh_ = true;
// 값을 계산하거나 상태를 갱신한다.
    state_ = armed_ ? SafetyState::armed : SafetyState::disarmed;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // freshness가 회복되어도 operator가 명시적으로 다시 arm해야 한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  bool arm() noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (fault_latched_ || !has_heartbeat_ || !heartbeat_fresh_) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 값을 계산하거나 상태를 갱신한다.
    armed_ = true;
// 값을 계산하거나 상태를 갱신한다.
    state_ = SafetyState::armed;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  void disarm() noexcept {
// 값을 계산하거나 상태를 갱신한다.
    armed_ = false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (fault_latched_) {
// 값을 계산하거나 상태를 갱신한다.
      state_ = SafetyState::fault_latched;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    } else if (has_heartbeat_ && !heartbeat_fresh_) {
// 값을 계산하거나 상태를 갱신한다.
      state_ = SafetyState::heartbeat_stale;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    } else {
// 값을 계산하거나 상태를 갱신한다.
      state_ = SafetyState::disarmed;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  constexpr SafetyState state() const noexcept { return state_; }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  constexpr bool fault_latched() const noexcept { return fault_latched_; }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  constexpr bool permits_command() const noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return armed_ && heartbeat_fresh_ && !fault_latched_;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 멤버의 접근 범위를 지정한다.
 private:
// 값을 계산하거나 상태를 갱신한다.
  std::uint32_t heartbeat_timeout_ms_;
// 값을 계산하거나 상태를 갱신한다.
  std::uint32_t last_heartbeat_ms_{0U};
// 값을 계산하거나 상태를 갱신한다.
  bool has_heartbeat_{false};
// 값을 계산하거나 상태를 갱신한다.
  bool heartbeat_fresh_{false};
// 값을 계산하거나 상태를 갱신한다.
  bool armed_{false};
// 값을 계산하거나 상태를 갱신한다.
  bool fault_latched_{false};
// 값을 계산하거나 상태를 갱신한다.
  SafetyState state_{SafetyState::disarmed};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace bldc
