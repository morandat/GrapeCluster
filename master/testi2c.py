from Slave import *
from CommunicatorI2C import CommunicatorI2C

def main_i2c():
    slv = Slave(0x42)
    comm = CommunicatorI2C()
    comm.send_instruction(slv, 0x00, 'test1')
    print(slv.get_data())
    comm.send_instruction(slv, 0x01, 'test2')
    print(slv.get_data())

main_i2c()
