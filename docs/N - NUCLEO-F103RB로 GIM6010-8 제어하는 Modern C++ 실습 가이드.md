---
title: "NUCLEO-F103RB로 GIM6010-8 제어하는 Modern C++ 실습 가이드"
aliases:
  - "GIM6010-8 STM32 CAN 수신 우선 실습"
tags:
  - bldc
  - can
  - stm32
  - modern-cpp
status: in_progress
type: study_note
created: 2026-07-24
updated: 2026-07-24
source_project: /Users/seoneum/orca/workspaces/bldc_tuto/bldc_control
---

# NUCLEO-F103RB로 GIM6010-8 제어하는 Modern C++ 실습 가이드

> [!important] 범위와 근거
> 이 guide는 GIM6010-8을 **통합 FOC smart actuator**로 다룬다. STM32는 CAN host이고 FOC·3상 PWM을 직접 구현하지 않는다. GIM 사양, endpoint, CAN 값은 커뮤니티 번역 매뉴얼의 주장으로서 판매자 원문과 내 제품 bench 확인 전까지 확정값이 아니다. 세부 근거는 project의 `docs/protocol-provenance.md`를 따른다.

연결 노트:

- [[S - GIM6010-8로 BLDC FOC와 2족 밸런싱봇 제어 공부하기]]
- [[N - EBIMU-9DOFV5를 Mac과 STM32에서 사용하는 방법]]
- [[N - macOS uv 가상환경과 Python 버전 관리]]

## 0. 위에서 아래로 보는 학습 지도

```text
행동/작업
  → 균형 제어·policy
  → 상태 추정(IMU + encoder)
  → 안전 필터와 actuator command
  → CAN transport/telemetry
  → GIM 내부 FOC·전류 루프
  → 전원·감속기·링크의 물리계
```

처음부터 RL이나 motor FOC 구현으로 내려가지 않는다. 먼저 **모터를 움직이지 않는 관찰**으로 통신과 안전 계약을 만든다. 이후 제한된 bench 단계에서만 속도·위치·토크를 분리해 검증하고, classical balance가 안정된 뒤 simulation/RL로 간다.

### 신뢰도 표기

| 표기 | 의미 |
|---|---|
| 커뮤니티 주장·bench 확인 대기 | GIM 영문 번역 매뉴얼에서 얻은 spec/protocol/USB 설명. 제조사 보증이나 내 제품 결과가 아니다. |
| SDK source-confirmed (SDK 한정) | 명시 commit의 SDK source가 보이는 동작. board port와 GIM firmware 호환은 별도다. |
| upstream ODrive 보조 근거 (비-GIM) | 공식 ODrive 문서가 뒷받침하는 ODrive 기능. GIM 호환 보증은 아니다. |
| bench confirmed | hardware/firmware 식별과 보존된 log가 있을 때만 쓴다. 이 문서에는 기록된 물리 성공이 없다. |

## 1. M0 — 전원 전 제품·배선 지도

### 배선 개념

```text
NUCLEO CAN_TX/CAN_RX → 3.3 V CAN transceiver logic side
                       transceiver CANH/CANL → GIM CANH/CANL
NUCLEO GND ─────────────────────────────────── GIM signal ground

MCU power: 별도 전원
motor power: 별도 모터 전원
```

- CAN_TX/CAN_RX는 logic signal이다. **CANH/CANL에 직접 연결하지 않고** 외부 3.3 V CAN transceiver를 반드시 둔다.
- 통신 기준을 위한 common ground를 연결하되, motor power와 MCU power는 분리한다.
- 120 Ω termination은 physical bus의 양 끝에만 둔다.
- 24 V+, GND, CANH, CANL은 판매자 pinout과 멀티미터로 독립 확인한다. 모터 전원을 MCU 전원 pin에 연결하지 않는다.

### 실습(아직 전원 인가 없음)

1. hardware/firmware version, connector, 판매자 manual을 기록한다.
2. pinout·극성·bus 끝 위치·common ground를 그린다.
3. fuse, jig, 손이 닿는 hardware power cut을 준비한다.
4. 회생 전류를 공급원이 흡수하는지와 brake resistor/regen clamp 계획을 검토한다.

### 이론 질문

- raw BLDC와 통합 FOC actuator의 책임 경계는 어디인가?
- rotor side, output side, gear ratio, joint sign, encoder zero를 왜 하나의 scale로 합치면 안 되는가?
- CAN의 differential pair와 logic-side TX/RX의 차이는 무엇인가?

### 통과 기준

