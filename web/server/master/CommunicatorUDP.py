import socket
from Communicator import Communicator


class CommunicatorUDP(Communicator):  # Communicator for UDP
    def __init__(self, port, address):
        self.__port = port
        self.__address = address

    def open_communication(self):
        self.__sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.__sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.__sock.bind((self.__address, self.__port))


    def send(self, message, address):
        self.__sock.sendto(message.encode(), (address, self.__port))

    def receive(self, size):
        return self.__sock.recvfrom(size)

    def close_communication(self):
        self.__sock.close()
        self.__sock.close()
        pass
