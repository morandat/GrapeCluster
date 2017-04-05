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
    #CLASS_ADDRESS = 0x00 # to define for each slave

    def __init__(self, i2c_add, instruction, stack_nb=None, mac_add=None, ip_address=None, pos=None):
        self.__instr=instruction
        self.CLASS_ADDRESS=i2c_add
        self.data = [-1, -1, -1, -1]
        super(Slave, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)
        self.send_instruction()

    def send_instruction(self):
        while True:
            try:
                self.write_byte(self.__instr)
                break
            except Exception as e :
                print("Write from", self.CLASS_ADDRESS, " failed trying again")
        for i in range(0, 4):
            while True:
                try:
                    tmp = self.read_byte(self._i2c)
                    self.data[i] = tmp
                    break
                except Exception as e:
                    print("Read from", self.CLASS_ADDRESS, "failed, trying again")
        self.decode_data()

    def decode_data(self):
        print("before decode", self.data)
        #add decode instructions
        print("after decode", self.data)

    def test_dede(self):
        self.write_byte(0x00)
        i = 0
        e = 0
        while (i < 4):
            try :
                if(e == 0):
                    tmp = self.read_byte(self._i2c)
                    print(tmp)
                    i = i + 1
                else:
                    tmp = self.read_byte(self._i2c)
                    print("err :")
                    print(tmp)
                    e = 0
                #break
            except Exception as e:
                e = 1
                print("plant", e)  






