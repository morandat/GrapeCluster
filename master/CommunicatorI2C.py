from Communicator import Communicator

class CommunicatorI2C(Communicator):  # Communicator for I2C
    def __init__(self, port, address):
        print("init i2c")
    def open_communication(self):
        print("open i2c")
    def send(self, message, address):
        print("send i2c")

    def receive(self, size):
        print("receive i2c")

    def close_communication(self):
        print("close i2c")
        pass
