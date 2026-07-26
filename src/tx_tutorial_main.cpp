// 이 파일은 기본 firmware가 아니다.
// platformio의 nucleo_f103rb_tx_tutorial 환경에서만 별도 main으로 선택된다.

// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>  // millisecond 주기와 node ID에 고정 크기 정수를 사용한다.

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/application.hpp"      // telemetry, timeout, fault, arm 상태를 관리한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/command_builder.hpp"  // 작은 MIT command를 clamp하고 승인 token을 만든다.
// 필요한 헤더와 타입 정의를 포함한다.
#include "stm32_bxcan_adapter.hpp"   // 승인된 token만 STM32 bxCAN으로 송신한다.

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace {

// 실제 Motor Wizard/제품 설정에서 확인한 node ID로 바꿔야 한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::uint8_t kNodeId = 0U;

// heartbeat 후보 period보다 긴 250 ms를 host stale timeout 예시로 사용한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::uint32_t kHeartbeatTimeoutMs = 250U;

// command period는 20 ms, 즉 50 Hz의 tutorial 값이다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::uint32_t kCommandPeriodMs = 20U;

// 물리 안전 점검 전에는 false를 유지한다.
// 실제 제품에서는 상수 대신 hardware button/상위 supervisor의 rising edge를 읽는다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr bool kOperatorArmInput = false;

// compile-time TX enable과 별개인 두 번째 runtime gate다.
// wiring, node ID, unit, sign, timeout을 확인하기 전에는 false를 유지한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr bool kRuntimeTransmitEnabled = false;

// 실제 system에서는 이 함수가 hardware arm switch를 읽어야 한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
bool read_operator_arm_input() noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return kOperatorArmInput;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 실제 system에서는 contactor/bench 상태를 포함한 runtime permission을 읽어야 한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
bool read_runtime_transmit_enable() noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return kRuntimeTransmitEnabled;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
int main() {
  // 이 source가 TX compile gate가 켜진 target에서만 build되는지 확인한다.
// 이 코드 줄의 도메인 동작을 수행한다.
  static_assert(bldc::command::kCanTransmitCompiled,
// 값을 계산하거나 상태를 갱신한다.
                "Use the nucleo_f103rb_tx_tutorial environment.");

  // STM32 HAL runtime과 1 ms SysTick을 초기화한다.
// 값을 계산하거나 상태를 갱신한다.
  HAL_Init();

  // 지정 node의 heartbeat/encoder와 250 ms stale timeout을 관리한다.
// 값을 계산하거나 상태를 갱신한다.
  bldc::TelemetryApplication application{kNodeId, kHeartbeatTimeoutMs};

  // motor authority를 protocol full scale보다 훨씬 작게 제한한 예시다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const bldc::command::MitLimits limits{
// 이 코드 줄의 도메인 동작을 수행한다.
      -0.25F,  // position 최소 [rad]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.25F,   // position 최대 [rad]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.5F,    // velocity 절댓값 한계 [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.10F,   // torque 절댓값 한계 [N·m]
// 이 코드 줄의 도메인 동작을 수행한다.
      1.0F,    // Kp 최대값
// 이 코드 줄의 도메인 동작을 수행한다.
      0.05F,   // Kd 최대값
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // 모든 request가 위 limit을 통과하도록 builder를 한 번 생성한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const bldc::command::CommandBuilder builder{limits};

  // GPIO, clock, 500 kbit/s 후보 bit timing, filter, CAN1을 시작한다.
// 값을 계산하거나 상태를 갱신한다.
  bldc::hal::Stm32BxcanAdapter can{};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!can.initialize()) {
    // CAN 초기화 실패 시 송신하지 않고 영구 safe state에 머문다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    for (;;) {
// 값을 계산하거나 상태를 갱신한다.
      application.disarm();
// 값을 계산하거나 상태를 갱신한다.
      application.update(HAL_GetTick());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // arm 입력의 rising edge를 검출하기 위한 이전 상태다.
// 값을 계산하거나 상태를 갱신한다.
  bool previous_arm_input = false;

  // arm edge가 heartbeat보다 먼저 와도 한 번은 pending 상태로 보관한다.
// 값을 계산하거나 상태를 갱신한다.
  bool arm_request_pending = false;

  // command rate를 제한하기 위한 마지막 송신 시각이다.
// 값을 계산하거나 상태를 갱신한다.
  std::uint32_t last_command_ms = HAL_GetTick();

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  for (;;) {
    // 이번 loop의 시간을 한 번만 읽어 같은 판단에서 일관되게 사용한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::uint32_t now_ms = HAL_GetTick();

    // FIFO0에 들어온 frame이 있으면 application에 전달한다.
// 값을 계산하거나 상태를 갱신한다.
    bldc::CanFrame received{};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (can.poll(received)) {
// 값을 계산하거나 상태를 갱신한다.
      static_cast<void>(application.ingest(received, now_ms));
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // frame이 없어도 heartbeat age를 갱신해 stale을 검출한다.
// 값을 계산하거나 상태를 갱신한다.
    application.update(now_ms);

    // operator arm 입력은 level이 아니라 false→true edge로 해석한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const bool arm_input = read_operator_arm_input();
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (arm_input && !previous_arm_input) {
// 값을 계산하거나 상태를 갱신한다.
      arm_request_pending = true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 값을 계산하거나 상태를 갱신한다.
    previous_arm_input = arm_input;

    // arm switch가 내려가면 즉시 disarm하고 pending 요청도 폐기한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!arm_input) {
// 값을 계산하거나 상태를 갱신한다.
      arm_request_pending = false;
// 값을 계산하거나 상태를 갱신한다.
      application.disarm();
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // 첫 healthy heartbeat가 온 뒤 pending arm을 정확히 한 번 소비한다.
    // stale 후 heartbeat가 돌아와도 새 rising edge 없이는 자동 재arm되지 않는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (arm_request_pending && application.arm()) {
// 값을 계산하거나 상태를 갱신한다.
      arm_request_pending = false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // compile gate, operator arm, runtime enable, fresh heartbeat, no fault를 확인한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const bool runtime_enabled = read_runtime_transmit_enable();
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!runtime_enabled || !application.safety().permits_command()) {
// 현재 반복 흐름을 제어한다.
      continue;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // 20 ms가 지나기 전에는 새 command를 만들지 않는다.
// 입력과 상태에 따라 실행 경로를 선택한다.
    if (static_cast<std::uint32_t>(now_ms - last_command_ms) <
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
        kCommandPeriodMs) {
// 현재 반복 흐름을 제어한다.
      continue;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 값을 계산하거나 상태를 갱신한다.
    last_command_ms = now_ms;

    // 기본 request는 gain과 torque가 모두 0인 no-motion-intent 예시다.
    // 실제 값을 넣기 전에 unit, sign, zero, gear ratio를 각각 bench에서 확인한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand request{
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,  // position [rad]
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,  // velocity [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,  // Kp
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,  // Kd
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,  // feedforward torque [N·m]
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

    // 모든 compile/runtime/safety gate가 true일 때만 AuthorizedFrame이 생긴다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto authorized = builder.build_mit(
// 값을 계산하거나 상태를 갱신한다.
        kNodeId, request, application.safety(), runtime_enabled);

    // NaN, 잘못된 limit/node, gate 실패 시 frame을 보내지 않는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!authorized.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
      application.disarm();
// 현재 반복 흐름을 제어한다.
      continue;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // raw CanFrame이 아니라 승인 token만 HAL adapter에 전달한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!can.transmit(*authorized, application.safety(), runtime_enabled)) {
      // HAL mailbox/API 실패 후에는 재시도하며 계속 움직이지 않도록 disarm한다.
// 값을 계산하거나 상태를 갱신한다.
      application.disarm();
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
