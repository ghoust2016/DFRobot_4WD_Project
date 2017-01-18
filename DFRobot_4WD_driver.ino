#include <Servo.h>
#include <Metro.h>
#include <TimerOne.h>
#include <MsTimer2.h>

Metro measureDistance = Metro(50);
Metro sweepServo = Metro(20);

int speedPin_M1 = 5;
int speedPin_M2 = 6;
int directionPin_M1 = 4;
int directionPin_M2 = 7;
unsigned long actualDistance = 0;
Servo myservo;
char servo_num = 9;
int pos = 70;
int sweepFlag = 1;
int URPWM = 3;
int URTRIG = 10;
char drive_advance_flag = 0;
char drive_back_flag = 0;
unsigned int timer_150ms_counter = 0;
unsigned int timer_200ms_counter = 0;
unsigned int timer_500ms_counter = 0;
unsigned int timer_1000ms_counter = 0;
char timer_150ms_flag = 0;
char timer_200ms_flag = 0;
char timer_500ms_flag = 0;
char timer_1000ms_flag = 0;
const char left = 1;
const char right = 2;
const char advance = 3;

void SensorSetup() {
  pinMode(URTRIG, OUTPUT);
  digitalWrite(URTRIG,HIGH);
  pinMode(URPWM, INPUT);
}

long MeasureDistance() {
  digitalWrite(URTRIG,LOW);
  digitalWrite(URTRIG,HIGH);
  unsigned long distance=pulseIn(URPWM,LOW);
  distance=distance/50;
  return distance;
}

void carStop() {
  analogWrite (speedPin_M2,0);
  digitalWrite(directionPin_M1,LOW);
  analogWrite (speedPin_M1,0);
  digitalWrite(directionPin_M2,LOW);
}

void msTimer2_func() {
  if (timer_500ms_counter < 500) {
    timer_500ms_counter ++;
  } else {
    timer_500ms_flag = 1;
    timer_500ms_counter = 0;
  }

  if (timer_1000ms_counter < 1000) {
    timer_1000ms_counter ++;
  } else {
    timer_1000ms_flag = 1;
    timer_1000ms_counter = 0;
  }

  if (drive_advance_flag == 1) {
    carAdvance(150,150);
    if (timer_200ms_counter < 200) {
      timer_200ms_counter ++;
    } else {
      drive_advance_flag = 0;
      timer_200ms_counter = 0;
    }
  }

  if (drive_back_flag == 1) {
    carBack(150,150);
    if (timer_150ms_counter < 150) {
      timer_150ms_counter ++;
    } else {
      drive_back_flag = 0;
      timer_150ms_counter = 0;
    }
  }
}

void carBack(long leftSpeed, long rightSpeed) {
  analogWrite (speedPin_M2,leftSpeed);
  digitalWrite(directionPin_M1,HIGH);
  analogWrite (speedPin_M1,rightSpeed);
  digitalWrite(directionPin_M2,HIGH);
}

void carAdvance(long leftSpeed, long rightSpeed) {
  analogWrite (speedPin_M2,leftSpeed);
  digitalWrite(directionPin_M1,LOW);
  analogWrite (speedPin_M1,rightSpeed);
  digitalWrite(directionPin_M2,LOW);
}

void carTurnLeft(long leftSpeed, long rightSpeed) {
  analogWrite (speedPin_M2,leftSpeed);
  digitalWrite(directionPin_M1,LOW);
  analogWrite (speedPin_M1,rightSpeed);
  digitalWrite(directionPin_M2,HIGH);
}

void carTurnRight(long leftSpeed, long rightSpeed) {
  analogWrite (speedPin_M2,leftSpeed);
  digitalWrite(directionPin_M1,HIGH);
  analogWrite (speedPin_M1,rightSpeed);
  digitalWrite(directionPin_M2,HIGH);
}

void servoSweep() {
  if (sweepServo.check() == 1) {
    if (sweepFlag) {
      if (pos>=50 && pos<=95) {
        pos=pos+5;
        myservo.write(pos);
      }
      if (pos>94) {
        sweepFlag = false;
      }
    } else {
      if (pos>=50 && pos<=100) {
        pos=pos-5;
        myservo.write(pos);
      }
      if (pos<51) {
        sweepFlag = true;
      }
    }
  }
}

char direction_state_update() {
  char direction= 0;
  // advance
  if (pos>=65 && pos<=80) {
    direction = advance;
  }
  // left
  if (pos>=50 && pos<65) {
    direction = left;
  }
  // right
  if (pos>80 && pos<=95) {
    direction = right;
  }
  return direction;
}

void drive_mode() {
  if (measureDistance.check() == 1) {
    actualDistance = MeasureDistance();
  }
  char data=0;
  data = direction_state_update();
  switch (data) {
   case advance:
    if (actualDistance <= 30) {
      drive_back_flag = 1;
    } else {
      drive_advance_flag = 1;
    }
    break;
   case left:
    if (actualDistance <= 30) {
      drive_back_flag = 1;
    } else {
      drive_advance_flag = 1;
    }
    break;
   case right:
    if (actualDistance <= 30) {
      drive_back_flag = 1;
    } else {
      drive_advance_flag = 1;
    }
    break;
   default:
    drive_back_flag = 0;
    drive_advance_flag = 0;
    break;
  }
}

void setup()
{
  MsTimer2::stop();
  myservo.attach(servo_num);
  myservo.write(pos);
  SensorSetup();
  MsTimer2::set(1, msTimer2_func);
  MsTimer2::start();
}

void loop()
{
  if (timer_500ms_flag  == 1) {
    drive_mode();
    timer_500ms_flag = 0;
  }
  if (timer_1000ms_flag  == 1) {
    servoSweep();
    timer_1000ms_flag = 0;
  }
}
