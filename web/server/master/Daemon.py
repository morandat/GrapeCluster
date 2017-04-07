# coding: utf-8
import socket
from threading import Thread
import os, sys
sys.path.append(os.path.dirname(os.path.abspath(__file__)))
from Stack import Stack
from Master import Master
from Slave import Slave

from Commands import Commands
from CommunicatorUDP import CommunicatorUDP
from CommunicatorI2C import CommunicatorI2C

from logging import getLogger
from Alimentation import Alimentation
from grape import *


class Daemon(Thread):
    def __init__(self, ip_address, orders_file_path):
        super(Daemon, self).__init__()

        self.__commands = Commands(orders_file_path)

        self.__master = Master(0, "00:00:00:00:00:00", ip_address, "0", 0)

        self.__master.add_stack(Stack(0,0))

        self.__udp_comm = CommunicatorUDP(42666, self.__master.get_ip_address())

        self.__udp_comm.open_communication()
        print("Master listening on {}:{}".format(ip_address, 42666))
        
        self.__i2c_comm = CommunicatorI2C()


    def run(self):
        nb_stack = 0
        nb_slave = 0
        value = 1
        
        while True:

            print("Broadcasting cpu request")
            self.__udp_comm.broadcast(self.__commands.get_index("cpu")+";", self.__master.get_cluster_ip_addresses())
            
            data, addr = self.__udp_comm.receive(1024)    
            print("received message: {} from {}".format(data, addr))
                            
            print(data[:-1])
            if data[:-1] == b"configure;":#To-Do: check why data is bstr
                if(nb_slave <= 5):
                    stack = self.__master.get_stack(nb_stack)
                    #To-do : check for ip_address conflict, implement some kind of simple DHCP. May look for DHCP Py libs
                    new_slave = Slave(nb_stack + RASP_CLASS_ADDRESSES[int(data[10:])-1], nb_stack, "AA:AA:AA:AA:AA:AA", addr[0], int(data.decode().split(";")[1]))
                    self.__master.get_stack(nb_stack).add_pi_device(new_slave)
                    value = value + 1
                    self.__udp_comm.send(self.__commands.get_index("configure") + ";" + addr[0] + ";", new_slave.get_ip_address())
                    print("Configured new slave of ip_addr {}".format(addr[0]))
                    nb_slave = nb_slave + 1
                else:
                    nb_stack = nb_stack + 1
                    new_stack = Stack(nb_stack, nb_stack)
                    self.__master.add_stack(new_stack)
                    nb_slave = 0


            elif data[:4] == b"cpu:":
                self.__master.get_slave_by_ip(addr[0]).cpu_usage = data[4:]
                value = value + 1
                print("Received cpu_usage ({}) from slave {}, updating value".format(addr[0], data[4:]))
                


    def get_master(self):
        return self.__master

    def get_udp_comm(self):
        return self.__udp_comm

    def get_i2c_comm(self):
        return self.__i2c_comm

    def get_commands(self):
        return self.__commands

    def enable_alimentation_stack(self, stack):
        """ TODO for all the stack
        Only for one stack
        """
        self.__master.enable_alim()

    def disable_alimentation_stack(self, stack):
        """ TODO for all the stack
        Only for one stack
        """
        self.__master.disable_alim()
