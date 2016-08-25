
import time, sys
import json
import wye.context

print "INIT"

# ---------------------------------------------------------------------------

sockets = wye.context.parse_outputs(sys.argv[1:])

# ---------------------------------------------------------------------------

print "RUNNING"
sys.stdout.flush()

sys.stderr.write("Sender is running.\n")

for i in range(0, 100):

    time.sleep(0.05)

    msg = i

    for s in sockets["output"]:
        s.send(json.dumps(msg))

