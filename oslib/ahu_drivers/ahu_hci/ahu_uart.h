#ifndef AHU_UART_H
#define AHU_UART_H

#include <stdbool.h>

/* change this to any other UART peripheral if desired */
// #define UART_DEVICE_NODE DT_CHOSEN(uart1)
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)
#define MSG_SIZE 32

#define AHU_UART_SHELL_RECV_STACK_PRIOTIY 1
#define AHU_UART_SHELL_RECV_STACK 1000

/* Declaring Receive (RX) buffer user in UART ISR callback */
static char rx_buf[MSG_SIZE];

/* Declaring the Receive (RX) Buffer character position */
static int rx_buf_pos;

extern struct k_msgq uart_msgq;

void serial_cb(const struct device *dev, void *user_data);

void print_uart(char *buf);

void ahu_uart_shell_receive(void);

void print_hci_packet_uart(void *buf, int dataSize);

void print_json_data(int* device_idx, int dev_id, float curr_data);

void set_state(int* all_flag, int number);

extern struct k_msgq packet_queue;

#endif