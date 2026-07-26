// 이 코드 줄의 도메인 동작을 수행한다.
#pragma once

// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>
// 필요한 헤더와 타입 정의를 포함한다.
#include <optional>

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/gim_protocol.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/safety_supervisor.hpp"

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace bldc {

// 재사용할 자료 또는 동작 계약을 선언한다.
struct HalTickTimestamp {
// 값을 계산하거나 상태를 갱신한다.
  std::uint32_t raw_ms;
// 값을 계산하거나 상태를 갱신한다.
  std::uint64_t monotonic_ms;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
class TelemetryApplication {
// 멤버의 접근 범위를 지정한다.
 public:
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  static constexpr std::uint8_t kDefaultNodeId = 0U;
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  static constexpr std::uint32_t kDefaultHeartbeatTimeoutMs = 250U;

// 이 코드 줄의 도메인 동작을 수행한다.
  explicit TelemetryApplication(
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
      const std::uint8_t node_id = kDefaultNodeId,
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
      const std::uint32_t heartbeat_timeout_ms = kDefaultHeartbeatTimeoutMs)
// 이 코드 줄의 도메인 동작을 수행한다.
      noexcept
// 이 코드 줄의 도메인 동작을 수행한다.
      : node_id_(node_id), safety_(heartbeat_timeout_ms) {}

  // 이 노드의 검증된 telemetry만 반영한다. 잘못되거나 다른 노드의 frame은
  // cache, timestamp, safety state를 바꾸지 않는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  bool ingest(const CanFrame& frame, const std::uint32_t hal_tick_ms) noexcept {
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto heartbeat = gim::decode_heartbeat(frame, node_id_);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (heartbeat.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
      telemetry_.observe_heartbeat(*heartbeat);
// 값을 계산하거나 상태를 갱신한다.
      heartbeat_received_at_ = clock_.advance(hal_tick_ms);
// 값을 계산하거나 상태를 갱신한다.
      safety_.observe_heartbeat(*heartbeat, hal_tick_ms);
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto encoder = gim::decode_encoder_estimates(frame, node_id_);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (encoder.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
      telemetry_.observe_encoder_estimates(*encoder);
// 값을 계산하거나 상태를 갱신한다.
      encoder_received_at_ = clock_.advance(hal_tick_ms);
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // HAL_GetTick() 값을 foreground loop에서 넣어야 수신이 없어도 stale 상태로
  // 전이된다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  void update(const std::uint32_t hal_tick_ms) noexcept {
// 값을 계산하거나 상태를 갱신한다.
    static_cast<void>(clock_.advance(hal_tick_ms));
// 값을 계산하거나 상태를 갱신한다.
    safety_.update(hal_tick_ms);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const gim::TelemetryState& telemetry() const noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return telemetry_.state();
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const std::optional<HalTickTimestamp>& heartbeat_received_at() const noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return heartbeat_received_at_;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const std::optional<HalTickTimestamp>& encoder_received_at() const noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return encoder_received_at_;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // operator 입력에서만 호출한다. heartbeat가 없거나 stale/fault이면 실패한다.
// 이 코드 줄의 도메인 동작을 수행한다.
  bool arm() noexcept { return safety_.arm(); }

  // runtime enable이 내려가면 즉시 command permission을 제거한다.
// 이 코드 줄의 도메인 동작을 수행한다.
  void disarm() noexcept { safety_.disarm(); }

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const SafetySupervisor& safety() const noexcept { return safety_; }

// 멤버의 접근 범위를 지정한다.
 private:
// 재사용할 자료 또는 동작 계약을 선언한다.
  class HalTickClock {
// 멤버의 접근 범위를 지정한다.
   public:
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    HalTickTimestamp advance(const std::uint32_t raw_ms) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      if (!initialized_) {
// 값을 계산하거나 상태를 갱신한다.
        initialized_ = true;
// 값을 계산하거나 상태를 갱신한다.
        last_raw_ms_ = raw_ms;
// 값을 계산하거나 상태를 갱신한다.
        monotonic_ms_ = raw_ms;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      } else {
// 값을 계산하거나 상태를 갱신한다.
        monotonic_ms_ += static_cast<std::uint32_t>(raw_ms - last_raw_ms_);
// 값을 계산하거나 상태를 갱신한다.
        last_raw_ms_ = raw_ms;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      }

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return HalTickTimestamp{raw_ms, monotonic_ms_};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

// 멤버의 접근 범위를 지정한다.
   private:
// 값을 계산하거나 상태를 갱신한다.
    std::uint32_t last_raw_ms_{0U};
// 값을 계산하거나 상태를 갱신한다.
    std::uint64_t monotonic_ms_{0U};
// 값을 계산하거나 상태를 갱신한다.
    bool initialized_{false};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

// 값을 계산하거나 상태를 갱신한다.
  std::uint8_t node_id_;
// 값을 계산하거나 상태를 갱신한다.
  gim::TelemetryCache telemetry_{};
// 값을 계산하거나 상태를 갱신한다.
  HalTickClock clock_{};
// 값을 계산하거나 상태를 갱신한다.
  std::optional<HalTickTimestamp> heartbeat_received_at_{};
// 값을 계산하거나 상태를 갱신한다.
  std::optional<HalTickTimestamp> encoder_received_at_{};
// 값을 계산하거나 상태를 갱신한다.
  SafetySupervisor safety_;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace bldc
