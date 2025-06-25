#include "Keyboard.h"

const unsigned long pTime = 30;
const unsigned long rTime = 370;
const unsigned long win_releaseTime = 500;
const int delayus = 50;
const int loop_delayms = 1;
const int NO_KEY = -1;
const int blueLedPin = 12;
const int yellowLedPin = 13;
const int IDLE = 0;
const int PUSH = 1;
const int WIN_RTIME_GET = 2;
const int WIN_RELEASE_WAIT = 3;
const int WIN_RELEASE = 4;
int state;
int keyNumber;
int keyNumber_old;
int keep_flag;
unsigned long wrGET;



//////////////////////////////////////////////
//           KEY MAP config start           //
//////////////////////////////////////////////
const int col_length = 5;
const int col[col_length] = {0, 1, 2, 3, 4};
//const int row_length = 6;
//const int row[row_length] = {5, 6, 7, 8, 9, 10};
const int row_length = 5;
const int row[row_length] = {5, 6, 7, 8, 9};
const int key_map[col_length*row_length] = {
  'l', 'd', 'p', '_', '_', //[ 0: 2]Win+l, Win+d, Ctrl+Alt+p
  '1', '2', '3', '4', '5', //[ 5:14]win+num (with keep)
  '6', '7', '8', '9', '0', //
  '1', '2', '3', '4', '5', //[15:24]Ctrl+Alt+num
  '6', '7', '8', '9', '0'  //
};

int key_press(int _num, int _keep_flag_old) {
  int _keep_flag = 0;

  if (0 <= _num & _num <= 2) { //[ 0: 2]Win+l, Win+m, Ctrl+Alt+p
    _keep_flag = 1;
    Keyboard.press(KEY_LEFT_GUI); //Win
    digitalWrite(blueLedPin, LOW); //led on
    delayMicroseconds(delayus);
    Keyboard.press(key_map[_num]);
    digitalWrite(yellowLedPin, LOW); //led on
    delay(pTime);
    Keyboard.release(key_map[_num]);
    delay(rTime);
    digitalWrite(yellowLedPin, HIGH); //led off

  } else if (5 <= _num & _num <= 14) { //[ 5:14]win+num (with keep)
    _keep_flag = 1;
    Keyboard.press(KEY_LEFT_GUI); //Win
    digitalWrite(blueLedPin, LOW); //led on
    delayMicroseconds(delayus);
    Keyboard.press(key_map[_num]);
    digitalWrite(yellowLedPin, LOW); //led on
    delay(pTime);
    Keyboard.release(key_map[_num]);
    delay(rTime);
    digitalWrite(yellowLedPin, HIGH); //led off

  } else if(15 <= _num & _num <= 24) { //[15:24]Ctrl+Alt+num
    if(_keep_flag_old == 1) {
      Keyboard.release(KEY_LEFT_GUI); //Win
      digitalWrite(blueLedPin, HIGH); //led off
      delayMicroseconds(delayus);
    }
    Keyboard.press(KEY_LEFT_CTRL);
    Keyboard.press(KEY_LEFT_ALT);
    delayMicroseconds(delayus);
    Keyboard.press(key_map[_num]);
    digitalWrite(yellowLedPin, LOW); //led on
    delay(pTime);
    Keyboard.release(key_map[_num]);
    delayMicroseconds(delayus);
    Keyboard.release(KEY_LEFT_ALT);
    Keyboard.release(KEY_LEFT_CTRL);
    delay(rTime);
    digitalWrite(yellowLedPin, HIGH); //led off

  } else {
    //nop
  }
  return _keep_flag;
}
//////////////////////////////////////////////
//            KEY MAP config end            //
//////////////////////////////////////////////




void setup() {
  pinMode(blueLedPin, OUTPUT);
  pinMode(yellowLedPin, OUTPUT);
  for(int i = 0; i < col_length; i++) {
    pinMode(col[i], INPUT_PULLUP);
  }
  for(int i = 0; i < row_length; i++) {
    pinMode(row[i], OUTPUT);
  }
  for(int i = 0; i < row_length; i++) {
    digitalWrite(row[i], HIGH);
  }

  state = IDLE;
  keyNumber = NO_KEY;
  keyNumber_old = NO_KEY;
  keep_flag = 0;
  wrGET = 0;
  Keyboard.begin();
}
void loop() {
  //scan
  keyNumber = NO_KEY;
  for(int i = 0; i < row_length; i++) {
    digitalWrite(row[i], LOW);
    delayMicroseconds(delayus);
    for(int j = 0; j < col_length; j++) {
      if(digitalRead(col[j]) == LOW) {
        keyNumber = col_length*i + j;
      }
    }
    digitalWrite(row[i], HIGH);
    delayMicroseconds(delayus);
  }

  switch (state) {
    case IDLE:
        if(keyNumber != NO_KEY) {
          keyNumber_old = keyNumber;
          state = PUSH;
        } else { //keyNumber == NO_KEY
          state = IDLE;
        }
        break;
    case PUSH:
        keep_flag = key_press(keyNumber_old, keep_flag);
        if(keep_flag == 1) {
          state = WIN_RTIME_GET;
        } else { //keep_flag == 0
          state = IDLE;
        }
        break;
    case WIN_RTIME_GET:
        wrGET = millis();
        state = WIN_RELEASE_WAIT;
        break;
    case WIN_RELEASE_WAIT:
        if(keyNumber != NO_KEY) {
          keyNumber_old = keyNumber;
          state = PUSH;
        } else if(millis() - wrGET >= win_releaseTime) { // && keyNumber == NO_KEY
          state = WIN_RELEASE;
        } else { //millis() - wrGET < win_releaseTime
          state = WIN_RELEASE_WAIT;
        }
        break;
    case WIN_RELEASE:
        Keyboard.release(KEY_LEFT_GUI); //Win
        digitalWrite(blueLedPin, HIGH); //led off
        state = IDLE;
        break;
    default:
        state = IDLE;
        break;
  }
  delay(loop_delayms);
}