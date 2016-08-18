
import time
import sys
import zmq
import random
import json
import socket
import wye

# ---------------------------------------------------------------------------

print "INIT"
sockets = wye.parse_outputs(sys.argv[1:])
fqdn = socket.getfqdn()
ctxt = zmq.Context()
skt = ctxt.socket(zmq.PULL)
port = skt.bind_to_random_port("tcp://*")
input="tcp://%s:%d" % (fqdn, port)
print "INPUT:input:%s" % input
print "RUNNING"
sys.stdout.flush()
sys.stderr.write("T1 is running.\n")

# ---------------------------------------------------------------------------

def handle(msg):
    val = msg["x"] / msg["y"]

    msg = { "div": val }
    msg = json.dumps(msg)
    sys.stderr.write("T1: %s\n" % msg)

    for s in sockets["output"]:
        s.send(msg)
    

while True:
    msg = skt.recv()
    handle(json.loads(msg))

