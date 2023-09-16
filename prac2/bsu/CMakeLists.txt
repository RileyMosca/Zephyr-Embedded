# SPDX-License-Identifier: Apache-2.0

# Setting Minimum CMake version
cmake_minimum_required(VERSION 3.20.0)

# Setting board to the Nordic Semiconductor NRD52840 Board
set(BOARD nrf52840dongle_nrf52840)

set(CONFS_DIR ~/csse4011/repo/prac2/bsu/configs)             # PRAC TWO ~/csse4011/repo/prac2/bsu/confs
set(BOARDS_DIR ~/csse4011/repo/prac2/bsu/boards)             # PRAC TWO ~/csse4011/repo/prac2/bsu/boards

# Setting Config files
set(CONF_FILE 
  prj.conf 
  usb_cdc.conf 
  bt.conf 
  console.conf
)

set(DTC_OVERLAY ${BOARDS_DIR}/nrf52840dongle_nrf52840.overlay)

# Finding the Zephyr Base Package
find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

# Initialising project as Prac 1
project(prac2)

# Setting CMAKE Relative Directory
set(CMAKE_DIR ~/csse4011/repo/prac2/bsu)

# Setting Global app_sources as relative to ahu -> ahu/src/
FILE(GLOB app_sources ${CMAKE_DIR}/src/*.c)

# Setting Target Source for app under app sources
target_sources(app PRIVATE ${app_sources})

# Setting required directories
set(REPO_DIR            ~/csse4011/repo/)                   # REPOSITORY ~/csse4011/repo
set(PRAC_TWO_DIR        ${REPO_DIR}/prac2)                  # PRAC TWO ~/csse4011/repo/prac2
set(BSU_SRC             ${PRAC_TWO_DIR}/bsu/src/)           # BSU SOURCE ~/csse4011/repo/prac2/bsu/src/
set(OSLIB_DIR           ${REPO_DIR}/oslib/)                 # OSLIB ~/csse4011/repo/oslib/
set(AHU_HCI_SRC_DIR     ${OSLIB_DIR}/ahu_drivers/ahu_hci/)  # AHU HCI SRC ~/csse4011/repo/oslib/ahu_drivers/ahu_hci/

# Including target sources as private
target_include_directories(app PRIVATE 
  ${REPO_DIR}
  ${PRAC_TWO_DIR}
  ${BSU_SRC}
  ${OSLIB_DIR}
  ${AHU_HCI_SRC_DIR}
)

target_sources(app PRIVATE 
  ${AHU_HCI_SRC_DIR}/shell_cli.c      # Including SHELL Command Line Interface (cli)
  ${AHU_HCI_SRC_DIR}/ahu_hci.c        # Including SHELL Command Line Interface (cli)
  ${AHU_HCI_SRC_DIR}/bsu_base.c
  ${BSU_SRC}/main.c               # Including main.c
)

# zephyr_library_include_directories(${ZEPHYR_BASE}/samples/bluetooth)


