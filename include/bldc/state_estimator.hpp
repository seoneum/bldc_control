// 이 코드 줄의 도메인 동작을 수행한다.
#pragma once

// atan2, cos, isfinite, remainder를 사용하기 위해 수학 헤더를 가져온다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <cmath>
// uint32_t와 uint64_t로 센서 timestamp의 크기를 명시한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <cstdint>
// numeric_limits로 유한한 float 입력을 검사한다.
// 필요한 헤더와 타입 정의를 포함한다.
#include <limits>

// 관련 심볼을 논리적인 네임스페이스로 묶는다.
namespace bldc::state {

// 호스트에서 전달되거나 STM32에서 받은 IMU 한 샘플을 표현한다.
// 재사용할 자료 또는 동작 계약을 선언한다.
struct ImuSample {
  // 센서가 샘플을 만든 monotonic timestamp다.
// 값을 계산하거나 상태를 갱신한다.
  std::uint64_t timestamp_us{0U};
  // body x축 가속도이며 단위는 m/s^2다.
// 값을 계산하거나 상태를 갱신한다.
  float accel_x_mps2{0.0F};
  // body y축 가속도이며 단위는 m/s^2다.
// 값을 계산하거나 상태를 갱신한다.
  float accel_y_mps2{0.0F};
  // body z축 가속도이며 단위는 m/s^2다.
// 값을 계산하거나 상태를 갱신한다.
  float accel_z_mps2{0.0F};
  // body x축 각속도이며 단위는 rad/s다.
// 값을 계산하거나 상태를 갱신한다.
  float gyro_x_rad_per_s{0.0F};
  // body y축 각속도이며 pitch rate로 사용할 후보 축이다.
// 값을 계산하거나 상태를 갱신한다.
  float gyro_y_rad_per_s{0.0F};
  // body z축 각속도이며 단위는 rad/s다.
// 값을 계산하거나 상태를 갱신한다.
  float gyro_z_rad_per_s{0.0F};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 양쪽 휠 엔코더에서 계산한 차체 속도 입력을 표현한다.
// 재사용할 자료 또는 동작 계약을 선언한다.
struct WheelOdometry {
  // 샘플을 만든 monotonic timestamp다.
// 값을 계산하거나 상태를 갱신한다.
  std::uint64_t timestamp_us{0U};
  // 왼쪽 휠 속도이며 단위는 rad/s다.
// 값을 계산하거나 상태를 갱신한다.
  float left_wheel_rad_per_s{0.0F};
  // 오른쪽 휠 속도이며 단위는 rad/s다.
// 값을 계산하거나 상태를 갱신한다.
  float right_wheel_rad_per_s{0.0F};
  // 바퀴 반지름이며 단위는 m다.
// 값을 계산하거나 상태를 갱신한다.
  float wheel_radius_m{0.0F};
  // 좌우 바퀴 간격이며 단위는 m다.
// 값을 계산하거나 상태를 갱신한다.
  float track_width_m{0.0F};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// D435i 계열 VSLAM backend가 출력하는 저주기 pose 보정 입력이다.
// 재사용할 자료 또는 동작 계약을 선언한다.
struct VslamPose {
  // VSLAM pose의 monotonic timestamp다.
// 값을 계산하거나 상태를 갱신한다.
  std::uint64_t timestamp_us{0U};
  // world frame x 위치이며 단위는 m다.
// 값을 계산하거나 상태를 갱신한다.
  float position_x_m{0.0F};
  // world frame y 위치이며 단위는 m다.
// 값을 계산하거나 상태를 갱신한다.
  float position_y_m{0.0F};
  // world frame z 위치이며 단위는 m다.
// 값을 계산하거나 상태를 갱신한다.
  float position_z_m{0.0F};
  // 수평면 yaw이며 단위는 rad다.
// 값을 계산하거나 상태를 갱신한다.
  float yaw_rad{0.0F};
  // backend가 보고한 tracking confidence다.
// 값을 계산하거나 상태를 갱신한다.
  float tracking_confidence{0.0F};
  // false이면 dropout/추적 실패이므로 pose를 사용하지 않는다.
// 값을 계산하거나 상태를 갱신한다.
  bool valid{false};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 제어기가 사용하는 융합 상태를 표현한다.
// 재사용할 자료 또는 동작 계약을 선언한다.
struct FusedState {
  // 마지막으로 성공한 IMU timestamp다.
// 값을 계산하거나 상태를 갱신한다.
  std::uint64_t imu_timestamp_us{0U};
  // 마지막으로 성공한 VSLAM timestamp다.
// 값을 계산하거나 상태를 갱신한다.
  std::uint64_t vslam_timestamp_us{0U};
  // IMU 입력이 현재 유효한지 나타낸다.
// 값을 계산하거나 상태를 갱신한다.
  bool imu_valid{false};
  // VSLAM 보정이 현재 유효한지 나타낸다.
// 값을 계산하거나 상태를 갱신한다.
  bool vslam_valid{false};
  // complementary filter가 계산한 roll이며 단위는 rad다.
// 값을 계산하거나 상태를 갱신한다.
  float roll_rad{0.0F};
  // complementary filter가 계산한 pitch이며 단위는 rad다.
// 값을 계산하거나 상태를 갱신한다.
  float pitch_rad{0.0F};
  // IMU gyro y축에서 가져온 pitch rate다.
// 값을 계산하거나 상태를 갱신한다.
  float pitch_rate_rad_per_s{0.0F};
  // IMU gyro z축에서 적분한 yaw 추정값이다.
// 값을 계산하거나 상태를 갱신한다.
  float yaw_rad{0.0F};
  // 휠 오도메트리로 계산한 전진 속도며 단위는 m/s다.
// 값을 계산하거나 상태를 갱신한다.
  float forward_velocity_m_per_s{0.0F};
  // VSLAM world frame x 위치며 단위는 m다.
// 값을 계산하거나 상태를 갱신한다.
  float position_x_m{0.0F};
  // VSLAM world frame y 위치며 단위는 m다.
// 값을 계산하거나 상태를 갱신한다.
  float position_y_m{0.0F};
  // VSLAM world frame z 위치며 단위는 m다.
// 값을 계산하거나 상태를 갱신한다.
  float position_z_m{0.0F};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// estimator의 축 convention과 필터 강도를 모아 둔다.
// 재사용할 자료 또는 동작 계약을 선언한다.
struct EstimatorConfig {
  // gyro 적분과 accelerometer pitch의 혼합 비율이다.
// 값을 계산하거나 상태를 갱신한다.
  float complementary_gyro_weight{0.98F};
  // VSLAM 위치를 기존 상태에 반영하는 비율이다.
// 값을 계산하거나 상태를 갱신한다.
  float vslam_position_weight{0.10F};
  // VSLAM yaw를 기존 상태에 반영하는 비율이다.
// 값을 계산하거나 상태를 갱신한다.
  float vslam_yaw_weight{0.10F};
  // 허용하는 최대 IMU sample interval이며 단위는 s다.
// 값을 계산하거나 상태를 갱신한다.
  float maximum_dt_s{0.05F};
  // 이 confidence보다 낮은 VSLAM pose는 버린다.
// 값을 계산하거나 상태를 갱신한다.
  float minimum_vslam_confidence{0.50F};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// IMU와 저주기 VSLAM을 제어용 상태로 변환하는 pure C++17 클래스다.
// 재사용할 자료 또는 동작 계약을 선언한다.
class StateEstimator {
// 멤버의 접근 범위를 지정한다.
 public:
  // 필터 configuration을 복사해 estimator를 초기화한다.
// 함수의 입력과 동작 계약을 선언한다.
  explicit StateEstimator(const EstimatorConfig& config = EstimatorConfig{}) noexcept
// 이 코드 줄의 도메인 동작을 수행한다.
      : config_(config) {}

