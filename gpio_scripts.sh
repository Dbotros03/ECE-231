#!/bin/bash

# Set up PWM on P8_19
echo 22 > /sys/class/gpio/export
echo out > /sys/class/gpio/gpio22/direction
echo 1 > /sys/class/gpio/gpio22/value

# Configure P8_8 as an input pin
echo 67 > /sys/class/gpio/export
echo in > /sys/class/gpio/gpio67/direction
