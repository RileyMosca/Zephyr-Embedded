# # Python Library Imports
import serial
import re
import matplotlib.pyplot as plt

# # define the serial port and baudrate
SERIAL_PORT_NAME = '/dev/ttyACM0'
SERIAL_BAUD_RATE = 115200

serial_port_data = serial.Serial(SERIAL_PORT_NAME, SERIAL_BAUD_RATE)

orientation_roll = 0
orientation_pitch = 0
altitude_coarse = 0
altitude_fine = 0
current_time = 0

o_roll_arr = []
o_pitch_arr = []
a_coarse_arr = []
a_fine_arr = []
time_arr = []

x_data1 = [0.1, 0.2, 0.3, 0.4]
y_data1 = [1, 2, 3, 4]

x_data2 = [0.1, 0.2, 0.3, 0.4]
y_data2 = [1, 4, 9, 16]

# Setting MatPlotLib figure

fig, (ax_o_r, ax_o_p, ax_a_c, ax_a_f) = plt.subplots(4)

# Setting X Labels
ax_o_r.set_xlabel('Time (s)')
ax_o_p.set_xlabel('Time (s)')
ax_a_c.set_xlabel('Time (s)')
ax_a_f.set_xlabel('Time (s)')

# Setting Y labels
ax_o_r.set_ylabel('Roll')
ax_o_p.set_ylabel('Roll')
ax_a_c.set_ylabel('Alt Coarse (m)')
ax_a_f.set_ylabel('Alt Fine (m)')

# Setting titles
ax_o_r.set_title('Roll, Pitch, Alt Coarse & Alt Fine Vs. Time')

def main():
    while True:
        try:
            str_port_data = serial_port_data.readline().decode("utf-8").strip()
            data  = str_port_data.split(",")[0]
            
            # if data.startswith(" "):
            if data[0] != "{" or data[0] != "}":
                start_index = data.find("<")
                end_index = data.find(">") + 1
                result = data[start_index:end_index]
                
                if result == "<13>":
                    orientation_roll = float(re.search(r"\[([-0-9.]+)\]", data).group(1))
                    o_roll_arr.append(orientation_roll)

                elif result == "<14>":
                    orientation_pitch = float(re.search(r"\[([-0-9.]+)\]", data).group(1))
                    o_pitch_arr.append(orientation_pitch)

                elif result == "<15>":
                    altitude_fine = float(re.search(r"\[([-0-9.]+)\]", data).group(1))
                    a_fine_arr.append(altitude_fine)
                    # a_fine_arr.pop()

                elif result == "<16>":
                    altitude_coarse = float(re.search(r"\[([-0-9.]+)\]", data).group(1))
                    a_coarse_arr.append(altitude_coarse)

                elif result == "<time>":
                    current_time = float(re.search(r"\[([-0-9.]+)\]", data).group(1))
                    current_time /= 1000000
                    time_arr.append(current_time)
            print("Data:\r\n")
            print(time_arr)
            print(o_pitch_arr)
            print(o_roll_arr)
            print(a_coarse_arr)
            print(a_fine_arr)
            print("-------\n")
        except KeyboardInterrupt:
            break

        
    # Plotting the data
    ax_o_r.plot(time_arr, o_roll_arr, label='Roll')
    ax_o_p.plot(time_arr, o_pitch_arr, label='Pitch')
    ax_a_c.plot(time_arr, a_coarse_arr, label='Altitude Coarse (m)')
    ax_a_f.plot(time_arr, a_fine_arr, label='Altitude Fine (m)')

    # Showing legends & plot
    ax_o_r.legend()
    ax_o_p.legend()
    ax_a_c.legend()
    ax_a_f.legend()

    plt.show()

main()