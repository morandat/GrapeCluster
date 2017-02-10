class CommunicatorUDP():  #Communicator for UDP
	
    def __init__(self,):
        self.__in_port = 42666
	
	def open_communication(self):        
		self.__in_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
		self.__out_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

		in_sock.bind((master.get_ip_address(), self.__in_port))
	
	def send(self, message, address, port):
        out_sock.sendto(message, address, port))

	def close_communication(self):
		pass
