#!/bin/bash

# Building the application
west build -p -b nrf52840dongle_nrf52840

# Enabling access to the ACM0 Port and/ or USB0 Port
{
    sudo chmod 777 /dev/ttyACM0 &&
    sudo chmod 777 /dev/ttyUSB0 ||
    echo "USB Devices Invalid :("
}

# Generating the ZIP for the bootloader
nrfutil pkg generate --hw-version 52 --sd-req=0x00 \
        --application build/zephyr/zephyr.hex \
        --application-version 1 practical_two.zip


# Loading the application to the bootloader
nrfutil dfu usb-serial -pkg practical_two.zip -p /dev/ttyACM0

# Removing the previous build
rm build/ -rf

# Removing the previous zip
rm -rf practical_two.zip