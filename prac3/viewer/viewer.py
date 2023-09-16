# Web Server Imports
import influxdb_client, os, time
from influxdb_client import InfluxDBClient, Point, WritePrecision
from influxdb_client.client.write_api import SYNCHRONOUS
import serial as ser

# ML Tensor Imports
import random
import numpy as np
import random
import gzip
import pickle
import torch
import torch.nn as nn
import torch.optim as optim
import pandas as pd
import sklearn
from sklearn.model_selection import train_test_split 
from sklearn.preprocessing import StandardScaler
from sklearn.metrics import confusion_matrix, f1_score, accuracy_score
from sklearn.neighbors import KNeighborsRegressor

############## Debugging Options ###############
debug = True
training = False

################ ML ALGORITHM #################
model_filename = 'model'
model_pickle_path = os.path.expanduser(f'~/csse4011/repo/prac3/ml_model/{model_filename}')
loaded_model = pickle.load(open(model_pickle_path, 'rb'))

########### Web Server API & Serial ###########

# Serial-Parameters
INITIAL_TIME = 0
SERIAL_PORT_NAME = '/dev/ttyACM0'
SERIAL_BAUD_RATE = 115200

serial_port_data = ser.Serial(SERIAL_PORT_NAME, SERIAL_BAUD_RATE)

# Web-Server Parameters
token = "tpok3ZM1HhSkLfYjFAMa5BGSQrZSBSwtDCy8aUXNcv6KZn4Xybiuzrw8kIXaKqhL1_WL4nGY4x8dmWBK4DKZzw=="
org = "Student"
url = "https://us-east-1-1.aws.cloud2.influxdata.com"

# API Bucket Parameters
rssi_bucket="rssi_time_series"
ml_bucket="ml_positional_data"

# Static Node Data
static_node_data = {
    "static_node_a": {
        "rssi": 0
    },
    "static_node_b": {
        "rssi": -10
    },
    "static_node_c": {
        "rssi": -20
    },
    "static_node_d": {
        "rssi": -30
    },
    "static_node_e": {
        "rssi": -40
    },
    "static_node_f": {
        "rssi": -50
    },
    "static_node_g": {
        "rssi": -60
    },
    "static_node_h": {
        "rssi": -70
    },
}

# Machine Learning Data
ml_positional_data = {
    "ml_data" : {
        "x_position" : 0,
        "y_position" : 0
    },
}

# An array of static node points (data and id)
static_node_points = []
    
def create_rssi_val_array() -> list[float]:
    """
        Creates an array of 8 floats that will
        be used in the machine learning
        algorithm to return an x and y value
    """
    rssi_vals = []
    for node in static_node_data:
        rssi_vals.append(float(static_node_data[node]["rssi"]))
    return rssi_vals

# Initialising Writing Clied
write_client = influxdb_client.InfluxDBClient(url=url, token=token, org=org)

# Define the write api
write_api = write_client.write_api(write_options=SYNCHRONOUS)

# Current time data, changes with every sleep delay
current_time = 0

# Constructing data and writing to API every 500ms
while True:

    # Reading a new line from serial input
    current_line = serial_port_data.readline().decode("utf-8").strip()

    # Splitting data by commas (for data)
    split_data = current_line.split(',')

    # We have received data here, process and assign it
    if len(split_data) >= 8:
        try:
            static_node_data["static_node_a"]["rssi"] = -1 * (int(split_data[0][-2]) * 10 + int(split_data[0][-1]))
        except ValueError:
            static_node_data["static_node_a"]["rssi"] = -1 * (int(split_data[0][-1]))
        
        static_node_data["static_node_b"]["rssi"] = int(split_data[1])
        static_node_data["static_node_c"]["rssi"] = int(split_data[2])
        static_node_data["static_node_d"]["rssi"] = int(split_data[3])
        static_node_data["static_node_e"]["rssi"] = int(split_data[4])
        static_node_data["static_node_f"]["rssi"] = int(split_data[5])
        static_node_data["static_node_g"]["rssi"] = int(split_data[6])
        static_node_data["static_node_h"]["rssi"] = int(split_data[7])

    # # Creating static node point (with all time and node rssi values)
    # for node, data in static_node_data.items():
    #     static_node_point = Point("node_data")\
    #         .tag("node", node)\
    #         .field("rssi", data["rssi"])\
        
    #     # Appending RSSI Data
    #     static_node_points.append(static_node_point)

    # # Writing to API Client
    # write_api.write(bucket=rssi_bucket, org=org, record=static_node_points)    
    
    # Vector of 8 floats
    ml_tensor_arr = [create_rssi_val_array()]

    # # Calculate ML X and Y Here
    result = loaded_model.predict(ml_tensor_arr) 
    x = result[0][0]
    y = result[0][1]
    

    # Assign ML X and Y Here
    ml_positional_data["ml_data"]["x_position"] = x
    ml_positional_data["ml_data"]["y_position"] = y

    # Printing Current Time
    if debug is True:
        print("------- DEBUG TEXT-------")
        print(f"Current Time: {current_time}\n")
        print("RSSI VALUES:\n"\
            "NODE A : {0}\n"\
            "NODE B : {1}\n"\
            "NODE C : {2}\n"\
            "NODE D : {3}\n"\
            "NODE E : {4}\n"\
            "NODE F : {5}\n"\
            "NODE G : {6}\n"\
            "NODE H : {7}\n".format(static_node_data["static_node_a"]["rssi"],
            static_node_data["static_node_b"]["rssi"],
            static_node_data["static_node_c"]["rssi"],
            static_node_data["static_node_d"]["rssi"],
            static_node_data["static_node_e"]["rssi"],
            static_node_data["static_node_f"]["rssi"],
            static_node_data["static_node_g"]["rssi"],
            static_node_data["static_node_h"]["rssi"]))
        
        print("X pos = {0}\nY pos = {1}\n".format(
            ml_positional_data["ml_data"]["x_position"], 
            ml_positional_data["ml_data"]["y_position"])
        )

    # # Creating ML Point containing x and y values
    # ml_data_point = Point("ml_pos_data") \
    #     .tag("data_type", "ml_data")\
    #     .field("x_position", ml_positional_data["ml_data"]["x_position"])\
    #     .field("y_position", ml_positional_data["ml_data"]["y_position"])

    # # Writing to Bucket of ML x and y
    # write_api.write(bucket=ml_bucket, org=org, record=ml_data_point)

    # Setting current time to add 500ms
    current_time += 1.0

    # Rounding to one decimal place
    round(current_time, 1)