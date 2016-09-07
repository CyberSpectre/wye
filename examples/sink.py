
import time
import sys
import zmq
import socket
import json
import os

fqdn = socket.getfqdn()
ctxt = zmq.Context()
skt = ctxt.socket(zmq.PULL)
port = skt.bind_to_random_port("tcp://*")

input="tcp://%s:%d" % (fqdn, port)

ctrl = os.fdopen(3, 'w')
ctrl.write("INIT\n")
ctrl.write("INPUT:input:%s\n" % input)
ctrl.write("RUNNING\n")
ctrl.flush()

def handle(msg):
    sys.stderr.write("Sink: %s" % json.dumps(msg) + "\n")

while True:
    msg = skt.recv()
    handle(json.loads(msg))

