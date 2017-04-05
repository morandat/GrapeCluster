from Communicator import Communicator
from Slave import Slave
class CommunicatorI2C(Communicator):  # Communicator for I2C
    def __init__(self):
        pass

    def open_communication(self):
        pass

    def sendI2C(self, slave):
        self.send_instruction(slave)

    def send_instruction(self, slave):
        while True:
            try:
                slave.write_byte(slave.get_instr())
                break
            except Exception as e:
                print(e)
                print("Write from", slave.CLASS_ADDRESS, " failed trying again")
        if slave.get_name_instruction() not in Slave.no_return_instructions:
            for i in range(0, 4):
                while True:
                    try:
                        tmp = slave.read_byte(slave._i2c)
                        slave.set_data(i,tmp)
                        break
                    except Exception as e:
                        print(e)
                        print("Read from", slave.CLASS_ADDRESS, "failed, trying again")
            slave.decode_data()
            return slave.decode_data