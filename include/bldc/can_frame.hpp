// 이 코드 줄의 도메인 동작을 수행한다.
#pragma once

// 필요한 헤더와 타입 정의를 포함한다.
#include <array>
// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>
// 필요한 헤더와 타입 정의를 포함한다.
#include <cstring>
// 필요한 헤더와 타입 정의를 포함한다.
#include <limits>
// 필요한 헤더와 타입 정의를 포함한다.
#include <optional>

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace bldc {

// 재사용할 자료 또는 동작 계약을 선언한다.
struct CanFrame {
// 값을 계산하거나 상태를 갱신한다.
  std::uint16_t identifier;
// 값을 계산하거나 상태를 갱신한다.
  bool extended;
// 값을 계산하거나 상태를 갱신한다.
  bool remote;
// 값을 계산하거나 상태를 갱신한다.
  std::uint8_t dlc;
// 값을 계산하거나 상태를 갱신한다.
  std::array<std::uint8_t, 8U> data;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 재사용할 자료 또는 동작 계약을 선언한다.
struct StandardId {
// 값을 계산하거나 상태를 갱신한다.
  std::uint8_t node_id;
// 값을 계산하거나 상태를 갱신한다.
  std::uint8_t command_id;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// CAN Simple 후보 계약은 node 6 bit와 command 5 bit만 허용하므로
// 잘못된 값이 11-bit arbitration ID로 잘려 나가지 않게 먼저 거부한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::optional<std::uint16_t> pack_standard_id(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::uint8_t node_id, const std::uint8_t command_id) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (node_id > 0x3FU || command_id > 0x1FU) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return static_cast<std::uint16_t>(
// 값을 계산하거나 상태를 갱신한다.
      (static_cast<std::uint16_t>(node_id) << 5U) | command_id);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::optional<StandardId> unpack_standard_id(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::uint16_t identifier) noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  if (identifier > 0x07FFU) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::nullopt;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  return StandardId{
// 이 코드 줄의 도메인 동작을 수행한다.
      static_cast<std::uint8_t>((identifier >> 5U) & 0x3FU),
// 이 코드 줄의 도메인 동작을 수행한다.
      static_cast<std::uint8_t>(identifier & 0x1FU),
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::uint32_t decode_u32_le(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 4U>& bytes) noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return static_cast<std::uint32_t>(bytes[0U]) |
// 이 코드 줄의 도메인 동작을 수행한다.
         (static_cast<std::uint32_t>(bytes[1U]) << 8U) |
// 이 코드 줄의 도메인 동작을 수행한다.
         (static_cast<std::uint32_t>(bytes[2U]) << 16U) |
// 값을 계산하거나 상태를 갱신한다.
         (static_cast<std::uint32_t>(bytes[3U]) << 24U);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// byte 조립 뒤 memcpy하면 alignment와 strict-aliasing에 의존하지 않고
// little-endian IEEE-754 bit pattern을 복원할 수 있다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
inline float decode_f32_le(
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 4U>& bytes) noexcept {
// 이 코드 줄의 도메인 동작을 수행한다.
  static_assert(sizeof(float) == sizeof(std::uint32_t) &&
// 이 코드 줄의 도메인 동작을 수행한다.
                    std::numeric_limits<float>::is_iec559,
// 값을 계산하거나 상태를 갱신한다.
                "BLDC protocol requires 32-bit IEC 559 floats.");
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const std::uint32_t bits = decode_u32_le(bytes);
// 값을 계산하거나 상태를 갱신한다.
  float value = 0.0F;
// 값을 계산하거나 상태를 갱신한다.
  std::memcpy(&value, &bits, sizeof(value));
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return value;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace bldc
