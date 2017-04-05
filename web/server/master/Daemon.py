# coding: utf-8
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
    def __init__(self, ip_address):
        super(Daemon, self).__init__()

        self.__master = Master(0, "00:00:00:00:00:00", ip_address, "0", 0)

        self.__master.add_stack(Stack(0,0))

        self.__udp_comm = CommunicatorUDP(42666, self.__master.get_ip_address())

        self.__udp_comm.open_communication()
        print("Master listening on {}:{}".format(ip_address, 42666))

    def run(self):
        while True:
            print("Broadcasting cpu request")
            self.__udp_comm.broadcast("1;", self.__master.get_cluster_ip_addresses())
            data, addr = self.__udp_comm.receive(1024)
            print("received message: {} from {}".format(data, addr))
            if data == b"configure":#To-Do: check why data is bstr
                stack = self.__master.get_stack(0)
                #To-do : check for ip_address conflict, implement some kind of simple DHCP. May look for DHCP Py libs
                new_slave = Slave(0, "AA:AA:AA:AA:AA:AA", addr[0], "0", len(stack.get_pi_devices()))
                self.__master.get_stack(0).add_pi_device(new_slave)
                self.__udp_comm.send("0;" + addr[0] + ";", new_slave.get_ip_address())
                print("Configured new slave of ip_addr {}".format(addr[0]))

            elif data[:4] == b"cpu:":
                self.__master.get_slave_by_ip(addr[0]).cpu_usage = data[4:]
                print("Received cpu_usage from slave {}, updating value".format(addr[0]))

    def get_master(self):
        return self.__master