  // 센서 timestamp와 누적 상태를 초기화한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  void reset() noexcept {
// 값을 계산하거나 상태를 갱신한다.
    state_ = FusedState{};
// 값을 계산하거나 상태를 갱신한다.
    last_imu_timestamp_us_ = 0U;
// 값을 계산하거나 상태를 갱신한다.
    initialized_ = false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // 한 IMU 샘플을 받아 attitude와 angular-rate를 갱신한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  bool update_imu(const ImuSample& sample) noexcept {
    // accelerometer와 gyro 모두 유한한 값인지 먼저 확인한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!finite_imu(sample)) {
// 값을 계산하거나 상태를 갱신한다.
      state_.imu_valid = false;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // 첫 샘플은 적분할 dt가 없으므로 기준 timestamp만 저장한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!initialized_) {
// 값을 계산하거나 상태를 갱신한다.
      initialized_ = true;
// 값을 계산하거나 상태를 갱신한다.
      last_imu_timestamp_us_ = sample.timestamp_us;
// 값을 계산하거나 상태를 갱신한다.
      state_.imu_timestamp_us = sample.timestamp_us;
// 값을 계산하거나 상태를 갱신한다.
      state_.imu_valid = true;
// 값을 계산하거나 상태를 갱신한다.
      state_.pitch_rad = accelerometer_pitch(sample);
// 값을 계산하거나 상태를 갱신한다.
      state_.roll_rad = accelerometer_roll(sample);
// 값을 계산하거나 상태를 갱신한다.
      state_.yaw_rad = 0.0F;
// 값을 계산하거나 상태를 갱신한다.
      state_.pitch_rate_rad_per_s = sample.gyro_y_rad_per_s;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // unsigned timestamp 차이를 double로 바꿔 microsecond rollover를 피한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const auto elapsed_us = sample.timestamp_us - last_imu_timestamp_us_;
    // timestamp가 뒤로 가거나 너무 긴 gap이면 이번 적분을 거부한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const float dt_s = static_cast<float>(elapsed_us) * 1.0e-6F;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    if (!(dt_s > 0.0F) || dt_s > config_.maximum_dt_s) {
// 값을 계산하거나 상태를 갱신한다.
      state_.imu_valid = false;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }

