import socket
from Stack import Stack
from Master import Master
from Slave import Slave

def broadcast(master):
    for stack in master.stacks:
        for pi_device in stack.pi_devices:
            if not pi_device._is_master():
                out_sock.sendto(message, (pi_device.get_ip_address(), out_port))

stack = Stack()

master = Master(0, "00:00:00:00:00:00", "127.0.0.2", "0", 0)
slave = Slave(0, "AA:AA:AA:AA:AA:AA", "127.0.0.1", "0", 0)

stack.add_pi_device(master)
stack.add_pi_device(slave)

master.addStack(stack)

out_port = 42333

in_port = 42666

message = "0"

in_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
out_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

in_sock.bind((master.get_ip_address, in_port))

broadcast(master)

while True:
    data, addr = in_sock.recvfrom(1024)
    print "received message:", data

