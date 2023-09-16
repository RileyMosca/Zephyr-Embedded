_______
# Advanced Embedded Systems Repository (Zephyr RTOS)
_______

## Repository Author:
Riley Leigh Mosca 

This repository contains all contents related to my contributions across three projects. All projects implement Zephyr RTOS, and build onto eachother. These series of projects focus on utilising a simple Host Controller Interface (HCI), which is abstracted as a protocol to be understood by 2 or more peripheral devices (or other microcontrollers of the same or different families). This repository focusses on the Application Host Unit (AHU) side , whose responsible for commanding data from additional peripherals/ microcontrollers (often referred to as the Sensor Control Unit (SCU)). 

## Practical One:
The first practical utilises a simple UART communication between the AHU and SCU to access sensor data from a number of peripherals on the SCU. This is achieved through the implementation of a shell command line interface. The main microcontollers used for this were part of the ESP32 and STM32 families.

## Practical Two:
This practical continued onto the first by replacing the UART communication into a Bluetooth Low -Energy (BLE) implementation. The same functionality exists, but with wireless communication. The HCI structure modified to allow for more control over peripheral devices (as additional peripherals were added to the SCU). In addition to practical one, data received over bluetooth was written to serial, to which a python script was able to extract, format and send the data to an InfluxDB API for querying and visualisation on Grafana. The main microcontollers used for this were part of the ESP32, Nordic Semiconductor and STM32 families.

## Practical Three:
The third practical introduced supervised machine learning techniques for localisation of a Mobile Node (Mobile Microcontroller) that reported data to a Base Node (Base Micrcontroller). The Mobile Node was constantly reading data from several Static Nodes (BLE Beacons) which contained information on the RSSI (Signal Strength) from the Static Node. This data was sent to the Base Node which was then written to the serial port of the Host PC. The Host PC was able to then decode the location of the Mobile Node (within a grid) based on the RSSI values recorded and the ML Model. This position (x and y) in addition to the 8 Static Nodes was sent to an InfluxDB API for querying and displaying on Grafana. The Base Node also implemnted a Singly-Linked-List using Zephyr libraries, to allow dynamic addition and removal of Static Nodes to the system of Static Nodes. 

