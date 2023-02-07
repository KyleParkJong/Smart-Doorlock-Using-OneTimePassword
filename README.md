Konkuk Univ. 3rd grade Electronical Basic Design Experiment termproject 
========================================================================

# 0. Topic of our Term-project
## __Smart Doorlock applying OTP__ 
 + Smart : Person detection using AI (TensorflowLite)
 + OTP (One-Time-Password) : Change to new password every 30 seconds
 + Worked in groups of three

## > Evalutation Criteria
* Creativity
* Number of modules used, AI used

## > Things provided in class
* Arduino Nano 33 BLE, ArduCam
* Person detection TensorflowLite training model 

# 1. Programming language & SW tools & HW used
* Arduino programming (C programming)
* Arduino IDE, Colab Tensorflow
* Arduino Uno, Arduino Nano 33 BLE, ArduCam

# 2. Our design

## > Block diagram

<img src="/images/block_diagram.png" width="90%" height="90%" title="ldpcd_result" alt="ldpcd_result"></img>

> A, B, C boards are Arduino Nano 33 BLE, all connected through physical wire.
> Three boards were used to perform different functions independently.

* B board 
  + Generates a new password every 30 seconds and transmits it to an application on a smart phone produced by App Inventor through Bluetooth.
  + Input received by 4x4 matrix and status displayed via LCD display
  + Send signals to A board

* A board
  + ArduCam is attached and it detects human using "Person Detection" TensorflowLite model
  + Shows current state through LEDs (Blue : human detected, Red : password wrong, Green : password correct)
  + Send signals to B board and C board

* C board
  + Controls servo motor, ultrasonic sensor and buzzer through signal from A board 


## > Flow diagram

<img src="/images/flow_diagram.png" width="80%" height="80%" title="ldpcd_result" alt="ldpcd_result"></img>

# 3. Source code

## A board 
* Person_detection_change.ino, Arduino_detection_responder.cpp
 + Coded based on reference code given from class 

## B board
* B_board_bluetooth.ino
 + Coded everything a-to-z

## C board
* C_board.ino
 + Coded everything a-to-z

# 4. Result

## Password correct (front)

<img src="/images/1.jpeg" width="80%" height="80%" title="ldpcd_result" alt="ldpcd_result"></img>

## Password wrong (front)

<img src="/images/2.jpeg" width="80%" height="80%" title="ldpcd_result" alt="ldpcd_result"></img>

## Door locked (back)

<img src="/images/4.jpeg" width="80%" height="80%" title="ldpcd_result" alt="ldpcd_result"></img>

* Can see servo motor blocking the door

## Door opened (back)

<img src="/images/3.jpeg" width="80%" height="80%" title="ldpcd_result" alt="ldpcd_result"></img>

* When the door is closed again with the door open, the motor automatically operates and locks the door by recognizing that the door is closed through an ultrasonic sensor.

## Checking password with a mobile phone application through Bluetooth.

<img src="/images/5.jpeg" width="80%" height="80%" title="ldpcd_result" alt="ldpcd_result"></img>

# 5. Video link

<https://drive.google.com/file/d/1uqLRO0FNanon8EQNDO_AI4ZBSms7FCh6/view?usp=share_link>

# 6. What I learned
* Arduino programming (C)
 + Basic of Arduino
 + Interrupt timing, Bluetooth
 + Other modules (Ultrasonic sensor,4x4 matrix, ArduCam, buzzer, servo, LCD display, 7 segments)
* Applied Tensorflow Lite AI model into Arduino
* Learned how to collaborate successfully and efficiently with team-mates 
