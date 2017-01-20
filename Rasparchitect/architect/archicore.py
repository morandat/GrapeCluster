import subprocess
import os


def command(cmd):
    result = ""

    try:
        result = subprocess.check_output(cmd)
    except OSError as e:
        if e.errno == os.errno.ENOENT:
            print("No such program")
        else:
            raise

    return result


class ImageManipulator:
    def __init__(self, path):
        self.isopath = path
        self.filename = os.path.basename(path)

    def open(self):
        try:
            #Create directory at /tmp if not existing
            command(["mkdir", "-p", "/tmp/"+str(self.filename.split(".")[0])])
            fdisk = command(["fdisk", "-l", self.isopath])
            iter = len(fdisk)
            while iter != 0:
                if(fdisk[iter].find("Device")):
                    break
                iter -= 1

            iter += 2
            imgPart = fdisk[iter].split()

            return fdisk
        except OSError as e:
            print(e)

    def close(self):
        try:
            return command(["sudo", "kpartx", "-d", self.isopath])
        except OSError as e:
            print(e)
