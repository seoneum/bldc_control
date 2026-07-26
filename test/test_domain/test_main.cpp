// 필요한 헤더와 타입 정의를 포함한다.
#include <array>
// 필요한 헤더와 타입 정의를 포함한다.
#include <cmath>
// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>
// 필요한 헤더와 타입 정의를 포함한다.
#include <limits>
// 필요한 헤더와 타입 정의를 포함한다.
#include <type_traits>

// 필요한 헤더와 타입 정의를 포함한다.
#include <unity.h>

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/can_frame.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/application.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/command_builder.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/gim_protocol.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/safety_supervisor.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/state_estimator.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/tv_lqr.hpp"
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/kalman_filter.hpp"

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace {

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::uint8_t kNodeId = 0x12U;
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::uint8_t kHeartbeatCommand =
// 값을 계산하거나 상태를 갱신한다.
    static_cast<std::uint8_t>(bldc::gim::Command::heartbeat);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::uint8_t kMitCommand =
// 값을 계산하거나 상태를 갱신한다.
    static_cast<std::uint8_t>(bldc::gim::Command::mit_control);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
constexpr std::uint8_t kEncoderEstimatesCommand =
// 값을 계산하거나 상태를 갱신한다.
    static_cast<std::uint8_t>(bldc::gim::Command::encoder_estimates);

// 이 코드 줄의 도메인 동작을 수행한다.
bldc::CanFrame make_frame(
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::uint16_t identifier,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 8U>& payload) noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return bldc::CanFrame{identifier, false, false, 8U, payload};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
bldc::gim::Heartbeat healthy_heartbeat() noexcept {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    return bldc::gim::Heartbeat{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        bldc::gim::AxisState::closed_loop_control,
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_standard_id_round_trip_uses_candidate_can_simple_layout() {
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto packed = bldc::pack_standard_id(kNodeId, kEncoderEstimatesCommand);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(packed.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX16(0x0249U, *packed);

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto unpacked = bldc::unpack_standard_id(*packed);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(unpacked.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_UINT8(kNodeId, unpacked->node_id);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_UINT8(kEncoderEstimatesCommand, unpacked->command_id);

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto maximum = bldc::pack_standard_id(0x3FU, 0x1FU);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(maximum.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX16(0x07FFU, *maximum);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_standard_id_rejects_out_of_range_node_command_and_identifier() {
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(bldc::pack_standard_id(0x40U, 0x01U).has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(bldc::pack_standard_id(0x01U, 0x20U).has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(bldc::unpack_standard_id(0x0800U).has_value());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_little_endian_u32_and_f32_decoding_matches_fixtures() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 4U> u32_bytes{
// 이 코드 줄의 도메인 동작을 수행한다.
        0x78U, 0x56U, 0x34U, 0x12U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 4U> f32_bytes{
// 이 코드 줄의 도메인 동작을 수행한다.
        0x00U, 0x00U, 0xC0U, 0x3FU,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX32(0x12345678U, bldc::decode_u32_le(u32_bytes));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 1.5F, bldc::decode_f32_le(f32_bytes));
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_heartbeat_decoder_returns_typed_candidate_telemetry() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 8U> payload{
// 이 코드 줄의 도메인 동작을 수행한다.
        0x78U, 0x56U, 0x34U, 0x12U,
// 이 코드 줄의 도메인 동작을 수행한다.
        0x08U, 0xA5U, 0x01U, 0x00U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto identifier = bldc::pack_standard_id(kNodeId, kHeartbeatCommand);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(identifier.has_value());

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto heartbeat =
// 값을 계산하거나 상태를 갱신한다.
        bldc::gim::decode_heartbeat(make_frame(*identifier, payload), kNodeId);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(heartbeat.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX32(0x12345678U, heartbeat->axis_error);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::gim::AxisState::closed_loop_control),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(heartbeat->axis_state));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_UINT8(0xA5U, heartbeat->flags);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_UINT8(0x00U, heartbeat->life_counter);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_encoder_decoder_returns_typed_little_endian_float_telemetry() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 8U> payload{
// 이 코드 줄의 도메인 동작을 수행한다.
        0x00U, 0x00U, 0xC0U, 0x3FU,
// 이 코드 줄의 도메인 동작을 수행한다.
        0x00U, 0x00U, 0x10U, 0xC0U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto identifier =
// 값을 계산하거나 상태를 갱신한다.
        bldc::pack_standard_id(kNodeId, kEncoderEstimatesCommand);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(identifier.has_value());

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto estimates = bldc::gim::decode_encoder_estimates(
// 값을 계산하거나 상태를 갱신한다.
        make_frame(*identifier, payload), kNodeId);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(estimates.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 1.5F, estimates->position);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, -2.25F, estimates->velocity);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_protocol_decoders_reject_extended_rtr_wrong_dlc_and_wrong_node() {
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const std::array<std::uint8_t, 8U> payload{};
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto heartbeat_identifier =
// 값을 계산하거나 상태를 갱신한다.
        bldc::pack_standard_id(kNodeId, kHeartbeatCommand);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto wrong_node_identifier =
// 이 코드 줄의 도메인 동작을 수행한다.
        bldc::pack_standard_id(static_cast<std::uint8_t>(kNodeId + 1U),
// 값을 계산하거나 상태를 갱신한다.
                               kHeartbeatCommand);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto encoder_identifier =
// 값을 계산하거나 상태를 갱신한다.
        bldc::pack_standard_id(kNodeId, kEncoderEstimatesCommand);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(heartbeat_identifier.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(wrong_node_identifier.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(encoder_identifier.has_value());

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::CanFrame extended{
// 값을 계산하거나 상태를 갱신한다.
        *heartbeat_identifier, true, false, 8U, payload};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::CanFrame remote{
// 값을 계산하거나 상태를 갱신한다.
        *heartbeat_identifier, false, true, 8U, payload};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::CanFrame short_payload{
// 값을 계산하거나 상태를 갱신한다.
        *heartbeat_identifier, false, false, 7U, payload};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::CanFrame wrong_node{
// 값을 계산하거나 상태를 갱신한다.
        *wrong_node_identifier, false, false, 8U, payload};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::CanFrame wrong_command{
// 값을 계산하거나 상태를 갱신한다.
        *encoder_identifier, false, false, 8U, payload};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::CanFrame encoder_short_payload{
// 값을 계산하거나 상태를 갱신한다.
        *encoder_identifier, false, false, 7U, payload};

// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        bldc::gim::decode_heartbeat(extended, kNodeId).has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        bldc::gim::decode_heartbeat(remote, kNodeId).has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        bldc::gim::decode_heartbeat(short_payload, kNodeId).has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        bldc::gim::decode_heartbeat(wrong_node, kNodeId).has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        bldc::gim::decode_heartbeat(wrong_command, kNodeId).has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 함수의 입력과 동작 계약을 선언한다.
        bldc::gim::decode_encoder_estimates(encoder_short_payload, kNodeId)
// 값을 계산하거나 상태를 갱신한다.
            .has_value());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_safety_supervisor_uses_strict_heartbeat_freshness_boundary() {
// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor supervisor{100U};
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto heartbeat = healthy_heartbeat();

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(supervisor.arm());

// 값을 계산하거나 상태를 갱신한다.
    supervisor.observe_heartbeat(heartbeat, 100U);
// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(100U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::disarmed),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(supervisor.state()));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.arm());

// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(199U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::armed),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(supervisor.state()));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.permits_command());

// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(200U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::heartbeat_stale),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(supervisor.state()));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(supervisor.permits_command());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_safety_supervisor_requires_runtime_rearm_after_disarm_or_timeout() {
// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor supervisor{100U};
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto heartbeat = healthy_heartbeat();

// 값을 계산하거나 상태를 갱신한다.
    supervisor.observe_heartbeat(heartbeat, 10U);
// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(10U);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.arm());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.permits_command());

// 값을 계산하거나 상태를 갱신한다.
    supervisor.disarm();
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::disarmed),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(supervisor.state()));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(supervisor.permits_command());

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.arm());
// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(110U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::heartbeat_stale),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(supervisor.state()));

// 값을 계산하거나 상태를 갱신한다.
    supervisor.observe_heartbeat(heartbeat, 111U);
// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(111U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::disarmed),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(supervisor.state()));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(supervisor.permits_command());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.arm());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_safety_supervisor_latches_faults_until_reconstruction() {
// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor supervisor{100U};
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto heartbeat = healthy_heartbeat();

// 값을 계산하거나 상태를 갱신한다.
    supervisor.observe_heartbeat(heartbeat, 10U);
// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(10U);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.arm());

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::gim::Heartbeat faulted{
// 이 코드 줄의 도메인 동작을 수행한다.
        0x00000001U,
// 이 코드 줄의 도메인 동작을 수행한다.
        bldc::gim::AxisState::closed_loop_control,
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 값을 계산하거나 상태를 갱신한다.
    supervisor.observe_heartbeat(faulted, 11U);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.fault_latched());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::fault_latched),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(supervisor.state()));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(supervisor.permits_command());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(supervisor.arm());

// 값을 계산하거나 상태를 갱신한다.
    supervisor.observe_heartbeat(heartbeat, 12U);
// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(12U);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.fault_latched());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(supervisor.arm());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_safety_supervisor_ignores_trajectory_done_but_latches_fault_flags() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::gim::Heartbeat trajectory_complete{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        bldc::gim::AxisState::closed_loop_control,
// 이 코드 줄의 도메인 동작을 수행한다.
        0x80U,
// 이 코드 줄의 도메인 동작을 수행한다.
        1U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor healthy_supervisor{100U};
// 값을 계산하거나 상태를 갱신한다.
    healthy_supervisor.observe_heartbeat(trajectory_complete, 10U);
// 값을 계산하거나 상태를 갱신한다.
    healthy_supervisor.update(10U);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(healthy_supervisor.fault_latched());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(healthy_supervisor.arm());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(healthy_supervisor.permits_command());

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::gim::Heartbeat motor_fault{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        bldc::gim::AxisState::closed_loop_control,
// 이 코드 줄의 도메인 동작을 수행한다.
        0x01U,
// 이 코드 줄의 도메인 동작을 수행한다.
        2U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor faulted_supervisor{100U};
// 값을 계산하거나 상태를 갱신한다.
    faulted_supervisor.observe_heartbeat(motor_fault, 10U);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(faulted_supervisor.fault_latched());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(faulted_supervisor.arm());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_command_sanitization_clamps_finite_values_to_conservative_limits() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitLimits limits{
// 이 코드 줄의 도메인 동작을 수행한다.
        -0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        1.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.25F,
// 이 코드 줄의 도메인 동작을 수행한다.
        2.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.1F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const bldc::command::CommandBuilder builder{limits};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand requested{
// 이 코드 줄의 도메인 동작을 수행한다.
        4.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        -3.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        8.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        5.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        2.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto sanitized = builder.sanitize(requested);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(sanitized.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.5F, sanitized->position_rad);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, -1.0F, sanitized->velocity_rad_per_s);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 2.0F, sanitized->kp);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.1F, sanitized->kd);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.25F, sanitized->torque_nm);

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand negative_gains{
// 이 코드 줄의 도메인 동작을 수행한다.
        -4.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        3.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        -1.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        -1.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        -2.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto lower_sanitized = builder.sanitize(negative_gains);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(lower_sanitized.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, -0.5F, lower_sanitized->position_rad);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 1.0F, lower_sanitized->velocity_rad_per_s);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.0F, lower_sanitized->kp);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.0F, lower_sanitized->kd);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, -0.25F, lower_sanitized->torque_nm);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_command_sanitization_rejects_non_finite_requests() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitLimits limits{
// 이 코드 줄의 도메인 동작을 수행한다.
        -0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        1.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.25F,
// 이 코드 줄의 도메인 동작을 수행한다.
        2.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.1F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const bldc::command::CommandBuilder builder{limits};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand nan_position{
// 이 코드 줄의 도메인 동작을 수행한다.
        std::numeric_limits<float>::quiet_NaN(),
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand infinite_velocity{
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        std::numeric_limits<float>::infinity(),
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(builder.sanitize(nan_position).has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(builder.sanitize(infinite_velocity).has_value());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_command_builder_rejects_limits_outside_fixed_wire_ranges() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand zero_command{
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<bldc::command::MitLimits, 6U> invalid_limits{{
// 이 코드 줄의 도메인 동작을 수행한다.
        {-13.0F, 0.5F, 1.0F, 0.25F, 2.0F, 0.1F},
// 이 코드 줄의 도메인 동작을 수행한다.
        {-0.5F, 13.0F, 1.0F, 0.25F, 2.0F, 0.1F},
// 이 코드 줄의 도메인 동작을 수행한다.
        {-0.5F, 0.5F, 66.0F, 0.25F, 2.0F, 0.1F},
// 이 코드 줄의 도메인 동작을 수행한다.
        {-0.5F, 0.5F, 1.0F, 51.0F, 2.0F, 0.1F},
// 이 코드 줄의 도메인 동작을 수행한다.
        {-0.5F, 0.5F, 1.0F, 0.25F, 501.0F, 0.1F},
// 이 코드 줄의 도메인 동작을 수행한다.
        {-0.5F, 0.5F, 1.0F, 0.25F, 2.0F, 5.1F},
// 값을 계산하거나 상태를 갱신한다.
    }};

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    for (const auto& limits : invalid_limits) {
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
        const bldc::command::CommandBuilder builder{limits};
// 값을 계산하거나 상태를 갱신한다.
        TEST_ASSERT_FALSE(builder.sanitize(zero_command).has_value());
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_mit_command_compilation_uses_candidate_identifier_and_eight_byte_layout() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitLimits limits{
// 이 코드 줄의 도메인 동작을 수행한다.
        -0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        1.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.25F,
// 이 코드 줄의 도메인 동작을 수행한다.
        2.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.1F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const bldc::command::CommandBuilder builder{limits};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand request{
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto expected_identifier = bldc::pack_standard_id(kNodeId, kMitCommand);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(expected_identifier.has_value());

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto frame = builder.compile_mit(kNodeId, request);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(frame.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX16(0x0248U, *expected_identifier);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX16(*expected_identifier, frame->identifier);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(frame->extended);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(frame->remote);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_UINT8(8U, frame->dlc);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x7FU, frame->data[0U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0xFFU, frame->data[1U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x7FU, frame->data[2U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0xF0U, frame->data[3U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x00U, frame->data[4U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x00U, frame->data[5U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x07U, frame->data[6U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0xFFU, frame->data[7U]);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_mit_command_compilation_keeps_protocol_scale_independent_of_safe_limits() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitLimits limits{
// 이 코드 줄의 도메인 동작을 수행한다.
        -0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        1.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.25F,
// 이 코드 줄의 도메인 동작을 수행한다.
        2.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.1F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const bldc::command::CommandBuilder builder{limits};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand request{
// 이 코드 줄의 도메인 동작을 수행한다.
        0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        1.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        2.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.1F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.25F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto frame = builder.compile_mit(kNodeId, request);

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(frame.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x85U, frame->data[0U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x1EU, frame->data[1U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x81U, frame->data[2U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0xF0U, frame->data[3U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x10U, frame->data[4U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x05U, frame->data[5U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x18U, frame->data[6U]);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX8(0x09U, frame->data[7U]);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}


// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_mit_command_requires_compile_time_and_runtime_transmit_gates() {
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitLimits limits{
// 이 코드 줄의 도메인 동작을 수행한다.
        -0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.5F,
// 이 코드 줄의 도메인 동작을 수행한다.
        1.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.25F,
// 이 코드 줄의 도메인 동작을 수행한다.
        2.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.1F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const bldc::command::CommandBuilder builder{limits};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::command::MitCommand request{
// 이 코드 줄의 도메인 동작을 수행한다.
        0.1F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 이 코드 줄의 도메인 동작을 수행한다.
        0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

// 컴파일 조건과 기능 게이트를 제어한다.
#if BLDC_ENABLE_CAN_TRANSMIT
// 이 코드 줄의 도메인 동작을 수행한다.
    static_assert(bldc::command::kCanTransmitCompiled,
// 값을 계산하거나 상태를 갱신한다.
                  "The transmit gate build must enable transmission.");

// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor no_heartbeat{100U};
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        builder.build_mit(kNodeId, request, no_heartbeat, true).has_value());

// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor supervisor{100U};
// 값을 계산하거나 상태를 갱신한다.
    supervisor.observe_heartbeat(healthy_heartbeat(), 100U);
// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(100U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        builder.build_mit(kNodeId, request, supervisor, true).has_value());

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.arm());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        builder.build_mit(kNodeId, request, supervisor, false).has_value());
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto authorized = builder.build_mit(kNodeId, request, supervisor, true);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(authorized.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_EQUAL_HEX16(0x0248U, authorized->frame().identifier);

// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(200U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        builder.build_mit(kNodeId, request, supervisor, true).has_value());

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::gim::Heartbeat axis_fault{
// 이 코드 줄의 도메인 동작을 수행한다.
        0x00000001U,
// 이 코드 줄의 도메인 동작을 수행한다.
        bldc::gim::AxisState::closed_loop_control,
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor axis_fault_supervisor{100U};
// 값을 계산하거나 상태를 갱신한다.
    axis_fault_supervisor.observe_heartbeat(axis_fault, 300U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(builder.build_mit(kNodeId, request, axis_fault_supervisor,
// 이 코드 줄의 도메인 동작을 수행한다.
                                        true)
// 값을 계산하거나 상태를 갱신한다.
                          .has_value());

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::gim::Heartbeat flagged_fault{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        bldc::gim::AxisState::closed_loop_control,
// 이 코드 줄의 도메인 동작을 수행한다.
        0x01U,
// 이 코드 줄의 도메인 동작을 수행한다.
        0U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor flagged_fault_supervisor{100U};
// 값을 계산하거나 상태를 갱신한다.
    flagged_fault_supervisor.observe_heartbeat(flagged_fault, 400U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(builder.build_mit(kNodeId, request, flagged_fault_supervisor,
// 이 코드 줄의 도메인 동작을 수행한다.
                                        true)
// 값을 계산하거나 상태를 갱신한다.
                          .has_value());
// 컴파일 조건과 기능 게이트를 제어한다.
#else
// 이 코드 줄의 도메인 동작을 수행한다.
    static_assert(!bldc::command::kCanTransmitCompiled,
// 값을 계산하거나 상태를 갱신한다.
                  "The default build must remain receive-only.");

// 값을 계산하거나 상태를 갱신한다.
    bldc::SafetySupervisor supervisor{100U};
// 값을 계산하거나 상태를 갱신한다.
    supervisor.observe_heartbeat(healthy_heartbeat(), 10U);
// 값을 계산하거나 상태를 갱신한다.
    supervisor.update(10U);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(supervisor.arm());

// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        builder.build_mit(kNodeId, request, supervisor, false).has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(
// 값을 계산하거나 상태를 갱신한다.
        builder.build_mit(kNodeId, request, supervisor, true).has_value());
// 컴파일 조건과 기능 게이트를 제어한다.
#endif
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_raw_frame_cannot_construct_authorized_transmit_token() {
// 이 코드 줄의 도메인 동작을 수행한다.
    static_assert(
// 이 코드 줄의 도메인 동작을 수행한다.
        !std::is_constructible_v<bldc::command::AuthorizedFrame, bldc::CanFrame>,
// 값을 계산하거나 상태를 갱신한다.
        "A raw CAN frame must not bypass command clamping and arming.");
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(true);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_application_composes_typed_telemetry_timestamps_and_stale_state() {
// 값을 계산하거나 상태를 갱신한다.
    bldc::TelemetryApplication application{kNodeId, 100U};
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto heartbeat_identifier =
// 값을 계산하거나 상태를 갱신한다.
        bldc::pack_standard_id(kNodeId, kHeartbeatCommand);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto wrong_identifier =
// 이 코드 줄의 도메인 동작을 수행한다.
        bldc::pack_standard_id(static_cast<std::uint8_t>(kNodeId + 1U),
// 값을 계산하거나 상태를 갱신한다.
                               kHeartbeatCommand);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(heartbeat_identifier.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(wrong_identifier.has_value());

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 8U> heartbeat_payload{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U, 0U, 0U, 0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        0x08U, 0U, 0U, 7U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_TRUE(application.ingest(
// 값을 계산하거나 상태를 갱신한다.
        make_frame(*heartbeat_identifier, heartbeat_payload), 100U));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(application.telemetry().heartbeat.has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 값을 계산하거나 상태를 갱신한다.
        7U, application.telemetry().heartbeat->life_counter);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(application.heartbeat_received_at().has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT32(
// 값을 계산하거나 상태를 갱신한다.
        100U, application.heartbeat_received_at()->raw_ms);

// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FALSE(application.ingest(
// 값을 계산하거나 상태를 갱신한다.
        make_frame(*wrong_identifier, heartbeat_payload), 150U));
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT32(
// 값을 계산하거나 상태를 갱신한다.
        100U, application.heartbeat_received_at()->raw_ms);

// 값을 계산하거나 상태를 갱신한다.
    application.update(199U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::disarmed),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(application.safety().state()));
// 값을 계산하거나 상태를 갱신한다.
    application.update(200U);
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT8(
// 이 코드 줄의 도메인 동작을 수행한다.
        static_cast<std::uint8_t>(bldc::SafetyState::heartbeat_stale),
// 값을 계산하거나 상태를 갱신한다.
        static_cast<std::uint8_t>(application.safety().state()));
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_application_extends_hal_tick_across_rollover() {
// 값을 계산하거나 상태를 갱신한다.
    bldc::TelemetryApplication application{kNodeId, 100U};
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto heartbeat_identifier =
// 값을 계산하거나 상태를 갱신한다.
        bldc::pack_standard_id(kNodeId, kHeartbeatCommand);
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto encoder_identifier =
// 값을 계산하거나 상태를 갱신한다.
        bldc::pack_standard_id(kNodeId, kEncoderEstimatesCommand);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(heartbeat_identifier.has_value());
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(encoder_identifier.has_value());

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 8U> heartbeat_payload{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U, 0U, 0U, 0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        0x08U, 0U, 0U, 1U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const std::array<std::uint8_t, 8U> encoder_payload{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U, 0U, 0U, 0U,
// 이 코드 줄의 도메인 동작을 수행한다.
        0U, 0U, 0U, 0U,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_TRUE(application.ingest(
// 값을 계산하거나 상태를 갱신한다.
        make_frame(*heartbeat_identifier, heartbeat_payload), 0xFFFFFFFAU));
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_TRUE(application.ingest(
// 값을 계산하거나 상태를 갱신한다.
        make_frame(*encoder_identifier, encoder_payload), 3U));

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(application.encoder_received_at().has_value());
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_EQUAL_UINT64(
// 이 코드 줄의 도메인 동작을 수행한다.
        4294967299ULL,
// 값을 계산하거나 상태를 갱신한다.
        application.encoder_received_at()->monotonic_ms);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_state_estimator_fuses_imu_wheel_and_vslam_contracts() {
// 값을 계산하거나 상태를 갱신한다.
    bldc::state::StateEstimator estimator{};

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::state::ImuSample first{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U, 0.0F, 0.0F, 9.81F, 0.0F, 0.0F, 0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::state::ImuSample second{
// 이 코드 줄의 도메인 동작을 수행한다.
        10000U, -1.0F, 0.0F, 9.76F, 0.0F, 0.2F, 0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(estimator.update_imu(first));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(estimator.update_imu(second));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(estimator.state().imu_valid);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(std::isfinite(estimator.state().pitch_rad));
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FLOAT_WITHIN(
// 값을 계산하거나 상태를 갱신한다.
        0.0001F, 0.2F, estimator.state().pitch_rate_rad_per_s);

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::state::WheelOdometry wheel{
// 이 코드 줄의 도메인 동작을 수행한다.
        10000U, 2.0F, 2.2F, 0.1F, 0.4F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(estimator.update_wheel_odometry(wheel));
// 이 코드 줄의 도메인 동작을 수행한다.
    TEST_ASSERT_FLOAT_WITHIN(
// 값을 계산하거나 상태를 갱신한다.
        0.0001F, 0.21F, estimator.state().forward_velocity_m_per_s);

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::state::VslamPose pose{
// 이 코드 줄의 도메인 동작을 수행한다.
        20000U, 1.0F, 2.0F, 0.1F, 0.2F, 0.9F, true,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(estimator.update_vslam(pose));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(estimator.state().vslam_valid);
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(std::isfinite(estimator.state().yaw_rad));
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
void test_state_estimator_rejects_bad_imu_gap_and_vslam_confidence() {
// 값을 계산하거나 상태를 갱신한다.
    bldc::state::StateEstimator estimator{};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::state::ImuSample first{
// 이 코드 줄의 도메인 동작을 수행한다.
        0U, 0.0F, 0.0F, 9.81F, 0.0F, 0.0F, 0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::state::ImuSample delayed{
// 이 코드 줄의 도메인 동작을 수행한다.
        100000U, 0.0F, 0.0F, 9.81F, 0.0F, 0.0F, 0.0F,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };

// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_TRUE(estimator.update_imu(first));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(estimator.update_imu(delayed));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(estimator.state().imu_valid);

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    const bldc::state::VslamPose low_confidence{
// 이 코드 줄의 도메인 동작을 수행한다.
        20000U, 0.0F, 0.0F, 0.0F, 0.0F, 0.1F, true,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    };
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(estimator.update_vslam(low_confidence));
// 값을 계산하거나 상태를 갱신한다.
    TEST_ASSERT_FALSE(estimator.state().vslam_valid);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 2상태/1입력 고정 fixture로 행렬 차원을 테스트 계약에 드러낸다.
bldc::control::TvLqrProblem canonical_tv_lqr_problem() noexcept {
    bldc::control::TvLqrProblem problem{};
    problem.A[0U] = {1.0F, 1.0F};
    problem.A[1U] = {0.0F, 1.0F};
    problem.B[0U] = {0.5F};
    problem.B[1U] = {1.0F};
    problem.Q[0U] = {1.0F, 0.0F};
    problem.Q[1U] = {0.0F, 0.1F};
    problem.R = 0.5F;
    problem.S = problem.Q;
    problem.horizon = 3U;
    return problem;
}

void test_tv_lqr_matches_g001_canonical_gains() {
    const auto result =
        bldc::control::solve_tv_lqr(canonical_tv_lqr_problem());

    TEST_ASSERT_TRUE(result.ok());
    TEST_ASSERT_EQUAL_UINT32(3U,
                             static_cast<std::uint32_t>(
                                 result.solution.horizon));
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 0.588235F, result.solution.K[2U][0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 0.705882F, result.solution.K[2U][1U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 0.638298F, result.solution.K[1U][0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 1.106383F, result.solution.K[1U][1U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 0.595960F, result.solution.K[0U][0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 1.107891F, result.solution.K[0U][1U]);
}

void test_tv_lqr_preserves_terminal_and_backward_cost_indexing() {
    const auto problem = canonical_tv_lqr_problem();
    const auto result = bldc::control::solve_tv_lqr(problem);

    TEST_ASSERT_TRUE(result.ok());
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, problem.S[0U][0U],
                             result.solution.P[problem.horizon][0U][0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, problem.S[1U][1U],
                             result.solution.P[problem.horizon][1U][1U]);
    TEST_ASSERT_TRUE(result.solution.P[0U][0U][0U] >
                     result.solution.P[problem.horizon][0U][0U]);
    TEST_ASSERT_TRUE(result.solution.K[0U][1U] != result.solution.K[2U][1U]);
}

// 잘못된 입력은 정규화 없이 역방향 재귀 전에 실패해야 한다.
void test_tv_lqr_rejects_non_finite_and_invalid_inputs() {
    auto non_finite = canonical_tv_lqr_problem();
    non_finite.A[0U][0U] = std::numeric_limits<float>::quiet_NaN();
    auto result = bldc::control::solve_tv_lqr(non_finite);
    TEST_ASSERT_FALSE(result.ok());
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::control::TvLqrError::non_finite_input),
        static_cast<std::uint8_t>(result.error));

    auto singular_r = canonical_tv_lqr_problem();
    singular_r.R = 0.0F;
    result = bldc::control::solve_tv_lqr(singular_r);
    TEST_ASSERT_FALSE(result.ok());
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::control::TvLqrError::singular_control_cost),
        static_cast<std::uint8_t>(result.error));

    auto non_symmetric = canonical_tv_lqr_problem();
    non_symmetric.Q[0U][1U] = 0.2F;
    result = bldc::control::solve_tv_lqr(non_symmetric);
    TEST_ASSERT_FALSE(result.ok());
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::control::TvLqrError::non_symmetric_cost),
        static_cast<std::uint8_t>(result.error));

    auto invalid_cost = canonical_tv_lqr_problem();
    invalid_cost.Q[0U][0U] = -1.0F;
    result = bldc::control::solve_tv_lqr(invalid_cost);
    TEST_ASSERT_FALSE(result.ok());
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::control::TvLqrError::invalid_cost_matrix),
        static_cast<std::uint8_t>(result.error));

    auto invalid_horizon = canonical_tv_lqr_problem();
    invalid_horizon.horizon = 0U;
    result = bldc::control::solve_tv_lqr(invalid_horizon);
    TEST_ASSERT_FALSE(result.ok());
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::control::TvLqrError::invalid_horizon),
        static_cast<std::uint8_t>(result.error));
    auto generated_invalid = canonical_tv_lqr_problem();
    generated_invalid.A[0U][0U] = 1.0e20F;
    result = bldc::control::solve_tv_lqr(generated_invalid);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::control::TvLqrError::invalid_generated_p),
        static_cast<std::uint8_t>(result.error));
}
void test_tv_lqr_rejects_finite_determinant_overflow() {
    auto input_overflow = canonical_tv_lqr_problem();
    input_overflow.Q = {{{std::numeric_limits<float>::max(), 0.0F},
                         {0.0F, std::numeric_limits<float>::max()}}};
    auto result = bldc::control::solve_tv_lqr(input_overflow);
    TEST_ASSERT_FALSE(result.ok());
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::control::TvLqrError::invalid_cost_matrix),
        static_cast<std::uint8_t>(result.error));

    auto generated_overflow = canonical_tv_lqr_problem();
    generated_overflow.horizon = 1U;
    generated_overflow.A = {{{1.0e10F, 0.0F}, {0.0F, 1.0e10F}}};
    result = bldc::control::solve_tv_lqr(generated_overflow);
    TEST_ASSERT_FALSE(result.ok());
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::control::TvLqrError::invalid_generated_p),
        static_cast<std::uint8_t>(result.error));
}

// 각 P 단계는 유한·대칭이고 제어 단계마다 K 벡터 하나를 저장한다.
void test_tv_lqr_result_is_finite_and_symmetric() {
    const auto result =
        bldc::control::solve_tv_lqr(canonical_tv_lqr_problem());

    TEST_ASSERT_TRUE(result.ok());
    for (std::size_t index = 0U; index <= result.solution.horizon; ++index) {
        TEST_ASSERT_TRUE(std::isfinite(result.solution.P[index][0U][0U]));
        TEST_ASSERT_TRUE(std::isfinite(result.solution.P[index][0U][1U]));
        TEST_ASSERT_TRUE(std::isfinite(result.solution.P[index][1U][0U]));
        TEST_ASSERT_TRUE(std::isfinite(result.solution.P[index][1U][1U]));
        TEST_ASSERT_FLOAT_WITHIN(0.000001F, result.solution.P[index][0U][1U],
                                 result.solution.P[index][1U][0U]);
        if (index < result.solution.horizon) {
            TEST_ASSERT_TRUE(std::isfinite(result.solution.K[index][0U]));
            TEST_ASSERT_TRUE(std::isfinite(result.solution.K[index][1U]));
        }
    }
}
// 2상태/1측정 고정 fixture로 KF 예측·갱신 차원을 드러낸다.
bldc::estimation::LinearKalmanModel canonical_kalman_model() noexcept {
    bldc::estimation::LinearKalmanModel model{};
    model.A = {{{1.0F, 1.0F}, {0.0F, 1.0F}}};
    model.B = {{{0.5F}, {1.0F}}};
    model.C = {1.0F, 0.0F};
    model.Q = {{{0.01F, 0.0F}, {0.0F, 0.1F}}};
    model.R = 0.25F;
    return model;
}

bldc::estimation::KalmanState canonical_kalman_state() noexcept {
    bldc::estimation::KalmanState state{};
    state.P = {{{1.0F, 0.0F}, {0.0F, 1.0F}}};
    return state;
}

void test_kalman_matches_canonical_predict_and_joseph_update() {
    const auto model = canonical_kalman_model();
    auto state = canonical_kalman_state();

    const auto predicted = bldc::estimation::predict(model, state, 1.0F, 1.0F);
    TEST_ASSERT_TRUE(predicted.ok());
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 0.5F, state.x[0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 1.0F, state.x[1U]);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 2.01F, state.P[0U][0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 1.0F, state.P[0U][1U]);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 1.1F, state.P[1U][1U]);

    const auto updated = bldc::estimation::update(model, state, 0.8F);
    TEST_ASSERT_TRUE(updated.ok());
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 0.3F, updated.innovation);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 0.766814F, state.x[0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 1.132743F, state.x[1U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 0.222345F, state.P[0U][0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 0.110619F, state.P[0U][1U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0002F, 0.657522F, state.P[1U][1U]);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, state.P[0U][1U], state.P[1U][0U]);
}

void test_kalman_rejects_invalid_covariance_noise_and_time() {
    const auto model = canonical_kalman_model();
    auto state = canonical_kalman_state();

    auto non_symmetric_q = model;
    non_symmetric_q.Q[0U][1U] = 0.2F;
    auto result = bldc::estimation::predict(non_symmetric_q, state, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_symmetric_covariance),
        static_cast<std::uint8_t>(result.error));
    auto non_finite = model;
    non_finite.A[0U][0U] = std::numeric_limits<float>::quiet_NaN();
    result = bldc::estimation::predict(non_finite, state, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_finite_input),
        static_cast<std::uint8_t>(result.error));

    auto invalid_p = canonical_kalman_state();
    invalid_p.P[0U][0U] = -1.0F;
    result = bldc::estimation::predict(model, invalid_p, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::estimation::KalmanError::invalid_covariance),
        static_cast<std::uint8_t>(result.error));

    auto invalid_r = model;
    invalid_r.R = 0.0F;
    result = bldc::estimation::predict(invalid_r, state, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::invalid_measurement_noise),
        static_cast<std::uint8_t>(result.error));

    result = bldc::estimation::predict(model, state, 1.0F, 0.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::estimation::KalmanError::invalid_dt),
        static_cast<std::uint8_t>(result.error));
    auto mismatched_period = model;
    result = bldc::estimation::predict(mismatched_period, state, 1.0F, 0.5F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::sample_period_mismatch),
        static_cast<std::uint8_t>(result.error));
    auto invalid_period = model;
    invalid_period.sample_period_s =
        std::numeric_limits<float>::quiet_NaN();
    result = bldc::estimation::predict(invalid_period, state, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::estimation::KalmanError::invalid_dt),
        static_cast<std::uint8_t>(result.error));

    auto timestamped = canonical_kalman_state();
    result = bldc::estimation::predict_at(model, timestamped, 1.0F, 1.0F, 10U);
    TEST_ASSERT_TRUE(result.ok());
    result = bldc::estimation::update_at(model, timestamped, 0.8F, 10U);
    TEST_ASSERT_TRUE(result.ok());
    result = bldc::estimation::predict_at(model, timestamped, 1.0F, 1.0F, 10U);
    result = bldc::estimation::predict_at(model, timestamped, 1.0F, 1.0F, 10U);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_monotonic_timestamp),
        static_cast<std::uint8_t>(result.error));
    auto out_of_order = timestamped;
    result = bldc::estimation::predict_at(model, out_of_order, 1.0F, 1.0F, 9U);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_monotonic_timestamp),
        static_cast<std::uint8_t>(result.error));
    result = bldc::estimation::predict_at(model, timestamped, 1.0F, 1.0F, 11U);
    TEST_ASSERT_TRUE(result.ok());
    result = bldc::estimation::update_at(model, timestamped, 0.8F, 11U);
    TEST_ASSERT_TRUE(result.ok());
    result = bldc::estimation::update_at(model, timestamped, 0.8F, 11U);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_monotonic_timestamp),
        static_cast<std::uint8_t>(result.error));
    result = bldc::estimation::update_at(model, timestamped, 0.8F, 10U);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_monotonic_timestamp),
        static_cast<std::uint8_t>(result.error));
}
void test_kalman_rejects_mixed_timestamp_update_without_consuming_epoch() {
    const auto model = canonical_kalman_model();
    auto state = canonical_kalman_state();

    auto result = bldc::estimation::predict_at(model, state, 1.0F, 1.0F, 42U);
    TEST_ASSERT_TRUE(result.ok());
    const auto predicted = state;

    result = bldc::estimation::predict(model, state, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::timestamp_required),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == predicted.x);
    TEST_ASSERT_TRUE(state.P == predicted.P);
    TEST_ASSERT_EQUAL_UINT64(state.timestamp, predicted.timestamp);
    TEST_ASSERT_TRUE(state.has_timestamp == predicted.has_timestamp);
    TEST_ASSERT_TRUE(state.timestamp_phase == predicted.timestamp_phase);

    result = bldc::estimation::update(model, state, 0.8F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::timestamp_required),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == predicted.x);
    TEST_ASSERT_TRUE(state.P == predicted.P);
    TEST_ASSERT_EQUAL_UINT64(state.timestamp, predicted.timestamp);
    TEST_ASSERT_TRUE(state.timestamp_phase == predicted.timestamp_phase);

    result = bldc::estimation::update_at(model, state, 0.8F, 42U);
    TEST_ASSERT_TRUE(result.ok());
    const auto updated = state;

    result = bldc::estimation::update_at(model, state, 0.8F, 42U);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_monotonic_timestamp),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == updated.x);
    TEST_ASSERT_TRUE(state.P == updated.P);
    TEST_ASSERT_EQUAL_UINT64(state.timestamp, updated.timestamp);
    TEST_ASSERT_TRUE(state.timestamp_phase == updated.timestamp_phase);
}

bool ekf_transition_fixture(const bldc::estimation::StateVector& x, float input,
                            float dt,
                            bldc::estimation::StateVector* next) noexcept {
    *next = {x[0U] + dt * x[1U] + 0.5F * input, x[1U] + input};
    return true;
}

bool ekf_transition_jacobian_fixture(const bldc::estimation::StateVector&, float,
                                     float,
                                     bldc::estimation::StateMatrix* jacobian) noexcept {
    *jacobian = {{{1.0F, 1.0F}, {0.0F, 1.0F}}};
    return true;
}

bool ekf_measurement_fixture(const bldc::estimation::StateVector& x,
                             float* measurement) noexcept {
    *measurement = x[0U];
    return true;
}

bool ekf_measurement_jacobian_fixture(
    const bldc::estimation::StateVector&,
    bldc::estimation::MeasurementVector* jacobian) noexcept {
    *jacobian = {1.0F, 0.0F};
    return true;
}
bool ekf_nonlinear_measurement_fixture(
    const bldc::estimation::StateVector& x, float* measurement) noexcept {
    *measurement = x[0U] * x[0U] + 1.0F;
    return true;
}

bool ekf_nonlinear_measurement_jacobian_fixture(
    const bldc::estimation::StateVector& x,
    bldc::estimation::MeasurementVector* jacobian) noexcept {
    *jacobian = {2.0F * x[0U], 0.0F};
    return true;
}
void test_ekf_enforces_fixed_sample_period() {
    bldc::estimation::EkfModel model{};
    model.transition = ekf_transition_fixture;
    model.transition_jacobian = ekf_transition_jacobian_fixture;
    model.Q = {{{0.01F, 0.0F}, {0.0F, 0.1F}}};
    model.R = 0.25F;
    auto state = canonical_kalman_state();

    const auto result =
        bldc::estimation::ekf_predict(model, state, 1.0F, 0.5F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::sample_period_mismatch),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == canonical_kalman_state().x);
    TEST_ASSERT_TRUE(state.P == canonical_kalman_state().P);
}

void test_ekf_timestamp_predict_update_lifecycle() {
    bldc::estimation::EkfModel model{};
    model.transition = ekf_transition_fixture;
    model.transition_jacobian = ekf_transition_jacobian_fixture;
    model.measurement = ekf_measurement_fixture;
    model.measurement_jacobian = ekf_measurement_jacobian_fixture;
    model.Q = {{{0.01F, 0.0F}, {0.0F, 0.1F}}};
    model.R = 0.25F;
    auto state = canonical_kalman_state();

    auto result =
        bldc::estimation::ekf_predict_at(model, state, 1.0F, 1.0F, 42U);
    TEST_ASSERT_TRUE(result.ok());
    const auto predicted = state;
    result = bldc::estimation::ekf_update_at(model, state, 1.0F, 42U);
    TEST_ASSERT_TRUE(result.ok());
    const auto updated = state;

    result = bldc::estimation::ekf_predict_at(model, state, 1.0F, 1.0F, 42U);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_monotonic_timestamp),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == updated.x);
    TEST_ASSERT_TRUE(state.P == updated.P);
    TEST_ASSERT_EQUAL_UINT64(state.timestamp, updated.timestamp);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(state.timestamp_phase),
        static_cast<std::uint8_t>(updated.timestamp_phase));

    result = bldc::estimation::ekf_update_at(model, state, 1.0F, 41U);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_monotonic_timestamp),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == updated.x);
    TEST_ASSERT_TRUE(state.P == updated.P);

    result = bldc::estimation::ekf_update_at(model, state, 1.0F, 42U);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_monotonic_timestamp),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == updated.x);
    TEST_ASSERT_TRUE(state.P == updated.P);
    TEST_ASSERT_TRUE(predicted.timestamp_phase ==
                     bldc::estimation::TimestampPhase::predicted);
}

void test_ekf_uses_nonlinear_measurement_prediction_for_innovation() {
    bldc::estimation::EkfModel model{};
    model.measurement = ekf_nonlinear_measurement_fixture;
    model.measurement_jacobian = ekf_nonlinear_measurement_jacobian_fixture;
    model.Q = {{{0.01F, 0.0F}, {0.0F, 0.1F}}};
    model.R = 1.0F;

    auto state = canonical_kalman_state();
    state.x = {2.0F, -1.0F};
    const auto result = bldc::estimation::ekf_update(model, state, 6.0F);

    TEST_ASSERT_TRUE(result.ok());
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 1.0F, result.innovation);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, 17.0F,
                             result.innovation_variance);
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 2.235294F, state.x[0U]);
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, -1.0F, state.x[1U]);
    TEST_ASSERT_FLOAT_WITHIN(0.0001F, 0.0588235F, state.P[0U][0U]);
}

