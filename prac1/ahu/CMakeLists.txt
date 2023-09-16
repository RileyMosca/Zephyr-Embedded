# SPDX-License-Identifier: Apache-2.0

# Setting Minimum CMake version
cmake_minimum_required(VERSION 3.20.0)

# Setting board to the ESP32 - C3
set(BOARD esp32c3_devkitm)

# Finding the Zephyr Base Package
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Initialising project as Prac 1
project(prac1)

# Setting CMAKE Relative Directory
set(CMAKE_DIR ~/csse4011/repo/prac1/ahu)

# Setting Global app_sources as relative to ahu -> ahu/src/
FILE(GLOB app_sources ${CMAKE_DIR}/src/*.c)

# Setting Target Source for app under app sources
target_sources(app PRIVATE ${app_sources})

# Setting required directories
set(REPO_DIR            ~/csse4011/repo/)               # REPOSITORY ~/csse4011/repo
set(PRAC_ONE_DIR        ${REPO_DIR}/prac1)              # PRAC ONE ~/csse4011/repo/prac1
set(AHU_SRC_DIR         ${PRAC_ONE_DIR}/ahu/src/)       # AHU SOURCE ~/csse4011/repo/prac1/ahu/src/
set(OSLIB_DIR           ${REPO_DIR}/oslib/)             # OSLIB ~/csse4011/repo/oslib/
set(AHU_DRIVERS_DIR     ${OSLIB_DIR}/ahu_drivers)       # AHU DRIVERS ~/csse4011/repo/oslib/ahu_drivers/
set(AHU_HCI_SRC_DIR     ${AHU_DRIVERS_DIR}/ahu_hci/)    # AHU HCI SRC ~/csse4011/repo/oslib/ahu_drivers/ahu_hci/

# Including target sources as private
target_include_directories(app PRIVATE 
  ${REPO_DIR}
  ${PRAC_ONE_DIR}
  ${AHU_SRC_DIR}
  ${OSLIB_DIR}
  ${AHU_DRIVERS_DIR}
  ${AHU_HCI_SRC_DIR}
)

target_sources(app PRIVATE 
  ${AHU_HCI_SRC_DIR}/shell_cli.c      # Including SHELL Command Line Interface (cli)
  ${AHU_HCI_SRC_DIR}/ahu_hci.c        # Including SHELL Command Line Interface (cli)
  ${AHU_HCI_SRC_DIR}/ahu_uart.c       # Including UART Host Controller Interface (HCI)
  ${AHU_SRC_DIR}/main.c               # Including main.c
)