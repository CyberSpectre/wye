
import time
import sys
import zmq
import random
import json
import argparse

print "INIT"

# ---------------------------------------------------------------------------

outputs = {}
sockets = {}

ctxt = zmq.Context()

for arg in sys.argv[1:]:
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

print "RUNNING"
sys.stdout.flush()

sys.stderr.write("Sender is running.\n")

while True:
    time.sleep(0.01)

    msg = random.randint(0, 100)

    for s in sockets["output"]:
        s.send(json.dumps(msg))

