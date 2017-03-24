from Stack import Stack

#func_stack = {
#  'get_power': Communicator.ask_stack_power,
#  'get_temp': Communicator.ask_stack_temp,
#  'shutdown': Communicator.send_stack_shutdown,
#  'options': Communicator.ask_stack_options,
#}

#func_slave = {
#  'get_slave': Communicator.ask_slave_slaves,
#  'get_config': Communicator. ask_slave_config,
#  'get_usage': Communicator.ask_slave_cpu,
#  'start': Communicator.ask_slave_start,
#  'stop': Communicator.ask_slave_stop,
#  'restart': Communicator.ask_slave_restart,
#  'options': Communicator.ask_slave_options,
#}

"""
Actions nécessaires pour l'instant : (temp)

Les actions possibles sur une stack sont, avec les routes correspondantes :
— Obtenir sa consommation d’électricité en volts :GET stack/power/<id>
— Obtenir sa température en degrés Celsius : GET stack/temp/<id>
— Allumer ou éteindre toutes les Raspberries de la stack : POST stack/shutdown/<id>
— Obtenir les actions disponibles : OPTIONS stack

Les actions possibles sur un slave sont :
— connaitre le slave à qui l’on parle : GET slave/index
— obtenir son identité (quintuplet de configuration + adresse IP) :GET slave/config/<id>
— obtenir son taux d’utilisation CPU :GET slave/cpu/<id>
— allumer, éteindre ou redémarrer le slave: respectivement POST slave/start, stop, restart/<id>
— obtenir les actions disponibles :OPTIONS slave

"""
          
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