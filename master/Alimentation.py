# coding: utf-8
from PiDevice import PiDevice

class Alimentation(PiDevice):
    CLASS_ADDRESS = 0x38  # 42 for slaves
    def __init__(self, stack_nb=None, mac_add=None, ip_address=None, i2c_add=0x42, pos=None, instruction=0x00, param=None):
        super(Alimentation, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)

    def enable_alimentation(self):
        self.write_byte(0x38, 0x00)

    def disable_alimentation(self):
        self.write_byte(0x38, 0xff)
