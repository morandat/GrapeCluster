import socket
from Stack import Stack
from Master import Master
from Slave import Slave
from CommunicatorUDP import CommunicatorUDP
from logging import getLogger

import sys

stack = Stack(0, 0)

master = Master(0, "00:00:00:00:00:00", "127.0.0.1", "0", 0)
slave = Slave(0, "AA:AA:AA:AA:AA:AA", "127.0.0.2", "0", 0)

stack.add_pi_device(master)
stack.add_pi_device(slave)

master.add_stack(stack)

port = 42666

comm = CommunicatorUDP(port, master.get_ip_address())

comm.open_communication()

comm.broadcast(sys.argv[1], master.get_cluster_ip_addresses())

while True:
    data, addr = comm.receive(10)
    print("received message: {} from {}".format(data, addr))

