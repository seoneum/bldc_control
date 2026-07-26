// 필요한 헤더와 타입 정의를 포함한다.
#include <array>     // 고정 길이 4-byte payload를 표현한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>   // uint8_t처럼 크기가 명확한 정수를 사용한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <iomanip>   // CAN ID를 16진수로 보기 좋게 출력한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <iostream>  // host tutorial 결과를 terminal에 출력한다.

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/can_frame.hpp"  // CAN ID와 little-endian helper를 가져온다.

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
int main() {
  // 예제 node ID다. 실제 값은 Motor Wizard/제품 설정에서 확인해야 한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  constexpr std::uint8_t node_id = 0x12U;

  // encoder estimates의 candidate command ID다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  constexpr std::uint8_t command_id = 0x09U;

  // CAN Simple 후보식 (node << 5) | command를 안전하게 적용한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto can_id = bldc::pack_standard_id(node_id, command_id);

  // 범위를 벗어난 node/command라면 optional이 비어 있으므로 즉시 중단한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!can_id.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
    std::cerr << "CAN ID 범위 오류\n";
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return 1;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // 계산된 11-bit arbitration ID를 16진수로 확인한다.
// 함수의 입력과 동작 계약을 선언한다.
  std::cout << "packed CAN ID = 0x" << std::hex << std::setw(3)
// 값을 계산하거나 상태를 갱신한다.
            << std::setfill('0') << *can_id << '\n';

  // ID를 다시 node와 command로 분리해 round-trip을 확인한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto fields = bldc::unpack_standard_id(*can_id);

  // 방금 만든 11-bit ID이므로 정상이라면 반드시 값이 있어야 한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (!fields.has_value()) {
// 값을 계산하거나 상태를 갱신한다.
    std::cerr << "CAN ID decode 오류\n";
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return 1;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // uint8_t는 문자로 출력될 수 있으므로 unsigned로 변환해 숫자로 본다.
// 함수의 입력과 동작 계약을 선언한다.
  std::cout << std::dec << "node=" << static_cast<unsigned>(fields->node_id)
// 값을 계산하거나 상태를 갱신한다.
            << " command=" << static_cast<unsigned>(fields->command_id) << '\n';

  // 0x12345678을 little-endian byte 순서로 나열한 fixture다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const std::array<std::uint8_t, 4U> bytes{0x78U, 0x56U, 0x34U, 0x12U};

  // 가장 낮은 자리 byte부터 조립해 원래 32-bit 값을 복원한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const std::uint32_t value = bldc::decode_u32_le(bytes);

  // 결과가 0x12345678인지 눈으로 확인한다.
// 값을 계산하거나 상태를 갱신한다.
  std::cout << "little-endian value = 0x" << std::hex << value << '\n';

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return 0;  // tutorial이 정상 종료됐음을 shell에 알린다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
