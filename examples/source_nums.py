
import time, sys
import json
import wye

# ---------------------------------------------------------------------------

print "INIT"
sockets = wye.parse_outputs(sys.argv[1:])
print "RUNNING"
sys.stdout.flush()

# ---------------------------------------------------------------------------

for i in range(0, 10000000):

    time.sleep(0.3)

    msg = i

    for s in sockets["output"]:
        s.send(json.dumps(msg))

