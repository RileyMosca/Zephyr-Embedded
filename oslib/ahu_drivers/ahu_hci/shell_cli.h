#ifndef SHELL_CLI_H
#define SHELL_CLI_H

#include <ctype.h>

/* Specifying time conversions */
#define MS_PER_SEC 1000
#define SEC_PER_MIN 60
#define MIN_PER_HR 60
#define HR_PER_DAY 24

/* Specifying Shell Command Register data for all specified commands */
#define TIME_CMD_MAND_ARGS 1        /* Time Shell command -> Mandatory Args */
#define TIME_CMD_OPT_ARGS 1         /* Time Shell command -> Optional Args */
#define TIME_INFO_MSG "Usage: time [f]. \n"\
                    "Shell command that displays the kernel time in milliseconds, "\
                    "or in HH:MM:SS format if [f] is specified"

#define RGB_CMD_MAND_ARGS 3        /* AHU RGB LED Shell command -> Mandatory Args */
#define RGB_CMD_OPT_ARGS 0         /* AHU RGB LED Shell command -> Optional Args */
#define RGB_INFO_MSG "Usage: rgb <state> <colour>. \n"\
                    "Shell command that controls the colour and state of the AHU RGB LED. \n"\
                    "<state>: o (on), f(off), t(toggle) \n"\
                    "<colour>: r(red), g(green), b(blue)\n"

#define TEMP_CMD_MAND_ARGS 2        /* SCU LSM6DSL Temp Sensor Shell command -> Mandatory Args */
#define TEMP_CMD_OPT_ARGS 0         /* SCU LSM6DSL Temp Sensor Shell command -> Optional Args */
#define TEMP_INFO_MSG "Usage: temperature r. \n"\
                    "Shell command that reads the current temperature from the LSM6DSL sensor. \n"

#define PRES_CMD_MAND_ARGS 2        /* SCU LPS22 pressure Sensor Shell command -> Mandatory Args */
#define PRES_CMD_OPT_ARGS 0         /* SCU LPS22 pressure Sensor Shell command -> Optional Args */
#define PRES_INFO_MSG "Usage: pressure r. \n"\
                    "Shell command that reads the current temperature from the LSM6DSL sensor. \n"

#define GYRO_CMD_MAND_ARGS 3        /* SCU LSM6DSL gyroscope Sensor Shell command -> Mandatory Args */
#define GYRO_CMD_OPT_ARGS 0         /* SCU LSM6DSL gyroscope Sensor Shell command -> Optional Args */
#define GYRO_INFO_MSG "Usage: gyro r <axis>. \n"\
                    "Shell command that reads the current gyroscope value from the LSM6DSL sensor at the given axis.\n"\
                    "<axis>: x(x-axis), y(y-axis) or z(z-axis).\n"

#define ACCEL_CMD_MAND_ARGS 3        /* SCU LSM6DSL accelerometer Sensor Shell command -> Mandatory Args */
#define ACCEL_CMD_OPT_ARGS 0         /* SCU LSM6DSL accelerometer Sensor Shell command -> Optional Args */
#define ACCEL_INFO_MSG "Usage: accel r <axis>. \n"\
                    "Shell command that reads the current acceleration value from the LSM6DSL sensor at the given axis.\n"\
                    "<axis>: x(x-axis), y(y-axis) or z(z-axis).\n"

#define MAG_CMD_MAND_ARGS 3        /* SCU LSM6DSL magnetometer Sensor Shell command -> Mandatory Args */
#define MAG_CMD_OPT_ARGS 0         /* SCU LSM6DSL magnetometer Sensor Shell command -> Optional Args */
#define MAG_INFO_MSG "Usage: mag r <axis>. \n"\
                    "Shell command that reads the current magnetometer value from the LSM6DSL sensor at the given axis.\n"\
                    "<axis>: x(x-axis), y(y-axis) or z(z-axis).\n"

#define PB_CMD_MAND_ARGS 2        /* SCU PB Shell command -> Mandatory Args */
#define PB_CMD_OPT_ARGS 0         /* SCU PB rate Shell command -> Optional Args */
#define PB_INFO_MSG "Usage: pb r. \n"\
                    "Shell command that reads the current SCU psuh button state.\n"

#define SAMPLE_CMD_MAND_ARGS 3        /* SCU Sampling rate Shell command -> Mandatory Args */
#define SAMPLE_CMD_OPT_ARGS 0         /* SCU Sampling rate Shell command -> Optional Args */
#define SAMPLE_INFO_MSG "Usage: sample w <rate>. \n"\
                    "Shell command that writes the current sampling rate to the SCU.\n"\
                    "<rate> a given sampling rate between 0 - 255 seconds.\n"

#define LED_CMD_MAND_ARGS 4        /* SCU LED Shell command -> Mandatory Args */
#define LED_CMD_OPT_ARGS 0         /* SCU LED Shell command -> Optional Args */
#define LED_INFO_MSG "Usage: led w <type> <state>. \n"\
                    "Shell command that controls the colour and state of the AHU RGB LED. \n"\
                    "<type>: LD0(LED 0), LD1(LED 1)\n"\
                    "<state>: h(on/high), l(off/low) \n"

