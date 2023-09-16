_______

# CSSE4011 - Practical One - Seismic Activity Monitoring System (SAMS)
Utilising an embedded systems design approach of Application Host Unit (AHU), Host Controller Unit (HCI) and Sensor Controller Unit (SCU). The AHU will be interfacing with the SCU via  the HCI (accomplished through Serial/ UART). The AHU is responsible for interfacing with the hardware of the ESP32-C3 as well as processing requests and responses with the B-L475E-IOT Platform. This will be achieved through SHELL commands, as a shell command is processed in the Bash Terminal, it will then process this data into a "packet" that is to be trasnmitted to the SCU. The SCU will receive this "packet" following the protocol (see X) and return data or control hardware as specified by the "packet".

The AHU will also allow data logging via .JSON formatting, for sensor data received, as well as debug, info, warn and error logging as per the Zephyr LOGGING API.

_______

# Repository Author:
Riley Mosca (s4535819 / 45358195)

# CSSE4011 Team 1:
- Riley Mosca (45358195)
- Sean Collins (47035007)

_______

# Practical One - Design Task Functionality:
1. AHU Shell Command Line Interface
2. SCU Shell Command Line Interface
3. HCI UART Protocol & UART Communication
4. Data formatting (.JSON) and displaying

_______

# Practical One - Folder Structure:
**The folder structure is relative to ~/csse4011/repo/**
<br>
repo                            <br>
├── oslib                       <br>
│   └── ahu_drivers             <br>
│       └── ahu_hci             <br>
│           ├── ahu_hci.c       <br>
│           ├── ahu_hci.h       <br>
│           ├── rgb.c           <br>
│           ├── rgb.h           <br>
│           ├── shell_cli.c     <br>
│           └── shell_cli.h     <br>
├── prac1                       <br>
│   ├── ahu                     <br>
│   │   ├── CMakeLists.txt      <br>
│   │   ├── prj.conf            <br>
│   │   └── src                 <br>
│   │       └── main.c          <br>
│   └── build                   <br>
└── README.md                   <br>
_______


# References/ Sources:
1. Shell support located in: `cd ~/csse4011/zephyrproject/zephyr/samples/subsys/shell`
2. UART support located in: `cd ~/csse4011/zephyrproject/zephyr/samples/drivers/uart/echo_bot`
3. Online resources: [CSSE4011 Guides](https://uqiotstudio.github.io/csse4011_zephyr_guides/)

_______

# Source Code/ Build Instructions:
1. Clone the repository: `git clone git@csse4011-gitea.zones.eait.uq.edu.au:45358195/repo.git`
2. Change into the building directory: `cd ~/csse4011/repo/prac1/ahu`
3. Connect the ESP32-C3 to your PC/ Virtual Machine.
4. Enable read/ write & execute permissions for your device: `sudo chmod 777 /dev/ttyUSB0`
<br> *Note: Device may be /dev/ttyACM0 or similar, check `ls -n /dev/tty` to verify*
5. Build the program: `west build -p -b esp32c3_devkitm`
6. Flash to the ESP32-C3: `west flash`

# User Instructions:
After the device has been flashed with the code built above.
1. STEP 1
2. STEP 2
3. STEP 3

_______

# Additional Notes:
Notes....
