# coding: utf-8
from PiDevice import PiDevice
from array import array

#key = bytearray([0x13, 0x00, 0x00, 0x00, 0x08, 0x00])

# instruction = {0: Slave.stop,
#                1: Slave.echo,
#                2: Slave.ls,
#                3: Slave.ps,
#                4: Slave.shutdown,
#                5: Slave.read,
#                6: Slave.error
#                }

class Slave(PiDevice):
    no_return_instructions = {'shutdown', 'restart'}
    #CLASS_ADDRESS = 0x00 # to define for each slave

    def __init__(self, i2c_add, numb_instr, name_instruction, stack_nb=0, mac_add="FF:FF:FF:FF:FF", ip_address="127.0.0.3", pos=0):
        self.__instr = numb_instr
        self.__name_instruction = name_instruction
        self.CLASS_ADDRESS = i2c_add
        self.__data = [-1, -1, -1, -1]
        super(Slave, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)
        print(self.__instr)
        print(self.CLASS_ADDRESS)
        #self.send_instruction()

    def decode_data(self):
        print("before decode", self.__data)
        #add decode instructions
        data_decoded = ['a', 'a', 'a', 'a']
        for i in range(0, 4):
            data_decoded[i] = chr(self.__data[i])
        print("after decode", data_decoded)

    def get_instr(self):
        return self.__instr

    def get_name_instruction(self):
        return self.__name_instruction