void test_ekf_wraps_configured_angle_innovation_at_boundary() {
    constexpr float kPi = 3.14159265358979323846F;
    bldc::estimation::EkfModel model{};
    model.measurement = ekf_measurement_fixture;
    model.measurement_jacobian = ekf_measurement_jacobian_fixture;
    model.Q = {{{0.01F, 0.0F}, {0.0F, 0.1F}}};
    model.R = 0.25F;
    model.wrap_innovation = true;

    auto state = canonical_kalman_state();
    state.x = {kPi - 0.01F, 0.0F};
    const auto result =
        bldc::estimation::ekf_update(model, state, -kPi + 0.01F);

    TEST_ASSERT_TRUE(result.ok());
    TEST_ASSERT_FLOAT_WITHIN(0.00001F, 0.02F, result.innovation);
    TEST_ASSERT_TRUE(state.x[0U] > kPi - 0.01F);
}
void test_ekf_reports_callback_failures_and_non_finite_outputs() {
    bldc::estimation::EkfModel model{};
    model.transition = ekf_transition_fixture;
    model.transition_jacobian = ekf_transition_jacobian_fixture;
    model.measurement = ekf_measurement_fixture;
    model.measurement_jacobian = ekf_measurement_jacobian_fixture;
    model.Q = {{{0.01F, 0.0F}, {0.0F, 0.1F}}};
    model.R = 0.25F;
    auto state = canonical_kalman_state();
    const auto initial = state;

    model.transition = nullptr;
    auto result = bldc::estimation::ekf_predict(model, state, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::estimation::KalmanError::callback_failure),
        static_cast<std::uint8_t>(result.error));

    model.transition = [](const bldc::estimation::StateVector&, float, float,
                          bldc::estimation::StateVector*) noexcept {
        return false;
    };
    result = bldc::estimation::ekf_predict(model, state, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::estimation::KalmanError::callback_failure),
        static_cast<std::uint8_t>(result.error));

    model.transition = [](const bldc::estimation::StateVector&, float, float,
                          bldc::estimation::StateVector* next) noexcept {
        *next = {std::numeric_limits<float>::quiet_NaN(), 0.0F};
        return true;
    };
    result = bldc::estimation::ekf_predict(model, state, 1.0F, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_finite_callback_result),
        static_cast<std::uint8_t>(result.error));
    model.measurement =
        [](const bldc::estimation::StateVector&, float*) noexcept {
            return false;
        };
    result = bldc::estimation::ekf_update(model, state, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::estimation::KalmanError::callback_failure),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == initial.x);
    TEST_ASSERT_TRUE(state.P == initial.P);

    model.measurement =
        [](const bldc::estimation::StateVector&,
           float* measurement) noexcept {
            *measurement = std::numeric_limits<float>::infinity();
            return true;
        };
    result = bldc::estimation::ekf_update(model, state, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_finite_callback_result),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == initial.x);
    TEST_ASSERT_TRUE(state.P == initial.P);

    model.measurement = ekf_measurement_fixture;
    model.measurement_jacobian =
        [](const bldc::estimation::StateVector&,
           bldc::estimation::MeasurementVector*) noexcept {
            return false;
        };
    result = bldc::estimation::ekf_update(model, state, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(bldc::estimation::KalmanError::callback_failure),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == initial.x);
    TEST_ASSERT_TRUE(state.P == initial.P);

    model.measurement_jacobian =
        [](const bldc::estimation::StateVector&,
           bldc::estimation::MeasurementVector* jacobian) noexcept {
            *jacobian = {std::numeric_limits<float>::quiet_NaN(), 0.0F};
            return true;
        };
    result = bldc::estimation::ekf_update(model, state, 1.0F);
    TEST_ASSERT_EQUAL_UINT8(
        static_cast<std::uint8_t>(
            bldc::estimation::KalmanError::non_finite_callback_result),
        static_cast<std::uint8_t>(result.error));
    TEST_ASSERT_TRUE(state.x == initial.x);
    TEST_ASSERT_TRUE(state.P == initial.P);
}

