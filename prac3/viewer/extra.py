# # Python Library Imports
import serial
import re

# # define the serial port and baudrate
SERIAL_PORT_NAME = '/dev/ttyACM0'
SERIAL_BAUD_RATE = 115200

serial_port_data = serial.Serial(SERIAL_PORT_NAME, SERIAL_BAUD_RATE)
iterval = 0
node_values = [0, 0, 0, 0, 0, 0, 0, 0]
def main():
    iterval = 0
    while True:
        try:
            iterval += 1
            str_port_data = serial_port_data.readline().decode("utf-8").strip()
            split_data = str_port_data.split(',')
            if split_data[0] == "":
                continue
            print(split_data)
            
        except KeyboardInterrupt:
            break

main()