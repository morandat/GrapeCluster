Slave deamon 
===

Lancer le daemon
---

make i2c
sudo ./daemon_i2c

sudo : obligatoire pour l'i2c qui ouvre un device dans /dev

make udp
sudo ./daemon_udp ip_address
ip_address = l'adresse IP du slave (utiliser ipconfig)

i2cslave.c
---
Permet de lire le bus i2c et de récupérer les 4 octets de réponses

udpslave.c
---
Permet de communiquer en UDP avec le slave

commands
---
Fonction implémentant les demandes du master

daemon_udp et daemon_i2c
---
Mains respectifs de l'udp et de l'i2c
Il est nécessaire d'avoir deux éxécutables différents car l'i2c est bloquant malgré un select avec timeout. 
