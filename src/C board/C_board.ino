#include <Servo.h>
Servo servo;

#define TRIG 9 //TRIG 핀 설정 (초음파 보내는 핀)
#define ECHO 8 //ECHO 핀 설정 (초음파 받는 핀)

int pos = 0;
byte correctPin = 3;
byte servoPin = 2;
byte buzzerPin = 4;
int n = 0;

long duration;
long distance;

int debounce_sensor=0;
volatile int state_correct=0;

int state_door=0;

int correctSound[] = {659, 622, 659, 622, 659, 493, 587, 523, 440};
int wrongSound[] = {150, 150, 150, 150, 150, 600, 600, 600, 600, 600};

static unsigned long lastTime = 0;
void Inturrupt_correct()
{
  unsigned long now = millis(); 
  if((now-lastTime)>100) 
  { 
    state_correct=1; // 
    lastTime=now; // 현재시간을 lasttime에 저장
  }
}

void setup() {
  // put your setup code here, to run once:

  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  
  pinMode(correctPin, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(correctPin),Inturrupt_correct,FALLING); 
  
  servo.attach(servoPin);
}

// 미 미플랫 미 미플랫 미 시 레 도 라
// 659, 622, 659, 622, 659, 493, 587, 523, 440

void loop() {
  // put your main code here, to run repeatedly:
  if(state_correct==1){
    state_correct=0;
    for(pos = 0; pos <= 90; pos += 1){
      if(pos % 10 == 0){
        tone(buzzerPin, correctSound[pos / 10]);
      }
      servo.write(pos);
      delay(23);
      }
      noTone(buzzerPin);
      delay(2000);
      state_door=1;
  }

  if(state_door==1)
  {
    long duration, distance;
    
    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIG, LOW);

    duration = pulseIn (ECHO, HIGH);
    distance = duration * 17 / 1000;
    
    
    if(distance >= 12)
    {
      debounce_sensor++;
      if(debounce_sensor>3)
      {
        debounce_sensor=0;
        state_door=0;
    
        for(pos = 90; pos >= 0; pos -= 1)
        {
          servo.write(pos);
          delay(23);
        }
      }
    }
    delay(1000);  
  }
}
