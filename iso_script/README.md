# Script de génération d'image

## Généralités

Ce script se divise en trois parties. Le `armmanager` qui se charge de monter, expand et chroot dans une image (de cross-compiler un
kernel également). Le `rasparchitect` qui est lancé à l'intérieur d'une image Raspberry, et qui se charge de l'installation de paquets, de
l'éventuelle compilation et installation du kernel. Enfin, le `bcm_slave_mod_install` qui permet une fois le Raspberry lancé de
faire une étape de post-installation en compilant les bons modules et les plaçant à charger au démarrage.

Pour les deux scripts armmanager et rasparchitect vous pouvez utiliser l'option -h ou --help qui vous donnera toutes les précisions
nécessaires sur les options disponibles.

### Etapes a suivre

Le cas le plus simple qui pourrait se présenter serait : prendre une image Raspbian, recompiler son kernel et installer convenablement
l'i2c dessus. Voici la démarche à suivre.

Placez vous dans le répertoire `iso_script/` du dépôt Git et lancez :
```
    ./armmanager -m -r -c /chemin/vers/raspbian.img
```
Vous pouvez maintenant vaquer à d'autres occupations, la compilation du kernel prend un temps assez considérable (entre 30 minutes
et 1 à 2 heures suivant les machines).
Une fois ceci fait vous serez en bash en chroot sur l'image raspbian, avec tout d'installé. Si vous le souhaitez vous
pouvez continuer de réaliser des actions dans le système, sinon quittez simplement avec `exit`.

Maintenant il faut flasher l'image sur une carte SD. Assurez vous avec `lsblk` que la carte SD n'est pas montée, et sinon démontez la
avec `umount /dev/sdX` où `sdX` correspond à votre carte SD. (sda, sdb, sdc ...)
```
    sudo dd if=/chemin/vers/raspbian.img of=/dev/sdX bs=4M status=progress
```
__/!\ Vérifiez bien la destination du `of` !! /!\__ Sinon vous risquez de griller des choses importantes sur votre ordinateur (risques de `dd` ...).

Une fois ceci fait, pluggez la carte SD dans une Raspberry, branchez la à un réseau ethernet et mettez le courant.
Vous aurez "normalement" l'accès en SSH vers la raspberry. Vous pouvez scanner le réseau avec des outils comme `nmap` ou `arp-scan`
pour savoir quelle est son adresse. Faites donc un ssh vers la cible : `ssh pi@xxx.xxx.xxx.xxx` où x est votre adresse ip. Connectez vous
avec le mot de passe `raspberry`. Allez ensuite dans votre `/home/pi` et dans le répertoire `armmanager`.
Vérifiez que le kernel chargé n'est plus le kernel par défaut (c'est souvent le dernier sorti, à la date de ce README la version
en question est la 4.9.20) via `uname -r`. Si ce n'est pas le cas, c'est que le raspberry n'a pas chargé le bon kernel. Soit il 
vous faudra modifier le `/boot/config.txt` pour sélectionner le kernel, soit à nouveau recompiler en lançant dans ce même répertoire
`armmanager` le script `rasparchitect` sans arguments (ou avec, c'est vous qui voyez : c'est ce script qui réalise la compilation), en sudo.
Enfin, lorsque vous êtes sûrs d'avoir le bon kernel, lancez :
```
    sudo ./bcm_slave_mod_install [numéro de la stack] [emplacement de la raspberry sur la stack de 1 à 6]
```
en connaissant le numéro de la stack sur laquelle est la raspberry, et son emplacement de 1 à 6 de cette dernière.
Le script vous demandera si oui ou non vous souhaitez installer les modules kernel. Cela fait partie d'un cas d'utilisation spécial
et encore non testé convenablement où `armmanager` est lancé avec l'option `-k` (cross-compilation plutôt que compilation depuis Qemu). Par défaut
répondez "no" à cette question, sauf si vous avez lancé la compilation du kernel via le tout premier script.

Une fois ceci fait vous pouvez finalement `sudo reboot`.

Vous devriez maintenant pouvoir voir un device du nom de `i2c_slave` dans /dev. En lançant la commande `daemon` en sudo, vous devriez
également pouvoir maintenant lancer le daemon i2c slave en lui donnant l'adresse IP adéquate. 


Vous pourrez donc par la suite manipuler manuellement les deux différents daemons puisque placés dans le /usr/local/bin.
Ils sont sous les noms de daemon_i2c et daemon_udp, et vont chercher leurs informations dans le /etc/daemon.d/