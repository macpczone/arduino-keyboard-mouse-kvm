#include <Arduino.h>

#if USE_ETHERNET
#include "Dhcp.cpp"
#include "Dns.cpp"
#include "Ethernet.cpp"
#include "EthernetClient.cpp"
#include "EthernetServer.cpp"
#include "EthernetUdp.cpp"
#include "socket.cpp"
#include "w5100.cpp"
#endif

#if USE_LCD
#include "LiquidCrystal.cpp"
#endif

#if USE_SD
#include "SD.cpp"
#include "Sd2Card.cpp"
#include "SdFile.cpp"
#include "SdVolume.cpp"
#include "File.cpp"
#endif

#if USE_SERVO
#include "Servo.cpp"
#endif

#if USE_SOFTSERIAL
#include "SoftwareSerial.cpp"
#endif

#if USE_SPI || USE_ETHERNET
#include "SPI.cpp"
#endif

#if USE_STEPPER
#include "Stepper.cpp"
#endif

#if USE_WIRE
#include "Wire.cpp"
#include "twi.c"
#endif

