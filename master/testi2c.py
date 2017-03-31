from grape import I2CDevice
from queue import Queue
def simple_test(self):
    data = Queue(4)
    I2CDevice.write_byte(0x42, 0x01)
    for i in range(0, 4):
            data.put_nowait(I2CDevice.read_byte(self._i2c))
    print(self.__data)


simple_test()