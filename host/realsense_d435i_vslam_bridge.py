#!/usr/bin/env python3
"""Capture D435i-family RGB-D and IMU samples for an external VSLAM backend.

This program intentionally stops at the sensor/host contract.  RTAB-Map,
ORB-SLAM3, or another VSLAM engine consumes the RGB-D/IMU stream and publishes
a pose record that matches bldc::state::VslamPose.
"""

# Command-line parsing keeps the capture tool reproducible.
# 필요한 표준·외부 모듈을 가져온다.
import argparse
# JSON Lines makes each timestamped sensor sample easy to inspect and replay.
# 필요한 표준·외부 모듈을 가져온다.
import json
# stdout is used when the caller wants to pipe the stream to another process.
# 필요한 표준·외부 모듈을 가져온다.
import sys
# Type annotations make the host-side contract easier to read.
# 필요한 표준·외부 모듈을 가져온다.
from typing import Any, TextIO


# 재사용할 함수·비동기 동작·자료 구조를 선언한다.
def parse_args() -> argparse.Namespace:
    # Create the command-line parser with a clear purpose.
# 입력을 변환하거나 상태·결과를 갱신한다.
    parser = argparse.ArgumentParser(
# 입력을 변환하거나 상태·결과를 갱신한다.
        description="Capture D435i RGB-D and IMU records for VSLAM input."
# 입력을 변환하거나 상태·결과를 갱신한다.
    )
    # Zero means capture until Ctrl-C, while a positive value is useful in tests.
# 입력을 변환하거나 상태·결과를 갱신한다.
    parser.add_argument("--frames", type=int, default=0)
    # A dash sends JSONL to stdout instead of creating a file.
# 입력을 변환하거나 상태·결과를 갱신한다.
    parser.add_argument("--output", default="-")
    # The color stream resolution is kept conservative for the first bring-up.
# 입력을 변환하거나 상태·결과를 갱신한다.
    parser.add_argument("--width", type=int, default=640)
    # The color stream height is paired with the selected width.
# 입력을 변환하거나 상태·결과를 갱신한다.
    parser.add_argument("--height", type=int, default=480)
    # Thirty RGB-D frames per second is a practical first capture rate.
# 입력을 변환하거나 상태·결과를 갱신한다.
    parser.add_argument("--fps", type=int, default=30)
    # Return the parsed configuration to the caller.
# 호출자에게 결과를 반환하거나 예외를 알린다.
    return parser.parse_args()


# 재사용할 함수·비동기 동작·자료 구조를 선언한다.
def emit(output: TextIO, record: dict[str, Any]) -> None:
    # Serialize one complete record so a partial line is never emitted.
# 입력을 변환하거나 상태·결과를 갱신한다.
    output.write(json.dumps(record, ensure_ascii=False, separators=(",", ":")))
    # Flush immediately so a downstream VSLAM process sees sensor timing.
# 입력을 변환하거나 상태·결과를 갱신한다.
    output.write("\n")
    # Flush is important when stdout is connected to a pipe.
# 입력을 변환하거나 상태·결과를 갱신한다.
    output.flush()


# 재사용할 함수·비동기 동작·자료 구조를 선언한다.
def motion_record(kind: str, frame: Any) -> dict[str, Any] | None:
    # A missing motion frame is a normal condition during startup/dropout.
# 조건·반복·예외·자원 흐름을 처리한다.
    if frame is None:
# 호출자에게 결과를 반환하거나 예외를 알린다.
        return None
    # Convert the RealSense motion vector to plain Python floats.
# 입력을 변환하거나 상태·결과를 갱신한다.
    motion = frame.get_motion_data()
    # Keep the device timestamp in milliseconds as provided by the SDK.
# 입력을 변환하거나 상태·결과를 갱신한다.
    timestamp_ms = float(frame.get_timestamp())
    # Return an explicit sensor kind so replay code does not depend on order.
# 호출자에게 결과를 반환하거나 예외를 알린다.
    return {
# 현재 처리 단계의 도메인 동작을 수행한다.
        "type": kind,
# 현재 처리 단계의 도메인 동작을 수행한다.
        "timestamp_ms": timestamp_ms,
# 현재 처리 단계의 도메인 동작을 수행한다.
        "x": float(motion.x),
# 현재 처리 단계의 도메인 동작을 수행한다.
        "y": float(motion.y),
# 현재 처리 단계의 도메인 동작을 수행한다.
        "z": float(motion.z),
# 입력을 변환하거나 상태·결과를 갱신한다.
        "units": "m/s^2" if kind == "accel" else "rad/s",
# 현재 처리 단계의 도메인 동작을 수행한다.
    }


