# coding: utf-8
from Stack import Stack

         
class Communicator():  # Interface Communicator to abstract communication (UDP or I2C)

# general actions
    def open_communication(self):
        raise NotImplementedError("Not implemented in interface")

    def send(self, message, address):
        raise NotImplementedError("Not implemented in interface")

    def broadcast(self, message, addresses):
        print(addresses)
        for address in addresses:
            self.send(message, address)

    def close_communication(self):
        raise NotImplementedError("Not implemented in interface")
        
# Action for stacks

    # Get the power usage (V)
    def ask_stack_power(self, stack):
        raise NotImplementedError("Not implemented in interface")

    # Get the temperature (°C)
    def ask_stack_temp(self, stack):
        raise NotImplementedError("Not implemented in interface")

    # Shutdown the rasp of the stack
    def send_stack_shutdown(self, stack):
        raise NotImplementedError("Not implemented in interface")

    # Get actions that are available
    def ask_stack_options(self, stack):
        raise NotImplementedError("Not implemented in interface")

# Actions for slaves

    # To know which slave we are talking
    def ask_slave_slaves(self, slave):
        raise NotImplementedError("Not implemented in interface")

    # Get the identity of slave
    def ask_slave_config(self, slave):
        raise NotImplementedError("Not implemented in interface")

    # Get the CPU usage
    def ask_slave_cpu(self, slave):
        raise NotImplementedError("Not implemented in interface")

    # Start, stop or restart slaves
    def ask_slave_start(self, slave):
        raise NotImplementedError("Not implemented in interface")

    def ask_slave_stop(self, slave):
        raise NotImplementedError("Not implemented in interface")

    def ask_slave_restart(self, slave):
        raise NotImplementedError("Not implemented in interface")

    # Get actions that are available
    def ask_slave_options(self, slave):
        raise NotImplementedError("Not implemented in interface")

