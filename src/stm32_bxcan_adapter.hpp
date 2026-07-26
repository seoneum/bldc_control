// 이 코드 줄의 도메인 동작을 수행한다.
#pragma once

// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>

// 필요한 헤더와 타입 정의를 포함한다.
#include "stm32f1xx_hal.h"

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/can_frame.hpp"

// 컴파일 조건과 기능 게이트를 제어한다.
#if defined(BLDC_ENABLE_CAN_TRANSMIT) && (BLDC_ENABLE_CAN_TRANSMIT != 0)
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/command_builder.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/safety_supervisor.hpp"
// 컴파일 조건과 기능 게이트를 제어한다.
#endif

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace bldc::hal {

// 재사용할 자료 또는 동작 계약을 선언한다.
class Stm32BxcanAdapter {
// 멤버의 접근 범위를 지정한다.
 public:
// 값을 계산하거나 상태를 갱신한다.
  Stm32BxcanAdapter() noexcept = default;

// 값을 계산하거나 상태를 갱신한다.
  bool initialize() noexcept;
// 값을 계산하거나 상태를 갱신한다.
  bool poll(CanFrame& frame) noexcept;

// 이 코드 줄의 도메인 동작을 수행한다.
  bool started() const noexcept { return started_; }

// 컴파일 조건과 기능 게이트를 제어한다.
#if defined(BLDC_ENABLE_CAN_TRANSMIT) && (BLDC_ENABLE_CAN_TRANSMIT != 0)
// 이 코드 줄의 도메인 동작을 수행한다.
  bool transmit(const command::AuthorizedFrame& frame,
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
                const SafetySupervisor& safety, bool runtime_armed) noexcept;
// 컴파일 조건과 기능 게이트를 제어한다.
#endif

// 멤버의 접근 범위를 지정한다.
 private:
// 값을 계산하거나 상태를 갱신한다.
  bool configure_system_clock() noexcept;
// 값을 계산하거나 상태를 갱신한다.
  bool configure_can() noexcept;
// 값을 계산하거나 상태를 갱신한다.
  bool configure_filter() noexcept;
// 값을 계산하거나 상태를 갱신한다.
  void mark_failed() noexcept;

// 값을 계산하거나 상태를 갱신한다.
  CAN_HandleTypeDef can_{};
// 값을 계산하거나 상태를 갱신한다.
  bool started_{false};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace bldc::hal
