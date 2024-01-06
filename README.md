# ESPHome Jiecang Desk Controller

Sensors and Actors for Jiecang Desk controle boxes using [ESPHome](https://esphome.io/).

Attention: I only have a RJ12 desk controller to test this but chances are very high
that all Jiecang controllers will work as they all use more or less the same serial protocol.

## Usage

### What you need

* ESPHome compatible microcontroller
* depending on your model of desk controller
    * cable with RJ12 connector (phone cable, RJ11 may also work)
    * cable with RJ45 connector (network cable)

### Wiring

#### RJ12

pin | function
----|---------
 1  | NC (pulled up)
 2  | GND
 3  | TX
 4  | VCC
 5  | GND
 6  | NC (pulled up)

#### RJ45

pin | function
----|---------
 1  | HS3 [^1]
 2  | TX
 3  | GND
 4  | RX
 5  | VCC
 6  | HS2 [^1]
 7  | HS1 [^1]
 8  | HS0 [^1]

[^1]: not used here

#### microcontroller

ESP | desk
----|-----
GND | GND
5V  | VCC
RX  | TX
TX  | RX

## Installation

Before installing the config, you need to copy `jiecang_desk_sensor.h` to your ESPHome sketch folder

For example: `scp jiecang_desk_sensor.h hassio@homeassistant:config/esphome/`

###  minimal vs full version

#### minimal (default)

The default/minimal version only contains basic functionality. This should cover most daily needs and is what I use myself.

#### full

The full version contains most known functions of the serial protocol. Many if them are commented out by default, you can
uncomment what you need and/or copy paste it to the simple sketch.
It also implements an __experimental__ continuous movement mode which basicly simulates holding the up/down buttons.

> [!CAUTION]
> Continous movement can only be stopped via this controller! __The buttons of the original controller will not stop the movement!__  
> This can cause damage or even hurt someone, use at own risk!

If you only need some of the extended functionality I recommend to copy only those part to the minimal version.

## Sources

Thanks to [phord/Jarvis](https://github.com/phord/Jarvis) for reverse engineering the UART interface and most control messages

