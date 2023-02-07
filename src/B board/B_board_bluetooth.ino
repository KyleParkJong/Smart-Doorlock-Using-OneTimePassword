//---------------BlueTooth---------------//
#include <ArduinoBLE.h>
BLEService DeviceInformation("180A");
BLECharCharacteristic Weight("2A98", BLEWrite | BLERead | BLENotify);  
BLECharCharacteristic WeightMeasurement("2A9D", BLEWrite | BLERead | BLENotify); 
//-------------------------------------------//

#include <NRF52_MBED_TimerInterrupt.h>
#include <NRF52_MBED_TimerInterrupt.hpp>
#include <NRF52_MBED_ISR_Timer.h>
#include <NRF52_MBED_ISR_Timer.hpp>

#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);

#include <Keypad.h>

NRF52_MBED_Timer ITimer(NRF_TIMER_3);
NRF52_MBED_ISRTimer ISR_Timer;

#define HW_TIMER_INTERVAL_MS  1
#define password_Init_period 1000 

int sampling_PD = D2;
int unsampling_PD = D5;
int correct_password_pin = D3;
int wrong_password_pin = D4;

bool startflag=false;
bool inputflag = false;


//-----------------------------------------------------------------//
//------타이머인터럽트에 의해 주기를 가지고 초기화될 데이터----------------//
char buf_password[30];
int password; // 주기로 초기화될 비밀번호 
volatile long password_Init_flag=0;
//-----------------------------------------------------------------//
volatile int detected_flag=0;
volatile int undetected_flag=0;

//-----------------------------------------------------------------//
//------4x4matrix 데이터----------------//
const byte rows = 4;    // 행(rows) 개수
const byte cols = 3;    // 열(columns) 개수

//byte rowPins[rows] = {9,10,11,12};
byte rowPins[rows] = {12, 11, 10, 9};
byte colPins[cols] = {8,7,6};
int keypad_ref=0; // 0은 입력이 없는상태
int password_input=0;

int sampling_password = 0;

int pf;
int pt;
int ptw;
int po;

int wrong_cnt=0;

char keys[rows][cols] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'s','0','e'}
};

void TimerHandler(){
  ISR_Timer.run();
}

int i,j,k,r;

volatile char pw[4];
void password_Init()
{ //인터럽트(주기로)
  password_Init_flag=1;
//  cnt_15--;
  password = random(10000);
  for(int z=0;z<4;z++)
  {
    int set_pw=10000;
    for(int x=z;x>=0;x--)
    {
      set_pw/=10;
    }
    pw[z]=((password/set_pw)%10)+48;
  }
}

static unsigned long lastTime1 = 0;
static unsigned long lastTime2 = 0;

//-----------------------------------------------//
//-----------------------------------------------//
void sample_detected(){
  unsigned long now = millis(); 
  if((now-lastTime1)>100) 
  { 
    detected_flag=1; 
    lastTime1=now; 
  }
}

void sample_undetected(){
  unsigned long now = millis(); 
  if((now-lastTime2)>100) 
  { 
    undetected_flag=1; 
    lastTime2=now; 
  }
}

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols);

void setup() {
  Serial.begin(9600);
  //
  lcd.init();
  lcd.noDisplay();
  ITimer.attachInterruptInterval(HW_TIMER_INTERVAL_MS * 500,TimerHandler);
  ISR_Timer.setInterval(password_Init_period, password_Init);
  //
  pinMode(correct_password_pin, OUTPUT);
  pinMode(wrong_password_pin, OUTPUT);
  digitalWrite(correct_password_pin, HIGH);
  digitalWrite(wrong_password_pin, HIGH);
  //
  pinMode(unsampling_PD, INPUT_PULLUP);
  pinMode(sampling_PD, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(sampling_PD),sample_detected,FALLING);
  attachInterrupt(digitalPinToInterrupt(unsampling_PD),sample_undetected,FALLING);

  randomSeed(analogRead(0));

  if (!BLE.begin()){
    Serial.println("starting BLE failed!");
    while(1);
  }

  //------ble----------------//
  BLE.setLocalName("BLE chat machine");

  BLE.setAdvertisedService(DeviceInformation); 
  DeviceInformation.addCharacteristic(Weight); 
  DeviceInformation.addCharacteristic(WeightMeasurement); 
  BLE.addService(DeviceInformation);

  Weight.writeValue(NULL);
  WeightMeasurement.writeValue(NULL);

  BLE.advertise();
  Serial.println("BLE LED Peripheral");
}