#define INIT_CMD_MAND_ARGS 1        /* Shell Init command -> Mandatory Args */
#define INIT_CMD_OPT_ARGS 0         /* Shell Init command -> Optional Args */
#define INIT_INFO_MSG "Shell Initialisation Command"

#define ALL_CMD_MAND_ARGS 2         /* All command -> Mandatory Args */
#define ALL_CMD_OPT_ARGS 0          /* All command -> Optional Args */
#define ALL_INFO_MSG "All command -> Toggles between continuous sampling of sensor data,\n"\
                    " and displays data in a JSON Array format to the shell."

#define USONIC_INFO_MSG "Usage - ultrasonic. \n"\
                        "Shell command that retreieves the ultrasonic raw data.\n"

#define USONIC_CMD_MAND_ARGS 1
#define USONIC_CMD_OPT_ARGS 0

#define OPITCH_INFO_MSG "Usage - orientation_pitch. \n"\
                        "Shell command that retreieves the orientation pitch data.\n"

#define OPITCH_CMD_MAND_ARGS 1
#define OPITCH_CMD_OPT_ARGS 0

#define OROLL_INFO_MSG "Usage - orientation_roll. \n"\
                        "Shell command that retreieves the orientation roll data.\n"

#define OROLL_CMD_MAND_ARGS 1
#define OROLL_CMD_OPT_ARGS 0

#define ALTITUDE_FINE_INFO_MSG "Usage - altitude_fine. \n"\
                        "Shell command that retreieves the altitude data.\n"

#define ALTITUDE_FINE_CMD_MAND_ARGS 1
#define ALTITUDE_FINE_CMD_OPT_ARGS 0

#define ALTITUDE_COARSE_INFO_MSG "Usage - altitude_coarse. \n"\
                        "Shell command that retreieves the altitude data.\n"

#define ALTITUDE_COARSE_CMD_MAND_ARGS 1
#define ALTITUDE_COARSE_CMD_OPT_ARGS 0

#define BSU_C_S_INFO_MSG "Usage - bsu c <s | p> [sample rate]. \n"\
                        "Shell command that starts/ stops cotinuous sampling \n"\
                        "(sample rate needed when starting)\n"

#define BSU_C_S_CMD_MAND_ARGS 3
#define BSU_C_S_CMD_OPT_ARGS 1

#define SNODE_INFO_MSG "Command to add/ remove or view static node data, usage: snode < add | remove | view> [-a].\n"\
                        "The \"-a\" option is to toggle all static nodes and only works for the \"view\" sub-command.\n"
#define SNODE_CMD_MAND_ARGS 2
#define SNODE_CMD_OPT_ARGS 1

/* Command state -> Toggled by SHELL EVENT via 'esc' keypress */
#define COMMAND_INACTIVE false
#define COMMAND_ACTIVE true

/* UART Shell Device Node setting */
#define UART_DEVICE_NODE DT_CHOSEN(zephyr_shell_uart)

/* Shell Thread Stack Size and Priority */
#define AHU_SHELL_CLI_THREAD_STACK 2048
#define AHU_SHELL_CLI_THREAD_PRIORITY 1

/* Defining Macros for Shell configuration */
#define SH_PURGE_HISTORY(history) z_shell_history_purge(history)
#define SH_OBSCURE_SET(shell, state) shell_obscure_set(shell, state)
#define SH_ROOT_CMD(root_cmd) shell_set_root_cmd(root_cmd)
#define SH_PROMPT_CHANGE(shell, new_prompt) shell_prompt_change(shell, new_prompt)

int ahu_display_time(const struct shell *shell, size_t argc, char **argv);

int ahu_control_rgb(const struct shell *shell, size_t argc, char **argv);

int scu_read_temp(const struct shell *shell, size_t argc, char **argv);

int scu_read_pressure(const struct shell *shell, size_t argc, char **argv);

int scu_read_acceleration(const struct shell *shell, size_t argc, char **argv);

int scu_read_gyroscope(const struct shell *shell, size_t argc, char **argv);

int scu_read_magnetometer(const struct shell *shell, size_t argc, char **argv);

int scu_read_pushbutton(const struct shell *shell, size_t argc, char **argv);

int scu_write_sample_rate(const struct shell *shell, size_t argc, char **argv);

int scu_write_leds(const struct shell *shell, size_t argc, char **argv);

int shell_initialise_settings(const struct shell *shell, size_t argc, char **argv);

int ahu_all_command(const struct shell *shell, size_t argc, char **argv);

void shell_escape_callback(const struct shell *shell, size_t argc, char **argv);

int bsu_usonic_command(const struct shell *shell, size_t argc, char **argv);

int bsu_orien_pitch_command(const struct shell *shell, size_t argc, char **argv);

int bsu_orien_roll_command(const struct shell *shell, size_t argc, char **argv);

int bsu_altitude_command_fine(const struct shell *shell, size_t argc, char **argv);

int bsu_altitude_command_coarse(const struct shell *shell, size_t argc, char **argv);

int bsu_sampling_command(const struct shell *shell, size_t argc, char **argv);

int snode_control(const struct shell *shell, size_t argc, char **argv);

void ahu_cli_shell_init(void);

void ahu_shell_cli_thread(void);

int determine_static_node(char cmd_node);

int test_cmd(const struct shell *shell, size_t argc, char **argv);

#endif