void test_angle_innovation_wraps_pi_boundary() {
    constexpr float kPi = 3.14159265358979323846F;
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, -kPi,
                             bldc::estimation::wrap_angle(kPi));
    TEST_ASSERT_FLOAT_WITHIN(0.000001F, -kPi,
                             bldc::estimation::wrap_angle(-kPi));
    TEST_ASSERT_FLOAT_WITHIN(
        0.000001F, -kPi,
        bldc::estimation::angle_innovation(kPi, 0.0F));
}
// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
int main(int, char**) {
// 값을 계산하거나 상태를 갱신한다.
    UNITY_BEGIN();
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_standard_id_round_trip_uses_candidate_can_simple_layout);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_standard_id_rejects_out_of_range_node_command_and_identifier);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_little_endian_u32_and_f32_decoding_matches_fixtures);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_heartbeat_decoder_returns_typed_candidate_telemetry);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_encoder_decoder_returns_typed_little_endian_float_telemetry);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_protocol_decoders_reject_extended_rtr_wrong_dlc_and_wrong_node);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_safety_supervisor_uses_strict_heartbeat_freshness_boundary);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_safety_supervisor_requires_runtime_rearm_after_disarm_or_timeout);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_safety_supervisor_latches_faults_until_reconstruction);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_safety_supervisor_ignores_trajectory_done_but_latches_fault_flags);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_command_sanitization_clamps_finite_values_to_conservative_limits);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_command_sanitization_rejects_non_finite_requests);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_command_builder_rejects_limits_outside_fixed_wire_ranges);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_mit_command_requires_compile_time_and_runtime_transmit_gates);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_mit_command_compilation_uses_candidate_identifier_and_eight_byte_layout);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_mit_command_compilation_keeps_protocol_scale_independent_of_safe_limits);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_raw_frame_cannot_construct_authorized_transmit_token);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_application_composes_typed_telemetry_timestamps_and_stale_state);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_application_extends_hal_tick_across_rollover);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_state_estimator_fuses_imu_wheel_and_vslam_contracts);
// 값을 계산하거나 상태를 갱신한다.
    RUN_TEST(test_state_estimator_rejects_bad_imu_gap_and_vslam_confidence);
