import subprocess
import os

class imageManipulator:
    def __init__(self, path):
        self.isopath = os.path.abspath(path)

    def command(self, command):
        result = ""

        try:
            result = subprocess.check_output(command)
        except OSError as e:
            if e.errno == os.errno.ENOENT:
                print("No such program")
            else:
                raise

        return result

    def open(self):
        try:
            self.command(["mkdir", "-p", "/temp/"])
            return self.command(["sudo", "kpartx", "-a", self.isopath])
        except OSError as e:
            print("problem")

    def close(self):
        try:
            return self.command(["sudo", "kpartx", "-d", self.isopath])
        except OSError as e:
            print("problem")


