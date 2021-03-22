# Device Driver

## 7 Segment Device Driver with sysfs attribute
### 구성파일
1. main.c
  - Device Driver 핵심 코드 저장
2. gpio.c
  - gpio관련 설정 함수 저장

### 동작방식
  1. device number 할당
  2. device file <-> VFS <-> cdev  연결을 위해 cdev 생성
  4. /sys/class 하위에 "7segment" 클래스 생성
  5. platform_driver 등록 및 probe함수 호출
  6. gpio_desc를 이용한 gpio direction설정
  7. /sys/class/7segment/ 하위에 "7segment_attr" 속성과 "number" 속성파일 생성 및 /dev/ 하위에 device file "7segment_attr" 디바이스파일 생성
  8. 0을 출력하고 종료

### 사용법
#### sysfs하위 attribute 이용
 1. echo "number" > /sys/class/7segment/7segment-attrs/number
 2. cat /sys/class/7segment/7segment-attrs/number
<img width="608" alt="sysfs" src="https://user-images.githubusercontent.com/50316116/111625751-8ff5f200-8830-11eb-8510-064ddc37aab4.png">

#### /dev 하위 device file 이용

### 001 ~ 007 및 beaglebone black은 공부기록 또는 미완성 프로젝트 입니다
