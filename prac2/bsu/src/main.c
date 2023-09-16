/**
 ************************************************************************
 * @file main.c
 * @author Wilfred MK, Aaron Helmore
 * @date 04.03.2021
 * @brief Spawns threads required by Base node.
 **********************************************************************
 **/

/* Zephyr Includes */
#include <zephyr/types.h>
#include <stddef.h>
#include <errno.h>
#include <zephyr.h>
#include <sys/printk.h>
#include <drivers/gpio.h>
#include <usb/usb_device.h>

/* Local Includes */
#include "bsu_base.h"
// #include "ahu_hci.h"
#include "shell_cli.h"

/**
 * @brief Enable USB Driver.
 * 
 */
void main(void) {

    /*
        Register and initialise the shell
    */
    ahu_cli_shell_init();

    /*
        Enable the USB for the NRF52840 Dongle
    */
	if (usb_enable(NULL)) {
		return;
    }
}
