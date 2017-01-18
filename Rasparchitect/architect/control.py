from Rasparchitect.architect import archicore

x = archicore.ImageManipulator("../../../raspbian-jessie.img")

returned = x.open()

print(archicore.command(["ls", "/tmp"]))
