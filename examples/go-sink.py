
import subprocess
import sys

asd = ["sh", "-c", "./go-sink " + " ".join(sys.argv[1:])]

subprocess.call(asd)

