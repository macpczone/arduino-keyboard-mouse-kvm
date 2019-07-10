# Arduino Keyboard Mouse KVM
Arduino code for keyboard and mouse control of a computer along with optional power control

Here is the Arduino code for an IP-KVM project that I am working on. This code is in the form of a Code::Blocks project that can be opened run to build an uploadable HEX file for an Arduino Pro Micro. The Arduino Pro Micro uses an ATMega32U4 chip, which has a built in USB port that can operate in OTG mode, which then allows it to be used as a keyboard or mouse.

https://github.com/macpczone/arduino-keyboard-mouse-kvm

There is a counterpart to this project which is at:

https://github.com/macpczone/keyboard-passthrough

and this other project contains a python file that can be run on another machine, which can then send the keycodes to the Arduino, so that it can pass them onto a machine that is to be controlled with a keyboard or mouse.


Below are the libraries that you will need to build the code:

[Arduino HID Project 2.4.4](https://github.com/NicoHood/HID)
This is the core HID library that provides the keyboard and mouse support.

[LCD Library for Arduino and Chipkit](https://bitbucket.org/fmalpartida/new-liquidcrystal/wiki/Home)
This library supports the 20x4 LCD with an LCM1602 IIC driver chip.

[SoftPWM Library](https://github.com/bhagman/SoftPWM)
This library provides the fading LED, which is just there to show that the code is working.

[TimerOne Library](https://github.com/PaulStoffregen/TimerOne)
This library is just used to provide the interrupt code for the fading LED.

The utils directory also contains some other code that I have written, which are used in the demonstration videos.

Here is an image of the how the Pro Micro is connected:

![Circuit](http://www.macpczone.co.uk/sites/default/files/styles/page_800/public/top_images/arduino_kvm_bb_0.png)
