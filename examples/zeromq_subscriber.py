
import time
import sys
import zmq
import random
import json
import socket
import wye.context

# ---------------------------------------------------------------------------

ctrl = os.fdopen(3, 'w')
ctrl.write("INIT\n")
sockets = wye.context.parse_outputs(sys.argv[1:])
ctxt = zmq.Context()
skt = ctxt.socket(zmq.SUB)
port = skt.connect("tcp://localhost:5555")
skt.setsockopt(zmq.SUBSCRIBE, "")
ctrl.write("RUNNING\n")
ctrl.flush()

# ---------------------------------------------------------------------------

def handle(msg):

    for s in sockets["output"]:
        s.send(msg)
    
while True:
    msg = skt.recv()
    handle(msg)

