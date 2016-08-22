
import time
import sys
import zmq
import random
import json

print "INIT"

outputs = sys.argv[1:]

ctxt = zmq.Context()
sockets = []

for v in outputs:
    skt = ctxt.socket(zmq.PUSH)
    skt.connect(v)
    sockets.append(skt)

print "RUNNING"
sys.stdout.flush()

sys.stderr.write("Sender is running.\n")

while True:
    time.sleep(1)

    msg = { "x": random.randint(0, 10) + 1, "y": random.randint(0, 10) + 1 }
    for s in sockets:
        sys.stderr.write("Source: %s\n" % json.dumps(msg))
        s.send(json.dumps(msg))

