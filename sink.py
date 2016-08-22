
import time
import sys
import zmq
import socket
import json

fqdn = socket.getfqdn()
ctxt = zmq.Context()
skt = ctxt.socket(zmq.PULL)
port = skt.bind_to_random_port("tcp://*")

input="tcp://%s:%d" % (fqdn, port)

print "INIT"
print "INPUT:%s" % input
print "RUNNING"
sys.stdout.flush()
sys.stderr.write("Done init.\n")

def handle(msg):
    sys.stderr.write("Sink: %s" % json.dumps(msg) + "\n")

while True:
    msg = skt.recv()
    handle(json.loads(msg))

