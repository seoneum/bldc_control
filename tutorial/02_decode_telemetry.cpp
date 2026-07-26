// 필요한 헤더와 타입 정의를 포함한다.
#include <array>     // CAN payload 8 byte를 고정 길이로 보관한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>   // protocol에서 정확한 정수 크기를 사용한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <iostream>  // decode 결과를 terminal에 출력한다.

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/gim_protocol.hpp"  // GIM heartbeat와 encoder decoder를 사용한다.

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace {

// 여러 예제에서 같은 방식으로 standard data frame을 만들기 위한 helper다.
// 이 코드 줄의 도메인 동작을 수행한다.
bldc::CanFrame make_frame(
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::uint16_t id,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 8U>& payload) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return bldc::CanFrame{id, false, false, 8U, payload};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
int main() {
  // 실제 node ID가 확인되기 전까지는 tutorial fixture로만 사용한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  constexpr std::uint8_t node_id = 0x12U;

  // heartbeat command ID를 enum에서 가져와 magic number 중복을 피한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  constexpr auto heartbeat_command =
// 값을 계산하거나 상태를 갱신한다.
      static_cast<std::uint8_t>(bldc::gim::Command::heartbeat);

  // encoder command ID도 같은 protocol enum을 source of truth로 사용한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  constexpr auto encoder_command =
// 값을 계산하거나 상태를 갱신한다.
      static_cast<std::uint8_t>(bldc::gim::Command::encoder_estimates);

  // node와 heartbeat command를 합쳐 arbitration ID를 만든다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto heartbeat_id = bldc::pack_standard_id(node_id, heartbeat_command);

  // node와 encoder command를 합쳐 encoder arbitration ID를 만든다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto encoder_id = bldc::pack_standard_id(node_id, encoder_command);

  // 두 ID 중 하나라도 잘못되면 frame을 만들 수 없으므로 종료한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!heartbeat_id.has_value() || !encoder_id.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
    std::cerr << "candidate CAN ID 생성 실패\n";
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return 1;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // error=0, axis_state=8, flags=0인 healthy heartbeat fixture다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const std::array<std::uint8_t, 8U> heartbeat_payload{
// 이 코드 줄의 도메인 동작을 수행한다.
      0x00U, 0x00U, 0x00U, 0x00U,  // axis_error = 0
// 이 코드 줄의 도메인 동작을 수행한다.
      0x08U,                          // closed_loop_control 후보 state
// 이 코드 줄의 도메인 동작을 수행한다.
      0x00U,                          // flags = 0
// 이 코드 줄의 도메인 동작을 수행한다.
      0x00U,                          // reserved/candidate field
// 이 코드 줄의 도메인 동작을 수행한다.
      0x01U,                          // life counter 후보값
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // 1.5f와 -2.25f의 little-endian IEEE-754 byte fixture다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const std::array<std::uint8_t, 8U> encoder_payload{
// 이 코드 줄의 도메인 동작을 수행한다.
      0x00U, 0x00U, 0xC0U, 0x3FU,  // position = 1.5
// 이 코드 줄의 도메인 동작을 수행한다.
      0x00U, 0x00U, 0x10U, 0xC0U,  // velocity = -2.25
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // raw heartbeat frame을 typed Heartbeat로 변환한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto heartbeat = bldc::gim::decode_heartbeat(
// 값을 계산하거나 상태를 갱신한다.
      make_frame(*heartbeat_id, heartbeat_payload), node_id);

  // raw encoder frame을 typed EncoderEstimates로 변환한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto encoder = bldc::gim::decode_encoder_estimates(
// 값을 계산하거나 상태를 갱신한다.
      make_frame(*encoder_id, encoder_payload), node_id);

  // decoder가 frame shape/node/command를 거부하면 optional이 비어 있다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!heartbeat.has_value() || !encoder.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
    std::cerr << "telemetry decode 실패\n";
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return 1;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // axis error를 먼저 확인해야 position 값보다 fault를 우선 처리할 수 있다.
// 값을 계산하거나 상태를 갱신한다.
  std::cout << "axis_error=" << heartbeat->axis_error << '\n';

  // candidate encoder 단위가 rotor/output 중 무엇인지는 bench에서 확인한다.
// 이 코드 줄의 도메인 동작을 수행한다.
  std::cout << "position=" << encoder->position
// 값을 계산하거나 상태를 갱신한다.
            << " velocity=" << encoder->velocity << '\n';

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return 0;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
