// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>   // millisecond timestamp에 고정 크기 정수를 사용한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <iostream>  // safety state 변화를 terminal에서 관찰한다.

// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/safety_supervisor.hpp"  // heartbeat 기반 safety state machine이다.

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace {

// enum 값을 학습용 숫자로 출력하는 작은 helper다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
unsigned state_number(const bldc::SafetyState state) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return static_cast<unsigned>(state);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace

// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
int main() {
  // heartbeat가 250 ms 이상 새로 오지 않으면 stale로 판정한다.
// 값을 계산하거나 상태를 갱신한다.
  bldc::SafetySupervisor safety{250U};

  // error와 fault flag가 없는 heartbeat fixture다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const bldc::gim::Heartbeat healthy{
// 이 코드 줄의 도메인 동작을 수행한다.
      0U,                                             // axis_error
// 이 코드 줄의 도메인 동작을 수행한다.
      bldc::gim::AxisState::closed_loop_control,     // axis_state
// 이 코드 줄의 도메인 동작을 수행한다.
      0U,                                             // flags
// 이 코드 줄의 도메인 동작을 수행한다.
      1U,                                             // life_counter
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // heartbeat를 받기 전에는 arm 요청이 실패해야 한다.
// 값을 계산하거나 상태를 갱신한다.
  std::cout << "arm before heartbeat=" << safety.arm() << '\n';

  // HAL_GetTick()==100 ms 시점에 healthy heartbeat를 받았다고 기록한다.
// 값을 계산하거나 상태를 갱신한다.
  safety.observe_heartbeat(healthy, 100U);

  // 현재 시각을 넣어 freshness state를 갱신한다.
// 값을 계산하거나 상태를 갱신한다.
  safety.update(100U);

  // heartbeat만으로 자동 arm하지 않고 operator가 명시적으로 요청한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const bool armed = safety.arm();

  // 성공적으로 arm됐는지와 현재 상태를 확인한다.
// 이 코드 줄의 도메인 동작을 수행한다.
  std::cout << "arm after heartbeat=" << armed
// 값을 계산하거나 상태를 갱신한다.
            << " state=" << state_number(safety.state()) << '\n';

  // 249 ms가 지난 시점은 250 ms timeout 경계 직전이므로 아직 fresh다.
// 값을 계산하거나 상태를 갱신한다.
  safety.update(349U);

  // fresh하고 armed일 때만 command가 허용된다.
// 값을 계산하거나 상태를 갱신한다.
  std::cout << "at 349ms permits=" << safety.permits_command() << '\n';

  // 정확히 250 ms가 지난 경계부터 stale로 판정한다.
// 값을 계산하거나 상태를 갱신한다.
  safety.update(350U);

  // stale 전이와 동시에 arm이 해제되어 command가 금지된다.
// 함수의 입력과 동작 계약을 선언한다.
  std::cout << "at 350ms permits=" << safety.permits_command()
// 값을 계산하거나 상태를 갱신한다.
            << " state=" << state_number(safety.state()) << '\n';

// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return 0;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
