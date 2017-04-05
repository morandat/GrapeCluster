from PiDevice import PiDevice
from queue import Queue, Full
from array import array

#key = bytearray([0x13, 0x00, 0x00, 0x00, 0x08, 0x00])

# instruction = {0: Slave.stop,
#                1: Slave.echo,
#                2: Slave.ls,
#                3: Slave.ps,
#                4: Slave.shutdown,
#                5: Slave.read,
#                6: Slave.error
#                }

class Slave(PiDevice):
    CLASS_ADDRESS = 0x00 # to define for each slave

    def __init__(self, i2c_add, instruction, stack_nb=None, mac_add=None, ip_address=None, pos=None):
        self.__instr=instruction
        self.CLASS_ADDRESS=i2c_add
        print (self.CLASS_ADDRESS)
        super(Slave, self).__init__(stack_nb, mac_add, ip_address, i2c_add, pos)
        print(self.CLASS_ADDRESS)


    def verif_key(self, key):
        self.write_byte(0x00, key)
        tmp = self.read_byte(0x00)
        if (tmp == 0) :
            return 0
        else :
            return -1

    def test_dede(self):
        self.write_byte(0x00)
        i = 0
        e = 0
        while (i < 4):
            try :
                if(e == 0):
                    tmp = self.read_byte(self._i2c)
                    print(tmp)
                    i = i + 1
                else:
                    tmp = self.read_byte(self._i2c)
                    print("err :")
                    print(tmp)
                    e = 0
                #break
            except Exception as e:
                e = 1
                print("plant", e)  



    def simple_test(self):
        data = [-1,-1,-1,-1]
        self.write_byte(self.__instr)
        for i in range(0, 4):
            while True :
                try:
                    tmp = self.read_byte(self._i2c)
                    #if tmp != 240 :
                    data[i] = tmp
                    #    break
                    break

                except Exception as e:
                    print("ça a planté", e)
        print(data)

    def init(self):
        # Régler la clé en fonction du slave a qui l'on parle

        if self.verif_key(self._i2c) == -1:
            print("La clé n'est pas bonne")
            return
        elif self.verif_key(self._i2c) == -2 :
            print("Problème initialisation du slave")
            return
        self.send_instruction()
        #if (tmp == -1):
        #    print("Problème instruction")
        #    return
        #print("C'est fini")

    #write a byte and wait for ack
    def write_ack(self, to_write):
        self.write_byte(self._i2c, to_write)
        if self.read_byte(self._i2c) == 1:
            self.getLastError()

    # write a byte and wait for ack
    def read_ack(self):
        try:
            self.__data.put_nowait(self.read_byte(self._i2c))
        except Full:
            self.write_byte(self._i2c, 1)
            return
        self.write_byte(self._i2c, 0)

    #send an instruction and its arguments bytes by bytes
    def send_instruction(self):
        #send integer corresponding to a command
        self.write_ack(self.__instr)

        #send a boolean corresponding to the existence of arguments
        if self.__param is not None:
            self.write_ack(1)
            for i in range(0, len(self.__param)):
                self.write_ack(self.__param[i])
        else:
            self.write_ack(0x00)

        self.receiveData()
        self.stop()

    # def stop(self):
    #     print("stop communication")
    #

    def getLastError(self):
        self.write_byte(self._i2c, 6)
        self.read_byte(self._i2c)

    def receiveData(self):
        size_to_receive = self.read_ack()
        for i in range (0, size_to_receive):
            self.read_ack()






    # def echo(self):
    #     print("is an echo")
    #     self.write_byte(0x00, 0) #numéro de l'instruction
    #     if self.read_byte(0x00) == 1:
    #         self.getLastError()
    #     self.write_byte(0x00, 1) #booléen indiquant si des paramètres arrivent
    #     if self.read_byte(0x00) == 1:
    #         self.getLastError()
    #     self.write_byte(0x00, 1)  #nombre de paquets arrivant avec les paramètres
    #     if self.read_byte(0x00) == 1:
    #         self.getLastError()
    #     self.write_byte(0x00, "Hello")  #paramètre
    #     if self.read_byte(0x00) == 1:
    #         self.getLastError()
    #
    #     self.receiveData();


    #def ls(self):
    #    print("is an ls")
    #def ps(self):
    #   print("is an ls")
    #def shutdown(self):
    #    print("is an ls")



