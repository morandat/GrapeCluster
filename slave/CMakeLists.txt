
project(Slave)
add_executable(Slave daemon.c udpslave.c i2cslave.c commands.c udpslave.h)