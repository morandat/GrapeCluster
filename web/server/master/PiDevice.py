import json

import psutil
from grape import I2CDevice

class PiDevice(I2CDevice):
    def __init__(self, stack_nb, mac_add, ip_address, i2c_add, pos):
        super(PiDevice, self).__init__(0, 0)#To-Do : bus, prefix
        self._stack_nb = stack_nb
        self._mac = mac_add
        self._i2c = i2c_add
        self._pos = pos
        self._ip_address = ip_address

        self.__last_cpu_usage = "-1"
        self.__last_ram_usage = "-1"

    def get_config(self):
        return (self._mac, self._ip_address, self._i2c_add, self._pos)

    def get_ip_address(self):
        return self._ip_address

    def __boot(self):
        pass

    def __shutdown(self):
        pass

    def set_pos(self, pos):
        self._pos = pos

    def set_last_cpu_usage(self, last_cpu_usage):
        self.__last_cpu_usage = last_cpu_usage

    def get_last_cpu_usage(self):
        return self.__last_cpu_usage

    def get_ram_usage(self):
        return self.__last_ram_usage

    def is_master(self):
        return False
