# coding: utf-8
from PiDevice import PiDevice
from grape import STACK_DEVICES
from Alimentation import Alimentation

class Master(PiDevice):
    def __init__(self, stack_nb, mac_add, ip_address, i2c_add, pos):
        super(Master, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)
        self.__stack_nb = stack_nb
        self.__stacks = []
        self.__alim = Alimentation()
        for s in STACK_DEVICES:
			#add_stack(s)
            pass

    def get_slave_by_ip(self, ip_address):
        for stack in self.__stacks:
            for pi_device in stack.get_pi_devices():
                if pi_device.get_ip_address() == ip_address:
                    return pi_device

    def get_slave_by_id(self, id):
        for stack in self.__stacks:
            for pi_device in stack.get_pi_devices():
                if pi_device.get_id() == id:
                    return pi_device

    def get_slave_by_i2c(self, i2c):
        for stack in self.__stacks:
            for pi_device in stack.get_pi_devices():
                if pi_device.get_i2c() == i2c:
                    return pi_device

    def get_stack(self, number):
        return self.__stacks[number]

    def get_stacks(self):
        return self.__stacks

    def get_cluster_ip_addresses(self):
        ip_addresses = []
        for stack in self.__stacks:
            ip_addresses.extend(    stack.get_ip_addresses())
        return ip_addresses

    def add_stack(self, stack):
        self.__stacks.append(stack)

    def is_master(self):
        return True

    def enable_alim(self):
        __alim.enable_alimentation()

    def disable_alim(self):
        __alim.disable_alimentation()

