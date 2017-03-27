
import subprocess
import sys

asd = ["sh", "-c", "./go-t2 " + " ".join(sys.argv[1:])]

subprocess.call(asd)

