// 고정 크기 정수로 timestamp와 센서 필드를 표현한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>
// 결과를 terminal에서 확인하기 위해 iostream을 사용한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <iostream>

// IMU와 VSLAM을 합치는 pure C++ estimator를 가져온다.
// 필요한 헤더와 타입 정의를 포함한다.
#include "bldc/state_estimator.hpp"

// 교육용 프로그램의 시작점이다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
int main() {
  // estimator의 기본 complementary filter 설정을 사용한다.
// 값을 계산하거나 상태를 갱신한다.
  bldc::state::StateEstimator estimator{};

  // 첫 IMU sample은 초기 pitch와 timestamp 기준점으로 사용된다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const bldc::state::ImuSample first_imu{
// 이 코드 줄의 도메인 동작을 수행한다.
      0U,       // timestamp [us]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.0F,     // accel x [m/s^2]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.0F,     // accel y [m/s^2]
// 이 코드 줄의 도메인 동작을 수행한다.
      9.81F,    // accel z [m/s^2]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.0F,     // gyro x [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.0F,     // gyro y [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.0F,     // gyro z [rad/s]
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // 첫 sample을 넣어 sensor clock과 attitude 초기값을 만든다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const bool first_ok = estimator.update_imu(first_imu);

  // 10 ms 뒤의 두 번째 sample에서 pitch rate를 0.2 rad/s로 가정한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const bldc::state::ImuSample second_imu{
// 이 코드 줄의 도메인 동작을 수행한다.
      10000U,   // timestamp [us]
// 이 코드 줄의 도메인 동작을 수행한다.
      -1.96F,   // pitch가 약간 nose-up인 가속도 예시
// 이 코드 줄의 도메인 동작을 수행한다.
      0.0F,     // accel y [m/s^2]
// 이 코드 줄의 도메인 동작을 수행한다.
      9.61F,    // accel z [m/s^2]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.0F,     // gyro x [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.2F,     // gyro y [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.0F,     // gyro z [rad/s]
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // 두 번째 sample로 gyro 적분과 accelerometer 보정을 실행한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const bool second_ok = estimator.update_imu(second_imu);

  // wheel radius 0.1 m와 좌우 wheel 속도를 사용해 전진 속도를 계산한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const bldc::state::WheelOdometry wheel{
// 이 코드 줄의 도메인 동작을 수행한다.
      10000U,   // timestamp [us]
// 이 코드 줄의 도메인 동작을 수행한다.
      2.0F,     // left wheel speed [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      2.2F,     // right wheel speed [rad/s]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.1F,     // wheel radius [m]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.4F,     // track width [m]
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // 휠 오도메트리는 balance state의 forward velocity를 갱신한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const bool wheel_ok = estimator.update_wheel_odometry(wheel);

  // D435iF VSLAM backend가 출력했다고 가정한 low-rate pose다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  const bldc::state::VslamPose pose{
// 이 코드 줄의 도메인 동작을 수행한다.
      20000U,   // timestamp [us]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.02F,    // world x [m]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.00F,    // world y [m]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.01F,    // world z [m]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.01F,    // yaw [rad]
// 이 코드 줄의 도메인 동작을 수행한다.
      0.90F,    // tracking confidence
// 이 코드 줄의 도메인 동작을 수행한다.
      true,     // valid pose
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  };

  // VSLAM pose는 pitch control을 대체하지 않고 전역 pose drift만 보정한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const bool pose_ok = estimator.update_vslam(pose);

  // 현재 상태를 읽어 학습자가 각 센서 경계와 결과를 확인한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const auto& state = estimator.state();

  // 각 update가 수락되었는지 출력한다.
// 이 코드 줄의 도메인 동작을 수행한다.
  std::cout << "imu=" << first_ok << "," << second_ok
// 값을 계산하거나 상태를 갱신한다.
            << " wheel=" << wheel_ok << " vslam=" << pose_ok << '\n';

  // pitch와 pitch rate는 balance controller의 빠른 입력이다.
// 이 코드 줄의 도메인 동작을 수행한다.
  std::cout << "pitch=" << state.pitch_rad
// 값을 계산하거나 상태를 갱신한다.
            << " pitch_rate=" << state.pitch_rate_rad_per_s << '\n';

  // forward velocity는 wheel encoder에서 나온 차체 속도다.
// 값을 계산하거나 상태를 갱신한다.
  std::cout << "forward_velocity=" << state.forward_velocity_m_per_s << '\n';

  // position과 yaw는 VSLAM이 느리게 보정하는 전역 상태다.
// 이 코드 줄의 도메인 동작을 수행한다.
  std::cout << "position=" << state.position_x_m << "," << state.position_y_m
// 값을 계산하거나 상태를 갱신한다.
            << "," << state.position_z_m << " yaw=" << state.yaw_rad << '\n';

  // host tutorial은 command를 송신하지 않고 성공적으로 종료한다.
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
  return 0;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
}
