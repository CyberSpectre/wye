
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
    time.sleep(0.01)

    msg = random.randint(0, 100)

    for s in sockets["output"]:
        s.send(json.dumps(msg))

