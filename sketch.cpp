#include <Arduino.h>

/*
  Passthrough keypresses from serial to Keyboard

 Not all pins on the Leonardo support change interrupts,
 so only the following can be used for RX:
 8, 9, 10, 11, 14 (MISO), 15 (SCK), 16 (MOSI).

 Protocol:
 2 bytes total:
   1 byte - press flag
     0x0 - next key released
     0x1 - next key pressed
   1 byte - character pressed
     for special chars look at arduino keyboard mapping

 */
#include <SoftwareSerial.h>
#include "KeyboardioHID.h"
#include <SoftPWM.h>
#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>

SoftwareSerial mySerial(9, 10); // RX, TX

// flag if the next key is pressed or released
bool press=true;
const int led = LED_BUILTIN_RX;  // the pin with a LED
const int keyled = 5;  // the pin with a LED
void blinkLED(void);
LiquidCrystal_I2C ui(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
char text[6];
Servo myservoa;  // create servo object to control a servo
Servo myservob;  // create servo object to control a servo
int timeout = 0;
void setup(){

  TXLED0;
  RXLED0;
  myservoa.attach(7);  // attaches the servo on pin 9 to the servo object
  myservob.attach(14);  // attaches the servo on pin 9 to the servo object
  pinMode(keyled, OUTPUT);
	digitalWrite(keyled, 0);
	delay(200);
	digitalWrite(keyled, 1);
	delay(200);
	digitalWrite(keyled, 0);
	delay(200);
	digitalWrite(keyled, 1);
	delay(200);
	digitalWrite(keyled, 0);
  ui.begin(20,4);         // initialize the lcd for 20 chars 4 lines, turn on backlight
//  ui.lcd_mode(0); // dual ht
  mySerial.begin(9600);
  ui.clear(); // display
//  ui.lcd_mode(1); // dual ht
  ui.print("Press a key to begin");
  ui.setCursor(0, 1);
  ui.print("or wait ten seconds");
  while(mySerial.available() <= 0 && timeout < 100) {
			delay(100);
			timeout++;
  }
  while(mySerial.available()) {
    mySerial.read();
    delay(200);
  }
  ui.clear(); // display
  ui.print("System ready!!");
	delay(1000);


  Timer1.initialize(1500000);
  Timer1.attachInterrupt(blinkLED); // blinkLED to run every 0.15 seconds

  BootKeyboard.begin();
  //Serial.begin(9600);
  // Initialize
  SoftPWMBegin();

  // Create and set pin 13 to 0 (off)
  SoftPWMSet(led, 0);

  // Set fade time for pin 13 to 100 ms fade-up time, and 500 ms fade-down time
  SoftPWMSetFadeTime(led, 300, 600);
  myservoa.write(45);              // tell servo to go to position in variable 'pos'
  myservob.write(135);
  delay(1000);
  myservoa.write(90);              // tell servo to go to position in variable 'pos'
  myservob.write(90);

}

int ledState = LOW;
int remember1 = 0;
int remember2 = 0;
int remember3 = 0;
int remember4 = 0;
int remember5 = 0;
int remember6 = 0;
int sendit = 0;
int gomouse = 0;
int gokeyboard = 0;
int mouse = 0;
int blank = 0;

void blinkLED(void)
{
  if (ledState == LOW) {
    ledState = HIGH;
    // Turn on - set to 100%
    SoftPWMSetPercent(led, 100);
  } else {
    ledState = LOW;
      // Turn off - set to 0%
    SoftPWMSetPercent(led, 0);

    // Wait for LED to turn off
  }
}

void downcheck(byte k){
        if (k == 128){
            remember1 = 1;
        }
        if (k == 130){
            remember2 = 1;
        }
        if (k == 50){
            remember3 = 1;
        }
        if (remember1 == 1 && remember2 == 1 && remember3 == 1){
            sendit = 1;
        }
#ifdef USE_MOUSE
            remember4 = 1;
        }
        if (k == 217){
            remember5 = 1;
        }
        if (remember1 == 1 && remember4 == 1 && remember5 == 1){
            gokeyboard = 1;
        }
        if (k == 218){
            remember6 = 1;
        }
        if (remember1 == 1 && remember4 == 1 && remember6 == 1){
            gomouse = 1;
        }
#endif // USE_MOUSE
}

