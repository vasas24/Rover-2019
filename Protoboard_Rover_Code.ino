#include <Wire.h>

//accelerometer address
const int MPU_addr = 0x68;

//pin definitions
int sonar_r = 31, sonar_l = 32;
int dr_dir_l = 0, dr_dir_r = 1, dr_pwm_l = 5, dr_pwm_r = 6;
int soil_top_dir1 = 18, soil_top_dir2 = 17, soil_top_pwm = 16, soil_bot_dir1 = 20, soil_bot_dir2 = 21, soil_bot_pwm = 22, soil_stby = 19;
int auger_dir1 = 37, auger_dir2 = 36, auger_pwm = 35, auger_stby = 38;
int green_led = 30, red_led = 29;

//global delay definitions, in seconds
int drive_time = 15;                                        
int slow_start_delay = 5;
int num_drills = 3;

//global delay definitions, in milliseconds
int door_delay = 400;
int auger_delay = 2000;

void setup() {
  Wire2.begin();
  Wire2.beginTransmission(MPU_addr);
  Wire2.write(0x6B);
  Wire2.write(0);
  Wire2.endTransmission(true);
  
  pinMode(sonar_r, INPUT); pinMode(sonar_l, INPUT);
  pinMode(dr_dir_l, OUTPUT); pinMode(dr_dir_r, OUTPUT); pinMode(dr_pwm_l, OUTPUT); pinMode(dr_pwm_r, OUTPUT);
  pinMode(soil_top_dir1, OUTPUT); pinMode(soil_top_dir2, OUTPUT); pinMode(soil_top_pwm, OUTPUT); pinMode(soil_bot_dir1, OUTPUT); pinMode(soil_bot_dir2, OUTPUT); pinMode(soil_bot_pwm, OUTPUT);
  pinMode(auger_dir1, OUTPUT); pinMode(auger_dir2, OUTPUT); pinMode(auger_pwm, OUTPUT);
  pinMode(auger_stby, OUTPUT); pinMode(soil_stby, OUTPUT);
  pinMode(green_led, OUTPUT); pinMode(red_led, OUTPUT);
  
  Serial.begin(9600);
  digitalWrite(auger_stby, HIGH); digitalWrite(soil_stby, HIGH); //Keep motor drivers powered.
}

void loop() {

  led(1);
  move_away();
  led(2);
  collect_soil();
  led(3);
  //Do nothing forever.
  while(1){
    delay(1000);
  }
}

//First Order Functions
void move_away(){
  int i = 0;

  for(int k = 0; k < 255; k++){
    forward(k);
    i = i + 5;
    delay(slow_start_delay);
  }
  
  while(i < (drive_time * 1000)){
    
    if(get_distance(0) < .5 or get_distance(1) < .5){
      if(get_distance(0) < get_distance(1)){
        while(get_distance(0) < get_distance(1)){
          right(100);
          i = i + 100;
          delay(100);
        }
      }else if(get_distance(0) > get_distance(1)){
        while(get_distance(0) < get_distance(1)){
          right(100);
          i = i + 100;
          delay(100);
        }
      }
      for(int k = 100; k < 255; k++){
        forward(k);
        i = i + 5;
        delay(slow_start_delay);
      }
    }
    
  }
}

void collect_soil(){
  top_door(1);
  for(int i = 0; i < num_drills; i++){
    while(check_level() == false){
     forward(100);
     delay(50);  
    }
    forward(0);
    delay(1000);
    auger();
    forward(100);
    delay(1000);
  }
  top_door(0);
}

//Second Order Functions Movement

void forward(int val){
  digitalWrite(dr_dir_l, LOW); digitalWrite(dr_dir_r, LOW);
  analogWrite(dr_pwm_l, val); analogWrite(dr_pwm_r, val);
}

void right(int val){
  digitalWrite(dr_dir_l, HIGH); digitalWrite(dr_dir_r, LOW);
  analogWrite(dr_pwm_l, 0); analogWrite(dr_pwm_r, val);
}

void left(int val){
  digitalWrite(dr_dir_l, LOW); digitalWrite(dr_dir_r, HIGH);
  analogWrite(dr_pwm_l, val); analogWrite(dr_pwm_r, 0);
}

void reverse(int val){
  digitalWrite(dr_dir_l, HIGH); digitalWrite(dr_dir_r, HIGH);
  analogWrite(dr_pwm_l, val); analogWrite(dr_pwm_r, val);
}

float get_distance(int dir){
  float distance;
  int input;
  if(dir == 0){
    input = analogRead(sonar_l);
    distance = input / 200;
    return distance;
  }else if(dir == 1){
    input = analogRead(sonar_r);
    distance = input / 200;
    return distance;
  }else
    return 0;
}

//Second Order Functions Collection

void top_door(int val){
  if(val == 0){
    digitalWrite(soil_top_dir1, 1); digitalWrite(soil_top_dir2, 0); analogWrite(soil_top_pwm, 150);
    delay(door_delay);
    analogWrite(soil_top_pwm, 0);
  }else if(val == 1){
    digitalWrite(soil_top_dir1, 0); digitalWrite(soil_top_dir2, 1); analogWrite(soil_top_pwm, 150);
    delay(door_delay);
    analogWrite(soil_top_pwm, 0);
  }
}

void bot_door(int val){
  if(val == 0){
    digitalWrite(soil_bot_dir1, 1); digitalWrite(soil_bot_dir2, 0); analogWrite(soil_bot_pwm, 150);
    delay(door_delay);
    analogWrite(soil_bot_pwm, 0);
  }else if(val == 1){
    digitalWrite(soil_bot_dir1, 0); digitalWrite(soil_bot_dir2, 1); analogWrite(soil_bot_pwm, 150);
    delay(door_delay);
    analogWrite(soil_bot_pwm, 0);
  }
}

bool check_level(){
  int16_t accx, accy, accz;
  
  Wire2.beginTransmission(MPU_addr);
  Wire2.write(0x3B);
  Wire2.endTransmission(false);
  Wire2.requestFrom(MPU_addr, 8, true);
   
  accx = Wire2.read() << 8 | Wire2.read();
  accy = Wire2.read() << 8 | Wire2.read();
  accz = Wire2.read() << 8 | Wire2.read();

  //do code after talking with mechs
}

void auger(){
  digitalWrite(auger_dir1, HIGH); digitalWrite(auger_dir2, LOW); analogWrite(auger_pwm, 255);
  delay(auger_delay);
  digitalWrite(auger_dir1, LOW); digitalWrite(auger_dir2, LOW); analogWrite(auger_pwm, 0);
  delay(350);
  digitalWrite(auger_dir1, LOW); digitalWrite(auger_dir2, HIGH); analogWrite(auger_pwm, 255);
  delay(auger_delay);
  digitalWrite(auger_dir1, LOW); digitalWrite(auger_dir2, LOW); analogWrite(auger_pwm, 0);
}

//Third Order Functions Collection

void log_data(){
  //There's no data silly.
}

//LED Functions

void led(int i){
  if(i == 0){
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, LOW);
  }else if(i == 1){
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, LOW);
  }else if(i == 2){
    digitalWrite(red_led, LOW);
    digitalWrite(green_led, HIGH);
  }else if(i == 3){
    digitalWrite(red_led, HIGH);
    digitalWrite(green_led, HIGH);
  }
}
