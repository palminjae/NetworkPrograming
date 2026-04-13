# 필요한 패키지 설치
sudo apt install iproute2

# NetEM 설정 (예: RTT 100ms + 손실률 0.1%)
sudo tc qdisc add dev eth0 root netem delay 50ms loss 0.1%
# (RTT = 편도 × 2이므로 delay는 50ms)

# 확인
tc qdisc show dev eth0

# 초기화
sudo tc qdisc del dev eth0 root