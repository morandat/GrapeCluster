class Communicator():  # Interface Communicator to abstract communication (UDP or I2C)
	def open_communication(self):
        raise NotImplementedError("Not implemented in interface")
	
	def send(self):
        raise NotImplementedError("Not implemented in interface")

	def close_communication(self):
        raise NotImplementedError("Not implemented in interface")