int v=0;

void loop(){
  BLEDevice central = BLE.central();
  
  if (central) 
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());
    
      while (central.connected()) 
      {
        if(password_Init_flag==1)
        { 
          if(v==0){
          Serial.print("password : ");
          Serial.println(password);
          }
          char val1=pw[v++];
          Weight.writeValue(val1);
          delay(2000);
        }
        if(v==4) {v=0;password_Init_flag=0;}
        
        char val2 = WeightMeasurement.value();
        if (vla2 != NULL)
        {
          Serial.println(val2);
          WeightMeasurement.writeValue(NULL);
        }
        
        if(detected_flag==1)
        {
          detected_flag=0;
          keypad_ref=0; password_input=0;
          wrong_cnt=0;
          lcd_print_on_Init();
        }
        if(undetected_flag == 1)
        {
          undetected_flag = 0;
          lcd_print_off_Init();
        }
        
        if(keypad_ref==0) sensing_s();
        else
        {
          char key = keypad.getKey();
          if(key){
          switch(keypad_ref) {
              case 1: 
                pf = key - '0';
                password_input +=pf*1000;
                lcd.setCursor(11, 1);
                lcd.print(pf);
                keypad_ref++;
                break;
              case 2: 
                pt = key - '0';
                password_input +=pt*100;
                lcd.setCursor(12, 1);
                lcd.print(pt);
                keypad_ref++;
                break;
              case 3: 
                ptw = key - '0';
                password_input += ptw*10;
                lcd.setCursor(13, 1);
                lcd.print(ptw);
                keypad_ref++;
                break;
              case 4: 
                po = key - '0';
                password_input += po;
                lcd.setCursor(14, 1);
                lcd.print(po);
                lcd.noCursor();
                keypad_ref++;
                break;
              case 5: 
                keypad_ref=0;
                if(key=='e')
                {
                  if(password_input == password)
                  { // 입력이 맞았을 때
                    wrong_cnt=0;
                    state_test1();
                  }
                  else{ //입력이 틀렸을때의 동작
                    wrong_cnt++;
                    if(wrong_cnt<3)
                    {
                      lcd_print_wrong_answer();
                    }
                    
                    else
                    {
                      wrong_cnt=0;
                      state_test2();
                    }
                  }
                  password_input=0;
                }
                else
                { // e가 아닌 아예 다른거 눌렀을 때
                  password_input=0;
                  wrong_cnt++;
                  if(wrong_cnt<3)
                  {
                    lcd_print_wrong_answer();
                  }
                  else
                  {
                    wrong_cnt=0;
                    state_test2();
                  }
                }
                break;
              default:
                keypad_ref=0;
                break;
          }
      }
  }
  delay(100);
}


void password_print()
{   
    password = random(10000);
    Serial.println("reset!"); 
    sprintf(buf_password,"%04d",password);
    Serial.write(buf_password);
    Serial.print(" / ");

}

void sensing_s()
{
  password_input=0;
  char key = keypad.getKey();
  if(key == 's')
  {
    lcd.cursor();
    keypad_ref=1;
  }
  else keypad_ref=0;
}

void lcd_print_on_Init(){
  lcd.backlight(); //백라이트 키기
      lcd.display();
      lcd.setCursor(4, 0);
      lcd.print("WELCOME!");
      lcd.setCursor(0, 1);
      lcd.print("PASSWORD : "); //4자리 수는 lcd.setCursor(11,1); 다음에 입력
}

void lcd_print_off_Init(){
  lcd.noBacklight();
  lcd.noDisplay();
  lcd.clear();
}

void lcd_print_wrong_answer(){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("wrong password!!");
      delay(1500);
      lcd.clear();
      lcd.setCursor(4, 0);
      lcd.print("WELCOME!");
      lcd.setCursor(0, 1);
      lcd.print("PASSWORD : "); //4자리 수는 lcd.setCursor(11,1); 다음에 입력
}

void state_test1(){
    digitalWrite(correct_password_pin, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("OPEN SESAME~"); 
    delay(3000);
    lcd_print_off_Init();
    digitalWrite(correct_password_pin, HIGH);
    delay(3000);
    lcd_print_on_Init();
}

void state_test2(){
    digitalWrite(wrong_password_pin, LOW);
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("GET OUT OF HERE!");  
    delay(3000);
    lcd_print_off_Init();
    digitalWrite(wrong_password_pin, HIGH);
  }
