/*
    Arduino Uno Smart Farm

    The circuit:
    * Arduino Uno A0 ─ soil moisture sensor module
    * Arduino Uno A1 ─ CdS photo resistor module
    * Arduino Uno A4(SDA) ─ LCD SDA and AHT10 temp·humi sensor module SDA
    * Arduino Uno A5(SCL) ─ LCD SCL and AHT10 temp·humi sensor module SCL
    * Arduino Uno 13 ─ plant LED bar
    * Arduino Uno 4 ─ 2ch motor driver B-IB
    * Arduino Uno 5 ─ 2ch motor driver B-IA
    * Arduino Uno 6 ─ 2ch motor driver A-IA
    * Arduino Uno 7 ─ 2ch motor driver A-IB
    * 2ch motor driver Motor A(+) ─ cooling fan (+)
    * 2ch motor driver Motor A(-) ─ cooling fan (-)
    * 2ch motor driver Motor B(+) ─ water pump motor (+)
    * 2ch motor driver Motor B(-) ─ water pump motor (-)

    https://github.com/makershands/smartFarm-Arduino

*/

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <AHT10.h>

#define B_IB 4       // 모터드라이버 A_1B 단자 연결 핀번호(워터펌프)
#define B_IA 5       // 모터드라이버 A_1A 단자 연결 핀번호
#define A_IA 6       // 모터드라이버 B_1A 단자 연결 핀번호(팬)
#define A_IB 7       // 모터드라이버 B_1B 단자 연결 핀번호

LiquidCrystal_I2C lcd(0x27, 16, 2); // LCD의 I2C 주소(0x27)와 행 수(2), 열 수(16)를 지정함
AHT10Class AHT10;                   // AHT10 클래스를 선언함

int SOIL_HUMI = A0;       // 토양 수분 센서 연결 핀번호를 지정함
int cds_pin = A1;         // 조도 센서에 사용할 핀 번호 지정
int led_pin = 13;         // LED에 사용할 핀 번호 지정
int soil, psoil;          // 수분 센서 값을 저장하기 위한 변수 선언
int val, cdsval, pcdsval; // 조도 센서 값을 저장하기 위한 변수 선언
float temp, humi = 0.0;   // 온습도 센서 값을 저장하기 위한 변수 선언

int waterpumpPower = 150; // 워터 펌프 모터의 기준값 지정
int fanPower = 150;       // 쿨링 팬의 기준값 지정

void setup() {
  /* AHT10 초기 설정 */
  Wire.begin();                                                             // I2C 통신을 시작함
  if(AHT10.begin(eAHT10Address_Low)) Serial.println("Init AHT10 Success."); // I2C 통신이 시작되면 메시지를 출력함
  else                               Serial.println("Init AHT10 Failure."); // I2C 통신에 실패하면 메시지를 출력함

  /* LCD 초기 설정 */
  lcd.init();      // LCD를 초기화함
  lcd.backlight(); // LCD의 백라이트를 켬
  /* 1행과 2행에 각각 메시지를 출력함 */
  lcd.setCursor(0,0); lcd.print("Hello,SmartFarm!");
  lcd.setCursor(0,1); lcd.print("Made by M2Hands!");

  /* 시리얼 통신 설정 */
  Serial.begin(9600);          // 9,600 baud의 속도로 시리얼 통신을 시작함
  Serial.println("Let's Go!"); // 시리얼 모니터에 "Let's Go!"를 출력함
  delay(1000);                 // 1,000 ms동안 대기

  /* 모터 드라이버의 핀 모드를 출력으로 설정함 */
  pinMode(A_IA, OUTPUT);
  pinMode(A_IB, OUTPUT);
  pinMode(B_IA, OUTPUT);
  pinMode(B_IB, OUTPUT);

  /* 모터 드라이버의 핀의 출력을 모두 끔 */
  analogWrite(A_IA, LOW);
  analogWrite(A_IB, LOW);
  analogWrite(B_IA, LOW);
  analogWrite(B_IB, LOW);

  pinMode(led_pin, OUTPUT); // LED의 핀 모드를 출력으로 설정함
}

void loop() {
  /* 온습도 센서 값을 받아서 변수에 저장함 */
  temp = AHT10.GetTemperature();
  humi = AHT10.GetHumidity();

  soil = analogRead(SOIL_HUMI);       // A0에서 읽은 값을 soil 변수에 저장
  psoil = map(soil, 1023, 0, 100, 0); // map함수를 사용하여 soil값을 1~100으로 변환한 값을 psoil에 저장
  val = analogRead(cds_pin);          // A1에서 읽은 값을 val 변수에 저장
  cdsval = map(val,0, 1023, 250, 0);  // map함수를 사용하여 val값을 1~250으로 변환한 값을 cdsval에 저장
  pcdsval = cdsval*0.4;               // 조도센서값을 0~100으로 표시하기 위한 설정
  
  lcd.init();      // LCD 초기화 init() 명령이 안먹으면 begin으로 수정
  lcd.clear();     // 이전에 출력한 값 지우기
  lcd.backlight(); // 배경화면 빛이 들어오도록 설정
  lcd.display();   // 내용을 표시

  /* LCD의 커서를 1행 1열에 놓고 "M: {토양 수분 센서 값}%"와 같이 출력 */
  lcd.setCursor(0, 0); lcd.print("M: "); lcd.print(psoil); lcd.print("%");
  /* LCD의 커서를 1행 10열에 놓고 "L: {조도 센서 값}%"와 같이 출력 */
  lcd.setCursor(9, 0); lcd.print("L: "); lcd.print(pcdsval); lcd.print("%");
  /* LCD의 커서를 2행 1열에 놓고 "T: {온도 센서 값}C"와 같이 출력 */
  lcd.setCursor(0, 1); lcd.print("T: "); lcd.print(temp, 1); lcd.print("C");
  /* LCD의 커서를 2행 10열에 놓고 "H: {습도 센서 값}%"와 같이 출력 */
  lcd.setCursor(9, 1); lcd.print("H: "); lcd.print(humi, 0); lcd.print("%");

  /* 시리얼 모니터에 토양 수분 센서, 조도 센서, 온습도 센서 값을 출력함 */
  Serial.print("수분: "); Serial.print(psoil);
  Serial.print("\t조도: "); Serial.print(cdsval);
  Serial.print("\t온도: "); Serial.print(temp);
  Serial.print("\t습도: "); Serial.println(humi);
  delay(1000);

  /* 토양 수분 센서 값이 30% 미만일 때 실행 */
  if(psoil < 30) {
    analogWrite(B_IA, waterpumpPower);
    digitalWrite(B_IB, LOW);    
  } else {
    digitalWrite(B_IA, LOW);
    digitalWrite(B_IB, LOW);
  }

  /* 온도 센서 값이 20도C 이상이거나 습도 센서 값이 60% 이상일 때 실행 */
  if(temp >= 24 || humi >= 60) {
    analogWrite(A_IA, fanPower); // 값을 변화시키면서 팬의 세기를 설정(0~255)
    digitalWrite(A_IB, LOW);
  } else { /*  */
    digitalWrite(A_IA, LOW);
    digitalWrite(A_IB, LOW);
  }

  if (pcdsval < 70) digitalWrite(led_pin, HIGH); // 조도 센서 값이 70% 미만이면 LED를 켬
  else              digitalWrite(led_pin, LOW);  // 아니면(조도 센서 값이 70% 이상이면) LED를 끔
}
