# BLDC Control — GIM6010-8 수신 우선 학습 프로젝트

## 목적과 상태

이 저장소는 **GIM6010-8 통합 FOC 액추에이터를 안전한 CAN host 관점에서 이해**하고, 이후 2륜 균형 제어로 확장하기 위한 Modern C++17 실습 기반이다. 모터 내부의 전류 루프와 FOC는 액추에이터 드라이버의 블랙박스로 두고, 이 프로젝트는 CAN frame, telemetry, 제한, timeout, fault 처리를 다룬다.

**현재 상태: 학습용 Modern C++ 코드와 receive-only firmware 골격을 작성했다. build, board flash, 실제 CAN 수신, motor/encoder 동작은 사용자가 직접 검증하는 단계이며 물리 동작 성공을 주장하지 않는다.** GIM 사양과 CAN protocol은 제조사 원문이 아닌 커뮤니티 번역 매뉴얼에 근거한 후보 계약이다. 근거와 확인 범위는 [docs/protocol-provenance.md](docs/protocol-provenance.md)를 따른다.

## 기본 안전 정책

- 기본 firmware는 **수신 전용**이다. `BLDC_ENABLE_CAN_TRANSMIT=0`이 NUCLEO-F103RB 환경에서 컴파일 시 송신 경로를 막는다.
- 컴파일 설정만으로 충분하지 않다. 송신을 허용하는 빌드에서도 runtime arm, command clamp, heartbeat freshness, fault latch가 모두 만족되어야 명령을 만들 수 있다.
- CAN software Estop은 hardware E-stop이나 전원 차단을 대체하지 않는다.
- 자동 workflow는 hardware 연결, flash, CAN 송신, calibration, actuator 설정 변경을 수행하지 않는다.

## 배선 개념 — 전원을 넣기 전에

```text
STM32 NUCLEO-F103RB                외부 3.3 V CAN transceiver          GIM6010-8
CAN_TX / CAN_RX  ── logic side ──> TXD / RXD       CANH / CANL ─────> CANH / CANL
STM32 GND ──────────────────────── GND ────────────────────────────> signal GND

별도 모터 전원(예: 판매자 확인 전압 범위) ──────────────────────────> motor power
STM32 전원 ────────────────────────────────────────────────────────> MCU power
```

1. **MCU logic pin을 CANH/CANL에 절대 직접 연결하지 않는다.** STM32 CAN_TX/CAN_RX와 버스 사이에는 STM32 logic 전압과 맞는 외부 3.3 V CAN transceiver가 반드시 있어야 한다.
2. 통신 기준을 위해 GND는 공통으로 하되, 모터 전원과 MCU 전원은 별도 설계·공급한다. CAN 커넥터의 모터 전원을 MCU pin에 연결하지 않는다.
3. 120 Ω 종단은 CAN 버스의 **물리적 양 끝에만** 둔다. 중간 노드마다 추가하지 않는다.
4. 실제 pinout, hardware/firmware version, 극성은 판매자 최신 매뉴얼과 제품 표기로 재확인한다. 커뮤니티 문서의 XT30 2+2 설명만으로 전원을 인가하지 않는다.

## 빌드와 테스트

```bash
pio test -e native
pio test -e native_tx_gate
pio run -e nucleo_f103rb
python3 test/host_numerical_fixtures.py
pio run -e nucleo_f103rb_tx_review
pio run -e nucleo_f103rb_tx_tutorial

# host-side IMU / wheel / VSLAM state estimator tutorial
c++ -std=c++17 -Iinclude tutorial/05_imu_state_estimator.cpp -o /tmp/05_imu
/tmp/05_imu

# optional D435iF RGB-D/IMU read-only capture; requires pyrealsense2
python3 host/realsense_d435i_vslam_bridge.py --frames 100 --output /tmp/d435i_capture.jsonl
```

