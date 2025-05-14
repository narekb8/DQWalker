# DQWalker

A small, Arduino based project to create a fully-fledged interactive pedometer inspired by toys from my childhood like Tamagotchis and Pokemon's Pokewalker, but instead based on the Slime from Dragon Quest and its variants from the game series. This was originally made as a birthday gift to my girlfriend which quickly spiraled out of control in its scope.

The code is essentially fully written aside from some small power-draw optimizations which cause clock errors on the MCU in their current implementations. PCB and 3D case designs are currently WIP, as I've never done designs before, but a working PCB module is complete. I hope to eventually port this over to the base SAMD21 SDK, but Arduino was used for the convenience of the available libraries (even if some of them don't work) and because this was technically my first foray into developing for microcontrollers.

## Device List - "Breakout" PCB
- LSM6DSO (SparkFun Breakout Board)
- SHARP Memory LCD LS013B7DH06 (replacing the DH03 that is used on the Adafruit board)
- Seeed Studio XIAO SAMD21
- Adafruit USB-C LiPo Charger and 350mAh battery

## TODO:
- ~Implement Low Power mode to make the device more battery efficient~
- ~Wire up the board to work on battery with charging support (likely with a tiny Adafruit board to start)~
- ~Create a functional PCB to at least wire up breakout boards to make a functioning device~
- Consolidate the above board to be a single, contained device (sans the XIAO)
- If I'm really feeling confident, include SAMD21 on the PCB