void upcheck(byte k){
        if (k == 128){
            remember1 = 0;
        }
        if (k == 130){
            remember2 = 0;
        }
        if (k == 50){
            remember3 = 0;
        }
        if (remember1 == 0 && remember2 == 0 && remember3 == 0 && sendit == 1){
            sendit = 0;
            BootKeyboard.press(HID_KEYBOARD_LEFT_CONTROL);
						BootKeyboard.sendReport();
            delay(100);
            BootKeyboard.press(HID_KEYBOARD_LEFT_ALT);
            BootKeyboard.sendReport();
            delay(100);
            BootKeyboard.press(HID_KEYBOARD_DELETE);
            BootKeyboard.sendReport();
            delay(100);
            BootKeyboard.releaseAll();
            BootKeyboard.sendReport();
						ui.clear(); // display
//						ui.lcd_mode(0); // dual ht
						ui.print("Sent");
						ui.setCursor(0, 1);
						ui.print("CTRL+ALT+DELETE");
            Timer1.detachInterrupt();
            SoftPWMSet(led, 0);
            RXLED0;
            digitalWrite(keyled, 1);
            delay(400);
            digitalWrite(keyled, 0);
            delay(400);
            digitalWrite(keyled, 1);
            delay(400);
            digitalWrite(keyled, 0);
            delay(400);
            digitalWrite(keyled, 1);
            delay(400);
            digitalWrite(keyled, 0);
            delay(400);
            digitalWrite(keyled, 1);
            delay(400);
            digitalWrite(keyled, 0);
            delay(400);
        }
#if USE_MOUSE
        if (k == 129){
            remember4 = 0;
        }
        if (k == 217){
            remember5 = 0;
        }
        if (remember1 == 0 && remember4 == 0 && remember5 == 0 && gokeyboard == 1){
            gokeyboard = 0;
            Mouse.end();
            Keyboard.begin();
						ui.clear(); // display
						ui.lcd_mode(0); // dual ht
						ui.print("Keyboard mode");
						ui.setCursor(0, 2);
						ui.print("enabled");
            mouse = 0;
        }
        if (k == 218){
            remember6 = 0;
        }
        if (remember1 == 0 && remember4 == 0 && remember6 == 0 && gomouse == 1){
            gomouse = 0;
            Keyboard.end();
            Mouse.begin();
						ui.clear(); // display
						ui.lcd_mode(0); // dual ht
						ui.print("Mouse mode");
 						ui.setCursor(0, 2);
						ui.print("enabled");
						mouse = 1;
        }
#endif // USE_MOUSE
}

void printkey (void){
//	ui.lcd_mode(1); // dual ht
	ui.clear(); // display
	ui.print("Key: ");
}

void loop(){
	for (byte i = 1; i < 255; i++){
		BootKeyboard.press(i);
		BootKeyboard.sendReport();
		delay(200);
		BootKeyboard.release(i);
		BootKeyboard.sendReport();
		delay(200);
	}

  if (mySerial.available())
  {
    byte k = mySerial.read();
    //Serial.write(k);
    //Serial.println(" received");
    if (k == 0){
      press = 0;
    }else if (k == 1){
      press = 1;
    }else {
      //mySerial.write(k);
      if (press){
				printkey();
        if (mouse == 0){
					itoa(k, text, 10);
					ui.print(String(text) + String(' '));
					BootKeyboard.press(k);
					BootKeyboard.sendReport();
            //mySerial.println(" pressed");
        } else {
#if USE_MOUSE
					switch (k) {
						case 218:
							// move mouse up
							Mouse.move(0, -40);
							ui.print("Mouse up");
							break;
						case 217:
							// move mouse down
							Mouse.move(0, 40);
							ui.print("Mouse down");
							break;
						case 216:
							// move mouse left
							Mouse.move(-40, 0);
							ui.print("Mouse left");
							break;
						case 215:
							// move mouse right
							Mouse.move(40, 0);
							ui.print("Mouse right");
							break;
						case 32:
							// perform mouse left click
							Mouse.click(MOUSE_LEFT);
							ui.print("Click left");
							break;
						case 130:
							// perform mouse left click
							Mouse.click(MOUSE_RIGHT);
							ui.print("Click right");
							break;
					}
#endif // USE_MOUSE
        }
        downcheck(k);

      } else {
        if (mouse == 0){
					printkey();
					itoa(k, text, 10);
					ui.print(String(text) + String(' '));
					BootKeyboard.release(k);
					BootKeyboard.sendReport();
        } else {
				}
        upcheck(k);
			}
        //mySerial.println(" released");
			digitalWrite(keyled, 1);
			delay(40);
			digitalWrite(keyled, 0);
		}
  }
  // Wait for LED to turn on - you could do other tasks here
}
