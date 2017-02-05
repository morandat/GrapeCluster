import socket

out_ip = "127.0.0.2"
out_port = 42333

in_ip = "127.0.0.1"
in_port = 42666

message = "0"

print "UDP target IP:", out_ip
print "UDP target port:", out_port
print "message:", message

in_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
out_sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)

out_sock.sendto(message, (out_ip, out_port))
in_sock.bind((in_ip, in_port))
while True:
    data, addr = in_sock.recvfrom(1024)
    print "received message:", data

