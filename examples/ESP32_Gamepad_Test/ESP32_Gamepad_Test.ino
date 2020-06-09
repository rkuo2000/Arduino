/*
 * This example turns the ESP32 into a Bluetooth LE gamepad that presses buttons and moves axis
 * 
 * Possible buttons are:
 * BUTTON_1 through to BUTTON_14* 
 * 
 * Possible DPAD/HAT switch position values are: 
 * DPAD_CENTERED, DPAD_UP, DPAD_UP_RIGHT, DPAD_RIGHT, DPAD_DOWN_RIGHT, 
 * DPAD_DOWN, DPAD_DOWN_LEFT, DPAD_LEFT, DPAD_UP_LEFT
 * 
 * bleGamepad.setAxes takes the following signed char parameters: 
 * (Left Thumb X, Left Thumb Y, Right Thumb X, Right Thumb Y, Left Trigger, Right Trigger, Hat switch position);
 */

// Testing BLE Gamepad from Button_1 to Button_14

#include <BleGamepad.h> 

BleGamepad bleGamepad;
int Button[14] = {BUTTON_1, BUTTON_2, BUTTON_3, BUTTON_4, BUTTON_5, BUTTON_6,BUTTON_7,
                   BUTTON_8, BUTTON_9, BUTTON_10, BUTTON_11, BUTTON_12, BUTTON_13, BUTTON_14}; 
int i = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleGamepad.begin();
}

void loop() {
  if(bleGamepad.isConnected()) {
    i++;
    if (i<=14) {
      Serial.println("Press Button "+String(i));
      bleGamepad.press(Button[i]);
      delay(500);

      Serial.println("Release Button "+String(i));
      bleGamepad.release(Button[i]);
      delay(500); 
    } 
    else
      i =0;  
  }
}
