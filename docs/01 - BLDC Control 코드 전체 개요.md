# BLDC Control 코드 전체 개요

## 1. 이 저장소의 목표

이 프로젝트는 GIM6010-8 통합 FOC 액추에이터를 STM32/CAN host에서 안전하게 다루고, 이후 2족·휠레그드 균형 제어로 확장하기 위한 학습 저장소다.

모터 내부의 다음 부분은 이 저장소의 직접 구현 범위가 아니다.

- 3상 PWM
- Clarke/Park 변환
- 전류 루프
- 모터 드라이버 내부 encoder 정류

이 저장소가 직접 다루는 부분은 다음이다.

```text
RealSense D435iF + IMU
        ↓ USB
Host capture / VSLAM backend
        ↓ pose + IMU contract
State estimator
        ↓ pitch, pitch rate, velocity, world pose
Classical balance controller / future RL policy
        ↓ bounded command
STM32 firmware
        ↓ external CAN transceiver
GIM6010-8 CAN actuator
```

## 2. 센서와 제어기의 역할 분리

| 장치/모듈 | 주 역할 | 제어 주기 후보 | 실패 시 동작 |
|---|---|---:|---|
| GIM encoder | 모터/휠 위치·속도 | CAN 주기 | stale이면 command 금지 |
| IMU | pitch, roll, angular rate | 100–200 Hz 이상 후보 | balance command 차단 |
| RealSense D435iF IMU | VSLAM의 inertial input, host-side 자세 보조 | gyro high-rate / accel lower-rate | VSLAM confidence 저하 |
| RealSense color/depth | feature tracking, depth scale, loop closure | 15–30 Hz 후보 | global pose 보정 중지 |
| VSLAM backend | world pose, yaw, drift correction | 10–30 Hz 후보 | wheel/IMU dead reckoning 유지 |
| STM32 safety | timeout, fault latch, arm, clamp | firmware loop | safe state |

중요한 결론:

- VSLAM은 균형 제어기의 유일한 입력이 아니다.
- D435iF의 카메라를 VSLAM용 카메라 하나로 사용해도 된다.
- D435iF 내부 IMU는 VSLAM의 inertial input으로 같이 사용한다.
- 빠른 pitch balance는 IMU와 wheel encoder가 담당한다.
- VSLAM은 느린 전역 위치·yaw 보정과 drift 억제를 담당한다.

## 3. 디렉터리와 파일 지도

### C++ domain

- `include/bldc/can_frame.hpp`
  - 11-bit standard CAN ID pack/unpack
  - little-endian integer/float decode
- `include/bldc/gim_protocol.hpp`
  - GIM heartbeat/encoder candidate telemetry decode
- `include/bldc/command_builder.hpp`
  - MIT command finite check, clamp, wire encoding, transmit gate
- `include/bldc/safety_supervisor.hpp`
  - disarmed, armed, heartbeat stale, fault latched state machine
- `include/bldc/application.hpp`
  - protocol decoder와 safety supervisor composition
- `include/bldc/state_estimator.hpp`
  - IMU + wheel odometry + VSLAM pose를 제어용 상태로 변환

### STM32 target

- `src/main.cpp`
  - receive-only 기본 firmware
  - CAN frame polling
  - telemetry ingest
  - heartbeat stale update
- `src/stm32_bxcan_adapter.*`
  - STM32F103 bxCAN HAL adapter
  - PA11/PA12 logic-side CAN
  - 외부 3.3 V CAN transceiver 필요
- `src/tx_tutorial_main.cpp`
  - 명시적 arm/runtime gate가 있는 송신 학습 코드
  - 기본 target에서는 source filter로 제외

### Host tutorial

- `tutorial/01_can_id_and_endian.cpp`
  - CAN ID와 little-endian 기초
- `tutorial/02_decode_telemetry.cpp`
  - heartbeat/encoder typed decode
- `tutorial/03_safety_supervisor.cpp`
  - timeout과 fault latch
- `tutorial/04_mit_frame_preview.cpp`
  - 송신하지 않는 MIT frame preview
- `tutorial/05_imu_state_estimator.cpp`
  - IMU, wheel odometry, VSLAM pose를 state estimator에 넣는 전체 흐름

### Host sensor bridge

- `host/realsense_d435i_vslam_bridge.py`
  - D435i 계열 color/depth/gyro/accel stream enable
  - timestamped JSONL capture
  - 외부 VSLAM backend가 소비할 센서 입력 계약
  - 실제 모터 CAN 송신은 하지 않음

## 4. 실행 흐름

### Pure domain

```text
CanFrame
  → gim::decode_heartbeat / decode_encoder_estimates
  → TelemetryCache
  → SafetySupervisor
  → StateEstimator
  → controller input
```

### Host sensor

```text
D435iF USB
  → pyrealsense2 pipeline
  → color/depth/gyro/accel timestamp
  → VSLAM backend
  → VslamPose JSON/transport contract
  → StateEstimator::update_vslam
```

### STM32

```text
bxCAN FIFO
  → Stm32BxcanAdapter::poll
  → TelemetryApplication::ingest
  → SafetySupervisor::update
  → command builder
  → AuthorizedFrame
  → HAL CAN transmit
```

송신은 다음 조건을 모두 만족할 때만 가능하다.

```text
compile-time transmit enabled
AND runtime transmit enabled
AND operator arm
AND fresh heartbeat
AND no latched fault
AND bounded finite command
```

## 5. 주석 작성 규칙

새로 추가한 교육용 C++/Python 코드는 다음 기준으로 작성했다.

- 센서 단위와 coordinate convention을 주석으로 명시한다.
- 안전 조건이 필요한 줄에는 실패 조건을 설명한다.
- VSLAM의 저주기 pose가 pitch controller를 대체하지 않는 이유를 적는다.
- 단순 문법보다 timestamp, frame, unit, gate를 우선 설명한다.
- 기존 production domain 코드는 모든 기호를 반복 설명하기보다 protocol/safety 의도를 중심으로 주석을 유지한다.

## 6. 현재 구현의 범위와 미구현 범위

현재 구현됨:

- C++17 pure state estimator
- accelerometer 기반 roll/pitch 초기값
- gyro 적분 + complementary filter
- wheel velocity → forward velocity
- VSLAM pose confidence/finite check
- VSLAM position/yaw 저가중치 보정
- RealSense SDK stream configuration 예제
- timestamped RGB-D/IMU JSONL bridge

아직 별도 통합이 필요한 부분:

- 실제 D435iF 장치의 SKU/firmware/stream profile 확인
- 실제 pyrealsense2 설치와 USB capture
- RTAB-Map 또는 ORB-SLAM3 중 하나 선택
- VSLAM pose를 STM32로 전달하는 UART/CAN/UDP transport
- IMU axes와 body frame sign calibration
- wheel encoder와 D435iF timestamp synchronization
- actual balance controller와 RL policy
- physical robot validation

## 7. 권장 학습 순서

1. `tutorial/01`로 CAN ID/byte order 이해
2. `tutorial/02`로 telemetry typed decode 이해
3. `tutorial/03`으로 stale/fault safety 이해
4. `tutorial/05`로 IMU·wheel·VSLAM state 역할 이해
5. `docs/02 - IMU·RealSense D435iF·VSLAM·균형제어 이론.md` 읽기
6. D435iF를 host에서 read-only capture
7. VSLAM backend 하나만 연결
8. pose dropout/latency를 재생 테스트
9. classical balance controller를 먼저 연결
10. 마지막에 RL policy를 같은 state/action contract에 연결
