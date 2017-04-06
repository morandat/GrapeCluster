# coding: utf-8
import socket
from Communicator import Communicator


class CommunicatorUDP(Communicator):  # Communicator for UDP
    def __init__(self, port, address):
        self.__port = port
        self.__address = address

    def open_communication(self):
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.__sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.__sock.bind((self.__address, self.__port))


    def send(self, message, address):
        self.__sock.sendto(message.encode(), (address, self.__port))

    def receive(self, size):
        return self.__sock.recvfrom(size)

    def close_communication(self):
        self.__sock.close()
        pass

# Action for stacks

    # Get the power usage (V)
    def ask_stack_power(self, stack):
        total=0
        nb=0
        for pi_device in stack.__pi_devices:
            if not pi_device.is_master():
                self.send("power" ,pi_device.get_ip_address())
                data, addr = self.receive(10) #taille à voir
                total= total + data
                nb=nb+1
        return total/nb

    # Get the temperature (°C)
    def ask_stack_temp(self, stack):
        total=0
        nb=0
        for pi_device in stack.__pi_devices:
            if not pi_device.is_master():
                self.send("temperature" ,pi_device.get_ip_address())
                data, addr = self.receive(10) #taille à voir
                total= total + data
                nb=nb+1
        return total/nb

    # Shutdown the rasp of the stack
    def send_stack_shutdown(self, stack):
        for pi_device in stack.__pi_devices:
            if not pi_device.is_master():
                self.send("shutdown" ,pi_device.get_ip_address())
                #verification of response ?

    # Get actions that are available
    def ask_stack_options(self, stack):
        #actions available to defined
        return

# Actions for slaves

    # To know which slave we are talking
    def ask_slave_slaves(self, slave):
        self.send("info", slave.ip_address)
        data, addr = self.receive(4)  # all responses are in 4 bytes
        return data

    # Get the identity of slave
    def ask_slave_config(self, slave):
        self.send("config", slave.ip_address)
        data, addr = self.receive(4)  
        return data

    # Get the CPU usage
    def ask_slave_cpu(self, slave):
        self.send("cpu", slave.ip_address)
        data, addr = self.receive(4)  
        return data

     # Start, stop or restart slaves
    def ask_slave_start(self, slave):
        self.send("start", slave.ip_address)
        data, addr = self.receive(4)  
        return data

    def ask_slave_stop(self, slave):
        self.send("stop", slave.ip_address)
        data, addr = self.receive(4)  
        return data

    def ask_slave_restart(self, slave):
        self.send("restart", slave.ip_address)
        data, addr = self.receive(4)  
        return data

    # Get actions that are available
    def ask_slave_options(self, slave):
        #actions available to defined
        return
