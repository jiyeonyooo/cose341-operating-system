# Operating System
고려대학교 COSE341 운영체제 수업 중 진행했던 개인 과제에 대한 레포지토리 입니다.
### 개발 환경
- OS: Window 10
- Virtual Box 7.0.20
- Ububtu 18.04.2
- Linux 4.20.11

<br>

## 1. System Call - 커널에 사용자 정의 System Call 추가
Linux 커널을 직접 빌드한 뒤 커널 공간에 중복이 허용되지 않는 정수 큐를 구현하고, 이를 호출하는 사용자 정의 시스템 콜을 추가 및 검증한 과제입니다.
#### 핵심 기능
- 커널 공간의 정수 큐 구현
- Enqueue/Dequeue 시스템콜 구현

#### 결과 보고서
[Kernel System call의 구현과 이해](system-call/report.pdf)

<br>
  
## 2. Virtual CPU -커널 가상 CPU 구현과 스케줄링 정책
커널 공간에서 싱글 코어 가상 CPU 객체를 구현하고 다양한 스케줄링 정책에 따른 응답시간과 대기시간을 비교하는 과제입니다.
#### 핵심 기능
- 커널 공간의 가상 CPU 객체 생성
- 스케줄링 정책 지원
  - FCFS (First-Come, First-Served)
  - SRTF (Shortest Remaining Time First)
  - RR (Round Robin)
  - Priority with Preemption
- CPU 스케줄링 정책 별 응답/대기 시간 측정

#### 결과 보고서
[가상 CPU & 스케줄링 – 구현과 분석](virtual-cpu/report.pdf)
