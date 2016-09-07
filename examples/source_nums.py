
import time, sys
import json
import wye
import os

# ---------------------------------------------------------------------------

ctrl = os.fdopen(3, 'w')
ctrl.write("INIT\n")
sockets = wye.parse_outputs(sys.argv[1:])
ctrl.write("RUNNING\n")
ctrl.flush()

# ---------------------------------------------------------------------------

for i in range(0, 10000000):

    time.sleep(0.3)

    msg = i

    for s in sockets["output"]:
        s.send(json.dumps(msg))

