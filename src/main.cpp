// 순수 C++ application layer를 가져온다.
// 이 layer는 STM32 HAL을 몰라도 CAN telemetry와 timeout을 처리할 수 있다.
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/application.hpp"

// STM32F103의 bxCAN peripheral을 HAL로 감싼 얇은 adapter를 가져온다.
// 필요한 헤더와 타입 정의를 포함한다.
#include "stm32_bxcan_adapter.hpp"

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
int main() {
  // HAL의 SysTick과 기본 MCU runtime을 먼저 초기화한다.
// 값을 계산하거나 상태를 갱신한다.
  HAL_Init();

  // GIM protocol frame을 typed telemetry로 바꾸고 heartbeat timeout을 감시한다.
  // 기본 node ID는 0이며, 실제 모터 node ID가 다르면 생성자 인자로 바꾼다.
// 값을 계산하거나 상태를 갱신한다.
  bldc::TelemetryApplication application{};

  // PA11(CAN_RX), PA12(CAN_TX)와 CAN1 peripheral을 담당하는 adapter다.
  // PA11/PA12는 CANH/CANL이 아니므로 외부 CAN transceiver가 필요하다.
// 값을 계산하거나 상태를 갱신한다.
  bldc::hal::Stm32BxcanAdapter can{};

  // clock, GPIO, CAN bit timing, receive filter, CAN start를 한 번에 준비한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!can.initialize()) {
    // 초기화 실패 시 motor command를 보내지 않고 이 안전 loop에 머문다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    for (;;) {
      // 수신이 없더라도 시간 상태를 갱신해 stale 판단 contract를 유지한다.
// 값을 계산하거나 상태를 갱신한다.
      application.update(HAL_GetTick());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // super-loop는 CAN frame을 기다리되 blocking delay를 사용하지 않는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  for (;;) {
    // 이번 loop에서 받을 최대 8-byte classic CAN frame 저장소다.
// 값을 계산하거나 상태를 갱신한다.
    bldc::CanFrame frame{};

    // FIFO0에 frame이 있을 때만 true가 되므로 CPU는 계속 safety를 갱신할 수 있다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (can.poll(frame)) {
      // 내 node의 heartbeat/encoder frame만 application state에 반영한다.
      // 다른 ID, RTR, extended, 잘못된 DLC frame은 ingest가 false로 거부한다.
// 값을 계산하거나 상태를 갱신한다.
      static_cast<void>(application.ingest(frame, HAL_GetTick()));
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // frame 수신 여부와 상관없이 heartbeat age와 fault state를 갱신한다.
// 값을 계산하거나 상태를 갱신한다.
    application.update(HAL_GetTick());

    // 기본 build에는 arm 호출도 transmit 호출도 없다.
    // 따라서 이 tutorial firmware는 CAN telemetry를 읽기만 한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
