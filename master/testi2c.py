from Slave import *
from CommunicatorI2C import CommunicatorI2C

def main_i2c():
    slv = Slave(0x42)
    comm = CommunicatorI2C()
    comm.send_instruction(slv, 0x00, 'test')
    if slv.get_data() == ['a', 'b', 'c', 'd']:
        print("TEST abcd : OK")
    else:
        print("TEST abcd : KO")

main_i2c()
