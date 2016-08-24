
import time
import sys
import zmq
import random
import json
import socket
import marshal
import types
import base64

print "INIT"

code = marshal.loads(base64.b64decode(sys.argv[1]))
func = types.FunctionType(code, globals())

# ---------------------------------------------------------------------------

outputs = {}
sockets = {}

ctxt = zmq.Context()

for arg in sys.argv[2:]:
    name = arg.split(":", 1)[0]
    outs = arg.split(":", 1)[1].split(",")

    if not outputs.has_key(name):
        outputs[name] = []

    outputs[name].append(outs)

    if not sockets.has_key(name):
        sockets[name] = []

    skt = ctxt.socket(zmq.PUSH)
    for v in outs:
        skt.connect(v)
    sockets[name].append(skt)

# ---------------------------------------------------------------------------

fqdn = socket.getfqdn()
ctxt = zmq.Context()
skt = ctxt.socket(zmq.PULL)
port = skt.bind_to_random_port("tcp://*")
input="tcp://%s:%d" % (fqdn, port)
print "INPUT:input:%s" % input

# ---------------------------------------------------------------------------

print "RUNNING"
sys.stdout.flush()

def handle(msg):

    msg = func(msg)
    if msg != None:

        msg = json.dumps(msg)
        for s in sockets["output"]:
            s.send(msg)

while True:
    msg = skt.recv()
    handle(json.loads(msg))

