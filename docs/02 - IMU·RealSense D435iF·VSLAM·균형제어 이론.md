# IMU · RealSense D435iF · VSLAM · 균형제어 이론

## 1. 먼저 결론

RealSense D435iF 하나만 VSLAM 카메라로 사용하는 구성은 가능하다. 다만 “카메라 하나만”이라는 말은 다음처럼 해석해야 한다.

```text
카메라 센서 구성: D435iF 하나
VSLAM backend: 하나만 선택
IMU 입력: D435iF 내부 IMU를 VSLAM에 함께 사용
balance 입력: IMU + wheel encoder를 별도 고주기 경로로 사용
```

VSLAM 하나가 모든 제어를 대신하지는 않는다. VSLAM은 camera/depth/IMU를 사용해 전역 pose를 추정하지만, feature dropout·motion blur·latency·loop-closure jump가 생길 수 있다. 균형 제어는 이런 지연을 기다리지 않고 더 빠른 IMU loop에서 실행되어야 한다.

## 2. 좌표계

최소한 다음 frame을 분리한다.

```text
world frame W       VSLAM이 만드는 전역 기준
camera frame C      D435i color/depth optical frame
imu frame I         D435i IMU frame
body frame B        로봇 torso 기준
wheel frame          좌우 휠 encoder 기준
motor frame          GIM output/rotor 계약 확인 전까지 분리
```

변환은 다음처럼 생각한다.

```text
T_WB = T_WC · T_CB
T_WC = VSLAM이 추정하는 camera pose
T_CB = 카메라 장착 위치/자세의 고정 extrinsic
```

주의:

- `camera frame`과 `body frame`의 축 방향을 가정하지 않는다.
- D435iF 장착 후 실제 yaw/pitch/roll extrinsic을 측정한다.
- IMU 축과 카메라 축이 다를 수 있으므로 SDK profile/장치 문서와 실제 움직임으로 sign을 확인한다.
- C++ 예제의 `accel_x`, `accel_y`, `accel_z` convention은 초기 후보일 뿐이다.

## 3. IMU 측정 모델

### 3.1 Accelerometer

정지 또는 저가속 구간에서는 accelerometer가 중력 방향을 알려 준다.

```text
pitch_acc = atan2(-a_x, sqrt(a_y^2 + a_z^2))
roll_acc  = atan2(a_y, a_z)
```

이 값은 linear acceleration이 커지면 중력과 움직임을 구분하지 못한다. 따라서 accelerometer만으로 balance를 제어하지 않는다.

### 3.2 Gyroscope

gyroscope는 각속도를 빠르게 측정한다.

```text
pitch_gyro[k] = pitch[k-1] + gyro_y[k] * dt
```

장점:

- 빠른 응답
- 짧은 시간 자세 변화에 강함
- balance loop에 적합

단점:

- bias를 적분하면 yaw/pitch/roll drift 발생
- timestamp gap과 sample jitter에 민감

### 3.3 Complementary filter

현재 C++ estimator는 가장 작은 형태의 complementary filter를 사용한다.

```text
pitch[k] = alpha * pitch_gyro[k]
         + (1 - alpha) * pitch_acc[k]
```

- `alpha`가 1에 가까우면 gyro를 더 신뢰한다.
- `alpha`가 낮아지면 accelerometer 보정이 커진다.
- 실제 주행에서는 vibration/linear acceleration에 따라 gain을 조정해야 한다.
- 최종 제품에서는 bias estimation, quaternion/EKF, contact-aware filtering을 검토한다.

## 4. Wheel odometry

좌우 휠의 각속도를 `w_L`, `w_R`, 휠 반지름을 `r`라고 하면 전진 속도의 1차 근사는 다음이다.

```text
v = r * (w_L + w_R) / 2
```

차동 구동의 yaw rate는 다음 후보식으로 시작할 수 있다.

```text
omega_z = r * (w_R - w_L) / track_width
```

하지만 휠레그드 로봇에서는 다음 오차가 크다.

- wheel slip
- contact loss
- compliant leg motion
- gearbox backlash
- encoder rotor/output unit 혼동
- uneven floor

따라서 wheel odometry는 IMU와 결합하고, VSLAM은 장기 drift를 보정한다.

## 5. RealSense D435iF의 역할

D435iF 계열은 다음 입력을 제공하는 RGB-D/IMU 장치로 취급한다.

- color image
- depth image
- gyroscope
- accelerometer
- device timestamp
- stream profile/intrinsics

초기 bring-up에서는 다음 순서로 진행한다.

1. USB 연결과 device identification
2. color/depth frame timestamp 확인
3. gyro/accel raw data 수신
4. stream profile과 intrinsics 기록
5. camera-to-IMU timestamp 관계 확인
6. camera를 고정하고 static sequence 기록
7. 외부 VSLAM backend에 연결
8. pose dropout과 relocalization을 기록

`host/realsense_d435i_vslam_bridge.py`는 1–3단계를 위한 read-only capture 골격이다. 실제 VSLAM 알고리즘을 임의로 구현한 것으로 간주하면 안 된다.

## 6. VSLAM의 내부 개념

VSLAM은 단순히 카메라 위치를 매 frame 독립적으로 계산하는 것이 아니다.

