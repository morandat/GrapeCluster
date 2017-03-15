import socket
from threading import Thread
import os, sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
from Stack import Stack
from Master import Master
from Slave import Slave
from CommunicatorUDP import CommunicatorUDP
from logging import getLogger

class Daemon(Thread):
    def __init__(self):
        super(Daemon, self).__init__()
        stack = Stack(0, 0)

        self.__master = Master(0, "00:00:00:00:00:00", "127.0.0.2", "0", 0)
        slave = Slave(0, "AA:AA:AA:AA:AA:AA", "127.0.0.3", "0", 0)

        stack.add_pi_device(self.__master)
        stack.add_pi_device(slave)

        self.__master.add_stack(stack)

        self.__udp_comm = CommunicatorUDP(42666, self.__master.get_ip_address())

        self.__udp_comm.open_communication()

    def run(self):
        self.__udp_comm.broadcast("0", self.__master.get_cluster_ip_addresses())

        while True:
            data, addr = self.__udp_comm.receive(10)
            print("r    eceived message: {} from {}".format(data, addr))

    def get_master(self):
        return self.__master