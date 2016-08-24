
import time
import sys
import zmq
import random
import json
import argparse
import wye

print "INIT"

# ---------------------------------------------------------------------------

sockets = wye.parse_outputs(sys.argv[1:])

# ---------------------------------------------------------------------------

print "RUNNING"
sys.stdout.flush()

sys.stderr.write("Sender is running.\n")

while True:
    time.sleep(1)

    msg = { "x": random.randint(0, 10) + 1, "y": random.randint(0, 10) + 1 }
    for s in sockets["output"]:
        sys.stderr.write("Source: %s\n" % json.dumps(msg))
        s.send(json.dumps(msg))

