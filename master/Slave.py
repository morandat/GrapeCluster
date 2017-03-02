from PiDevice import PiDevice


class Slave(PiDevice):
    def __init__(self, stack_nb, mac_add, ip_address, i2c_add, pos):
        super(Slave, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)
