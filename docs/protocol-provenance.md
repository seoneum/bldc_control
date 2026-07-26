# GIM6010-8 protocol provenance와 bench 확인 목록

## 판정 규칙

이 표는 **문서가 무엇을 말하는지**를 추적한다. 표의 GIM 값은 제품에서 측정된 사실이 아니다.

- **커뮤니티 주장·bench 확인 대기**: `robertbergman2/GIM6010-8` 번역 매뉴얼. 판매자 원문, hardware/firmware별 문서, 내 제품 readback/bench가 우선한다.
- **SDK source-confirmed (SDK 한정)**: 해당 commit의 SDK source에 존재하는 구현/예제. 이 SDK가 내 GIM firmware에 맞거나 안전하다는 증거는 아니다.
- **upstream ODrive 보조 근거 (비-GIM)**: ODrive 공식 지원 surface의 기능 문서. GIM 호환성·endpoint·설정 형식을 보장하지 않는다.
- **bench confirmed**: 제품 식별과 보존된 log가 있을 때만 부여한다. 현재 이 문서에는 해당 주장이 없다.

## 주장별 원전 표

| URL | Commit | Path | Section | Claim | Confidence |
|---|---|---|---|---|---|
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/02-motor-specs.typ | `a80131f` | `src/02-motor-specs.typ` | electrical/mechanical characteristics tables | GIM6010-8은 8:1 감속기와 pole pair 14로 설명된다. 정격 120 rpm, 최대 420 rpm, 정격 5 N·m, stall 11 N·m, 정격 10.5 A, stall 25 A도 이 표의 주장이다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/03-driver-info.typ | `a80131f` | `src/03-driver-info.typ` | main components table | magnetic encoder chip을 MA732 14-bit absolute로 설명한다. single-turn 여부와 실제 장착 위치·출력축 단위는 제품별 확인이 필요하다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/04-debugging.typ | `a80131f` | `src/04-debugging.typ` | torque control example / torque-constant setup | 사양표의 `0.47 N·m/A`와 별도로 예시는 `8.23 / 12.3 ≈ 0.669 N·m/A`를 사용하고, 정격/실속 수치 역산값은 약 `0.44–0.476 N·m/A`다. 어느 값도 제품 설정값으로 채택하지 않는다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/03-driver-info.typ | `a80131f` | `src/03-driver-info.typ` | driver specifications and interface table | driver rated voltage는 15–48 VDC, min/max 후보는 12/72 VDC, CAN 최대는 1 Mbit/s라고 설명한다. Type-C는 configuration/debug용이며 motor power 공급원이 아니다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/03-driver-info.typ | `a80131f` | `src/03-driver-info.typ` | thermal/brake/limit-switch interface notes | motor/driver 10K NTC와 기본 90°C alarm 조정 가능성, external brake resistor, limit switch interface를 설명한다. hardware version별 지원 차이는 제품별 확인이 필요하다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/04-debugging.typ | `a80131f` | `src/04-debugging.typ` | USB setup, `odrivetool`, calibration | ODrive-compatible USB interface와 `odrivetool` 기반 discovery/calibration 흐름, GIM-specific endpoint/example을 제시한다. 이는 USB endpoint/descriptor와 모든 GIM firmware의 호환 보증이 아니다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/04-debugging.typ | `a80131f` | `src/04-debugging.typ` | unit convention notes | USB position/velocity/torque는 rotor-side, CAN MIT position/velocity/torque는 output-side라고 설명하는 반면, 일반 CAN Simple encoder estimate 설명에는 rotor position/velocity가 있다. 실물 unit mapping은 미해결이다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/05-can-protocol.typ | `a80131f` | `src/05-can-protocol.typ` | CAN Simple frame layout / introduction | classic standard CAN data frame, 11-bit arbitration ID, 8-byte payload, little-endian, `CAN_ID = (node_id << 5) \| cmd_id`, node ID 후보 `0–63`을 제시한다. CAN 최대 1 Mbit/s와 factory default 500 kbit/s도 이 파일의 주장이다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/05-can-protocol.typ | `a80131f` | `src/05-can-protocol.typ` | periodic telemetry | heartbeat 기본 후보 period는 100 ms, encoder estimate 기본 후보 period는 10 ms다. host timeout은 이 수치가 아니라 measured jitter와 failure policy로 정해야 한다. | 커뮤니티 주장·bench 확인 대기 |
| https://github.com/robertbergman2/GIM6010-8/blob/a80131f/src/05-can-protocol.typ | `a80131f` | `src/05-can-protocol.typ` | command ID table | 후보 command ID: heartbeat `0x001`, software Estop `0x002`, axis state `0x007`, MIT `0x008`, encoder `0x009`, controller mode `0x00B`, input position/velocity/torque `0x00C/0x00D/0x00E`, limits `0x00F`, Iq `0x014`, bus voltage/current `0x017`, clear errors `0x018`, torque setpoint/estimate `0x01C`. | 커뮤니티 주장·bench 확인 대기 |
| https://gitee.com/cyberbeast/mwmotorsdk/blob/a3cbea086b6063dd9ecdc7b135911ce02e159712/README.md | `a3cbea086b6063dd9ecdc7b135911ce02e159712` | `README.md` | STM32 examples / CAN setup | SDK README는 F407 bxCAN과 G431 FDCAN example, 500 Kbps, receive interrupt, CubeMX/HAL 구성을 설명한다. 다른 board의 CAN kernel clock/bit timing에는 그대로 적용할 수 없다. | SDK source-confirmed (SDK 한정) |
| https://gitee.com/cyberbeast/mwmotorsdk/blob/a3cbea086b6063dd9ecdc7b135911ce02e159712/MWSDK/MWMotor.h | `a3cbea086b6063dd9ecdc7b135911ce02e159712` | `MWSDK/MWMotor.h` | `FIRMWARE_VERSION_NUMBER`, motor access and API declarations | firmware version macro, motor access information, sender/notifier contract와 controller/axis/position/velocity API가 정의되어 있다. SDK default array 범위는 protocol node range와 다를 수 있으므로 node ID 16 이상은 macro/array를 별도 검토한다. | SDK source-confirmed (SDK 한정) |
| https://gitee.com/cyberbeast/mwmotorsdk/blob/a3cbea086b6063dd9ecdc7b135911ce02e159712/STM32F407IGHSDK/Core/Src/can.c | `a3cbea086b6063dd9ecdc7b135911ce02e159712` | `STM32F407IGHSDK/Core/Src/can.c` | `CanFilter_Init`, `HAL_CAN_Start`, RX FIFO notification | F407 example는 filter, peripheral start, RX FIFO interrupt의 bxCAN bring-up 순서를 보인다. NUCLEO-F103RB port는 이 파일의 pin/clock/prescaler를 복사하지 않고 F103 clock과 Cube HAL에 맞춰 계산한다. | SDK source-confirmed (SDK 한정) |
| https://gitee.com/cyberbeast/mwmotorsdk/blob/a3cbea086b6063dd9ecdc7b135911ce02e159712/STM32G431SDK/USER/Src/MWTest.c | `a3cbea086b6063dd9ecdc7b135911ce02e159712` | `STM32G431SDK/USER/Src/MWTest.c` | `MWFunctionTest()` | example는 endpoint 95에 `50.0`을 쓰고 motor/encoder calibration 뒤 compile option에 따라 position `10` 또는 velocity `10` command를 낼 수 있다. 이것은 passive monitor나 safe default가 아니다. | SDK source-confirmed (SDK 한정) |
| https://docs.odriverobotics.com/v/latest/interfaces/odrivetool.html | upstream current docs | N/A | `odrivetool` interface and CLI commands | 공식 ODrive 문서는 `odrivetool` command-line tool, `backup-config`, `restore-config`, `liveplotter` 등의 ODrive surface 기능을 설명한다. | upstream ODrive 보조 근거 (비-GIM) |
| https://docs.odriverobotics.com/v/latest/guides/odrivetool-setup.html | upstream current docs | N/A | setup / installation | 공식 ODrive 문서는 host 설치와 interactive tool setup을 설명한다. macOS host에서 ODrive tool 설치가 가능한 것과 GIM device 호환은 별개다. | upstream ODrive 보조 근거 (비-GIM) |
| https://docs.odriverobotics.com/v/latest/manual/control.html | upstream current docs | N/A | control modes and control structure | 공식 ODrive 문서는 controller mode/cascade 개념을 설명한다. GIM의 mode ID, units, limits, endpoint semantics를 확정하지 않는다. | upstream ODrive 보조 근거 (비-GIM) |

