Slave deamon 
===

Lancer le démon
---

make 
sudo ./deamon @ip_slave

@ip_slave : adresse ip du slave
sudo : obligatoire pour l'i2c

i2cslave.c
---
Permet de lire le bus i2c et de récupérer les 4 octets de réponses

udpslave
---
Permet de communiquer en UDP avec le slave

commands
---
Fonction implémentant les demandes du master

daemon
---
Permet de faire le lien entre UDP et i2c
