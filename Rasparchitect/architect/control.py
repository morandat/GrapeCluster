from GrapeCluster.Rasparchitect.architect.core import imageManipulator

x = imageManipulator("../../../Pidora-2014-R3.img")

returned = x.command("pwd");
print(returned)

x.open()
x.close()