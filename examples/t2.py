
import time
import sys
import zmq
import random
import json
import socket
import wye
import os

# ---------------------------------------------------------------------------

ctrl = os.fdopen(3, 'w')
ctrl.write("INIT\n")
sockets = wye.parse_outputs(sys.argv[1:])
fqdn = socket.getfqdn()
ctxt = zmq.Context()
skt = ctxt.socket(zmq.PULL)
port = skt.bind_to_random_port("tcp://*")
input="tcp://%s:%d" % (fqdn, port)
ctrl.write("INPUT:input:%s\n" % input)
ctrl.write("RUNNING\n")
ctrl.flush()
sys.stderr.write("T2 is running.\n")

# ---------------------------------------------------------------------------

def handle(msg):
    val = msg["x"] * msg["y"]

    msg = { "mul": val }
    msg = json.dumps(msg)
    sys.stderr.write("T2: %s\n" % msg)

    for s in sockets["output"]:
        s.send(msg)
    

while True:
    msg = skt.recv()
    handle(json.loads(msg))