## 첫 접근의 보수적 결론

1. GIM community translation은 ODrive-compatible USB interface와 GIM-specific endpoint/example을 **주장**한다.
2. ODrive 공식 문서는 지원 ODrive surface에서의 `odrivetool` capability만 입증한다.
3. 따라서 GIM에서는 read-only discovery가 먼저다. 연결되어 실제 identity/voltage/error endpoint를 읽을 수 있는지 기록한다.
4. `backup-config`는 **그 GIM firmware에서 성공적으로 생성됨을 확인할 수 있을 때만** 변경 전 보관 용도로 쓴다.
5. 첫 접근에서 restore, calibration, firmware flash/DFU, persistent save, control state/mode/input write, clear-error를 하지 않는다.
6. Motor Wizard는 Windows-only fallback이다. macOS에서는 native로 실행할 수 없으며, GUI-only 요구가 검증되었을 때 실제 Windows PC에서만 고려한다.

이 결론은 physical success를 주장하지 않는다. read-only discovery나 backup 결과가 아직 없으면 그 자체가 미확인 상태다.

## bench에서 해소할 미확정 항목

### 제품 식별·전원·배선

- [ ] 판매자에게 받은 최신 원문 manual과 정확한 hardware/firmware version
- [ ] 내 옵션의 CAN/RS485/CANOpen 지원 여부
- [ ] XT30 2+2의 24 V+, GND, CANH, CANL pinout과 polarity
- [ ] rated/min/max voltage(15–48 VDC, 12/72 VDC 후보)의 해당 제품 적용 여부
- [ ] CAN default 500 kbit/s와 max 1 Mbit/s 후보의 actual baud 및 bit timing
- [ ] external 3.3 V transceiver, common ground, bus-end-only 120 Ω의 실제 wiring