// terminal P[N]과 K[N-1]..K[0]를 분리한 역방향 재귀를 검증한다.
    RUN_TEST(test_tv_lqr_matches_g001_canonical_gains);
    RUN_TEST(test_tv_lqr_preserves_terminal_and_backward_cost_indexing);
    RUN_TEST(test_tv_lqr_rejects_non_finite_and_invalid_inputs);
    RUN_TEST(test_tv_lqr_rejects_finite_determinant_overflow);
    RUN_TEST(test_tv_lqr_result_is_finite_and_symmetric);
    RUN_TEST(test_kalman_matches_canonical_predict_and_joseph_update);
    RUN_TEST(test_kalman_rejects_invalid_covariance_noise_and_time);
    RUN_TEST(test_kalman_rejects_mixed_timestamp_update_without_consuming_epoch);
    RUN_TEST(test_ekf_reports_callback_failures_and_non_finite_outputs);
    RUN_TEST(test_ekf_enforces_fixed_sample_period);
    RUN_TEST(test_ekf_timestamp_predict_update_lifecycle);
    RUN_TEST(test_ekf_uses_nonlinear_measurement_prediction_for_innovation);
    RUN_TEST(test_ekf_wraps_configured_angle_innovation_at_boundary);
    RUN_TEST(test_angle_innovation_wraps_pi_boundary);
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return UNITY_END();
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
