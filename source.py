
import time
import sys

outputs = sys.argv[1:]

print "INIT"
print "NOTICE:%s" % outputs
print "RUNNING"
sys.stdout.flush()

while True:
    time.sleep(0.2)

