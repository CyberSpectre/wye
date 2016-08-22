
import time
import sys
import zmq

print "INIT"

outputs = sys.argv[1:]

if len(outputs) != 1:
    print "ERROR:Should be exactly one output."
    sys.stdout.flush()
    time.sleep(10)
    sys.exit(1)

ctxt = zmq.Context()
skt = ctxt.socket(zmq.PUSH)
skt.connect(outputs[0])

print "RUNNING"
sys.stdout.flush()

sys.stderr.write("Sender is running.\n")

while True:
    time.sleep(1)
    sys.stderr.write("Send...\n")
    skt.send('{ "one": "two"}')