    // gyro 적분으로 빠른 pitch 예측값을 만든다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const float gyro_pitch = state_.pitch_rad +
// 값을 계산하거나 상태를 갱신한다.
                             sample.gyro_y_rad_per_s * dt_s;
    // 가속도 기반 pitch는 장기 drift를 줄이는 느린 기준값으로 사용한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const float accel_pitch = accelerometer_pitch(sample);
    // 두 추정값을 complementary filter로 결합한다.
// 이 코드 줄의 도메인 동작을 수행한다.
    state_.pitch_rad = config_.complementary_gyro_weight * gyro_pitch +
// 값을 계산하거나 상태를 갱신한다.
                       (1.0F - config_.complementary_gyro_weight) * accel_pitch;
    // roll도 같은 방식으로 계산해 상태 디버깅에 사용한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const float gyro_roll = state_.roll_rad + sample.gyro_x_rad_per_s * dt_s;
    // accelerometer roll을 느린 기준값으로 사용한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const float accel_roll = accelerometer_roll(sample);
    // roll 예측과 roll 기준값을 혼합한다.
// 이 코드 줄의 도메인 동작을 수행한다.
    state_.roll_rad = config_.complementary_gyro_weight * gyro_roll +
// 값을 계산하거나 상태를 갱신한다.
                      (1.0F - config_.complementary_gyro_weight) * accel_roll;
    // gyro y축은 balance controller가 사용할 pitch rate로 보존한다.
// 값을 계산하거나 상태를 갱신한다.
    state_.pitch_rate_rad_per_s = sample.gyro_y_rad_per_s;
    // yaw는 자기장/visual correction 없이는 gyro 적분 drift가 생긴다.
// 값을 계산하거나 상태를 갱신한다.
    state_.yaw_rad = wrap_angle(state_.yaw_rad + sample.gyro_z_rad_per_s * dt_s);
    // 다음 샘플을 위한 timestamp를 저장한다.
// 값을 계산하거나 상태를 갱신한다.
    last_imu_timestamp_us_ = sample.timestamp_us;
    // 마지막 유효 IMU timestamp를 공개 상태에 복사한다.
// 값을 계산하거나 상태를 갱신한다.
    state_.imu_timestamp_us = sample.timestamp_us;
    // 이 시점의 IMU 상태를 유효하다고 표시한다.
// 값을 계산하거나 상태를 갱신한다.
    state_.imu_valid = true;
    // 유효한 IMU update가 완료되었음을 반환한다.
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // 양쪽 휠 속도를 차체 전진 속도로 변환한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  bool update_wheel_odometry(const WheelOdometry& odometry) noexcept {
    // 기구 파라미터와 센서값의 유한성을 함께 확인한다.
// 입력과 상태에 따라 실행 경로를 선택한다.
    if (!std::isfinite(odometry.left_wheel_rad_per_s) ||
// 이 코드 줄의 도메인 동작을 수행한다.
        !std::isfinite(odometry.right_wheel_rad_per_s) ||
// 이 코드 줄의 도메인 동작을 수행한다.
        !std::isfinite(odometry.wheel_radius_m) ||
// 이 코드 줄의 도메인 동작을 수행한다.
        !std::isfinite(odometry.track_width_m) ||
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
        odometry.wheel_radius_m <= 0.0F || odometry.track_width_m <= 0.0F) {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
    // 좌우 wheel linear velocity의 평균을 전진 속도로 사용한다.
// 이 코드 줄의 도메인 동작을 수행한다.
    state_.forward_velocity_m_per_s =
// 이 코드 줄의 도메인 동작을 수행한다.
        0.5F * odometry.wheel_radius_m *
// 값을 계산하거나 상태를 갱신한다.
        (odometry.left_wheel_rad_per_s + odometry.right_wheel_rad_per_s);
    // 휠 오도메트리 자체는 pitch를 결정하지 않지만 입력 유효성을 확인한다.
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // VSLAM이 제공하는 저주기 전역 pose를 필터 상태에 부드럽게 반영한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  bool update_vslam(const VslamPose& pose) noexcept {
    // invalid pose와 낮은 confidence는 제어기 상태를 오염시키지 않는다.
// 입력과 상태에 따라 실행 경로를 선택한다.
    if (!pose.valid ||
// 이 코드 줄의 도메인 동작을 수행한다.
        pose.tracking_confidence < config_.minimum_vslam_confidence ||
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
        !finite_pose(pose)) {
// 값을 계산하거나 상태를 갱신한다.
      state_.vslam_valid = false;
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
      return false;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
    }
    // 위치는 VSLAM 세계 좌표를 보정값으로 천천히 반영한다.
// 이 코드 줄의 도메인 동작을 수행한다.
    state_.position_x_m = blend(state_.position_x_m, pose.position_x_m,
// 값을 계산하거나 상태를 갱신한다.
                                config_.vslam_position_weight);
    // y 위치도 같은 방식으로 보정한다.
// 이 코드 줄의 도메인 동작을 수행한다.
    state_.position_y_m = blend(state_.position_y_m, pose.position_y_m,
// 값을 계산하거나 상태를 갱신한다.
                                config_.vslam_position_weight);
    // z 위치는 평면 주행에서도 자세/낙상 진단에 유용하다.
// 이 코드 줄의 도메인 동작을 수행한다.
    state_.position_z_m = blend(state_.position_z_m, pose.position_z_m,
// 값을 계산하거나 상태를 갱신한다.
                                config_.vslam_position_weight);
    // yaw는 wrap된 차이를 보간해 +pi/-pi 경계에서 튀지 않게 한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const float yaw_error = wrap_angle(pose.yaw_rad - state_.yaw_rad);
    // VSLAM yaw 보정도 작은 weight로만 반영한다.
// 이 코드 줄의 도메인 동작을 수행한다.
    state_.yaw_rad = wrap_angle(state_.yaw_rad +
// 값을 계산하거나 상태를 갱신한다.
                                 config_.vslam_yaw_weight * yaw_error);
    // 마지막 VSLAM timestamp를 공개 상태에 저장한다.
// 값을 계산하거나 상태를 갱신한다.
    state_.vslam_timestamp_us = pose.timestamp_us;
    // 유효한 VSLAM 보정이 마지막으로 들어왔음을 표시한다.
// 값을 계산하거나 상태를 갱신한다.
    state_.vslam_valid = true;
    // pose가 사용되었음을 반환한다.
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return true;
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // 현재 융합 상태를 읽기 전용으로 반환한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  const FusedState& state() const noexcept { return state_; }

// 멤버의 접근 범위를 지정한다.
 private:
  // 모든 IMU scalar가 finite인지 확인한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  static bool finite_imu(const ImuSample& sample) noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::isfinite(sample.accel_x_mps2) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(sample.accel_y_mps2) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(sample.accel_z_mps2) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(sample.gyro_x_rad_per_s) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(sample.gyro_y_rad_per_s) &&
// 값을 계산하거나 상태를 갱신한다.
           std::isfinite(sample.gyro_z_rad_per_s);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // 모든 VSLAM pose scalar가 finite인지 확인한다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  static bool finite_pose(const VslamPose& pose) noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::isfinite(pose.position_x_m) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(pose.position_y_m) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(pose.position_z_m) &&
// 이 코드 줄의 도메인 동작을 수행한다.
           std::isfinite(pose.yaw_rad) &&
// 값을 계산하거나 상태를 갱신한다.
           std::isfinite(pose.tracking_confidence);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // x축 전방, z축 위, pitch positive nose-up convention의 가속도 pitch다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  static float accelerometer_pitch(const ImuSample& sample) noexcept {
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const float lateral = std::sqrt(sample.accel_y_mps2 * sample.accel_y_mps2 +
// 값을 계산하거나 상태를 갱신한다.
                                    sample.accel_z_mps2 * sample.accel_z_mps2);
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::atan2(-sample.accel_x_mps2, lateral);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // x축 전방, y축 좌측 convention의 가속도 roll이다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  static float accelerometer_roll(const ImuSample& sample) noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::atan2(sample.accel_y_mps2, sample.accel_z_mps2);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // angle을 [-pi, pi] 범위로 접는다.
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  static float wrap_angle(const float angle) noexcept {
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return std::remainder(angle, 2.0F * 3.14159265358979323846F);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // old 값에서 new 값으로 weight만큼 이동한다.
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
  static float blend(const float old_value, const float new_value,
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
                     const float weight) noexcept {
// 단위와 수명이 명확한 상수·상태·함수를 선언한다.
    const float bounded_weight = weight < 0.0F ? 0.0F :
// 값을 계산하거나 상태를 갱신한다.
                                  (weight > 1.0F ? 1.0F : weight);
// 호출자에게 결과를 반환하거나 오류 경로로 전환한다.
    return old_value + bounded_weight * (new_value - old_value);
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
  }

  // 설정값을 보관한다.
// 값을 계산하거나 상태를 갱신한다.
  EstimatorConfig config_{};
  // 융합 결과를 보관한다.
// 값을 계산하거나 상태를 갱신한다.
  FusedState state_{};
  // 마지막 IMU timestamp를 보관한다.
// 값을 계산하거나 상태를 갱신한다.
  std::uint64_t last_imu_timestamp_us_{0U};
  // 첫 IMU 샘플이 들어왔는지 표시한다.
// 값을 계산하거나 상태를 갱신한다.
  bool initialized_{false};
// 현재 함수·조건·타입의 코드 블록 경계를 표시한다.
};

// 이 코드 줄의 도메인 동작을 수행한다.
}  // namespace bldc::state
