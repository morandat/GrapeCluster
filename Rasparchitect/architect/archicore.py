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
            command(["mkdir", "-p", "/tmp/"+self.filename.])
            print(self.isopath)
            return command(["fdisk", "-l", self.isopath])
        except OSError as e:
            print(e)

    def close(self):
        try:
            return command(["sudo", "kpartx", "-d", self.isopath])
        except OSError as e:
            print(e)
