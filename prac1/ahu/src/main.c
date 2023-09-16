/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>

/* Zephyr Includes */
#include <zephyr/shell/shell.h>
#include <zephyr/shell/shell_uart.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>

/* Local Source Includes */
#include "shell_cli.h"
#include "ahu_hci.h"
#include "ahu_uart.h"

// void main(void) {}