# 재사용할 함수·비동기 동작·자료 구조를 선언한다.
def rgbd_record(color: Any, depth: Any) -> dict[str, Any] | None:
    # VSLAM needs synchronized color and depth timestamps, not only frame counts.
# 조건·반복·예외·자원 흐름을 처리한다.
    if color is None or depth is None:
# 호출자에게 결과를 반환하거나 예외를 알린다.
        return None
    # Record metadata first; pixel transport can be attached by the selected backend.
# 호출자에게 결과를 반환하거나 예외를 알린다.
    return {
# 현재 처리 단계의 도메인 동작을 수행한다.
        "type": "rgbd",
# 현재 처리 단계의 도메인 동작을 수행한다.
        "color_timestamp_ms": float(color.get_timestamp()),
# 현재 처리 단계의 도메인 동작을 수행한다.
        "depth_timestamp_ms": float(depth.get_timestamp()),
# 현재 처리 단계의 도메인 동작을 수행한다.
        "color_frame_number": int(color.get_frame_number()),
# 현재 처리 단계의 도메인 동작을 수행한다.
        "depth_frame_number": int(depth.get_frame_number()),
# 현재 처리 단계의 도메인 동작을 수행한다.
        "color_profile": str(color.get_profile()),
# 현재 처리 단계의 도메인 동작을 수행한다.
        "depth_profile": str(depth.get_profile()),
# 현재 처리 단계의 도메인 동작을 수행한다.
    }


# 재사용할 함수·비동기 동작·자료 구조를 선언한다.
def main() -> int:
    # Import the vendor SDK only when the capture command is actually executed.
# 조건·반복·예외·자원 흐름을 처리한다.
    try:
# 필요한 표준·외부 모듈을 가져온다.
        import pyrealsense2 as rs
# 조건·반복·예외·자원 흐름을 처리한다.
    except ImportError as exc:
        # Make the missing optional dependency actionable without hiding the cause.
# 호출자에게 결과를 반환하거나 예외를 알린다.
        raise SystemExit(
# 현재 처리 단계의 도메인 동작을 수행한다.
            "pyrealsense2 is not installed; install Intel RealSense SDK Python bindings first."
# 현재 처리 단계의 도메인 동작을 수행한다.
        ) from exc

    # Parse the requested capture configuration.
# 입력을 변환하거나 상태·결과를 갱신한다.
    args = parse_args()
    # Create the SDK pipeline that owns the D435i USB device.
# 입력을 변환하거나 상태·결과를 갱신한다.
    pipeline = rs.pipeline()
    # Build a stream configuration rather than relying on device defaults.
# 입력을 변환하거나 상태·결과를 갱신한다.
    config = rs.config()
    # Enable the RGB stream used by feature tracking and loop closure.
# 현재 처리 단계의 도메인 동작을 수행한다.
    config.enable_stream(
# 현재 처리 단계의 도메인 동작을 수행한다.
        rs.stream.color, args.width, args.height, rs.format.bgr8, args.fps
# 입력을 변환하거나 상태·결과를 갱신한다.
    )
    # Enable the depth stream used by RGB-D odometry and scale recovery.
# 현재 처리 단계의 도메인 동작을 수행한다.
    config.enable_stream(
# 현재 처리 단계의 도메인 동작을 수행한다.
        rs.stream.depth, args.width, args.height, rs.format.z16, args.fps
# 입력을 변환하거나 상태·결과를 갱신한다.
    )
    # Enable the high-rate gyroscope stream used for rotational propagation.
# 입력을 변환하거나 상태·결과를 갱신한다.
    config.enable_stream(rs.stream.gyro, rs.format.motion_xyz32f, 200)
    # Enable the accelerometer stream used for gravity and pitch/roll correction.
# 입력을 변환하거나 상태·결과를 갱신한다.
    config.enable_stream(rs.stream.accel, rs.format.motion_xyz32f, 63)

    # Select stdout or a user-provided JSONL file.
# 현재 처리 단계의 도메인 동작을 수행한다.
    output: TextIO
# 입력을 변환하거나 상태·결과를 갱신한다.
    close_output = False
