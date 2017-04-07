# coding: utf-8
from PiDevice import PiDevice

class Heat(PiDevice):
    CLASS_ADDRESS = 0x48  # 42 for slaves
    def __init__(self, stack_nb=None, mac_add=None, ip_address=None, i2c_add=0x48, pos=0, instruction=0x00, param=None):
        super(Heat, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)

    THM_REG = 0x00

    def get(self):
        while(True):
            try:
                data = self.read_word(self.THM_REG)
                break
            except Exception as e:
                print("I2C Heat not found")
        print("{0:b}".format(data >> 8 | ((data & 0xff) << 8)))
        print(data & 0x0080)
        if data & 0x0080:
            return - (~(data & 0x7F)) - (0.5 if data & 0x8000 else 0.0)
        else:
            return (data & 0xff) + (0.5 if 0x8000 else 0.0)
