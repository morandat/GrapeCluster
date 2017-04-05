# coding: utf-8
from IStack import IStack
from logging import getLogger
from grape import STACK_DEVICES

MAX_PI_DEVICES_PER_STACK = 7

class Stack(IStack):
    def __init__(self, bus, prefix):
        self._prefix = prefix
        self._bus = bus
        self.__pi_devices = []
        self.__devices = {}
        for dev in STACK_DEVICES:
            if dev.probe(bus, prefix):
                self.__devices[dev] = dev(bus, prefix)
        for dev in self.__devices.items():
            try:
                dev.setup()
            except Exception as e:
                getLogger(__name__).error("Unable to setup %s: %s", dev, e)
        self.__pi_devices = []

    def get_pi_devices(self):
        return self.__pi_devices

    def add_pi_device(self, pi_device):
        if len(self.__pi_devices) <= MAX_PI_DEVICES_PER_STACK:
            self.__pi_devices.append(pi_device)
            pi_device.set_pos(len(self.__pi_devices)-1)
        else:
            raise IndexError("Trying to add more than 7 Pi devices to the same stack")

    def get_ip_addresses(self):
        ip_addresses = []
        for pi_device in self.__pi_devices:
            if not pi_device.is_master():
                ip_addresses.append(pi_device.get_ip_address())
        return ip_addresses

    def __getitem__(self, kind):
        return self.__devices[kind]