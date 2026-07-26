// 필요한 헤더와 타입 정의를 포함한다.
#include "stm32_bxcan_adapter.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include <array>

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace bldc::hal {

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
bool Stm32BxcanAdapter::initialize() noexcept {
// 값을 계산하거나 상태를 갱신한다.
  started_ = false;

// 입력과 상태에 따라 실행 경로를 선택한다.
  if (!configure_system_clock() || !configure_can() || !configure_filter() ||
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      HAL_CAN_Start(&can_) != HAL_OK) {
// 값을 계산하거나 상태를 갱신한다.
    mark_failed();
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 값을 계산하거나 상태를 갱신한다.
  started_ = true;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
bool Stm32BxcanAdapter::configure_system_clock() noexcept {
// 값을 계산하거나 상태를 갱신한다.
  RCC_OscInitTypeDef oscillator{};
// 값을 계산하거나 상태를 갱신한다.
  oscillator.OscillatorType = RCC_OSCILLATORTYPE_HSE;
// 값을 계산하거나 상태를 갱신한다.
  oscillator.HSEState = RCC_HSE_BYPASS;
// 값을 계산하거나 상태를 갱신한다.
  oscillator.HSIState = RCC_HSI_ON;
// 값을 계산하거나 상태를 갱신한다.
  oscillator.PLL.PLLState = RCC_PLL_ON;
// 값을 계산하거나 상태를 갱신한다.
  oscillator.PLL.PLLSource = RCC_PLLSOURCE_HSE;
// 값을 계산하거나 상태를 갱신한다.
  oscillator.PLL.PLLMUL = RCC_PLL_MUL9;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (HAL_RCC_OscConfig(&oscillator) != HAL_OK) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 값을 계산하거나 상태를 갱신한다.
  RCC_ClkInitTypeDef clocks{};
// 이 코드 줄의 도메인 동작을 수행한다.
  clocks.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK |
// 값을 계산하거나 상태를 갱신한다.
                     RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
// 값을 계산하거나 상태를 갱신한다.
  clocks.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
// 값을 계산하거나 상태를 갱신한다.
  clocks.AHBCLKDivider = RCC_SYSCLK_DIV1;
// 값을 계산하거나 상태를 갱신한다.
  clocks.APB1CLKDivider = RCC_HCLK_DIV2;
// 값을 계산하거나 상태를 갱신한다.
  clocks.APB2CLKDivider = RCC_HCLK_DIV1;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return HAL_RCC_ClockConfig(&clocks, FLASH_LATENCY_2) == HAL_OK;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
bool Stm32BxcanAdapter::configure_can() noexcept {
// 값을 계산하거나 상태를 갱신한다.
  __HAL_RCC_AFIO_CLK_ENABLE();
// 값을 계산하거나 상태를 갱신한다.
  __HAL_RCC_GPIOA_CLK_ENABLE();
// 값을 계산하거나 상태를 갱신한다.
  __HAL_RCC_CAN1_CLK_ENABLE();

// 값을 계산하거나 상태를 갱신한다.
  GPIO_InitTypeDef gpio{};
// 값을 계산하거나 상태를 갱신한다.
  gpio.Pin = GPIO_PIN_11;
// 값을 계산하거나 상태를 갱신한다.
  gpio.Mode = GPIO_MODE_INPUT;
// 값을 계산하거나 상태를 갱신한다.
  gpio.Pull = GPIO_NOPULL;
// 값을 계산하거나 상태를 갱신한다.
  HAL_GPIO_Init(GPIOA, &gpio);

// 값을 계산하거나 상태를 갱신한다.
  gpio.Pin = GPIO_PIN_12;
// 값을 계산하거나 상태를 갱신한다.
  gpio.Mode = GPIO_MODE_AF_PP;
// 값을 계산하거나 상태를 갱신한다.
  gpio.Speed = GPIO_SPEED_FREQ_HIGH;
// 값을 계산하거나 상태를 갱신한다.
  HAL_GPIO_Init(GPIOA, &gpio);

  // PA11/PA12는 logic-side CAN_RX/CAN_TX다. CANH/CANL에는 직접 연결하지
  // 않고 반드시 3.3 V 호환 CAN transceiver를 사이에 둔다.

// 값을 계산하거나 상태를 갱신한다.
  can_.Instance = CAN1;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.Prescaler = 4U;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.Mode = CAN_MODE_NORMAL;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.SyncJumpWidth = CAN_SJW_1TQ;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.TimeSeg1 = CAN_BS1_13TQ;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.TimeSeg2 = CAN_BS2_4TQ;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.TimeTriggeredMode = DISABLE;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.AutoBusOff = DISABLE;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.AutoWakeUp = DISABLE;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.AutoRetransmission = DISABLE;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.ReceiveFifoLocked = DISABLE;
// 값을 계산하거나 상태를 갱신한다.
  can_.Init.TransmitFifoPriority = DISABLE;

  // APB1 36 MHz에서 36 MHz / (4 * (1 + 13 + 4)) = 500 kbit/s다.
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return HAL_CAN_Init(&can_) == HAL_OK;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
bool Stm32BxcanAdapter::configure_filter() noexcept {
// 값을 계산하거나 상태를 갱신한다.
  CAN_FilterTypeDef filter{};
// 값을 계산하거나 상태를 갱신한다.
  filter.FilterBank = 0U;
// 값을 계산하거나 상태를 갱신한다.
  filter.FilterMode = CAN_FILTERMODE_IDMASK;
// 값을 계산하거나 상태를 갱신한다.
  filter.FilterScale = CAN_FILTERSCALE_32BIT;
// 값을 계산하거나 상태를 갱신한다.
  filter.FilterIdHigh = 0U;
// 값을 계산하거나 상태를 갱신한다.
  filter.FilterIdLow = 0U;
// 값을 계산하거나 상태를 갱신한다.
  filter.FilterMaskIdHigh = 0U;
  // IDE=0, RTR=0만 통과시키고 11-bit standard identifier는 모두 받는다.
// 이 코드 줄의 도메인 동작을 수행한다.
  filter.FilterMaskIdLow =
// 값을 계산하거나 상태를 갱신한다.
      static_cast<std::uint16_t>(CAN_ID_EXT | CAN_RTR_REMOTE);
// 값을 계산하거나 상태를 갱신한다.
  filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
// 값을 계산하거나 상태를 갱신한다.
  filter.FilterActivation = ENABLE;
// 값을 계산하거나 상태를 갱신한다.
  filter.SlaveStartFilterBank = 14U;

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return HAL_CAN_ConfigFilter(&can_, &filter) == HAL_OK;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
bool Stm32BxcanAdapter::poll(CanFrame& frame) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!started_ || HAL_CAN_GetRxFifoFillLevel(&can_, CAN_RX_FIFO0) == 0U) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 값을 계산하거나 상태를 갱신한다.
  CAN_RxHeaderTypeDef header{};
// 값을 계산하거나 상태를 갱신한다.
  std::uint8_t data[8U]{};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (HAL_CAN_GetRxMessage(&can_, CAN_RX_FIFO0, &header, data) != HAL_OK) {
// 값을 계산하거나 상태를 갱신한다.
    mark_failed();
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 입력과 상태에 따라 실행 경로를 선택한다.
  if (header.IDE != CAN_ID_STD || header.RTR != CAN_RTR_DATA ||
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      header.StdId > 0x07FFU || header.DLC > 8U) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 값을 계산하거나 상태를 갱신한다.
  CanFrame received{};
// 값을 계산하거나 상태를 갱신한다.
  received.identifier = static_cast<std::uint16_t>(header.StdId);
// 값을 계산하거나 상태를 갱신한다.
  received.extended = false;
// 값을 계산하거나 상태를 갱신한다.
  received.remote = false;
// 값을 계산하거나 상태를 갱신한다.
  received.dlc = static_cast<std::uint8_t>(header.DLC);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  for (std::uint8_t index = 0U; index < received.dlc; ++index) {
// 값을 계산하거나 상태를 갱신한다.
    received.data[index] = data[index];
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 값을 계산하거나 상태를 갱신한다.
  frame = received;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 컴파일 조건과 기능 게이트를 제어한다.
#if defined(BLDC_ENABLE_CAN_TRANSMIT) && (BLDC_ENABLE_CAN_TRANSMIT != 0)
// 이 코드 줄의 도메인 동작을 수행한다.
bool Stm32BxcanAdapter::transmit(const command::AuthorizedFrame& frame,
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
                                 const SafetySupervisor& safety,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
                                 const bool runtime_armed) noexcept {
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const CanFrame& raw_frame = frame.frame();
// 입력과 상태에 따라 실행 경로를 선택한다.
  if (!started_ || !runtime_armed || !safety.permits_command() ||
// 이 코드 줄의 도메인 동작을 수행한다.
      raw_frame.extended || raw_frame.remote ||
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
      raw_frame.identifier > 0x07FFU || raw_frame.dlc > 8U) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 값을 계산하거나 상태를 갱신한다.
  CAN_TxHeaderTypeDef header{};
// 값을 계산하거나 상태를 갱신한다.
  header.StdId = raw_frame.identifier;
// 값을 계산하거나 상태를 갱신한다.
  header.ExtId = 0U;
// 값을 계산하거나 상태를 갱신한다.
  header.IDE = CAN_ID_STD;
// 값을 계산하거나 상태를 갱신한다.
  header.RTR = CAN_RTR_DATA;
// 값을 계산하거나 상태를 갱신한다.
  header.DLC = raw_frame.dlc;
// 값을 계산하거나 상태를 갱신한다.
  header.TransmitGlobalTime = DISABLE;

// 값을 계산하거나 상태를 갱신한다.
  std::array<std::uint8_t, 8U> data = raw_frame.data;
// 값을 계산하거나 상태를 갱신한다.
  std::uint32_t mailbox = 0U;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (HAL_CAN_AddTxMessage(&can_, &header, data.data(), &mailbox) != HAL_OK) {
// 값을 계산하거나 상태를 갱신한다.
    mark_failed();
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
// 컴파일 조건과 기능 게이트를 제어한다.
#endif

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void Stm32BxcanAdapter::mark_failed() noexcept {
// 값을 계산하거나 상태를 갱신한다.
  started_ = false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace bldc::hal
