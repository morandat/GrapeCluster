from PiDevice import PiDevice
from grape import STACK_DEVICES

class Master(PiDevice):
    def __init__(self, stack_nb, mac_add, ip_address, i2c_add, pos):
        super(Master, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)
        self.__stack_nb = stack_nb
        self.__stacks = []
        for s in STACK_DEVICES:
            pass
            # To-Do: initialisation

    def get_slave_by_ip(self, ip_address):
        for stack in self.__stacks:
            for pi_device in stack.get_pi_devices():
                if pi_device.get_ip_address() == ip_address:
                    return pi_device
        return None

    def get_slave_by_id(self, id):
        for stack in self.__stacks:
            for pi_device in stack.get_pi_devices():
                if pi_device.get_id() == id:
                    return pi_device
        return None

    def get_stack(self, number):
        return self.__stacks[number]

    def get_stacks(self):
        return self.__stacks

    def get_cluster_ip_addresses(self):
        ip_addresses = []
        for stack in self.__stacks:
            ip_addresses.extend(stack.get_ip_addresses())
        return ip_addresses

    def add_stack(self, stack):
        self.__stacks.append(stack)

    def is_master(self):
        return True
