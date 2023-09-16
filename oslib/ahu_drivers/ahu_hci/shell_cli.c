/**
***************************************************************
* @file ~/csse4011/csse4011_repo/ahu_drivers_ahu/cli.c
* @author Riley Mosca - 45358195
* @date 03 - 02 - 2023
* @brief 
***************************************************************
* EXTERNAL FUNCTIONS
***************************************************************
*
***************************************************************
*/

/* Standard Includes */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* Zephyr Includes */
#include <zephyr/shell/shell.h>
#include <zephyr/logging/log.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/drivers/gpio.h>
#include <zephyr/device.h>
#include <zephyr/kernel.h>
#include <zephyr/sys/dlist.h>

/* Local Includes*/
#include "shell_cli.h"
#include "ahu_hci.h"
#include "snode_linked_list.h"

/*
    Initialise the Logging Module, 
    registering it to "app" as the 
    main.c application defined in 
    CMakeLists.txt
*/
LOG_MODULE_REGISTER(SHELL_LOG, 4);

/* Initialisation of the Shell Thread */
K_THREAD_DEFINE(ahu_cli_tid, AHU_SHELL_CLI_THREAD_STACK, ahu_shell_cli_thread, NULL, NULL, NULL, AHU_SHELL_CLI_THREAD_PRIORITY, 0, 0);

/* Previously removed node */
char prevRemovedNode;
int isAdded;

/* 
    Shell command call back that displays the AHU kernel time
*/
int ahu_display_time(const struct shell *shell, size_t argc, char **argv) {

    uint32_t hours = 0, minutes = 0, seconds = 0, milliseconds = 0;

    /* Kernel time in ms, seconds, minutes and hours*/
    milliseconds  = (k_uptime_get()); 
    seconds = (milliseconds / MS_PER_SEC) % SEC_PER_MIN;
    minutes = (milliseconds / (MS_PER_SEC * SEC_PER_MIN)) % MIN_PER_HR;
    hours = (milliseconds / (MS_PER_SEC * SEC_PER_MIN * MIN_PER_HR)) % HR_PER_DAY;
    
    if((argc == 2) && 
        ((strcmp(argv[1], "f") == 0) | (strcmp(argv[1], "F") == 0))) {
        
        shell_print(shell,"%02d:%02d:%02d\n", hours, minutes, seconds);
    } else {
        shell_print(shell, "%lld\n", k_uptime_get());
    
    }
    LOG_DBG("Time data has been sent\n");
    return 0;
}

/* 
    Shell command call back that writes to the RGB LED
*/
int ahu_control_rgb(const struct shell *shell, size_t argc, char **argv) {

    /* Checking what state we want to set the RGB LED to */
    if((strcmp((argv[1]), "o") == 0) || (strcmp((argv[1]), "O") == 0)) {

    } else if((strcmp((argv[1]), "f") == 0) || (strcmp((argv[1]), "F") == 0)) {

    } else if((strcmp((argv[1]), "t") == 0) || (strcmp((argv[1]), "T") == 0)) {

    } else {
        LOG_ERR("Incorrect parameters used for argument 2");
    }

    /* Checking what colour we want to set the RGB LED to*/
    if((strcmp((argv[2]), "r") == 0) || (strcmp((argv[2]), "R") == 0)) {

    } else if((strcmp((argv[2]), "g") == 0) || (strcmp((argv[2]), "G") == 0)) {

    } else if((strcmp((argv[2]), "b") == 0) || (strcmp((argv[2]), "B") == 0)) {

    } else {
        LOG_ERR("Incorrect parameters used for argument 3");
    }
    return 0;
}


