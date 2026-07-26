# AGENTS.md — BLDC Control 작업 규칙

## 언어와 범위

- Embedded, control, protocol domain은 **Modern C++ 우선**으로 작성한다.
- STM32F1 target은 C++17을 기준으로 한다. 예외와 RTTI에는 의존하지 않는다.
- Python은 RL, 데이터 수집·분석, plotting처럼 host ecosystem이 우세한 작업에만 쓴다. firmware/protocol domain을 Python으로 우회하지 않는다.
- 모터 내부 FOC와 3상 PWM은 이 프로젝트의 구현 범위가 아니다. GIM6010-8은 CAN host가 다루는 smart actuator로 취급한다.

## TDD와 설계

- pure logic은 strict RED/GREEN으로 작업한다. 먼저 native Unity test에 관찰 가능한 계약을 추가하고, 실패(RED)를 확인한 뒤 최소 구현으로 GREEN을 만든다.
- protocol frame, endian, ID 범위, stale timeout, fault latch, command gate의 정상·경계·오류 경로를 test한다.
- hardware HAL은 domain logic과 분리한다. domain test가 STM32 HAL, time source, CAN peripheral을 필요로 하게 만들지 않는다.
- source of truth는 `test/test_domain/test_main.cpp`의 public pure API다. API를 바꿀 때는 test와 call site를 함께 바꾼다.
- 복잡성보다 명시성을 택한다. hidden global state, silent unit conversion, unbounded allocation, speculative abstraction을 피한다.

## 주석과 문서

- 제어·안전·protocol의 의도, 단위, 실패 조건에는 간결한 한국어 교육 주석을 쓴다.
- 사용자가 교육용 코드를 요청한 경우에는 새 tutorial/host adapter 코드의 각 논리 줄에 한 줄 주석을 붙여 실행 흐름을 따라갈 수 있게 한다.
- 단순 production 문법까지 무의미하게 반복 주석하지 말고, line-by-line 주석은 timestamp, frame, unit, coordinate, gate, failure path를 우선한다.
- GIM spec/protocol은 커뮤니티 번역 매뉴얼 근거의 **확인 대기 주장**으로 표기한다. SDK source와 upstream ODrive 문서도 제품 호환 증거로 과장하지 않는다.
- unit이 rotor/output 중 무엇인지, sign, zero, gear ratio는 독립 계약이다. bench 확인 전 임의로 8:1을 곱하거나 나누지 않는다.

## 송신과 안전

- 기본 firmware는 receive-only이다. `BLDC_ENABLE_CAN_TRANSMIT=0`을 기본으로 유지한다.
- 명시적으로 송신을 허용하는 build에서도 compile-time gate만으로 충분하지 않다. runtime에서 아래 조건을 모두 확인한다.
  - 명시적 arm 상태
  - range clamp를 통과한 command
  - fresh heartbeat와 monotonic timestamp
  - fault latch 없음
  - deadline/timeout 유효
- 조건 하나라도 실패하면 command를 만들거나 보내지 않고 safe state를 유지한다.
- software Estop, CAN timeout, fault recovery는 hardware power cut/E-stop을 대체하지 않는다.
- fault가 발생한 뒤 자동 재시작하지 않는다. 원인 기록과 별도 operator 절차를 요구한다.

## Hardware와 automation 경계

- 자동 test, build, CI, agent workflow는 hardware를 조작하지 않는다: flash, CAN transmit, calibration, Motor Wizard/odrivetool write, actuator setting mutation, 전원 인가를 하지 않는다.
- receive-only telemetry decode와 host-side fake transport test를 우선한다.
- physical bench 작업은 사람이 수행하는 별도 절차다. 판매자 pinout, hardware/firmware version, fuse, jig, accessible power cut, common ground, bus-end-only 120 Ω, regen 계획을 먼저 확인한다.
- Motor Wizard는 Windows-only 보조 도구다. macOS native 실행을 전제로 하지 않는다.

## 확인 명령

```bash
pio test -e native
pio test -e native_tx_gate
pio run -e nucleo_f103rb
```

`native`는 pure domain test만, `native_tx_gate`는 `BLDC_ENABLE_CAN_TRANSMIT=1`로 pure test만 컴파일해 HAL·hardware 없이 runtime-disabled, no-heartbeat, unarmed, authorized, stale, faulted branch를 독립 검증한다. `nucleo_f103rb`는 TX-off 기본 firmware의 compile-only target이다. 어느 명령도 hardware 성공, GIM protocol 호환, CAN 송신, physical motion의 증거로 해석하지 않는다.
