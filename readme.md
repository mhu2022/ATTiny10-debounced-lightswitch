
# Uploading to the ATTiny10
manual upload via Monitor

## Upload programmer
On an Arduino UNO, upload:
programmer_tpi\ArduinoISP_ATtiny10TPI\ArduinoISP_ATtiny10TPI.ino with the arduino IDE
After that place a 10uF Capacitor over the RST and GND pins.

## Connection to upload the compiled program:
F:\ARDUINO\ATTINY\ATTINY10\attiny10-lightswitch\.pio\build\attiny10\firmware.hex
connect:

```code
   Arduino Uno             ATtiny10
   ----------+          +----------------
   (SS#)  10 |--[R]-----| 6 (RESET#/PB3)
             |          |
   (MOSI) 11 |--[R]--+--| 1 (TPIDATA/PB0)
             |       |  |
   (MISO) 12 |--[R]--+  |
             |          |
   (SCK)  13 |--[R]-----| 3 (TPICLK/PB1)
   ----------+          +----------------
 *                                                *
   ----------+          +----------------
   (HVP)   9 |---       | 6 (RESET#/PB3)
             |          |
 *                                                *
    -[R]-  =  a 220 - 1K Ohm resistor
 *                                                *
    GND                  D3 (mode selection)
```

## Compiling

## Uploading
After compiling the program, the binary can be uploaded via the Monitor in platformio (VSCode)

### platformio.ini:
```code
monitor_port = COM10
monitor_speed = 9600
monitor_echo = yes
monitor_filters = send_on_enter
monitor_eol = CRLF
```

## Program the flash memory
paste the content of the .pio\build\attiny10\firmware.hex as one-line After the capital P:
```code
>P:100000000AC011C010C00FC00EC00DC00CC00BC084:100010000AC009C008C011271FBFCFE5D0E0DEBF6E:10002000CDBF02D00BC0ECCF4FE35DE063E04150A9:1000300050406040E1F700C00000F6CFF894FFCFD9:00000001FF

NVM enabled
chip erased

Successfully wrote program: 64 of 1024 bytes
 in 0.2619999885 Seconds
>NVM enabled
```


## Examples of failing to program the flash memory
```code
>P
NVM enabled
chip erased
couldn't receive data:
hex file format error
```

```code
>P :100000000AC011C010C00FC00EC00DC00CC00BC084
:100010000AC009C008C011271FBFCFE5D0E0DEBF6ENVM enabled
chip erased

:10002000CDBF02D017C0ECCF099A119A4FE956E81Ccouldn't receive data:
hex file
:1000300061E0415050406040E1F700C0000011987D format error
```



## Erase the flash memory
```code
>E
NVM enabled
chip erased

>NVM enabled
Received unknown command

>NVM enabled
Received unknown command
```


2026-04-19
https://docs.platformio.org/en/latest/core/userguide/device/cmd_monitor.html#cmd-device-monitor-filters





# Functionality Overview of main.cpp

This is a **low-power button controlled switch** for an ATtiny10 microcontroller that controls a device based on manual push button input. Here's the breakdown:

## Core Purpose
Demonstrate debouncing and toggle switching with an ATTiny10.

## Key Components

**Hardware Pins:**
- `PB0` - Output signal (bit-banged communication)
- `PB2` - Manual button input (wakeup trigger)

## Interrupt Handlers

- **Pin Change Interrupt (`PCINT0_vect`)** - Button press on PB2 immediately triggers a measurement

## Power Strategy

- Runs in **SLEEP_MODE_PWR_DOWN** (deepest sleep)
- ADC completely disabled
- Only woken by button press
- This minimizes current draw during sleep periods



DUMP OF A NE ATTINY10
>D
NVM enabled
Current memory state:

registers, SRAM
      +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
0000: 01 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0010: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0020: 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 
0030: 00 00 00 00 00 00 03 00 00 87 00 03 00 00 00 00 
0040: 4B 8A 34 BB 0C F1 75 00 E5 C6 41 30 C2 72 CD 3E 
0050: DB 5D 95 D0 0F 1E 2F 2C 55 10 59 D0 3F 43 EE 5F 
NVM lock
      +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
3F00: FF FF 
configuration
      +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
3F40: FF FF 
calibration
      +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
3F80: 87 FF 
device ID
      +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
3FC0: 1E 90 03 FF 
program
      +0 +1 +2 +3 +4 +5 +6 +7 +8 +9 +A +B +C +D +E +F
4000: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4010: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4020: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4030: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4040: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4050: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4060: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4070: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4080: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4090: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
40A0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
40B0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
40C0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
40D0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
40E0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
40F0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4100: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4110: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4120: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4130: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4140: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4150: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4160: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4170: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4180: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4190: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
41A0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
41B0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
41C0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
41D0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
41E0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
41F0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4200: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4210: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4220: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4230: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4240: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4250: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4260: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4270: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4280: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4290: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
42A0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
42B0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
42C0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
42D0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
42E0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
42F0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4300: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4310: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4320: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4330: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4340: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4350: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4360: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4370: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4380: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
4390: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
43A0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
43B0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
43C0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
43D0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
43E0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF 
43F0: FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF FF  

>NVM enabled
Received unknown command

>NVM enabled
Received unknown command

>