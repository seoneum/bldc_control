// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>   // node ID와 CAN byte에 고정 크기 정수를 사용한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <iomanip>   // payload를 2자리 16진수로 출력한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <iostream>  // clamp와 frame preview 결과를 보여 준다.

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/command_builder.hpp"  // MIT clamp와 candidate encoder를 사용한다.

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
int main() {
  // 실제 node ID가 아니라 host tutorial에서만 사용하는 fixture다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  constexpr std::uint8_t node_id = 0x12U;

  // protocol full scale보다 훨씬 작은 학습용 authority를 정의한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const bldc::command::MitLimits limits{
// 이 코드 줄의 도메인 동작을 수행한다.
      -0.5F,  // position 최소값 [rad]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.5F,   // position 최대값 [rad]
// 이 코드 줄의 도메인 동작을 수행한다.
      1.0F,   // velocity 절댓값 한계 [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.25F,  // torque 절댓값 한계 [N·m]
// 이 코드 줄의 도메인 동작을 수행한다.
      2.0F,   // Kp 최대값
// 이 코드 줄의 도메인 동작을 수행한다.
      0.1F,   // Kd 최대값
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // 모든 MIT command는 이 builder의 limit와 gate를 거치게 한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const bldc::command::CommandBuilder builder{limits};

  // 일부러 limit보다 큰 값을 요청해 clamp 동작을 관찰한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const bldc::command::MitCommand requested{
// 이 코드 줄의 도메인 동작을 수행한다.
      4.0F,   // position 요청 [rad]
// 이 코드 줄의 도메인 동작을 수행한다.
      -3.0F,  // velocity 요청 [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      8.0F,   // Kp 요청
// 이 코드 줄의 도메인 동작을 수행한다.
      5.0F,   // Kd 요청
// 이 코드 줄의 도메인 동작을 수행한다.
      2.0F,   // torque 요청 [N·m]
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // sanitize는 hardware와 무관하게 유한성 검사와 clamp만 수행한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto safe = builder.sanitize(requested);

  // limit 자체가 잘못됐거나 NaN/Inf가 있으면 command를 만들지 않는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!safe.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
    std::cerr << "command sanitize 실패\n";
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return 1;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // 실제 적용될 작은 command를 사람이 먼저 읽어 확인한다.
// 이 코드 줄의 도메인 동작을 수행한다.
  std::cout << "clamped position=" << safe->position_rad
// 이 코드 줄의 도메인 동작을 수행한다.
            << " velocity=" << safe->velocity_rad_per_s
// 이 코드 줄의 도메인 동작을 수행한다.
            << " kp=" << safe->kp
// 이 코드 줄의 도메인 동작을 수행한다.
            << " kd=" << safe->kd
// 값을 계산하거나 상태를 갱신한다.
            << " torque=" << safe->torque_nm << '\n';

  // compile_mit는 candidate 8-byte frame을 메모리에서 만들 뿐 송신하지 않는다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto frame = builder.compile_mit(node_id, requested);

  // 잘못된 node ID나 command이면 frame preview도 거부한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!frame.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
    std::cerr << "MIT frame preview 실패\n";
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return 1;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // arbitration ID를 출력해 (node<<5)|0x08 구조를 확인한다.
// 값을 계산하거나 상태를 갱신한다.
  std::cout << "candidate CAN ID=0x" << std::hex << frame->identifier << '\n';

  // payload 8 byte를 순서대로 출력한다. 이 코드는 CAN peripheral을 사용하지 않는다.
// 값을 계산하거나 상태를 갱신한다.
  std::cout << "payload=";
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  for (const std::uint8_t byte : frame->data) {
// 함수의 입력과 동작 계약을 선언한다.
    std::cout << ' ' << std::setw(2) << std::setfill('0')
// 값을 계산하거나 상태를 갱신한다.
              << static_cast<unsigned>(byte);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }
// 값을 계산하거나 상태를 갱신한다.
  std::cout << '\n';

  // 실제 HAL 송신은 AuthorizedFrame과 별도의 compile/runtime/safety gate가 필요하다.
// 이 코드 줄의 도메인 동작을 수행한다.
  std::cout << "CAN transmit compiled="
// 값을 계산하거나 상태를 갱신한다.
            << bldc::command::kCanTransmitCompiled << '\n';

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return 0;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
