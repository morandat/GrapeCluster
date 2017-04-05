from Slave import *

def main_i2c():
    slv = Slave(0x42, 0x00, 'test')
    slv.send_instruction()

main_i2c()