- `native`는 pure domain의 RED/GREEN TDD용이다.
- `native_tx_gate`는 `BLDC_ENABLE_CAN_TRANSMIT=1`로 pure test만 컴파일하며 HAL·hardware 없이 runtime-disabled, no-heartbeat, unarmed, authorized, stale, faulted branch를 독립 검증한다.
- 기본 firmware target인 `nucleo_f103rb`는 C++17, `-fno-exceptions`, `BLDC_ENABLE_CAN_TRANSMIT=0`으로 설정되어 있다. source는 RTTI에도 의존하지 않는다.
- `nucleo_f103rb_tx_review`는 command/HAL 송신 API를 compile하지만 receive-only `main.cpp`를 유지한다.
- `nucleo_f103rb_tx_tutorial`은 `src/tx_tutorial_main.cpp`를 선택해 arm edge → fresh heartbeat → clamp → `AuthorizedFrame` → HAL 송신 흐름을 보여 준다. 두 runtime gate는 기본 `false`다.
- NUCLEO build command는 flash하지 않으며 hardware 동작이나 CAN 송신을 증명하지 않는다.

### 작성된 TDD 계약

`test/test_domain/test_main.cpp`에는 CAN ID 범위, endian, heartbeat/encoder decode, 잘못된 frame 거부, stale timeout, fault latch, re-arm, command clamp, compile/runtime transmit gate, HAL tick rollover에 대한 native Unity test가 들어 있다. pass 여부는 아래 명령으로 사용자가 직접 확인한다.

```bash
pio test -e native
pio test -e native_tx_gate
```

### 사용자가 직접 확인할 명령

```bash
# 순수 C++ protocol/safety logic
pio test -e native
pio test -e native_tx_gate

# NUCLEO-F103RB compile
pio run -e nucleo_f103rb

# tutorial host 예제
c++ -std=c++17 -Iinclude tutorial/01_can_id_and_endian.cpp -o /tmp/01_can
/tmp/01_can
```

이 명령의 성공은 code/build 확인이며 실제 GIM firmware 호환이나 motor 동작 확인은 아니다.

## 첫 전원 인가 전 Gate

다음 모두가 비어 있지 않을 때에만 사람이 감독하는 별도 bench 절차를 검토한다.

- [ ] 제품 hardware/firmware version과 판매자 최신 manual 확보
- [ ] motor power, GND, CANH, CANL pinout 및 극성의 독립 확인
- [ ] 링크·바퀴를 제거하고 shaft를 안전한 jig에 고정
- [ ] fuse와 손이 닿는 hardware power cut/contactor 준비
- [ ] bus-end-only 120 Ω termination 및 common ground 확인
- [ ] 회생 전류 흡수, brake resistor/regen clamp, bus-voltage 관찰 계획 검토
- [ ] 온도/current/velocity limit, heartbeat timeout, stale telemetry, fault latch의 확인 계획 작성

비정상 방향, jig 흔들림, bus voltage 상승, 과열, heartbeat 소실, encoder jump, 반복 fault, 정지 명령 뒤 가속은 즉시 중단 조건이다.

## Passive-first 흐름

1. 제품과 wiring, version을 기록한다.
2. USB가 지원되었다는 커뮤니티 주장에 따라 `odrivetool`로 voltage/error/version을 **읽기만** 시도한다.
3. 호환 장치에서 성공적으로 생성될 때에만 `backup-config`로 변경 전 설정을 보관한다.
4. STM32에서는 application data frame 송신을 비활성화한 채 heartbeat와 encoder를 decode한다. 현재 adapter는 ACK가 필요한 정상 CAN 수신을 위해 normal mode를 쓰지만 송신 API는 기본 build에서 제거된다.
5. heartbeat timeout과 fault/stale-data 처리가 검증된 뒤에도, motion/calibration/write는 별도의 사람이 감독하는 안전 검토와 bench 단계에 남긴다.

GIM 번역 매뉴얼은 ODrive-compatible USB interface 및 GIM 전용 endpoint/example을 주장한다. ODrive 공식 문서는 지원 ODrive surface에서의 `odrivetool` 능력만 뒷받침하며 GIM 호환을 보장하지 않는다. Motor Wizard는 Windows-only 보조 경로이며 macOS에서는 native 실행할 수 없다.