```text
feature detection
  → feature matching/tracking
  → camera motion estimation
  → depth/scale constraint
  → IMU preintegration 또는 visual-inertial fusion
  → local map
  → loop closure
  → pose graph optimization
```

D435iF를 사용하는 이유는 다음 세 가지 입력이 함께 있기 때문이다.

- color feature: 반복 가능한 시각 특징
- depth: scale과 3D 구조
- IMU: 빠른 회전/이동 예측

### 하나의 VSLAM backend만 사용하는 이유

RTAB-Map과 ORB-SLAM3를 동시에 돌려서 서로 다른 pose를 섞지 않는다. 다음 중 하나를 선택하고 output contract를 고정한다.

```text
VslamPose {
  timestamp_us
  position_x_m
  position_y_m
  position_z_m
  yaw_rad
  tracking_confidence
  valid
}
```

backend가 바뀌어도 이 계약만 유지하면 controller/state estimator는 바꾸지 않는다.

## 7. VSLAM과 균형제어의 시간 스케일

권장 개념 주기:

```text
IMU ingest              100–200 Hz 이상 후보
balance controller      100–200 Hz 후보
wheel encoder/CAN       device contract에 따라 확인
RGB-D/VSLAM             10–30 Hz 후보
high-level policy       20–50 Hz 후보
```

정확한 값은 장치와 통신 latency를 측정해 정한다. 숫자를 먼저 고정하고 센서가 따라오기를 기대하지 않는다.

VSLAM pose가 늦게 도착하면:

- balance loop는 마지막 유효 IMU 상태로 계속 실행한다.
- VSLAM correction은 confidence와 age를 확인한다.
- 오래된 pose를 새 pose로 착각하지 않는다.
- relocalization으로 pose가 크게 jump하면 즉시 controller에 넣지 않고 reset/hold를 거친다.

## 8. State estimator 구조

현재 C++ `StateEstimator`는 다음 입력을 받는다.

```text
update_imu(ImuSample)
update_wheel_odometry(WheelOdometry)
update_vslam(VslamPose)
```

출력은 다음과 같다.

```text
roll_rad
pitch_rad
pitch_rate_rad_per_s
yaw_rad
forward_velocity_m_per_s
position_x_m
position_y_m
position_z_m
imu_valid
vslam_valid
```

이 출력은 아직 최종 balance controller가 아니다. 다음 단계에서 classical controller가 사용한다.

```text
pitch error
+ pitch rate
+ height/CoM error
+ wheel velocity error
→ wheel torque/velocity command
```

## 9. 센서 융합의 신뢰도

### IMU invalid

- NaN/Inf
- timestamp 역전
- 허용 dt보다 긴 gap
- 장치 연결 해제

동작:

```text
balance command 금지 또는 낮은 authority
fault/event 기록
operator 재확인
```

### VSLAM invalid

- tracking lost
- confidence threshold 미달
- timestamp stale
- relocalization jump

동작:

```text
position/yaw correction 중지
IMU + wheel dead reckoning 유지
global navigation command 중지
```

VSLAM이 끊겼다고 즉시 motor를 강제로 끄는 것이 항상 정답은 아니다. balance loop는 짧은 시간 IMU로 안전하게 유지할 수 있지만, timeout 정책과 실제 로봇의 안정성에 따라 safe stop을 선택해야 한다.

## 10. RealSense 장착과 calibration

필수 calibration:

- color intrinsics
- depth intrinsics
- color-depth extrinsic
- IMU axis/sign
- IMU-to-camera extrinsic
- camera-to-body extrinsic
- time offset
- body pitch zero
- wheel encoder zero/sign

실험 순서:

1. 카메라를 로봇 body에 rigid mount
2. 바닥에 로봇을 고정
3. static IMU raw log
4. x/y/z 방향으로 손으로 작은 회전
5. 각 축의 gyro sign 확인
6. color/depth alignment 확인
7. VSLAM static drift 확인
8. 휠을 공중에 띄우고 encoder sign 확인
9. 모든 sign을 표로 고정
10. controller 연결

## 11. 현재 코드가 하지 않는 것

다음은 아직 구현하지 않았다.

- 실제 `pyrealsense2` 장치 연결 검증
- 특정 VSLAM backend의 pose API 호출
- ROS2 topic/TF bridge
- USB packet loss recovery
- hardware timestamp synchronization
- quaternion 기반 EKF
- IMU bias estimator
- camera pose를 STM32 CAN frame으로 전송
- VSLAM relocalization jump rejection

이 항목들은 장치와 선택 backend를 정한 뒤 별도 단계로 구현한다.

## 12. 권장 최종 아키텍처

```text
[Host computer]
D435iF USB
  ├─ color/depth capture
  ├─ gyro/accel capture
  └─ one selected VSLAM backend
        ↓ VslamPose + IMU state contract
[State estimator]
  ├─ IMU complementary/EKF
  ├─ wheel odometry
  └─ VSLAM slow correction
        ↓
[Classical balance controller]
        ↓
[Safety supervisor]
  ├─ arm
  ├─ timeout
  ├─ fault latch
  ├─ clamp
  └─ fall/tilt gate
        ↓
[STM32 + CAN transceiver]
        ↓
[GIM6010-8]
```

핵심 원칙은 `VSLAM이 모든 것을 해결한다`가 아니라 `IMU가 빠른 안정성을 담당하고, VSLAM이 느린 전역 기준을 보정한다`이다.
