import random
import socket
import time
import sys
import json

with open("socket_definition.json", 'r') as f:
    socket_def = json.load(f)

# Server
server_socket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
server_address = ('127.0.0.1', socket_def['PYTHON_SERVER_PORT'])
client_address = ('127.0.0.1',socket_def['CPP_CLIENT_PORT'])
print('starting up on {} port {}'.format(server_address[0],server_address[1]))
server_socket.bind(server_address)

def process_recv_message(recv):
    print(recv)

while(True):
    message = server_socket.recv(512)
    process_recv_message(message)
    server_socket.sendto(b'mat dai vcl', client_address)
    
server_socket.close()


# In[ ]:




