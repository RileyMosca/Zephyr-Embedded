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

/* Zephyr Includes */
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

/* Local Includes*/
#include "ahu_uart.h"
#include "ahu_hci.h"


/* Device modes */
#define MODE_ALL_ON 1
#define MODE_ALL_OFF 2
#define MODE_GENERAL 3

/* queue to store up to 10 messages (aligned to 4-byte boundary) */
K_MSGQ_DEFINE(uart_msgq, MSG_SIZE, 10, 4);

K_THREAD_DEFINE(ahu_uart_tid, AHU_UART_SHELL_RECV_STACK, ahu_uart_shell_receive, NULL, NULL, NULL, AHU_UART_SHELL_RECV_STACK_PRIOTIY, 0, 0);

#define UART_NODE_LABEL DT_NODELABEL(uart1)
static const struct device *const uart_dev = DEVICE_DT_GET(UART_NODE_LABEL);

/*
 	Read characters from UART until line end is detected. 
	Afterwards push the data to the message queue.
 */
void serial_cb(const struct device *dev, void *user_data) {
	uint8_t c;

	if (!uart_irq_update(uart_dev)) {
		return;
	}

	if (!uart_irq_rx_ready(uart_dev)) {
		return;
	}

	/* read until FIFO empty */
	while (uart_fifo_read(uart_dev, &c, 1) == 1) {

		if ((rx_buf_pos == 0) && (c != 0xAA)) {

		} else if (rx_buf_pos == sizeof(hci_packet)) {
			printk("i\r\n");
			/* terminate string */
			rx_buf[rx_buf_pos] = c;

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;

		} else if ((rx_buf_pos > 0) 
			 && ((c == '\n' || c == '\r'))) {

			/* terminate string */
			rx_buf[rx_buf_pos] = c;

			/* if queue is full, message is silently dropped */
			k_msgq_put(&uart_msgq, &rx_buf, K_NO_WAIT);

			/* reset the buffer (it was copied to the msgq) */
			rx_buf_pos = 0;
		} else if (rx_buf_pos < (sizeof(rx_buf) - 1)) {
			rx_buf[rx_buf_pos++] = c;
		}
	}
}

/*
	Print a struct to the UART interface
*/
void print_hci_packet_uart(void *buf, int dataSize) {
	char* str = (char* ) buf;

	for (int i = 0; i < dataSize; i++) {

		uart_poll_out(uart_dev, str[i]);
	}
}

/*
	Print a null-terminated string character by character to the UART interface
*/
void print_uart(char *buf)
{
	int msg_len = strlen(buf);

	for (int i = 0; i < msg_len; i++) {
		uart_poll_out(uart_dev, buf[i]);
	}
}

void ahu_uart_shell_receive(void)
{
	char tx_buf[MSG_SIZE];

	if (!device_is_ready(uart_dev)) {
		printk("UART device not found!");
		return;
	}

	/* configure interrupt and callback to receive data */
	uart_irq_callback_user_data_set(uart_dev, serial_cb, NULL);
	uart_irq_rx_enable(uart_dev);

	/* Initialise packet data*/
	hci_packet tx_packet, prev_tx_packet, rx_packet, prev_rx_packet;

    init_hci_packet(&rx_packet), init_hci_packet(&tx_packet);
	init_hci_packet(&prev_tx_packet), init_hci_packet(&prev_rx_packet);	

	/* Current Data is the current sensor data, with the */
	int device_idx = 0;
	int all_flag = 0;
	float current_data;

	while (1) {

		/* Transmitting packet data */
		if(k_msgq_get(&packet_queue, &tx_packet, K_FOREVER) == 0) {
			
			/* Verifying packet to transmit is unique */
			if(!compare_data_packets(prev_tx_packet, tx_packet)) {
				/* print the HCI packet */
				print_hci_packet_uart(&tx_packet, sizeof(hci_packet));  

				/* Set the current state */
				set_state(&all_flag, tx_packet.msg_device_rw_hw);
			}
		}

		/* Receiving packet data */
		if(k_msgq_get(&uart_msgq, &rx_packet, K_NO_WAIT) == 0) {
			
			/* Only responding to unique data packets */
			if(!compare_data_packets(prev_rx_packet, rx_packet)) {

				/* All command is active */
				if(all_flag == MODE_ALL_ON) {
					
					/* Copy floating point data safely */
					memcpy(&current_data, rx_packet.msg_data, sizeof(float));

					/* Print JSON Data */
					print_json_data(&device_idx, rx_packet.msg_device_rw_hw >> 4, current_data);

					/* Clear data at every iteration */
					memset(&current_data, 0, sizeof(float));

				} else if(all_flag == MODE_ALL_OFF){
					/*	Do nothing, intentionally empty */

				} else if(all_flag == MODE_GENERAL) {
					/* Regular device operation */
					get_device_data_units_string(rx_packet, (0));
				}
			}
		}
		prev_tx_packet = tx_packet;
		prev_rx_packet = rx_packet;

		k_sleep(K_MSEC(10));
	}
}

void print_json_data(int* device_idx, int dev_id, float curr_data) {

	/* Insert JSON Header */
	if(*device_idx == 0) printf("\r\n{\r\n");
	
	/* Print JSON Data*/
	printf("\t{%d : [%.3f]}\r\n", (dev_id), curr_data);

	/* Insert JSON Footer */
	if(*device_idx == 11) printf("}\r\n");

	/* Increment and Limit Device Index */
	*device_idx += 1; *device_idx %= 12;
}

void set_state(int* all_flag, int number) {

	/* Sets the state based on the commands */
	if((number & 0b00001111) == 0b00001111) {
		*all_flag = MODE_ALL_ON;
	} else if((number & 0b00001111) == 0b00000011) {
		*all_flag = MODE_ALL_OFF;
	} else {
		*all_flag = MODE_GENERAL;
	}
}