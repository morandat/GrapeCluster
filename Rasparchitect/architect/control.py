from GrapeCluster.Rasparchitect.architect import archicore

x = archicore.ImageManipulator("../../../Pidora-2014-R3.img")

returned = x.open()
val = returned.splitlines()
end = val[8].split()
print(returned)
