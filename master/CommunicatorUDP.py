import socket
from Communicator import Communicator


class CommunicatorUDP(Communicator):  # Communicator for UDP
    def __init__(self, port, address):
        self.__port = port
        self.__address = address

    def open_communication(self):
        self.__in_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.__out_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

        self.__in_sock.bind((self.__address, self.__port))

    def send(self, message, address):
        self.__out_sock.sendto(message.encode(), (address, self.__port))

    def receive(self, size):
        return self.__in_sock.recvfrom(size)

    def close_communication(self):
        self.__in_sock.close()
        self.__out_sock.close()
        pass
