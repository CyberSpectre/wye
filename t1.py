
import time
import sys
import zmq
import random
import json
import socket

print "INIT"

# ---------------------------------------------------------------------------

outputs = sys.argv[1:]
ctxt = zmq.Context()
sockets = []

for v in outputs:
    skt = ctxt.socket(zmq.PUSH)
    skt.connect(v)
    sockets.append(skt)

# ---------------------------------------------------------------------------

fqdn = socket.getfqdn()
ctxt = zmq.Context()
skt = ctxt.socket(zmq.PULL)
port = skt.bind_to_random_port("tcp://*")
input="tcp://%s:%d" % (fqdn, port)
print "INPUT:%s" % input

# ---------------------------------------------------------------------------

print "RUNNING"
sys.stdout.flush()

sys.stderr.write("T1 is running.\n")

def handle(msg):
    val = msg["x"] / msg["y"]

    msg = { "div": val }
    msg = json.dumps(msg)
    sys.stderr.write("T2: %s\n" % msg)

    for s in sockets:
        s.send(msg)
    

while True:
    msg = skt.recv()
    handle(json.loads(msg))

