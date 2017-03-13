from PiDevice import PiDevice

class Slave(PiDevice):
    CLASS_ADDRESS = 0x42  # 42 for slaves
    def __init__(self, stack_nb, mac_add, ip_address, i2c_add, pos):
        super(Slave, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)
        self.init(self)

    def verif_key(self, key):
        self.write_byte(0x00, key)
        tmp = self.read_byte(0x00)
        if (tmp == 0) :
            return 0
        else :
            return -1

    def init(self):
        self.__instr = 0
        # Régler la clé en fonction du slave a qui l'on parle
        self.__key = 0x01  # valeur par défaut
        if self.verif_key(self.__key) == -1:
            print("Wrong key")
        if self.verif_key(self.__key) == -1:
            print("La clé n'est pas bonne")
            return
        elif self.verif_key(self.__key) == -2 :
            print("Problème initialisation du slave")
            return
        tmp=instruction[self.__instr]()
        if (tmp == -1):
            print("Problème instruction")
            return

       # length = self.read_byte(0x00)
        #instruction[self.__instr](length)

        print("C'est fini")


   #  def send_instruction(self,instr):
   #     instruction[self.]
   #     self.write_byte(0x00, instr)


    def echo(self):
        print("is an echo")
        self.write_byte(0x00, 0) #numéro de l'instruction
        if self.read_byte(0x00) == 1:
            self.getLastError()
        self.write_byte(0x00, 1) #booléen indiquant si des paramètres arrivent
        if self.read_byte(0x00) == 1:
            self.getLastError()
        self.write_byte(0x00, 1)  #nombre de paquets arrivant avec les paramètres
        if self.read_byte(0x00) == 1:
            self.getLastError()
        self.write_byte(0x00, "Hello")  #paramètre
        if self.read_byte(0x00) == 1:
            self.getLastError()

        self.receiveData();

    def receiveData(self):
        print("receive from  slave")
    def ls(self):
        print("is an ls")
    def ps(self):
        print("is an ls")
    def shutdown(self):
        print("is an ls")
    def stop(self):
        print("stop communication")
    def getLastError(self):
        self.write_byte(0x00, 6)


instruction = {0: Slave.stop,
               1: Slave.echo,
               2: Slave.ls,
               3: Slave.ps,
               4: Slave.shutdown,
               5: Slave.read,
               6: Slave.error
               }