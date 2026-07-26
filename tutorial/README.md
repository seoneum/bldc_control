# Modern C++ Top-down Tutorial

이 폴더는 STM32 HAL보다 먼저 **순수 C++ domain logic**을 이해하기 위한 실행 가능한 예제다. 모터나 CAN hardware에 연결하지 않는다.

## 학습 순서

| 순서 | 파일 | 배울 것 |
|---:|---|---|
| 1 | `01_can_id_and_endian.cpp` | node ID와 command ID로 11-bit CAN ID를 만드는 법, little-endian 해석 |
| 2 | `02_decode_telemetry.cpp` | heartbeat와 encoder frame을 typed data로 바꾸는 법 |
| 3 | `03_safety_supervisor.cpp` | heartbeat가 끊기거나 fault가 생기면 command를 금지하는 법 |
| 4 | `04_mit_frame_preview.cpp` | 작은 MIT command를 clamp하고 **송신 없이** 8-byte frame을 미리 보는 법 |
| 5 | `../src/main.cpp` | 위 부품을 NUCLEO receive-only loop에 조립하는 법 |
| 6 | `../src/stm32_bxcan_adapter.cpp` | PA11/PA12와 bxCAN 500 kbit/s 후보 설정을 HAL에 연결하는 법 |
| 7 | `../src/tx_tutorial_main.cpp` | arm edge → clamp → 승인 token → 실제 HAL 송신 순서; gate 기본 false |
| 8 | `05_imu_state_estimator.cpp` | IMU complementary filter, wheel odometry, VSLAM pose 보정 |

## Mac에서 host 예제 실행

project root에서 실행한다.

```bash
c++ -std=c++17 -Iinclude tutorial/01_can_id_and_endian.cpp -o /tmp/01_can
/tmp/01_can

c++ -std=c++17 -Iinclude tutorial/02_decode_telemetry.cpp -o /tmp/02_telemetry
/tmp/02_telemetry

c++ -std=c++17 -Iinclude tutorial/03_safety_supervisor.cpp -o /tmp/03_safety
/tmp/03_safety

c++ -std=c++17 -Iinclude tutorial/04_mit_frame_preview.cpp -o /tmp/04_mit
/tmp/04_mit

c++ -std=c++17 -Iinclude tutorial/05_imu_state_estimator.cpp -o /tmp/05_imu
/tmp/05_imu
```

## RealSense D435iF host capture

D435iF를 STM32에 직접 연결하지 않고 host USB에 연결한다. 먼저 read-only RGB-D/IMU JSONL을 기록한다.

```bash
python3 host/realsense_d435i_vslam_bridge.py --frames 100 --output /tmp/d435i_capture.jsonl
```

`pyrealsense2`가 설치되어 있어야 하며, 이 명령은 실제 VSLAM이나 motor CAN 송신을 실행하지 않는다. JSONL은 선택한 VSLAM backend의 입력 계약을 확인하기 위한 raw capture다.

## 읽을 때 스스로 답할 질문

1. CAN ID에서 node와 command를 왜 분리하는가?
2. encoder payload 8 byte가 왜 `float` 두 개가 되는가?
3. healthy heartbeat가 다시 와도 왜 자동으로 재가동하지 않는가?
4. `compile_mit()`과 `build_mit()`의 차이는 무엇인가?
5. `BLDC_ENABLE_CAN_TRANSMIT=0`인데도 MIT encoder가 존재하는 이유는 무엇인가?

## 중요한 안전 경계

- 이 예제는 candidate protocol을 학습하는 코드다.
- GIM6010-8 firmware와 byte contract가 실제로 일치하는지는 사용자가 bench에서 확인해야 한다.
- `04_mit_frame_preview.cpp`는 CAN frame을 메모리에서 만들기만 하며 hardware로 보내지 않는다.
- 기본 `nucleo_f103rb` firmware도 수신 전용이다.
- 송신 흐름은 `pio run -e nucleo_f103rb_tx_tutorial`로 compile하며, `tx_tutorial_main.cpp`의 operator/runtime gate는 기본 `false`다.
- 실제 motor command 전에 판매자 manual, node ID, baud, unit, sign, timeout을 확인한다.
