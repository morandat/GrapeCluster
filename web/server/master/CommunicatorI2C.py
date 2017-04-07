from Communicator import Communicator
from Slave import Slave
class CommunicatorI2C(Communicator):  # Communicator for I2C
    def __init__(self):
        pass

    def open_communication(self):
        pass #send something directly, open communication is not needed

    def send_instruction(self, slave, num_instr, name_instruction):
        while True:
            try:
                slave.write_byte(num_instr)
                break
            except Exception as e:
                print("Write from", slave.CLASS_ADDRESS, " failed trying again(", e, ")")
        if name_instruction not in Slave.no_return_instructions:
            for i in range(0, 4):
                while True:
                    try:
                        tmp = slave.read_byte()
                        slave.set_data(i, tmp)
                        break
                    except Exception as e:
                        print("Read from", slave.CLASS_ADDRESS, "failed, trying again (", e, ")")
            slave.decode_data()
            return slave.decode_data

    def send_custom_instruction(self, slave, instr, name_instr):
        while True:
            try:
                slave.write_byte(instr)
                break
            except Exception as e:
                print(e)
                print("Write from", slave.CLASS_ADDRESS, " failed trying again")
        if name_instr not in Slave.no_return_instructions:
            for i in range(0, 4):
                while True:
                    try:
                        tmp = slave.read_byte(slave.get_i2c())
                        slave.set_data(i, tmp)
                        break
                    except Exception as e:
                        print(e)
                        print("Read from", slave.CLASS_ADDRESS, "failed, trying again")
            slave.decode_data()
            return slave.decode_data

