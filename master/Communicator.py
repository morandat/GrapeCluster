class Communicator():  # Interface Communicator to abstract communication (UDP or I2C)
    def open_communication(self):
        raise NotImplementedError("Not implemented in interface")

    def send(self, message, address):
        raise NotImplementedError("Not implemented in interface")

    def broadcast(self, message, addresses):
        print(addresses)
        for address in addresses:
            self.send(message, address)

    def close_communication(self):
        raise NotImplementedError("Not implemented in interface")