- [ ] 전원을 인가하지 않고 각 connector와 극성을 식별했다.
- [ ] common ground와 양 끝 120 Ω 위치를 설명할 수 있다.
- [ ] hardware power cut과 regen 대응 계획이 있다.

**결과 기록**

- Hardware version::
- Firmware version::
- Seller manual/link::
- Pinout source::
- Power-cut method::
- 관찰/결론::

## 2. M1 — USB read-only discovery와 조건부 backup

GIM 번역 매뉴얼은 ODrive-compatible USB interface와 GIM-specific endpoint/example을 주장한다. 이는 **커뮤니티 주장·bench 확인 대기**다. 공식 ODrive 문서는 지원 ODrive surface의 `odrivetool` 사용법만 보증하며 GIM device의 USB descriptor, endpoint, 설정 형식 호환을 보증하지 않는다.

### 안전한 discovery

1. 링크·바퀴를 제거하고 shaft를 jig에 고정한다.
2. 사람 감독 bench에서 전원·data-capable Type-C를 연결한다. Type-C는 motor drive power가 아니다.
3. 호환 장치로 나타난 경우에만 voltage, current, hardware/firmware version, CAN baud, error를 **read-only**로 확인한다.
4. 성공적으로 GIM firmware에서 생성됨을 확인할 수 있을 때에만 `odrivetool backup-config`로 변경 전 JSON을 보관한다.
5. backup 파일에는 hardware/firmware version, 날짜, 제품 식별을 함께 기록한다.

다음은 M1에서 하지 않는다: restore, calibration, firmware flash/DFU, clear error, control mode/state/input write, persistent save. backup이 되지 않거나 endpoint가 다르면 write로 해결하지 않는다.

### Mac에서 조건부로 사용하는 명령

CLI 자체 확인:

```bash
~/venvs/gim6010/bin/odrivetool --version
~/venvs/gim6010/bin/odrivetool --help
```

interactive shell 실행:

```bash
~/venvs/gim6010/bin/odrivetool
```

실제로 `odrv0`가 발견되고 endpoint가 존재할 때만 다음 값을 읽는다.

```python
odrv0.vbus_voltage
odrv0.ibus
odrv0.hw_version_major
odrv0.hw_version_minor
odrv0.can.config.baud_rate
```

호환 장치에서 정상적으로 읽히고 backup command가 성공할 때만 shell을 종료한 뒤 실행한다.

```bash
mkdir -p ~/Documents/GIM6010-backups
~/venvs/gim6010/bin/odrivetool backup-config   ~/Documents/GIM6010-backups/gim6010_factory_before_changes.json
```

`odrivetool --version` 성공은 Mac 프로그램 실행 확인일 뿐 GIM 호환 확인이 아니다. endpoint가 없으면 write, restore, firmware update로 해결하려 하지 않고 연결 결과를 기록한다.

Motor Wizard는 Windows-only fallback이다. macOS에서는 native로 실행할 수 없으며, Windows GUI-only 항목을 확인해야 하고 판매자가 요구하는 경우에만 별도 Windows PC에서 사용한다.

### 이론 질문

- pole pair 14, encoder offset, electrical angle은 FOC torque에 어떻게 연결되는가?
- 설정 JSON을 다른 firmware나 다른 axis에 복원하면 왜 위험한가?
- read-only discovery의 실패가 왜 firmware update의 근거가 아닌가?

### 통과 기준

- [ ] motion/write 없이 identification endpoint를 읽었거나, 연결 실패를 정확히 기록했다.
- [ ] 생성 성공이 확인된 경우에만 backup path와 version을 보존했다.
- [ ] 이 단계에서 GIM 호환이나 물리 동작 성공을 주장하지 않았다.

**결과 기록**

- `odrivetool` version::
- Device discovery result::
- Hardware/firmware readback::
- Backup path (성공한 경우만)::
- Read-only endpoint notes::
- 관찰/결론::

## 3. M2 — Modern C++ CAN 수신 전용 contract

이 repository의 기본 target은 `BLDC_ENABLE_CAN_TRANSMIT=0`이다. C++17 domain은 native test로 먼저 검증하고, STM32 HAL은 transport adapter에 격리한다.

### code-only verification과 bench를 분리

| 구분 | 지금 실행 가능한 범위 | 주장할 수 없는 것 |
|---|---|---|
| Code-only | standard 11-bit ID range, 8-byte frame shape, little-endian decode, heartbeat freshness, clamp/fault/arm gate의 native test | cable, transceiver, bitrate, GIM firmware 호환, 모터 반응 |
| Compile-only | NUCLEO-F103RB의 C++17 build와 receive-only compile gate | flash, CAN 송신, calibration, physical safety |
| Future bench | 사람이 준비한 jig에서 passive heartbeat/encoder/voltage/current 관찰 | motion 성공; 별도 safety review 전에는 command 송신도 아님 |

