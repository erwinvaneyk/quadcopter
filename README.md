# Quadcopter Controller
This repository contains all resources for a quadcopter controller for the [X32 softcore](http://x32.ewi.tudelft.nl/). The quadcopter controller software was developed as part of the quadcopter project of the course IN4073 Embedded Real-Time Systems taught at the TU Delft.

## Requirements
See the [project resource page](http://www.st.ewi.tudelft.nl/~koen/in4073/Resources/index.html).

## Usage

### 1. Uploading the control software to the qr.
- Ensure that the quadcopter is accessible over a usb-serial link. By default `/dev/ttyusb0` is used.
- In `ufo/fpga`, make and upload the code to the quadcopter. Run `make uploadrun` to perform this whole process in one command.
- To verify that the software has been succesfully uploaded and is running on the quadcopter, led 1 should be blinking at 1 Hz.

### 2. Starting the terminal program
- In `ufo/pc`, compile the terminal using `make term`. The program will be compiled to an executable `term`.
- To run the terminal simply run `./term`. Again here the the usb-serial link is assumed to be at `/dev/ttyusb0`.
- The program has various configurable options, which can be listed using `./term -h`.
- Note: the use of a joystick is by default disabled. To enable it, run the program as `./term -j <path/to/joysticktty>`.

## 3. Starting the qr.
- The quadcopter is by default in **safe mode**. This means that the quadcopter will remain stationary and will not respond to any control input.
- To start flying, press `2` to enable manual mode for example. You will now be able to fly using the quadcopter using the controls listed in the terminal. For the full specification of the controls see section C in the [project manual](http://www.st.ewi.tudelft.nl/~koen/in4073/assignment.pdf).