/* 
    Shell command call back that reads temperature from SCU
*/
int scu_read_temp(const struct shell *shell, size_t argc, char **argv) {

    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;

    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t message_device = TEMP_DEV;
    uint8_t read_write_bits = DEVICE_READ;
    uint8_t hardware_bits = NO_HW_ACTION;
    uint16_t sample_rate = SAMPLING_MASK;

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/* 
    Shell command call back that reads pressure from SCU
*/
int scu_read_pressure(const struct shell *shell, size_t argc, char **argv) {

    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;

    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t message_device = PRES_DEV;
    uint8_t read_write_bits = DEVICE_READ;
    uint8_t hardware_bits = NO_HW_ACTION;
    uint16_t sample_rate = SAMPLING_MASK;

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/* 
    Shell command call back that reads Acceleration values from SCU
*/
int scu_read_acceleration(const struct shell *shell, size_t argc, char **argv) {

    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;
    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t message_device = 0;
    uint8_t read_write_bits = DEVICE_READ;
    uint8_t hardware_bits = NO_HW_ACTION;
    uint16_t sample_rate = SAMPLING_MASK;

    /* Check the argument specifying the axis */
    if((strcmp(argv[2], "x") == 0) | (strcmp(argv[2], "X") == 0)) {
        message_device = ACCEL_X_DEV;

    } else if((strcmp(argv[2], "y") == 0) | (strcmp(argv[2], "Y") == 0)) {
        message_device = ACCEL_Y_DEV;

    } else if((strcmp(argv[2], "z") == 0) | (strcmp(argv[2], "Z") == 0)) {
        message_device = ACCEL_Z_DEV;

    } else {
        LOG_ERR("Incorrect parameters used for argument 3");
    }

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/* 
    Shell command call back that reads Gyroscope values from SCU
*/
int scu_read_gyroscope(const struct shell *shell, size_t argc, char **argv) {

    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;

    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t message_device = 0;
    uint8_t read_write_bits = DEVICE_READ;
    uint8_t hardware_bits = NO_HW_ACTION;
    uint16_t sample_rate = SAMPLING_MASK;

    /* Check the argument specifying the axis */
    if((strcmp(argv[2], "x") == 0) | (strcmp(argv[2], "X") == 0)){
        message_device = GYRO_X_DEV;

    } else if((strcmp(argv[2], "y") == 0) | (strcmp(argv[2], "Y") == 0)) {
        message_device = GYRO_Y_DEV;
        
    } else if((strcmp(argv[2], "z") == 0) | (strcmp(argv[2], "Z") == 0)) {
        message_device = GYRO_Z_DEV;

    } else {
        LOG_ERR("Incorrect parameters used for argument 3");
    }

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/* 
    Shell command call back that reads magnetometer values from SCU
*/
int scu_read_magnetometer(const struct shell *shell, size_t argc, char **argv) {

    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;

    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t message_device = 0;
    uint8_t read_write_bits = DEVICE_READ;
    uint8_t hardware_bits = NO_HW_ACTION;
    uint16_t sample_rate = SAMPLING_MASK;

    /* Check the argument specifying the axis */
    if((strcmp(argv[2], "x") == 0) | (strcmp(argv[2], "X") == 0)) {
        message_device = MAGN_X_DEV;

    } else if((strcmp(argv[2], "y") == 0) | (strcmp(argv[2], "Y") == 0)) {
        message_device = MAGN_Y_DEV;

    } else if((strcmp(argv[2], "z") == 0) | (strcmp(argv[2], "Z") == 0)) {
        message_device = MAGN_Z_DEV;

    } else {
        LOG_ERR("Incorrect parameters used for argument 3");
    }

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/* 
    Shell command call back that reads Pushbutton state from SCU
*/
int scu_read_pushbutton(const struct shell *shell, size_t argc, char **argv) {

    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;

    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t message_device = PB_DEV;
    uint8_t read_write_bits = DEVICE_READ;
    uint8_t hardware_bits = NO_HW_ACTION;
    uint16_t sample_rate = SAMPLING_MASK;

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );
    
    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/* 
    Shell command call back that writes to SCU sampling rate
*/
int scu_write_sample_rate(const struct shell *shell, size_t argc, char **argv) {

    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;

    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t message_device = SAMPLE_DEV;
    uint8_t read_write_bits = DEVICE_WRITE;
    uint8_t hardware_bits = NO_HW_ACTION;

    /* Sampling rate value is stored as the 3rd argument */
    uint16_t sample_rate = atoi(argv[2]);

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/* 
    Shell command call back that writes to SCU LEDs
*/
int scu_write_leds(const struct shell *shell, size_t argc, char **argv) {

    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;

    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t read_write_bits = DEVICE_WRITE; 
    uint16_t sample_rate = SAMPLING_MASK;
    uint8_t hardware_bits = 0;
    uint8_t message_device = 0;

    /* Check the argument specifying the axis */
    if((strcmp(argv[2], "ld0") == 0) | (strcmp(argv[2], "LD0") == 0)) {
        message_device = SCU_LD0_DEV;

    } else if((strcmp(argv[2], "ld1") == 0) | (strcmp(argv[2], "LD1") == 0)) {
        message_device = SCU_LD1_DEV;

    } else {
        LOG_ERR("Incorrect parameters used for argument 3");
    }

    /* Check the argument specifying the axis */
    if((strcmp(argv[3], "h") == 0) | (strcmp(argv[3], "H") == 0)) {
        hardware_bits = HW_SET_HIGH;

    } else if((strcmp(argv[3], "l") == 0) | (strcmp(argv[3], "L") == 0)) {
        hardware_bits = HW_SET_LOW;
        
    } else {
        LOG_ERR("Incorrect parameters used for argument 4");
    }

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/* 
    Shell command to initialise default shell
    settings (if not configured by prj.conf) 
*/
int shell_initialise_settings(const struct shell *shell, size_t argc, char **argv) {

    SH_PURGE_HISTORY(shell->history);
    SH_OBSCURE_SET(shell, false);
    SH_ROOT_CMD(NULL);
    SH_PROMPT_CHANGE(shell, "Team 1 - CSSE4011~$");

    return 0;
}

/* 
    Command to display sensor data in .JSON format
*/
int ahu_all_command(const struct shell *shell, size_t argc, char **argv) {
    /* Initialise a HCI Packet to default */
    hci_packet msg_packet;

    init_hci_packet(&msg_packet);

    /* Initialising integers for packet construction */
    uint8_t message_type = TYPE_REQUEST;
    uint8_t  hardware_bits = NO_HW_ACTION; 
    uint16_t sample_rate = SAMPLING_MASK;
    uint8_t message_device = ALL_DEVS;
    uint8_t read_write_bits = 0;

    /* Check the argument specifying the axis */
    if((strcmp(argv[1], "o") == 0) | (strcmp(argv[1], "O") == 0)) {
        read_write_bits = DEVICE_CONT_SAMPLE;
        hardware_bits = 0b11; 

    } else if((strcmp(argv[1], "f") == 0) | (strcmp(argv[1], "F") == 0)) {
        read_write_bits = DEVICE_NO_READ;
        hardware_bits = 0b11; 
        
    } else {
        /* Error */
    }

    /* Setting new values in HCI Packet contents */
    construct_packet_message(
        &msg_packet, 
        sample_rate, 
        message_type, 
        message_device, 
        read_write_bits, 
        hardware_bits
    );
    
    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/*
    Shell event callback for when the escape
    or 'esc' keypress is detected, this is used
    in the "all" command for toggling continuous
    sampling
*/
void shell_escape_callback(const struct shell *shell, size_t argc, char **argv) {

}

/*
    Shell callback function that generates HCI packets for utrasonic data
*/
int bsu_usonic_command(const struct shell *shell, size_t argc, char **argv) {
    /*
        Initialise HCI Message packet
        to default values
    */
    hci_packet msg_packet;
    init_hci_packet(&msg_packet);

    /*
        Values for BSU Ultrasonic 
        data request
    */
    uint8_t rw_bits = DEVICE_READ;
    uint8_t hw_bits = NO_HW_ACTION;
    uint8_t msg_type = TYPE_REQUEST;
    uint8_t dev_sample_rate = SAMPLING_MASK;
    uint8_t device = ULTRAS_DEV;
    /*
        Populate HCI Packet
    */
    construct_packet_message(
        &msg_packet, 
        dev_sample_rate, 
        msg_type, 
        device, 
        rw_bits, 
        hw_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/*
    Shell callback function that generates HCI packets for orientation - pitch
*/
int bsu_orien_pitch_command(const struct shell *shell, size_t argc, char **argv) {
    /*
        Initialise HCI Message packet
        to default values
    */
    hci_packet msg_packet;
    init_hci_packet(&msg_packet);

    /*
        Values for BSU orientation
        pitch data request
    */
    uint8_t rw_bits = DEVICE_READ;
    uint8_t hw_bits = NO_HW_ACTION;
    uint8_t msg_type = TYPE_REQUEST;
    uint8_t dev_sample_rate = SAMPLING_MASK;
    uint8_t device = PITCH_DEV;
    /*
        Populate HCI Packet
    */
    construct_packet_message(
        &msg_packet, 
        dev_sample_rate, 
        msg_type, 
        device, 
        rw_bits, 
        hw_bits
    );
    
    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/*
    Shell callback function that generates HCI packets for orientation - roll
*/
int bsu_orien_roll_command(const struct shell *shell, size_t argc, char **argv) {
    /*
        Initialise HCI Message packet
        to default values
    */
    hci_packet msg_packet;
    init_hci_packet(&msg_packet);

    /*
        Values for BSU orientation 
        roll data request
    */
    uint8_t rw_bits = DEVICE_READ;
    uint8_t hw_bits = NO_HW_ACTION;
    uint8_t msg_type = TYPE_REQUEST;
    uint8_t dev_sample_rate = SAMPLING_MASK;
    uint8_t device = ROLL_DEV;
    /*
        Populate HCI Packet
    */
    construct_packet_message(
        &msg_packet, 
        dev_sample_rate, 
        msg_type, 
        device, 
        rw_bits, 
        hw_bits
    );
    
    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/*
    Shell callback function that generates HCI packets for altitude
*/
int bsu_altitude_fine_command(const struct shell *shell, size_t argc, char **argv) {
    /*
        Initialise HCI Message packet
        to default values
    */
    hci_packet msg_packet;
    init_hci_packet(&msg_packet);

    /*
        Values for BSU altitude 
        data request
    */
    uint8_t rw_bits = DEVICE_READ;
    uint8_t hw_bits = NO_HW_ACTION;
    uint8_t msg_type = TYPE_REQUEST;
    uint8_t dev_sample_rate = SAMPLING_MASK;
    uint8_t device = FIN_ALT_DEV;
    /*
        Populate HCI Packet
    */
    construct_packet_message(
        &msg_packet, 
        dev_sample_rate, 
        msg_type, 
        device, 
        rw_bits, 
        hw_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

int bsu_altitude_coarse_command(const struct shell *shell, size_t argc, char **argv) {
    /*
        Initialise HCI Message packet
        to default values
    */
    hci_packet msg_packet;
    init_hci_packet(&msg_packet);

    /*
        Values for BSU altitude 
        data request
    */
    uint8_t rw_bits = DEVICE_READ;
    uint8_t hw_bits = NO_HW_ACTION;
    uint8_t msg_type = TYPE_REQUEST;
    uint8_t dev_sample_rate = SAMPLING_MASK;
    uint8_t device = COA_ALT_DEV;
    /*
        Populate HCI Packet
    */
    construct_packet_message(
        &msg_packet, 
        dev_sample_rate, 
        msg_type, 
        device, 
        rw_bits, 
        hw_bits
    );

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}

/*
    Shell callback function that generates HCI packets for BSU Continuous sampling
*/
int bsu_sampling_command(const struct shell *shell, size_t argc, char **argv) {
    /*
        Storing the sampling rate state
    */
    char* sample_state = argv[2];
    char* sample_rate_str = "";
    int sample_rate_int = 0;
    int on_off_flag = 2;
    /*
        If a sample rate is specified,
        assign it!
    */
    if(argc == 4) {
        sample_rate_str = argv[3];
        sample_rate_int = atoi(sample_rate_str);
    }

    /*
        Checking what device to assign
        based on on or off
    */
    if(strcmp(sample_state, "s") == 0) {
        on_off_flag = 1;

    } else if(strcmp(sample_state, "f") == 0) {
        on_off_flag = 0;
    }

    /*
        Initialise HCI Message packet
        to default values
    */
    hci_packet msg_packet;
    init_hci_packet(&msg_packet);

    /*
        Values for BSU sampling 
        rate request
    */
    uint8_t rw_bits = DEVICE_READ;
    uint8_t hw_bits = NO_HW_ACTION;
    uint8_t msg_type = TYPE_REQUEST;
    uint8_t dev_sample_rate = sample_rate_int;
    uint8_t device = on_off_flag == 1 ? CON_SMP_DEV : NO_DEV;
    /*
        Populate HCI Packet
    */
    construct_packet_message(
        &msg_packet, 
        dev_sample_rate, 
        msg_type, 
        device, 
        rw_bits, 
        hw_bits
    );
    memcpy(&(msg_packet.msg_data), &sample_rate_int, sizeof(int));

    k_msgq_put(&packet_queue, &msg_packet, K_NO_WAIT);

    return 0;
}
/*
    Shell CB Function - Controlling or querying the state of a static node
*/
int snode_control(const struct shell *shell, size_t argc, char **argv) {
    /*
        Checking arguments, to determine whether to
        query (one or all nodes) or to add/ remove 
        a static node.
    */
   int device = 0;
   

    if((!strcmp(argv[1], "init")) || (!strcmp(argv[1], "INIT"))) {
        sys_dlist_init(&node_list);
        init_static_node_linked_list();


    } else if((!strcmp(argv[1], "add")) || (!strcmp(argv[1], "ADD"))) {
        int idxToAdd = determine_static_node(prevRemovedNode);
        add_given_node_at_position(argv[2][0], idxToAdd);


    } else if((!strcmp(argv[1], "remove")) || (!strcmp(argv[1], "REMOVE")))  {
        remove_node(argv[2][0]);
        prevRemovedNode =  argv[2][0];

    } else if((!strcmp(argv[1], "view")) || (!strcmp(argv[1], "VIEW")))  {

        if((argc == 3) && (!strcmp(argv[2], "-a"))) {
            static_node *nodeCheck;
            SYS_DLIST_FOR_EACH_CONTAINER(&node_list, nodeCheck, node) {
                display_static_node_data(nodeCheck);

            }
        } else {
            static_node *nodeCheck;
            SYS_DLIST_FOR_EACH_CONTAINER(&node_list, nodeCheck, node) {
                
                /* If node name matches the input char, print the data */
                if(nodeCheck->node_name[5] == argv[2][0]) {
                    display_static_node_data(nodeCheck);
                }
            }
        }
    }

    /*
        Checking which singular node to 
        add/ remove or query 
    */
    if((device == SNODE_QUERY) || (device == SNODE_ADD) || (device == SNODE_RM)) {
        int node_id = determine_static_node(argv[2][0]);

        if((node_id >= 0) && (node_id <= 11)) {
            // static_node = node_id;
        }
    }
    return 0;

}

/*
    Based on static node label entered
    return an integer for this node
*/
int determine_static_node(char cmd_node) {

    int ascii_diff = isupper(cmd_node) ? 65 : 97;

    return (int)(cmd_node - ascii_diff);
}

int test_cmd(const struct shell *shell, size_t argc, char **argv) {
    int xPos = 0;
    int yPos = 0;
    while(1) {
        xPos +=1;
        xPos %= 50;
        yPos+=50;
        yPos%=50;
        shell_print(shell, "[%d, %d]", xPos, yPos);
        k_sleep(K_MSEC(500));
    }
    return 0;

}

void ahu_cli_shell_init(void) {
    /*--------------------  Shell Command Registration --------------------*/
    /* Init Shell Command */
    SHELL_CMD_ARG_REGISTER(init, NULL, INIT_INFO_MSG, shell_initialise_settings, INIT_CMD_MAND_ARGS, INIT_CMD_OPT_ARGS);

    /* Registering AHU Commands that READ data and write to peripherals */
    SHELL_CMD_ARG_REGISTER(time, NULL, TIME_INFO_MSG, ahu_display_time, TIME_CMD_MAND_ARGS, TIME_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(rgb, NULL, RGB_INFO_MSG, ahu_control_rgb, RGB_CMD_MAND_ARGS, RGB_CMD_OPT_ARGS);

    /* Registering SCU Commands that READ from peripherals/ sensors */
    SHELL_CMD_ARG_REGISTER(temperature, NULL, TEMP_INFO_MSG,  scu_read_temp,TEMP_CMD_MAND_ARGS, TEMP_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(pressure, NULL, PRES_INFO_MSG, scu_read_pressure, PRES_CMD_MAND_ARGS, PRES_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(gyro, NULL, GYRO_INFO_MSG, scu_read_gyroscope, GYRO_CMD_MAND_ARGS, GYRO_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(accel, NULL, ACCEL_INFO_MSG, scu_read_acceleration, ACCEL_CMD_MAND_ARGS, ACCEL_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(mag, NULL, MAG_INFO_MSG, scu_read_magnetometer, MAG_CMD_MAND_ARGS, MAG_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(pb, NULL, PB_INFO_MSG, scu_read_pushbutton, PB_CMD_MAND_ARGS, PB_CMD_OPT_ARGS);

    /* Registering SCU Commands that WRITE to peripherals/ data */
    SHELL_CMD_ARG_REGISTER(sample, NULL, SAMPLE_INFO_MSG, scu_write_sample_rate, SAMPLE_CMD_MAND_ARGS, SAMPLE_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(led, NULL, LED_INFO_MSG, scu_write_leds, LED_CMD_MAND_ARGS, LED_CMD_OPT_ARGS);

    /* JSON -> All command */
    SHELL_CMD_ARG_REGISTER(all, NULL, ALL_INFO_MSG, ahu_all_command, ALL_CMD_MAND_ARGS, ALL_CMD_OPT_ARGS);

    /* BSU Commands */
    SHELL_CMD_ARG_REGISTER(ultrasonic, NULL, USONIC_INFO_MSG, bsu_usonic_command, USONIC_CMD_MAND_ARGS, USONIC_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(orientation_pitch, NULL, OPITCH_INFO_MSG, bsu_orien_pitch_command, OPITCH_CMD_MAND_ARGS, OPITCH_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(orientation_roll, NULL, OROLL_INFO_MSG, bsu_orien_roll_command, OROLL_CMD_MAND_ARGS, OROLL_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(altitude_fine, NULL, ALTITUDE_FINE_INFO_MSG, bsu_altitude_fine_command, ALTITUDE_FINE_CMD_MAND_ARGS, ALTITUDE_FINE_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(altitude_coarse, NULL, ALTITUDE_COARSE_INFO_MSG, bsu_altitude_coarse_command, ALTITUDE_COARSE_CMD_MAND_ARGS, ALTITUDE_COARSE_CMD_OPT_ARGS);
    SHELL_CMD_ARG_REGISTER(bsu_sampling, NULL, BSU_C_S_INFO_MSG, bsu_sampling_command, BSU_C_S_CMD_MAND_ARGS, BSU_C_S_CMD_OPT_ARGS);
    
    /*--------------------  NRF Base RSSI Commands --------------------*/
    SHELL_CMD_ARG_REGISTER(snode, NULL, SNODE_INFO_MSG, snode_control, SNODE_CMD_MAND_ARGS, SNODE_CMD_OPT_ARGS);

    /*--------------------  NRF Base RSSI Commands --------------------*/
    SHELL_CMD_ARG_REGISTER(test, NULL, "TEST CMD ONLY", test_cmd, 1, 0);
}   

K_MSGQ_DEFINE(packet_queue, sizeof(hci_packet), 10, 4);

/*
    Main thread for Application Host Unit's (AHU) shell CLI interface
*/
void ahu_shell_cli_thread(void) {

    while (1) {
        /* Process Queue sending here */
        k_sleep(K_MSEC(10));
    }
}