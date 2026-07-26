"""재현 가능한 TV-LQR/KF host-side 수치 fixture 검증."""

import numpy as np


def check_tv_lqr() -> list[np.ndarray]:
    # 고정된 2-state/1-input fixture로 terminal cost부터 역방향 Riccati를 계산한다.
    a = np.array([[1.0, 1.0], [0.0, 1.0]])
    b = np.array([[0.5], [1.0]])
    q = np.diag([1.0, 0.1])
    r = np.array([[0.5]])
    p = q.copy()
    gains: list[np.ndarray] = []
    for _ in range(3):
        # 입력 cost의 scalar Schur complement를 먼저 계산한다.
        g = r + b.T @ p @ b
        # 현재 단계 feedback gain을 계산하고 다음 단계 cost를 역산한다.
        k = np.linalg.solve(g, b.T @ p @ a)
        gains.append(k[0])
        p = q + a.T @ p @ a - a.T @ p @ b @ np.linalg.solve(g, b.T @ p @ a)
    expected = [
        np.array([0.5882353, 0.7058824]),
        np.array([0.6382979, 1.1063830]),
        np.array([0.5959596, 1.1078905]),
    ]
    for actual, wanted in zip(gains, expected):
        np.testing.assert_allclose(actual, wanted, atol=1e-5)
    return gains


def check_kalman() -> tuple[np.ndarray, np.ndarray]:
    # 같은 2-state fixture로 predict와 Joseph covariance update를 재현한다.
    a = np.array([[1.0, 1.0], [0.0, 1.0]])
    b = np.array([[0.5], [1.0]])
    c = np.array([[1.0, 0.0]])
    q = np.diag([0.01, 0.1])
    r = np.array([[0.25]])
    x = np.array([0.0, 0.0])
    p = np.eye(2)
    x = a @ x + b[:, 0]
    p = a @ p @ a.T + q
    innovation = 0.8 - (c @ x)[0]
    innovation_variance = c @ p @ c.T + r
    k = p @ c.T @ np.linalg.inv(innovation_variance)
    identity = np.eye(2)
    x = x + k[:, 0] * innovation
    p = (identity - k @ c) @ p @ (identity - k @ c).T + k @ r @ k.T
    np.testing.assert_allclose(x, [0.7668139, 1.1327434], atol=1e-5)
    np.testing.assert_allclose(
        p,
        [[0.222345, 0.110619], [0.110619, 0.657522]],
        atol=1e-5,
    )
    return x, p


def main() -> None:
    # 두 fixture의 성공 여부와 dependency version을 stdout에 남긴다.
    gains = check_tv_lqr()
    state, covariance = check_kalman()
    print("host numerical fixtures: PASSED")
    print(f"numpy={np.__version__}")
    print(f"K[0..2]={gains[2]}, {gains[1]}, {gains[0]}")
    print(f"x+={state}")
    print(f"P+={covariance}")


if __name__ == "__main__":
    main()
