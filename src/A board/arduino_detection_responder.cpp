/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#if defined(ARDUINO) && !defined(ARDUINO_ARDUINO_NANO33BLE)
#define ARDUINO_EXCLUDE_CODE
#endif  // defined(ARDUINO) && !defined(ARDUINO_ARDUINO_NANO33BLE)

#ifndef ARDUINO_EXCLUDE_CODE

#include "detection_responder.h"

#include "Arduino.h"

//추가한 부분
volatile int cntperson = 0; // 초기 cnt 설정.
volatile int cntnotper = 0;
volatile bool runflag = false; //runflag==true 라면 프로그램이 실행중인 것이다.

// Flash the blue LED after each inference
void RespondToDetection(tflite::ErrorReporter* error_reporter,
                        int8_t person_score, int8_t no_person_score) {
  static bool is_initialized = false;
  if (!is_initialized) {
    // Pins for the built-in RGB LEDs on the Arduino Nano 33 BLE Sense
    pinMode(LEDR, OUTPUT);
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
    pinMode(A7, OUTPUT); //사람을 5회이상 연속 감지하면 켜지는 LED
    pinMode(D2, OUTPUT); // 프로그램 시작을 알리는 HIGH신호를 다른 보드에 보내기 위함
    pinMode(D10, OUTPUT);
    digitalWrite(D2, HIGH);
    digitalWrite(D10, HIGH);
    is_initialized = true;
  }

  // Note: The RGB LEDs on the Arduino Nano 33 BLE
  // Sense are on when the pin is LOW, off when HIGH.

  // Switch the person/not person LEDs off
  digitalWrite(LEDG, HIGH);
  digitalWrite(LEDR, HIGH);

  // Flash the blue LED after every inference.
  digitalWrite(LEDB, LOW);
  delay(100);
  digitalWrite(LEDB, HIGH);

  // Switch on the green LED when a person is detected,
  // the red when no person is detected
  if (person_score > -20){//no_person_score) {// 더 쉽게 인식하기 위해 바꿈
    digitalWrite(LEDG, LOW);
    digitalWrite(LEDR, HIGH);
  } else {
    digitalWrite(LEDG, HIGH);
    digitalWrite(LEDR, LOW);
  }
  //추가한 부분
  if (person_score >-20){
    cntperson = (cntperson + 1)%60; 
    cntnotper = 0; //20번 detect 중 한번이라도 사람이 감지된다면 display가 꺼질 일은 없음
  }
  else {
    cntnotper = (cntnotper + 1)%60; 
    cntperson = 0;
  }
  if ((cntperson >= 5) && (runflag==false)){ // 연속 5번 이상 person을 감지할 경우 프로그램 시작.
    digitalWrite(A7, HIGH); //프로그램 시작을 알리는 LED. 
    digitalWrite(D2, LOW); //HIGH 신호를 D2로 보냄. 프로그램(display) 시작.
    delay(500);
    digitalWrite(D2, HIGH);
    runflag=true;
  }
  if ((cntnotper >=10) && (runflag==true)){ //연속 20번 이상 notperson일 경우 실행중이던 display가 꺼짐
    digitalWrite(A7, LOW); // display 중이지 않음
    digitalWrite(D10, LOW); //LOW 신호를 D2(다른 보드로 들어가는 신호)를 통해 보냄.
    delay(500);
    digitalWrite(D10, HIGH);
    runflag=false;
  }
  TF_LITE_REPORT_ERROR(error_reporter, "Person score: %d No person score: %d [CNT: %d][NOT: %d]",
                       person_score, no_person_score,cntperson , cntnotper);
}

#endif  // ARDUINO_EXCLUDE_CODE
