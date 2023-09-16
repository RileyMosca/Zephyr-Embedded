/**
***************************************************************
* @file ~/csse4011/csse4011_repo/ahu_drivers_ahu/ahu_hci.c
* @author Riley Mosca - 45358195
* @date 03 - 02 - 2023
* @brief 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************/

/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Zephyr Includes */
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

/* Local Includes*/
#include "ahu_hci.h"

char* device_strs[16] = {"Temperature: ", "Pressure: ", 
        "Acceleration (X): ", "Acceleration (Y)", "Acceleration (Z)",
        "Gyroscope (X): ", "Gyroscope (Y): ", "Gyroscope (Z): ",
        "Magnetometer (X): ", "Magnetometer (Y): ", "Magnetometer (Z): ",
        "Push Button: ", "SCU LED 0: ", "SCU LED 1:",
        "All Devices", "Sample Rate: "
};

char* device_units[16] = {"°C\n", "psi\n", 
        "m/s^2\n", "m/s^2\n", "m/s^2\n",
        "dps\n", "dps\n", "dps\n",
        "Gauss\n", "Gauss\n", "Gauss\n",
        "", "", "",
        "", " samples/second\n"
};

uint8_t device_ids[16] = {
    0b0000, 0b0001, 0b0010, 0b0011,
    0b0100, 0b0101, 0b0110, 0b0111,
    0b1000, 0b1001, 0b1010, 0b1011,
    0b1100, 0b1101, 0b1110, 0b1111
};

static float device_data[11];

/* 
    Function which accesses a pointer to a HCI Packet and
    initialises the data of that packet to default values

    @param packet_msg: A pointer to the hci_packet we wish
    to configure to default
*/
void init_hci_packet(hci_packet* packet_msg) {

    packet_msg->msg_preamble = PREAMBLE; 
    packet_msg->msg_type_len = PACKET_LENGTH;
    packet_msg->msg_rw_hw = NO_HW_ACTION;
    packet_msg->msg_dev_id = 0;

    ZEROS(packet_msg->msg_data, 0, sizeof(packet_msg->msg_data));
}

/* 
    Function which accesses a pointer to a HCI Packet and
    constructs it based on the SHELL Command we enter

    @param packet_msg: A pointer to the hci_packet we wish
    to construct

    @param dev_rw_hw: 8 bits that represent the device (bits 8 to 5)
    and the read/write bits (bits 4 to 3) and the HW Instruction
    bits (bits 2 to 1)

    @param dev_sample_rate: 8 bits that represent the device
    sampling rate (for the sensors of the SCU)
*/
void construct_packet_message(hci_packet* packet_msg, uint16_t dev_sample_rate, 
    uint8_t msg_type, uint8_t device, uint8_t rw_bits, uint8_t hw_bits) {

    /* Values that are not handled by the Application Host Unit*/
    packet_msg->msg_preamble = PREAMBLE; 
    packet_msg->msg_type_len = (msg_type << 4) | PACKET_LENGTH;
    ZEROS(packet_msg->msg_data, 0, sizeof(packet_msg->msg_data));

    /* Values that are handled by the AHU through the SHELL commands*/
    packet_msg->msg_rw_hw = (rw_bits << 2) | (hw_bits);

    packet_msg->msg_dev_id = device;
    
    /* Setting 16 bits of data message to sampling rate */
    packet_msg->msg_data[0] = (dev_sample_rate >> 8);
    packet_msg->msg_data[1] = (dev_sample_rate ) & 0b11111111;

    /* Padding last 8 bits of data message with a new line termiantor */
    packet_msg->msg_data[4] = '\n';
}

/*
    Function that detects compares packet contents, it will return true
    if theyre the same and false if they are different
*/
bool compare_data_packets(hci_packet packet, hci_packet comparison) {

    /* Comparing the packet message data */
    bool preamble = packet.msg_preamble == comparison.msg_preamble;
    bool type_len = packet.msg_type_len == comparison.msg_type_len;
    bool rw_hw = packet.msg_rw_hw == comparison.msg_rw_hw;
    bool dev_id = packet.msg_dev_id == comparison.msg_dev_id;
    bool data_0 = packet.msg_data[0] == comparison.msg_data[0];
    bool data_1 = packet.msg_data[1] == comparison.msg_data[1];
    bool data_2 = packet.msg_data[2] == comparison.msg_data[2];
    bool data_3 = packet.msg_data[3] == comparison.msg_data[3];
    bool data_4 = packet.msg_data[4] == comparison.msg_data[4];

    bool data = data_0 && data_1 && data_2 && data_3 && data_4;

    return preamble && type_len && rw_hw && type_len && data && dev_id;
} 

/*
    Function that returns the device id as a unsigned
    8bit integer, in the form 0b0000ABCD, where ABCD
    are the last 4 bits representing the device that
    maps to the macro
*/
uint8_t get_device_id(hci_packet packet) {
    return packet.msg_dev_id;
}


/*
    Function that takes in a packet, and returns a human representation
    of that device.
*/
char* get_device_string(hci_packet packet) {
    uint8_t device = get_device_id(packet);
    return device_strs[device];
}

/*
    Function that gets the device units given the device
    from the packet
*/
char* get_device_units(hci_packet packet) {
    uint8_t device = get_device_id(packet);
    return device_units[device];
}


/*
    Function that prints a string representing the data and units
*/
void get_device_data_units_string(hci_packet packet, int sampleFlag) {
    char* device = get_device_string(packet);
    char* units = get_device_units(packet);
    float data;
    uint16_t sample_rate = (packet.msg_data[0] << 8) | packet.msg_data[1];

    if(sampleFlag) {
        printf("\r\n%s %d %s\r\n", device, sample_rate, units);
    }
    else {
        memcpy(&data, packet.msg_data,sizeof(float));
        printf("\r\n%s %f %s\r\n", device, data, units);
    }    
}