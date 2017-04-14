Master
===

i2c
---

Au niveau de l'interface pour le moment on passe par l'UDP néamoins on peut envoyer une instruction

~~~
cd master
pyhton3
~~~

```python
from Slave import *
from CommunicatorI2C import *
sl = Slave(@_ip)
com = CommunicatorI2C()
```

Envoyer une des intructions suivante:

```python
com.send_custom_instruction(sl, 0x00, 'test')
com.send_custom_instruction(sl, 0x01, 'cpu')
com.send_custom_instruction(sl, 0x02, 'shutdown')
com.send_custom_instruction(sl, 0x03, 'reboot')
com.send_custom_instruction(sl, 0x04, 'get_ip')
com.send_custom_instruction(sl, 0X05, 'get_i2c')
com.send_custom_instruction(sl, 0x07, 'is_network')
```

A la fin on voit un tableau de données s'afficher

