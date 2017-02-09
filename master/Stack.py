import IStack

MAX_PI_DEVICES_PER_STACK = 7

class Stack:
    def __init__(self):
        self.__pi_devices = []

    def add_pi_device(self, pi_device):
        if len(self.__pi_devices) <= MAX_PI_DEVICES_PER_STACK:
            self.__pi_devices.append(pi_device)
            pi_device.set_pos(len(self.__pi_devices)-1)
        else:
            raise StandardError("Trying to add more than 7 Pi devices to the same stack")