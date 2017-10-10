# arduino-keyboard-mouse-kvm
Arduino code for keyboard and mouse control of a computer along with optional power control

Here is the Arduino code for an IP-KVM project that I am working on. This code is in the form of a Code::Blocks project that can be opened run to build an uploadable HEX file for an Arduino Pro Micro. The Arduino Pro Micro uses an ATMega32U4 chip, which has a built in USB port that can operate in OTG mode, which then allows it to be used as a keyboard or mouse.

There is a counterpart to this project which is at:

https://github.com/macpczone/keyboard-passthrough

and this other project contains a python file that can be run on another machine, which can then send the keycodes to the Arduino, so that it can pass them onto a machine that is to be controlled with a keyboard or mouse.