### 후보 CAN 계약 — 커뮤니티 주장·bench 확인 대기

- classic CAN standard data frame, 11-bit ID, payload 최대 8 byte
- ID layout: `(node_id << 5) | command_id`; node ID 후보 범위 `0–63`
- little-endian
- factory/default bitrate 후보 `500 kbit/s`, 최대 후보 `1 Mbit/s`
- heartbeat 기본 후보 period `100 ms`, encoder estimate 기본 후보 period `10 ms`
- command ID 후보: heartbeat `0x001`, Estop `0x002`, axis state `0x007`, MIT `0x008`, encoder `0x009`, controller mode `0x00B`, input position/velocity/torque `0x00C/0x00D/0x00E`, limits `0x00F`, Iq `0x014`, bus voltage/current `0x017`, clear errors `0x018`, torque setpoint/estimate `0x01C`

첫 bench는 송신 없이 heartbeat와 encoder timestamp만 관찰한다. cable을 분리한 heartbeat loss도 host가 stale로 판단해야 한다. CAN software Estop은 hardware power cut의 대체물이 아니다.

### 이론 질문

- arbitration ID, 11-bit ID, byte order가 decoder test에 어떻게 나타나는가?
- telemetry period와 host timeout을 같게 두면 왜 위험한가?
- stale timestamp와 fault latch는 motion gate에서 어떤 역할을 하는가?

### 통과 기준

- [ ] native test가 candidate frame과 error branch를 다룬다.
- [ ] firmware 기본값에서 송신 code path가 compile-time 차단된다.
- [ ] `nucleo_f103rb_tx_tutorial`의 operator/runtime gate가 기본 `false`임을 읽고 설명할 수 있다.
- [ ] future bench 계획에 command 없는 passive monitor와 heartbeat timeout 확인이 있다.

**결과 기록**

- Native test evidence::
- Candidate node ID::
- Observed bitrate (bench 후)::
- Observed heartbeat/encoder period (bench 후)::
- Timeout decision::
- 관찰/결론::

## 4. M3 — 제한된 motion은 별도 future bench

M3는 code-only 단계가 아니다. M2의 receive-only observation, timeout, fault, clamp contract가 검증되고 사람이 작성한 안전 검토가 끝난 뒤에만 분리 수행한다.

순서는 ramped velocity → 작은 filtered position → 안전 fixture의 torque → 저게인 MIT다. 각 단계에서 command, measured position/velocity/torque, Iq, bus voltage/current, temperature, heartbeat/fault, timestamp를 함께 보관한다.

### 이론 질문

- cascade current/velocity/position loop와 saturation/anti-windup은 무엇인가?
- torque mode가 왜 low speed에서도 위험할 수 있는가?
- bus voltage가 stop 때 오르면 regen과 power supply는 어떤 관계인가?

### 통과 기준

- [ ] mode마다 command와 feedback graph가 있다.
- [ ] rotor/output unit과 sign을 측정으로 구분했다.
- [ ] timeout, fault, power cut의 서로 다른 역할을 설명할 수 있다.

**결과 기록**

- Fixture::
- Current/velocity/temperature limits::
- Mode::
- Command/feedback log::
- Regen observation::
- 관찰/결론::

## 5. M4–M9 — actuator에서 균형·RL까지

| Module | 먼저 하는 일 | 이론 prompt | 통과 산출물 |
|---|---|---|---|
| M4 식별 | latency, friction, backlash, thermal, torque fixture를 분리 측정 | `τ = F r`, `P = τω`, torque constant와 bus/q-axis current 차이 | actuator model sheet |
| M5 simulation | inverted pendulum의 PD/PID/LQR와 delay/saturation model | `x=[θ, θ̇, p, ṗ]^T`, controllability, linearization | simulated balance |
| M6 2-wheel mule | tether/rail에서 IMU fusion, sign, timeout, fall gate | estimator, deadline, jitter, safety supervisor | tethered balance |
| M7 model port | 실측 mass/COM/inertia/limits/sign/unit로 simulation model 재구성 | actuator adapter와 observation mismatch | GIM-based simulation |
| M8 sim2sim | observation/action/frame-stack 고정, ONNX parity | scale, delay, numerical parity | reproducible rollout |
| M9 sim-to-real | clip, rate limit, tilt/fall guard, process-kill safe stop | domain randomization, handover discontinuity | bounded physical-test plan |

