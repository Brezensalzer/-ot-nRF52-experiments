#!/usr/bin/python3
# ---------------------------------------------------------
# thread2mqtt.py
# ---------------------------------------------------------

import paho.mqtt.client as paho
import socket 
#import time

mqtt_client_id = "Thread_Border_Router"
mqtt_broker_host = "<hostname>"
mqtt_broker_port = 1883
mqtt_topic = "thread/"

# ---------------------------------------------------------
# mqtt callbacks
# ---------------------------------------------------------
def on_connect(client, userdata, flags, rc):
    print("Connection returned " + str(rc))

def on_disconnect(client, userdata, rc):
    time.sleep(120) # reconnect Versuch nach 2 Minuten
    global mqtt
    mqtt.reconnect()

def on_log(client, userdata, level, buf):
    print(str(buf))

# ---------------------------------------------------------
# connect to mqtt broker
# ---------------------------------------------------------
mqtt = paho.Client(client_id=mqtt_client_id)

mqtt.on_connect = on_connect
mqtt.on_disconnect = on_disconnect
mqtt.on_log = on_log
mqtt.username_pw_set('<user>', '<password>')

mqtt.connect(host=mqtt_broker_host, port=mqtt_broker_port, keepalive=180)
mqtt.loop()

# ---------------------------------------------------------
# bind to UDP socket
# ---------------------------------------------------------
UDP_IP = "::"     # IPv6 any address
UDP_PORT = 4711

sock = socket.socket(socket.AF_INET6, socket.SOCK_DGRAM) # UDP
sock.bind((UDP_IP, UDP_PORT))

while True:
    data, addr = sock.recvfrom(1024) # buffer size is 1024 bytes
    message = data.decode("utf-8")
    payload = message.rstrip('\n')
    mqtt.publish(mqtt_topic,payload)
    mqtt.loop()