### telemetry·protocol·watchdog

- [ ] node ID와 `(node_id << 5) | cmd_id` layout
- [ ] classic 11-bit/8-byte frame, little-endian, 각 command ID와 payload layout
- [ ] heartbeat 100 ms와 encoder 10 ms 후보의 measured period/jitter
- [ ] host timestamp 기준 stale timeout와 safe action
- [ ] device-side host-command watchdog/timeout의 존재, setting, restart behavior
- [ ] heartbeat loss, fault, CAN cable removal 후 automatic restart 금지

### actuator 계약

- [ ] MA732, pole pair 14, 8:1 gearing의 제품 적용 여부
- [ ] USB/CAN Simple/MIT 각각의 rotor-side vs output-side position/velocity/torque unit
- [ ] joint direction, motor sign, user zero를 gear ratio와 독립적으로 기록
- [ ] torque constant `0.47`, `0.44–0.476`, `0.669 N·m/A` 충돌의 seller answer와 low-load fixture 대조
- [ ] feedback current가 bus current인지 q-axis current인지와 torque estimate 의미
- [ ] current/velocity/thermal limits와 motor/driver alarm threshold

### motion 전 안전

- [ ] shaft fixture, fuse, 접근 가능한 hardware power cut/E-stop
- [ ] power supply/BMS의 regen absorption capability
- [ ] brake resistor 또는 regen clamp의 resistance/power/threshold
- [ ] stop 시 bus voltage/current logging 및 over-voltage stop condition
- [ ] explicit runtime arm, command range clamp, fresh heartbeat, fault latch가 모두 true일 때만 future command 후보가 되도록 구현

## 금지된 추론

- "ODrive tool이 Mac에서 실행된다" → "GIM이 호환된다"로 추론하지 않는다.
- community CAN table → 내 motor의 command permission으로 추론하지 않는다.
- SDK example source → F103 bit timing, pinout, safety의 정답으로 추론하지 않는다.
- software Estop → hardware E-stop/contactor와 동등하다고 추론하지 않는다.
- GIM spec 숫자 → sim-to-real actuator limit 또는 physical success로 추론하지 않는다.