GIM 8:1 gear ratio, MA732 14-bit encoder, pole pair 14, torque constant 후보 `0.47`, `0.44–0.476`, `0.669 N·m/A`은 모두 확정 계약이 아니다. 특히 torque constant 불일치와 CAN Simple의 rotor/output 단위 모호성은 seller manual과 bench로 해결해야 한다.

## 6. 정확한 읽기 순서

### 이 repository: tutorial에서 firmware로

1. `tutorial/README.md` — 전체 학습 순서와 실행 명령
2. `tutorial/01_can_id_and_endian.cpp` — 11-bit ID와 little-endian
3. `tutorial/02_decode_telemetry.cpp` — heartbeat와 encoder decode
4. `tutorial/03_safety_supervisor.cpp` — timeout, fault, explicit arm
5. `tutorial/04_mit_frame_preview.cpp` — 송신 없는 clamp/frame preview
6. `platformio.ini` — native와 NUCLEO C++17/송신 gate
7. `test/test_domain/test_main.cpp` — pure API와 boundary contract
8. `include/bldc/can_frame.hpp`
9. `include/bldc/gim_protocol.hpp`
10. `include/bldc/command_builder.hpp`
11. `include/bldc/safety_supervisor.hpp`
12. `include/bldc/application.hpp`
13. `src/stm32_bxcan_adapter.hpp`
14. `src/stm32_bxcan_adapter.cpp`
15. `src/main.cpp` — receive-only top-level composition
16. `src/tx_tutorial_main.cpp` — 별도 target의 arm edge, clamp, 승인 token, HAL 송신 흐름

### 제조사 계열 SDK: 위험한 test보다 먼저 contract

1. `README.md`
2. `MWSDK/MWMotor.h`
3. `MWSDK/MWMotor.c`
4. `STM32F407IGHSDK/Core/Src/can.c`
5. `STM32G431SDK/USER/Src/CANDrive.c`
6. `STM32G431SDK/USER/Src/MWTest.c`

마지막 `MWTest.c`는 자동 test가 endpoint write, calibration, position/velocity command를 수행할 수 있다는 점을 먼저 확인하기 위한 읽기 대상이다. 이 repository로 복사하거나 자동 실행하지 않는다.

### Reference simulation: action contract부터

1. `README.md`
2. `velocity_env_cfg.py`
3. `flat_env_stand_drive_cfg.py`
4. `flamingo_rev01_5_2.py`
5. `observations.py`
6. `state_handler.py`
7. `play.py`

## 7. 어디를 바꿔야 하는가

| 바꾸려는 X | 우선 변경 위치 | 함께 확인할 계약 | bench 전 금지 |
|---|---|---|---|
| CAN ID/frame decode | `include/bldc/can_frame.hpp`, `include/bldc/gim_protocol.hpp` | `test/test_domain/test_main.cpp` fixture와 11-bit/8-byte 범위 | 추측 ID를 physical bus에 송신 |
| command shape/clamp | `include/bldc/command_builder.hpp` | range, NaN, arm test와 고정 wire scale | limit 없는 command |
| heartbeat/fault state | `include/bldc/safety_supervisor.hpp` | stale timestamp, latch, re-arm test | automatic restart |
| bxCAN timing/filter/RX polling | `src/stm32_bxcan_adapter.hpp`, `src/stm32_bxcan_adapter.cpp` | 500 kbit/s는 후보값; actual kernel clock과 filter | SDK timing 숫자의 무복사 적용 |
| default transmit policy | `platformio.ini` 및 firmware gate | `BLDC_ENABLE_CAN_TRANSMIT=0`, runtime arm/clamp/fresh/fault gate | default 송신 enable |
| motor unit/sign/zero | 제품별 adapter config와 기록표 | rotor/output, direction, zero, gear를 독립적으로 bench 기록 | 8:1을 무조건 곱/나눔 |
| RL action/observation | reference model config와 adapter boundary | physical signal availability, scale, delay | 기존 policy의 drop-in deployment |

## 8. 다음 사람 감독 bench 전 최종 Gate

- [ ] seller manual과 내 hardware/firmware version 확보
- [ ] pinout, polarity, common ground, bus-end-only 120 Ω 확인
- [ ] jig, fuse, accessible hardware power cut 준비
- [ ] regen/bus-voltage monitoring 계획 확인
- [ ] receive-only telemetry와 timeout/fault record 확보
- [ ] command range clamp/runtime arm/fresh heartbeat/fault latch의 code contract 확인
- [ ] motion, restore, calibration, write가 자동 workflow에 없음을 확인

이 항목이 미완료면 M0–M2의 code-only/read-only 기록을 개선한다. 모터를 움직여 빈 칸을 채우지 않는다.
