from i2c_device import i2c_dev

instruction =  {0 : echo,
                1 : ls,
}
 
class slave(i2c_device):



	def init(self):
		self.__instr = 0x00
		#Régler la clé en fonction du slave a qui l'on parle
		self.__key = 0x01
		if(verif_key(__key) == -1):
			print "La clé n'est pas bonne"
			return
		else if (verif_key(__key) == -2):
			print "Problème initialisation du slave"
			return
		
		tmp = send_instruction(__instr)
		if(tmp == -1):
			print "Problème instruction"
			return 

		length = _dev_read(0x00)
		instruction[__instr](length)
		
		print("C'est fini")


	def echo(length):
		#Faire l'analyse des réponses 
		print "is an echo"
    
 
	def ls(length):
    	print "is an ls"
 

    def verif_key(key):
    	if(_dev_write(0x00,key) == -1)
    		return -2
    	tmp = _dev_read(0x00)
    	if(tmp == 0)
    		return 0
    	else 
    		return -1


    def send_instruction(instr):
    	return _dev_write(0x00, instr)




