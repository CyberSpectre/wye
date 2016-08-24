
import time
import sys
import zmq
import random
import json
import socket
import marshal
import types
import base64
import wye

print "INIT"

code = marshal.loads(base64.b64decode(sys.argv[1]))
func = types.FunctionType(code, globals())

# ---------------------------------------------------------------------------

sockets = wye.parse_outputs(sys.argv[2:])

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