## 파일 지도

| 경로 | 역할 |
|---|---|
| `tutorial/` | CAN ID → telemetry → safety → MIT preview 순서의 한국어 주석 Modern C++ 예제 |
| `test/test_domain/test_main.cpp` | CAN frame, GIM protocol, command builder, safety supervisor의 순수 API 계약 |
| `include/bldc/` | C++17 domain protocol, safety supervisor, command gate, receive-only application API |
| `src/stm32_bxcan_adapter.*` | STM32 bxCAN HAL adapter |
| `src/tx_tutorial_main.cpp` | 명시적 arm/runtime gate가 있는 별도 송신 tutorial main; 기본 target에서는 제외 |
| `platformio.ini` | native TDD와 NUCLEO-F103RB C++17/송신 차단 build 설정 |
| `docs/N - NUCLEO-F103RB로 GIM6010-8 제어하는 Modern C++ 실습 가이드.md` | top-down 학습·bench 준비 guide |
| `docs/protocol-provenance.md` | 주장별 URL, commit, 신뢰도, bench 미해결 항목 |
| `AGENTS.md` | 구현·검증·hardware automation 경계 |
| `include/bldc/state_estimator.hpp` | IMU complementary filter, wheel odometry, VSLAM pose correction |
| `include/bldc/tv_lqr.hpp` | fixed-size hardware-free finite-horizon TV-LQR/Riccati domain API |
| `include/bldc/kalman_filter.hpp` | fixed-size linear KF/EKF, covariance, timestamp, and failure contracts |
| `docs/03_자료_감사와_학습경로.md` | current-code audit, canonical numerical fixtures, learning route, and evidence boundary |
| `test/host_numerical_fixtures.py` | NumPy canonical TV-LQR/KF host-side numerical fixture check |
| `host/realsense_d435i_vslam_bridge.py` | RealSense D435iF host-side color/depth/gyro/accel JSONL capture |
| `docs/01 - BLDC Control 코드 전체 개요.md` | 코드 계층과 파일별 역할 |
| `docs/02 - IMU·RealSense D435iF·VSLAM·균형제어 이론.md` | 센서 융합·좌표계·시간축·VSLAM 이론 |

## 학습 허브
- Obsidian 학습 route: `Study - WBR 제어·추정 ADHD 학습 로드맵`
- 동반 노트: `Study - LQR·TV-LQR 수학과 C++ 코드`, `Study - 확률론적 로보틱스·Kalman·EKF와 C++ 코드`
- 전체 route는 vectors/matrices → state-space → LQR/Riccati → TV-LQR → Gaussian/Bayes → KF → EKF → C++ tests → simulation 순서다.
## 송신 Gate의 의미

`BLDC_ENABLE_CAN_TRANSMIT=0`은 compile-time 기본값이다. 따라서 accidental transmit이 가능한 firmware를 기본 산출물로 만들지 않는다. 향후 명시적으로 송신을 켜더라도 다음 runtime 조건이 모두 참이어야 한다.

```text
operator-approved arm
AND bounded command (range clamp)
AND fresh heartbeat
AND no latched fault
AND valid heartbeat timeout
=> command eligible
```

이 조건 중 하나라도 거짓이면 safe state를 유지한다. 이 논리는 물리적 전원 차단을 대체하지 않는다.

## 문서의 신뢰도 표기

- **커뮤니티 주장·bench 확인 대기**: GIM 번역 매뉴얼의 spec, pinout, protocol, endpoint, timing 값.
- **SDK source-confirmed (SDK 한정)**: 특정 commit의 코드가 보이는 동작; 내 제품 firmware와의 일치는 별도 확인.
- **upstream ODrive 보조 근거 (비-GIM)**: 공식 ODrive 지원 surface의 도구·제어 설명; GIM 호환 보증 아님.
- **bench confirmed**: 내 제품의 hardware/firmware와 기록된 측정이 있어야만 사용할 수 있는 표기. 현재 이 저장소에는 해당 물리 성공 주장이 없다.
