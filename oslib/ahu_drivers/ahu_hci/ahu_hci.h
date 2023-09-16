#ifndef AHU_HCI_H
#define AHU_HCI_H

#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

extern struct k_msgq packet_queue;

/* Defining Packet Contents below*/
#define PREAMBLE 0b10101010        /* Packet Preamble 0xAA -> 1010 1010 */

/* Message Type Bits */
#define TYPE_REQUEST 0b0001              /* Rquest Type 0x01 -> 0001 */
#define TYPE_RESPONSE 0b0010             /* Rquest Type 0x02 -> 0010 */

/* Message Length Bits */
#define PACKET_LENGTH 0b1001        /* Length (bytes) 0x09 -> 1001 */

/* Device Filler Bits*/
#define DEVICE_MASK 0b0000          /* Device Bits 0x0 -> 0000 (will be populated)*/

/* Read/ Write Bits */
#define DEVICE_NO_READ  0b00         /* End continuous sampling */
#define DEVICE_CONT_SAMPLE 0b11
#define DEVICE_READ 0b01            /* Reading 0x01 -> 01 */
#define DEVICE_WRITE 0b10           /* Writing 0x02 -> 10 */


/* Hardware Instruction Bits*/
#define NO_HW_ACTION 0b00           /* No Harware Action (we aren't controlling HW) 0x0 -> 00*/
#define HW_SET_LOW 0b01             /* Configure Hardware to Logical Low 0x01 -> 01*/
#define HW_SET_HIGH 0b10            /* Configure Hardware to Logical High 0x02 -> 10*/

#define SAMPLING_MASK 0b0000000000000000

#define DATA_BYTE_SIZE 16

/* Device IDs */
#define NO_DEV      0b00000
#define TEMP_DEV    0b00001
#define PRES_DEV    0b00010
#define ACCEL_X_DEV 0b00011
#define ACCEL_Y_DEV 0b00100
#define ACCEL_Z_DEV 0b00101
#define GYRO_X_DEV  0b00110
#define GYRO_Y_DEV  0b00111
#define GYRO_Z_DEV  0b01000
#define MAGN_X_DEV  0b01001
#define MAGN_Y_DEV  0b01010
#define MAGN_Z_DEV  0b01011
#define ULTRAS_DEV  0b01100
#define ROLL_DEV    0b01101
#define PITCH_DEV   0b01110
#define FIN_ALT_DEV 0b01111 // Fine Altitude
#define COA_ALT_DEV 0b10000 // Coarse Altitude
#define PB_DEV      0b10001
#define SCU_LD0_DEV 0b10010
#define SCU_LD1_DEV 0b10011
#define ALL_DEVS    0b10100
#define CON_SMP_DEV 0b10101 // Continuous sampling
#define SAMPLE_DEV  0b10110 // Change sample rate
#define SNODE_RM    0b10111 // Remove Static Node
#define SNODE_ADD   0b11000 // Add Static Node
#define SNODE_QUERY 0b11001 // Query Static Node
#define SNODE_QUERY_ALL 0b11010 // Query all static Nodes

#define N_DEVS 21

/* Initialised array to 40 bits set to 0 */
#define ZEROS(MSG_DATA, VALUE, SIZE) memset(MSG_DATA, VALUE, SIZE)

typedef struct __attribute__((__packed__)) hci_packet_t {
    
    /* 8-bit Message Preamble (pre-defined) */
    uint8_t msg_preamble;

    /* 
        8-bit variable, storing two variables
        4-bit Message Type (Bits 8 downto 5) &
        4-bit Message length (Bits 4 downto 1)
    */
    uint8_t msg_type_len;
    
    /* 
        8-bit variable, storing two variables
        4-bit Empty/unused (Bits 8 downto 5) &
        2-bit Read/ Write bits (Bits 4 downto 3)
        2-bit Hardware Instructions (Bits 2 downto 1)
    */
    uint8_t msg_rw_hw;

    /*
        8-bit variable, storing the device id
    */
    uint8_t msg_dev_id;

    /* 
        40 bit/ 5 bytes (each index
        containing 8 bits) of data
        16 bytes now
    */
    int8_t msg_data[DATA_BYTE_SIZE];

} hci_packet;

void init_hci_packet(hci_packet* packet_msg);

void construct_packet_message(hci_packet* packet_msg, uint16_t dev_sample_rate, 
    uint8_t msg_type, uint8_t device, uint8_t rw_bits, uint8_t hw_bits);

bool compare_data_packets(hci_packet packet, hci_packet comparison);

uint8_t get_device_id(hci_packet packet);

char* get_device_string(hci_packet packet);

char* get_device_units(hci_packet packet);

void get_device_data_units_string(hci_packet packet, int sampleFlag);

#endif