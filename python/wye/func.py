
import time
import sys
import zmq
import json
import socket
import marshal
import types
import base64
import wye

def handle(msg):

    msg = func(msg)
    if msg != None:

        msg = json.dumps(msg)
        for s in sockets["output"]:
            s.send(msg)

def run_lambda(lam, outputs):

    global func
    global sockets

    # -----------------------------------------------------------------------

    sys.stdout.write("INIT\n")

    lam = marshal.loads(base64.b64decode(lam))
    func = types.FunctionType(lam, globals())

    sockets = wye.parse_outputs(outputs)

    # -----------------------------------------------------------------------

    fqdn = socket.getfqdn()
    ctxt = zmq.Context()
    skt = ctxt.socket(zmq.PULL)
    port = skt.bind_to_random_port("tcp://*")
    input="tcp://%s:%d" % (fqdn, port)
    sys.stdout.write("INPUT:input:%s\n" % input)

    sys.stdout.write("RUNNING\n")
    sys.stdout.flush()

    # -----------------------------------------------------------------------

    while True:
        msg = skt.recv()
        handle(json.loads(msg))


def run_generator(lam, outputs):

    global func
    global sockets

    # -----------------------------------------------------------------------

    sys.stdout.write("INIT\n")

    lam = marshal.loads(base64.b64decode(lam))
    func = types.FunctionType(lam, globals())

    sockets = wye.parse_outputs(outputs)

    sys.stdout.write("RUNNING\n")
    sys.stdout.flush()

    # -----------------------------------------------------------------------

    gen = func()
    
    while True:

        msg = next(gen)
        msg = json.dumps(msg)
        sys.stderr.flush()
        for s in sockets["output"]:
            s.send(msg)

