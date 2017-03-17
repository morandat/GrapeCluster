from Stack import Stack

class Communicator():  # Interface Communicator to abstract communication (UDP or I2C)

# Actions générales
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
# Actions/Demandes sur les stacks

    # Obtenir la consommation d'éléctricité (en volts)
    def ask_stack_power(self, stack):
        raise NotImplementedError("Not implemented in interface")

    # Obtenir la température (en degré)
    def ask_stack_temp(self, stack):
        raise NotImplementedError("Not implemented in interface")

    # Allumer ou éteindre les rasp
    def send_stack_shutdown(self, stack):
        raise NotImplementedError("Not implemented in interface")

    # Obtenir les actions disponibles
    def ask_stack_options(self, stack):
        raise NotImplementedError("Not implemented in interface")

# Actions sur les slaves

    # Connaître le slave à qui l'on parle ??
    def ask_slave_slaves(self, slave):
        raise NotImplementedError("Not implemented in interface")

    # Obtenir l'identité (quintuplet de configuration + adresse IP)
    def ask_slave_config(self, slave):
        raise NotImplementedError("Not implemented in interface")

    # Obtenir son taux d'utilisation CPU
    def ask_slave_cpu(self, slave):
        raise NotImplementedError("Not implemented in interface")

    # Allumer/éteindre le slave
    def ask_slave_start(self, slave):
        raise NotImplementedError("Not implemented in interface")

    def ask_slave_stop(self, slave):
        raise NotImplementedError("Not implemented in interface")

    def ask_slave_restart(self, slave):
        raise NotImplementedError("Not implemented in interface")

    # Obtenir les actions disponibles
    def ask_slave_options(self, slave):
        raise NotImplementedError("Not implemented in interface")

