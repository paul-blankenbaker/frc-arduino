# UNO Encoder Tester

This project allows an Ardiuno UNO to act as 3 digital inputs similar to what you would find on a roboRIO. The intended purpose of this hardware/software combination is to measure feedback from quadrature encoders that might be connected to the roboRIO without the need of a roboRIO.

* Verify quadrature encoder is working prior to installing in robot.
* Verify wiring from the encoder to the roboRIO (unplug the 2 or 3 PWM cables from the roboRIO into the UNO and verify that the encoder is still working).

While this project was designed as a test for the AMT103 encoder (http://www.cui.com/product/resource/amt10-v.pdf), it should work with any 5V encoder that can be used with an FRC roboRIO (http://www.ni.com/pdf/manuals/374474a.pdf).

## Hardware Overview

This project can be completed on a bread board, but if you want to make it a permanent tool, it is suggested to solder up a prototype board that can be quickly plugged into your UNO.

In addition to the primary purpose of testing an encoder, this project provides the following features:

* Three LEDs are provided that indicate the state of the A channel, B channel and IDX channel from the encoder.
* One momentary button is provided. The button is used to control the behavior of the board.
* One PWM output and an analog input is provided. This is not currently supported, but will eventually allow the board to control a single speed controller (in case you need to drive a PWM controlled motor to rotate the encoder during test).
* Two digital outputs are combined to provide a Relay control.

## Software Overview

The reference implementation provides the following behavior:

* 3 digital inputs are used to read data quickly from the encoder channels.
* 3 digital outputs are used to light LEDs to reflect the state of the 3 encoder channels.
* 1 digital input is used as a "user interface" (the button).
  * Clicking a digital input quickly will change the state
  * Holding down for at least a second will cause report to be generated to the console and counts to be zeroed.
* 2 digital outputs provide the ability to control a FRC Relay.
* 1 digital output operates in PWM mode with the intent of being able to control a FRC speed controller.
* 1 analog input is used as the user setting for the PWM output signal.

## Troubleshooting

### Floating Inputs

Being a programmer, I tend to think of reading digital inputs as coming back as 0 or 1 indicating that there is either 0V or 5V on the digital input line. While the Arduino does need to map the results of reading a digital input as either a one or zero for programming purposes, it does not necessarily mean that there is 0V or 5V currently connected. Here are some typical states you might find on an Arduino pin:

* 0V when the pin is tied to ground.
* 5V when the pin is tied to a 5V source.
* Some voltage in the range of 0V to 5V provided by a connected sensor. However, these values will typically map to a logical true or false cleanly.
* Some _RANDOM FLOATING_ value when the pin is not physically connected.

In the floating case the software running on the Uno will see semi random true/false values.

It is easy to create an example of this using this project. Simply run the program without connecting an encoder (leave the ChA, ChB and IDX channels disconnected). The corresponding ChA, ChB and IDX LEDs might be on, off, flickering or some combination when running in this state and simply running your finger across the ChA, ChB and IDX pins may cause them to temporarily change.