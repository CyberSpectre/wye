
import time
import sys
import zmq
import random
import json
import argparse
import wye
import os

# ---------------------------------------------------------------------------

ctrl = os.fdopen(3, 'w')

ctrl.write("INIT\n")
sockets = wye.parse_outputs(sys.argv[1:])
ctrl.write("RUNNING\n")
ctrl.flush()
sys.stderr.write("Sender is running.\n")

# ---------------------------------------------------------------------------

while True:
    time.sleep(1)

    msg = { "x": random.randint(0, 10) + 1, "y": random.randint(0, 10) + 1 }
    sys.stderr.write("Source: %s\n" % json.dumps(msg))
    for s in sockets["output"]:
        s.send(json.dumps(msg))

