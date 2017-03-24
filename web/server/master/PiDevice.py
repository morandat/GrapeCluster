import json
import datetime

import psutil
from grape import I2CDevice

class PiDevice(I2CDevice):
    nb = 0

    def __init__(self, stack_nb, mac_add, ip_address, i2c_add, pos):
        super(PiDevice, self).__init__(0, 0)#To-Do : bus, prefix

        self.__id = PiDevice.nb
        PiDevice.nb+=1

        self._stack_nb = stack_nb
        self._mac = mac_add
        self._i2c = int(i2c_add)
        self._pos = int(pos)
        self._ip_address = ip_address

        self.__cpu_usage = -1
        self.__last_ram_usage = -1

        self.__cpu_usage_timestamp = -1

        self.__os = "Raspbian"


    def get_identity(self):
        return json.dump(self)

    def get_ip_address(self):
        return self._ip_address

    def __boot(self):
        pass

    def __shutdown(self):
        pass

    def set_pos(self, pos):
        self._pos = pos

    def get_pos(self):
        return self._pos

    def get_cpu_usage(self):
        return self.__cpu_usage

    def set_cpu_usage(self, cpu_usage):
        self.__cpu_usage = cpu_usage
        self.__cpu_usage_timestamp = datetime.datetime.now()

    def get_cpu_usage_timestamp(self):
        return self.__cpu_usage_timestamp

    def get_ram_usage(self):
        return self.__last_ram_usage

    def get_os(self):
        return self.__os

    def get_i2c_address(self):
        return self._i2c

    def get_id(self):
        return self.__id

    def is_master(self):
        return False
