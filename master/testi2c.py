from Slave import *
from CommunicatorI2C import CommunicatorI2C

def main_i2c():
    slv = Slave(0x42, 0x00, 'test')
    comm = CommunicatorI2C()
    comm.send_instruction(slv)

main_i2c()
