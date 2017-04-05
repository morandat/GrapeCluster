from Communicator import Communicator
from Slave import Slave
class CommunicatorI2C(Communicator):  # Communicator for I2C
    def __init__(self, i2c_add, instr):
        self.__slv = Slave(i2c_add, instr)
    def open_communication(self, i2c_add, instr):
        self.__slv = Slave(i2c_add, instr)

    def send(self, message, address):
        self.__slv.send_instruction()

