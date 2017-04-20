
import subprocess
import sys

asd = ["sh", "-c", "./go-source " + " ".join(sys.argv[1:])]

subprocess.call(asd)

