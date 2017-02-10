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

    def get_stack(self, number):
        return self.__stacks[number]

    def get_stacks(self):
        return self.__stacks

    def add_stack(self, stack):
        self.__stacks.append(stack)

    def is_master(self):
        return True