#ifndef BSU_DRIVER_H
#define BSU_DRIVER_H

#include <zephyr/shell/shell.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/usb/usb_device.h>
#include <stddef.h>
#include <zephyr/kernel.h>
#include <string.h>
#include <zephyr/toolchain.h>
#include <zephyr/logging/log.h>
#include <drivers/gpio.h>
#include <stdarg.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdio.h>
#include <zephyr/logging/log_msg.h>
#include <zephyr/logging/log_instance.h>
#include <zephyr/logging/log_core.h>
#include <zephyr/data/json.h>
#include <usb/usb_device.h>
#include <zephyr/types.h>
#include <errno.h>
#include <zephyr/zephyr.h>
#include <zephyr/sys/printk.h>
#include <bluetooth/bluetooth.h>
#include <zephyr/bluetooth/hci.h>
#include <zephyr/bluetooth/conn.h>
#include <zephyr/bluetooth/uuid.h>
#include <zephyr/bluetooth/gatt.h>
#include <zephyr/sys/byteorder.h>

#include "ahu_hci.h"

#define BSU_SCAN_INFO_MSG "usage: bsu_scan < o | f >"
#define BSU_SCAN_MAND_ARGS 2
#define BSU_SCAN_OPT_ARGS 0

/* Debug Thread Stack size */
#define THREAD_NRF_LED_STACK 512

/* Debug Thread Priority */
#define THREAD_NRF_LED_PRIORITY 10

/* 1000 msec = 1 sec */
#define SLEEP_TIME_MS 1000

/* 1000 msec = 1 sec */
#define BLED_LED_SLEEP 1000

/* The devicetree node identifier for the "led0" alias. */
#define LED0_NODE DT_ALIAS(led1)
#define LED0 DT_GPIO_LABEL(LED0_NODE, gpios)
#define PIN DT_GPIO_PIN(LED0_NODE, gpios)
#define FLAGS DT_GPIO_FLAGS(LED0_NODE, gpios)

/* The devicetree node identifier for the "led0" alias. */
#define LED1_NODE DT_ALIAS(led3)
#define LED1 DT_GPIO_LABEL(LED1_NODE, gpios)
#define PIN1 DT_GPIO_PIN(LED1_NODE, gpios)
#define FLAGS1 DT_GPIO_FLAGS(LED1_NODE, gpios)

#define BSU_BT_THREAD_PRIORITY 2
#define BSU_BT_THREAD_STACK 2048

#define BSU_LED_THREAD_PRIORITY 1
#define BSU_LED_THREAD_STACK 512

#define UUID_BUFFER_SIZE 16

#define HCI_BSU_UUID \
    BT_UUID_DECLARE_128(0xAA, 0x43, 0x54, 0xDD, 0xFD, 0x4E, 0x89, 0xBE, 0x32, 0x22, 0xFF, 0x13, 0x3F, 0x2A, 0x29, 0x95)

#define HCI_AHU_UUID \
    BT_UUID_DECLARE_128(0xDD, 0x43, 0x54, 0xDD, 0xFD, 0x4E, 0x89, 0xBE, 0x32, 0x22, 0xFF, 0x13, 0x3F, 0x2A, 0x29, 0x95)

#define HCI_CHAR_BSU_UUID \
    BT_UUID_DECLARE_128(0xBB, 0x43, 0x54, 0xDD, 0xFD, 0x4E, 0x89, 0xBE, 0x32, 0x22, 0xFF, 0x13, 0x3F, 0x2A, 0x29, 0x95)

#define HCI_CHAR_AHU_UUID \
    BT_UUID_DECLARE_128(0xCC, 0x43, 0x54, 0xDD, 0xFD, 0x4E, 0x89, 0xBE, 0x32, 0x22, 0xFF, 0x13, 0x3F, 0x2A, 0x29, 0x95)


static void gatt_write(uint16_t handle, hci_packet *pack);

void cmd_scan(const struct shell *shell, size_t argc, char **argv);

bool bsu_valid_device(int device_id);

#endif