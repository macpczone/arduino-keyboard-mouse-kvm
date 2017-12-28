/* Arduino Keyboard Mouse
 * Copyright (C) 2017  Mac PC Zone
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

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
#include "HID-Project.h"
#include <SoftPWM.h>
#include <TimerOne.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#if USE_SERVO
#include <Servo.h>
#include <EEPROM.h>
#endif
#include <keysims.h>
#if USE_RF24
#include <SPI.h>
#include <nRF24L01.h>
#include "RF24.h"
#include "xxtea-iot-crypt.h"
#endif

SoftwareSerial mySerial(9, 10); // RX, TX

#include <printf.h>
// flag if the next key is pressed or released
bool press=true;
const int led = LED_BUILTIN_RX;  // the pin with a LED
const int keyled = 5;  // the pin with a LED

#if USE_SERVO
const int servopin1 = 7;
const int servopin2 = 8;
Servo myservoa;  // create servo object to control a servo
Servo myservob;  // create servo object to control a servo
int servodelay = 200;
int rsdelay = 200;
int powerdelay = 10000; // hold the power button in for 10 seconds
#endif
void blinkLED(void);
void setcursor (uint8_t, uint8_t);
void sendnewline (void);

LiquidCrystal_I2C ui(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);
char text[6];
int timeout = 0;
int countdown = 10;
int rdpin = 6;
#include "printit.h"

#if USE_RF24
RF24 radio(18,19);                        // Set up nRF24L01 radio on SPI bus plus pins 7 & 8
//RF24 radio(6,4);                        // Set up nRF24L01 radio on SPI bus plus pins 7 & 8
const byte addresses[][6] = {"1Node","2Node"};
const uint64_t pipes[2] = { 0xABCDABCD71LL, 0x544d52687CLL };   // Radio pipe addresses for the 2 nodes to communicate.
char data[32], gotChars[32], payload[32];
uint8_t value;
bool alive, status;
#endif

void setup()
{
#if USE_RF24
    strcpy(payload, "Master Received OK");
    xxtea.setKey F("Crypt Password");
    pinMode(rdpin, INPUT_PULLUP);
#endif
    TXLED0;
    RXLED0;
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
    mySerial.begin(9600);
    ui.clear(); // display
//  ui.lcd_mode(1); // dual ht
#if USE_RF24
    String plaintext = F("**Radio check!**"); //16 chars == 16 bytes
    String result = xxtea.encrypt(plaintext);
    printf_begin();
    if (digitalRead(rdpin) == 0){ goto skipradio;}
    value = radio.begin();
    printit(F("Setup is: "));
    printit(value);
    sendnewline();
    delay(4000);
    ui.clear(); // display
    radio.setChannel(100);
//    radio.enableAckPayload();                    // Allow optional ack payloads
//    radio.enableDynamicPayloads();               // Ack payloads are dynamic payloads
    // Set the PA Level low to prevent power supply related issues since this is a
//  getting_started sketch, and the likelihood of close proximity of the devices. RF24_PA_MAX is default.
    radio.setPALevel(RF24_PA_LOW);
    // Open a writing and reading pipe on each radio, with opposite addresses
    radio.setDataRate(RF24_250KBPS);
//    radio.setAutoAck(1);                     // Ensure autoACK is enabled
//    radio.setRetries(1,15);                  // Optionally, increase the delay between retries & # of retries
    printit(F("Checking the radio"));
    value = radio.getChannel();
    setcursor(0, 1);
    printit(F("Channel is: "));
    printit(value);
    delay(3000);
    value = radio.getPALevel();
    setcursor(0, 2);
    printit(F("Power is: "));
    printit(value);
    delay(3000);
    value = radio.getDataRate();
    setcursor(0, 3);
    printit(F("Rate is: "));
    printit(value);
    sendnewline();
    radio.openWritingPipe(addresses[0]);
    radio.openReadingPipe(1,addresses[1]);
    delay(3000);
    ui.clear(); // display
    radio.startListening();                 // Start listening
    delay(100);
    radio.stopListening();                                    // First, stop listening so we can talk.
//    radio.writeAckPayload(1,&payload,32);          // Pre-load an ack-paylod into the FIFO buffer for pipe 1
    printit(F("Encrypted length:"));
    printit(result.length());
    setcursor(0, 1);
    printit(F("Encrypted data:"));
    setcursor(0, 2);
    printit(result);
    sendnewline();
    delay(2000);
    ui.clear();
//    strncpy(data, plaintext.c_str(), 32);
    strncpy(data, "Turn it on", 32);
    printit(F("Sending data"));
    status = radio.write(&data,32);
//    radio.txStandBy();               // Returns 0 if failed. 1 if success. Blocks only until MAX_RT timeout or success. Data flushed on fail.
    setcursor(0, 1);
    printit(F("Sent data maybe"));
    setcursor(0, 2);
    if ( status ){                         // Send the counter variable to the other radio
            radio.startListening();                 // Start listening
            unsigned long started_waiting_at = micros();               // Set up a timeout period, get the current microseconds
            boolean timeout = false;                                   // Set up a variable to indicate if a response was received or not

            while ( ! radio.available() ){                             // While nothing is received
              if (micros() - started_waiting_at > 20000000 ){            // If waited longer than 200ms, indicate timeout and exit while loop
                  timeout = true;
                  break;
              }
            }

            if ( timeout ){                                             // Describe the results
                printit(F("Failed, response timed out."));
            }else{
                                                                        // Grab the response, compare, and send to debugging spew
                radio.read( &gotChars, 32 );                  // Read it, and display the response time

                printit(F("Got response: "));
                setcursor(0, 3);
                printit(gotChars);
            }

    }else{        printit(F("Sending failed.")); }          // If no ack response, sending failed
    sendnewline();
    radio.printDetails();
skipradio:
#endif
    sendnewline();

    delay(3000);  // Try again later
    ui.clear();
    printit(F("Press a key to begin"));
    setcursor(0, 1);
    printit(F("or wait ten seconds"));
    setcursor(0, 2);
    while(mySerial.available() <= 0 && timeout < 100) {
        delay(100);
        if (timeout % 10 == 0) {
            printit(countdown);
            printit(F(":"));
            countdown--;
            if (countdown == 1) {
                setcursor(0, 3);
            }
        }
        timeout++;
    }
    while(mySerial.available()) {
        mySerial.read();
        delay(200);
    }
    ui.clear(); // display
    sendnewline();
    printit(F("System ready!!"));
    sendnewline();
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
#if USE_SERVO
    myservoa.attach(servopin1);  // attaches the servo on pin 9 to the servo object
    myservob.attach(servopin2);
    myservoa.write(0);              // tell servo to go to position in variable 'pos'
    myservob.write(0);
    delay(1000);
    myservoa.detach();  // attaches the servo on pin 9 to the servo object
    myservob.detach();  // attaches the servo on pin 9 to the servo object
#endif
}

int ledState = LOW;
int remember1 = 0;
int remember2 = 0;
int remember3 = 0;
int remember4 = 0;
int remember5 = 0;
int remember6 = 0;
int remember7 = 0;
int remember8 = 0;
int remember9 = 0;
int remember10 = 0;
int remember11 = 0;
int sendit = 0;
int gomouse = 0;
int gokeyboard = 0;
int goservo = 0;
int servoadj = 0;
int mouse = 0;
int blank = 0;
int reset = 0;
int poweroff = 0;
int poweron = 0;
int rservo = 0;
int pservo = 0;
int rightshift = 0;
int mousemove = 40;
int mousebiginc = 100;
int mousesmallinc = 5;
int mousemext;

void setcursor(uint8_t col, uint8_t row)
{
    ui.setCursor(col, row);
    mySerial.println();
}

void sendnewline(void)
{
    mySerial.println();
}

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

void downcheck(byte k)
{
    if (k == 128) {
        remember1 = 1;
    }
    if (k == 130) {
        remember2 = 1;
    }
    if (k == 50) {
        remember3 = 1;
    }
    if (remember1 == 1 && remember2 == 1 && remember3 == 1) {
        sendit = 1;
    }
#ifdef USE_MOUSE
    if (k == 129) {
        remember4 = 1;
    }
    if (k == 217) {
        remember5 = 1;
    }
    if (remember1 == 1 && remember4 == 1 && remember5 == 1) {
        gokeyboard = 1;
    }
    if (k == 218) {
        remember6 = 1;
    }
    if (remember1 == 1 && remember4 == 1 && remember6 == 1) {
        gomouse = 1;
    }
#endif // USE_MOUSE
#if USE_SERVO
    if (k == 52) {
        remember7 = 1;
    }
    if (remember1 == 1 && remember2 == 1 && remember7 == 1) {
        goservo = 1;
    }
    if (k == 53) {
        remember8 = 1;
    }
    if (remember1 == 1 && remember2 == 1 && remember8 == 1) {
        gokeyboard = 1;
    }
    if (k == 55) {
        remember9 = 1;
    }
    if (remember1 == 1 && remember2 == 1 && remember9 == 1) {
        reset = 1;
    }
    if (k == 56) {
        remember10 = 1;
    }
    if (remember1 == 1 && remember2 == 1 && remember10 == 1) {
        poweroff = 1;
    }
    if (k == 57) {
        remember11 = 1;
    }
    if (remember1 == 1 && remember2 == 1 && remember11 == 1) {
        poweron = 1;
    }
    if (k == 133) {
        rightshift = 1;
    }
#endif
}

void upcheck(byte k)
{
    if (k == 128) {
        remember1 = 0;
    }
    if (k == 130) {
        remember2 = 0;
    }
    if (k == 50) {
        remember3 = 0;
    }
    if (remember1 == 0 && remember2 == 0 && remember3 == 0 && sendit == 1) {
        sendit = 0;
        BootKeyboard.press(KEY_LEFT_CTRL);
        delay(100);
        BootKeyboard.press(KEY_LEFT_ALT);
        delay(100);
        BootKeyboard.press(KEY_DELETE);
        delay(100);
        BootKeyboard.releaseAll();
        ui.clear(); // display
//      ui.lcd_mode(0); // dual ht
        printit(F("Sent"));
        setcursor(0, 1);
        printit(F("CTRL+ALT+DELETE"));
        sendnewline();
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
        return;
    }
#if USE_MOUSE
    if (k == 129) {
        remember4 = 0;
    }
    if (k == 217) {
        remember5 = 0;
    }
    if (remember1 == 0 && remember4 == 0 && remember5 == 0 && gokeyboard == 1) {
        gokeyboard = 0;
        Mouse.end();
        BootKeyboard.begin();
        ui.clear(); // display
        printit(F("Keyboard mode"));
        setcursor(0, 2);
        printit(F("enabled"));
        sendnewline();
        mouse = 0;
        return;
    }
    if (k == 218) {
        remember6 = 0;
    }
    if (remember1 == 0 && remember4 == 0 && remember6 == 0 && gomouse == 1) {
        gomouse = 0;
        BootKeyboard.end();
        Mouse.begin();
        ui.clear(); // display
        printit(F("Mouse mode"));
        setcursor(0, 2);
        printit(F("enabled"));
        sendnewline();
        mouse = 1;
        return;
    }
#endif // USE_MOUSE
#if USE_SERVO
    if (k == 52) {
        remember7 = 0;
    }
    if (remember1 == 0 && remember2 == 0 && remember7 == 0 && goservo == 1) {
        goservo = 0;
        mouse = 0;
        servoadj = 1;
        ui.clear(); // display
        //ui.lcd_mode(0); // dual ht
        printit(F("Servo adjust mode"));
        setcursor(0, 1);
        printit(F("enabled. Press arrow"));
        setcursor(0, 2);
        printit(F("keys to move and"));
        setcursor(0, 3);
        printit(F("Space bar to set."));
        sendnewline();
        return;
    }
    if (k == 53) {
        remember8 = 0;
    }
    if (remember1 == 0 && remember2 == 0 && remember8 == 0 && gokeyboard == 1) {
        gokeyboard = 0;
        mouse = 0;
        servoadj = 0;
        ui.clear(); // display
        //ui.lcd_mode(0); // dual ht
        printit(F("Servo adjust mode"));
        setcursor(0, 1);
        printit(F("disabled."));
        sendnewline();
        myservoa.attach(servopin1);  // attaches the servo on pin 9 to the servo object
        myservob.attach(servopin2);
        myservoa.write(0);              // tell servo to go to position in variable 'pos'
        myservob.write(0);              // tell servo to go to position in variable 'pos'
        delay(1000);
        myservoa.detach();  // attaches the servo on pin 9 to the servo object
        myservob.detach();  // attaches the servo on pin 9 to the servo object
        return;
    }
    if (k == 55) {
        remember9 = 0;
    }
    if (remember1 == 0 && remember2 == 0 && remember9 == 0 && reset == 1) {
        reset = 0;
        ui.clear(); // display
        printit(F("Resetting the"));
        setcursor(0, 1);
        printit(F("computer now!!"));
        sendnewline();
        myservoa.attach(7);  // attaches the servo on pin 9 to the servo object
        myservoa.write(EEPROM.read(0));              // tell servo to go to position in variable 'pos'
        delay(1500);
        myservoa.write(0);              // tell servo to go to position in variable 'pos'
        delay(1000);
        myservoa.detach();  // attaches the servo on pin 9 to the servo object
        return;
    }
    if (k == 56) {
        remember10 = 0;
    }
    if (remember1 == 0 && remember2 == 0 && remember10 == 0 && poweroff == 1) {
        poweroff = 0;
        ui.clear(); // display
        printit(F("Powering off the"));
        setcursor(0, 1);
        printit(F("computer now!!"));
        sendnewline();
        myservob.attach(servopin2);  // attaches the servo on pin 9 to the servo object
        myservob.write(EEPROM.read(1));              // tell servo to go to position in variable 'pos'
        delay(powerdelay);
        myservob.write(0);              // tell servo to go to position in variable 'pos'
        delay(1000);
        myservob.detach();  // attaches the servo on pin 9 to the servo object
        return;
    }
    if (k == 57) {
        remember11 = 0;
    }
    if (remember1 == 0 && remember2 == 0 && remember11 == 0 && poweron == 1) {
        poweron = 0;
        ui.clear(); // display
        printit(F("Powering on the"));
        setcursor(0, 1);
        printit(F("computer now."));
        sendnewline();
        myservob.attach(servopin2);  // attaches the servo on pin 9 to the servo object
        myservob.write(EEPROM.read(1));              // tell servo to go to position in variable 'pos'
        delay(2000);
        myservob.write(0);              // tell servo to go to position in variable 'pos'
        delay(1000);
        myservob.detach();  // attaches the servo on pin 9 to the servo object
        return;
    }
    if (k == 133) {
        rightshift = 0;
    }
#endif
}

void printkey (void)
{
//	ui.lcd_mode(1); // dual ht
    ui.clear(); // display
    printit(F("A:: Key: "));
}

void loop()
{

    if (mySerial.available()) {
        byte k = mySerial.read();
        itoa(k, text, 10);
        mySerial.print(F("A:: "));
        mySerial.print(text);
        mySerial.println(F(" received"));
        if (k == 0) {
            press = 0;
        } else if (k == 1) {
            press = 1;
        } else {
            //mySerial.write(k);
            if (press) {
                if (mouse == 0 && servoadj == 0) {
                    printkey();
                    itoa(k, text, 10);
                    printit(String(text) + String(F(" Pressed")));
                    sendnewline();
                    BootKeyboard.press((KeyboardKeycode)pgm_read_byte(&keysims[k]));
                    //mySerial.println(" pressed");
                } else if (servoadj == 1) {
#if USE_SERVO
                    switch (k) {
                    case 218:
                        // move mouse up
                        if (rservo < 170) {
                            rservo = rservo + rightshift * 9 + 1;
                        }
                        myservoa.attach(servopin1);  // attaches the servo on pin 9 to the servo object
                        myservoa.write(rservo);              // tell servo to go to position in variable 'pos'
                        delay(servodelay + rightshift * rsdelay);
                        myservoa.detach();  // attaches the servo on pin 9 to the servo object
                        ui.clear(); // display
                        printit(F("Servo reset in."));
                        setcursor(0, 1);
                        printit(F("Servo pos is: "));
                        printit(rservo);
                        sendnewline();
                        break;
                    case 217:
                        // move mouse down
                        if (rservo > 10) {
                            rservo = rservo - rightshift * 9 - 1;
                        }
                        myservoa.attach(servopin1);  // attaches the servo on pin 9 to the servo object
                        myservoa.write(rservo);              // tell servo to go to position in variable 'pos'
                        delay(servodelay + rightshift * rsdelay);
                        myservoa.detach();  // attaches the servo on pin 9 to the servo object
                        ui.clear(); // display
                        printit(F("Servo reset out"));
                        setcursor(0, 1);
                        printit(F("Servo pos is: "));
                        printit(rservo);
                        sendnewline();
                        break;
                    case 216:
                        // move mouse left
                        if (pservo < 170) {
                            pservo = pservo + rightshift * 9 + 1;
                        }
                        myservob.attach(servopin2);  // attaches the servo on pin 9 to the servo object
                        myservob.write(pservo);              // tell servo to go to position in variable 'pos'
                        delay(servodelay + rightshift * rsdelay);
                        myservob.detach();  // attaches the servo on pin 9 to the servo object
                        ui.clear(); // display
                        printit(F("Servo power in"));
                        setcursor(0, 1);
                        printit(F("Servo pos is: "));
                        printit(pservo);
                        sendnewline();
                        break;
                    case 215:
                        // move mouse right
                        if (pservo > 10) {
                            pservo = pservo - rightshift * 9 - 1;
                        }
                        myservob.attach(servopin2);  // attaches the servo on pin 9 to the servo object
                        myservob.write(pservo);              // tell servo to go to position in variable 'pos'
                        delay(servodelay + rightshift * rsdelay);
                        myservob.detach();  // attaches the servo on pin 9 to the servo object
                        ui.clear(); // display
                        printit(F("Servo power out"));
                        setcursor(0, 1);
                        printit(F("Servo pos is: "));
                        printit(pservo);
                        sendnewline();
                        break;
                    case 32:
                        // perform mouse left click
                        EEPROM.update(0, rservo);
                        EEPROM.update(1, pservo);
                        ui.clear(); // display
                        printit(F("Positions saved"));
                        setcursor(0, 1);
                        printit(F("to EEPROM"));
                        sendnewline();
                        break;
                    }
#endif
                } else {
#if USE_MOUSE
                    mousemext = remember1 * (mousebiginc - mousemove) - remember4 * (mousemove - mousesmallinc);
                    ui.clear(); // display
                    switch (k) {
                    case 218:
                        // move mouse up
                        Mouse.move(0, -40 - mousemext);
                        printit(F("Mouse up"));
                        break;
                    case 217:
                        // move mouse down
                        Mouse.move(0, 40 + mousemext);
                        printit(F("Mouse down"));
                        break;
                    case 216:
                        // move mouse left
                        Mouse.move(-40 - mousemext, 0);
                        printit(F("Mouse left"));
                        break;
                    case 215:
                        // move mouse right
                        Mouse.move(40 + mousemext, 0);
                        printit(F("Mouse right"));
                        break;
                    case 32:
                        // perform mouse left click
                        Mouse.click(MOUSE_LEFT);
                        printit(F("Click left"));
                        break;
                    case 134:
                        // perform mouse right click
                        Mouse.click(MOUSE_RIGHT);
                        printit(F("Click right"));
                        break;
                    }
                    sendnewline();
#endif // USE_MOUSE
                }
                downcheck(k);

            } else {
                if (mouse == 0 && servoadj == 0) {
                    printkey();
                    itoa(k, text, 10);
                    printit(String(text) + String(F(" Released")));
                    sendnewline();
                    BootKeyboard.release((KeyboardKeycode)pgm_read_byte(&keysims[k]));
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
