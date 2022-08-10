# bobsleigh-monitor

This repo contains the Arduino sketch used to monitor the force applied to a custom bobsleigh arm with the goal of giving athletes useful data for training.

## Hardware

This code targets the Arduino Nano IoT 33.

## Libraries

This code uses [bogde's excellent library](https://github.com/bogde/HX711) for HX711 load cell amplifiers

The code also uses the [Arduino_LSM6DS3](https://www.arduino.cc/reference/en/libraries/arduino_lsm6ds3/) library for accelerometer and gyroscope readings.