# 조건·반복·예외·자원 흐름을 처리한다.
    if args.output == "-":
        # Use stdout for shell pipelines and manual inspection.
# 입력을 변환하거나 상태·결과를 갱신한다.
        output = sys.stdout
# 조건·반복·예외·자원 흐름을 처리한다.
    else:
        # Open the requested capture file with UTF-8 text encoding.
# 입력을 변환하거나 상태·결과를 갱신한다.
        output = open(args.output, "w", encoding="utf-8")
        # Remember to close the file after the pipeline stops.
# 입력을 변환하거나 상태·결과를 갱신한다.
        close_output = True

    # Start all configured D435i streams.
# 입력을 변환하거나 상태·결과를 갱신한다.
    pipeline.start(config)
    # Count synchronized framesets rather than individual gyro samples.
# 입력을 변환하거나 상태·결과를 갱신한다.
    captured_frames = 0
# 조건·반복·예외·자원 흐름을 처리한다.
    try:
        # Continue until the requested count or Ctrl-C.
# 조건·반복·예외·자원 흐름을 처리한다.
        while args.frames <= 0 or captured_frames < args.frames:
            # Wait for the next synchronized frame set from the device.
# 입력을 변환하거나 상태·결과를 갱신한다.
            frames = pipeline.wait_for_frames()
            # Read the color image metadata for the VSLAM front-end.
# 입력을 변환하거나 상태·결과를 갱신한다.
            color = frames.get_color_frame()
            # Read the depth image metadata for RGB-D geometry.
# 입력을 변환하거나 상태·결과를 갱신한다.
            depth = frames.get_depth_frame()
            # Read the latest gyro frame in this frameset when available.
# 입력을 변환하거나 상태·결과를 갱신한다.
            gyro = frames.first_or_default(rs.stream.gyro)
            # Read the latest accelerometer frame in this frameset when available.
# 입력을 변환하거나 상태·결과를 갱신한다.
            accel = frames.first_or_default(rs.stream.accel)
            # Emit the synchronized RGB-D metadata record first.
# 입력을 변환하거나 상태·결과를 갱신한다.
            rgbd = rgbd_record(color, depth)
            # Avoid emitting an incomplete RGB-D record.
# 조건·반복·예외·자원 흐름을 처리한다.
            if rgbd is not None:
# 입력을 변환하거나 상태·결과를 갱신한다.
                emit(output, rgbd)
            # Convert and emit the gyro sample if this frameset contains one.
# 입력을 변환하거나 상태·결과를 갱신한다.
            gyro_json = motion_record("gyro", gyro)
            # Avoid emitting a missing gyro sample.
# 조건·반복·예외·자원 흐름을 처리한다.
            if gyro_json is not None:
# 입력을 변환하거나 상태·결과를 갱신한다.
                emit(output, gyro_json)
            # Convert and emit the accelerometer sample if this frameset contains one.
# 입력을 변환하거나 상태·결과를 갱신한다.
            accel_json = motion_record("accel", accel)
            # Avoid emitting a missing accelerometer sample.
# 조건·반복·예외·자원 흐름을 처리한다.
            if accel_json is not None:
# 입력을 변환하거나 상태·결과를 갱신한다.
                emit(output, accel_json)
            # Count one completed frameset.
# 입력을 변환하거나 상태·결과를 갱신한다.
            captured_frames += 1
# 조건·반복·예외·자원 흐름을 처리한다.
    except KeyboardInterrupt:
        # Ctrl-C is a normal way to stop a live capture.
# 현재 처리 단계의 도메인 동작을 수행한다.
        pass
# 조건·반복·예외·자원 흐름을 처리한다.
    finally:
        # Stop USB streaming before closing the output file.
# 입력을 변환하거나 상태·결과를 갱신한다.
        pipeline.stop()
        # Close only files opened by this process.
# 조건·반복·예외·자원 흐름을 처리한다.
        if close_output:
# 입력을 변환하거나 상태·결과를 갱신한다.
            output.close()

    # Report successful capture shutdown to the shell.
# 호출자에게 결과를 반환하거나 예외를 알린다.
    return 0


# Run the CLI only when this file is executed directly.
# 조건·반복·예외·자원 흐름을 처리한다.
if __name__ == "__main__":
    # Exit with the return code from main.
# 호출자에게 결과를 반환하거나 예외를 알린다.
    raise SystemExit(main())
