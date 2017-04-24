
"""
wye - streaming support.
"""

__author__ = 'Cyber MacGeddon'
__license__ = 'GPLv3'

import json
import requests
import sys
import os
import marshal
import base64
import zmq

class Worker:

    def __init__(self, job, name, model, params, outputs=None, parallelism=1):
        self.name = name
        self.model = model
        self.params = params
        self.job = job
        if outputs == None:
            self.outputs = {}
        else:
            self.outputs = outputs
        self.parallelism = parallelism

    def connect(self, output_name, outputs):
        self.outputs[output_name] = outputs

    def implement(self):

        self.inputs = {}
        self.ids = []

        for instance in range(0, self.parallelism):

            req = {
                "operation": "create-worker",
                "name": "%s-%04d" % (self.name, instance),
                "model": self.model,
                "job_id": self.job.id,
            }

            for v in self.params:
                req[v] = self.params[v]

            if len(self.outputs) > 0:

                req["outputs"] = {}

                for v in self.outputs:
                    o_name = v

                    wals = []

                    for w in self.outputs[v]:

                        elt = w[0]
                        name = w[1]

                        was = []
                        ins = elt.get_inputs()

                        for x in ins:
                            was.append(ins[x][name])

                        wals.append(was)

                    req["outputs"][o_name] = wals

            res = requests.put(self.job.context.url, data=json.dumps(req))
            if res.status_code != 200:
                raise RuntimeError, res.reason

            res = res.json()

            id = res["id"]
            self.ids.append(id)

            if res.has_key("inputs"):
                self.inputs[id] = res["inputs"]

            sys.stderr.write("Worker %s (%s) added.\n" % (req["name"], id))

        return self.ids

    def get_inputs(self):
        return self.inputs

class Job:

    def __init__(self, context, name, description):
        self.context = context
        self.name = name
        self.description = description
        self.workers = []

    def define_executable_worker(self, name, exe, outputs=None, parallelism=1):
        return self.define_worker(name, "executable", {"exe":exe}, outputs,
                                  parallelism)

    def define_python_worker(self, name, file, outputs=None, parallelism=1):
        return self.define_worker(name, "python", {"file":file}, outputs,
                                  parallelism)

    def define_lambda_worker(self, name, func, outputs=None, parallelism=1):
        func = base64.b64encode(marshal.dumps(func.func_code))
        return self.define_worker(name, "lambda",
                                  {"lambda":func, "call": "run_lambda"},
                                  outputs, parallelism)

    def define_lua_worker(self, name, file, outputs=None, parallelism=1):
        return self.define_worker(name, "lua", {"file":file}, outputs,
                                  parallelism)

    def define_generator_worker(self, name, func, outputs=None, parallelism=1):
        func = base64.b64encode(marshal.dumps(func.func_code))
        return self.define_worker(name, "lambda",
                                  {"lambda":func, "call": "run_generator"},
                                  outputs, parallelism)

    def define_worker(self, name, model, params, outputs=None, parallelism=1):
        worker = Worker(self, name, model, params, outputs, parallelism)
        self.workers.append(worker)
        return worker

    def implement(self):

        req = {
            "operation": "create-job",
            "name": self.name,
            "description": self.description
        }

        res = requests.put(self.context.url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        res = res.json()
        self.id = res["id"]

        # The API forces streams to be created back-to-front, so the
        # self.workers sequence is in order to be created.
        for v in self.workers:
            v.implement()

        return self.id
        
class Context:

    def __init__(self, url="http://localhost:8080"):
        self.url = url

    def define_job(self, name, description):
        return Job(self, name, description)

    def get_workers(self):

        req = {
            "operation": "get-workers"
        }

        res = requests.put(self.url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        res = res.json()

        return res

    def get_jobs(self):

        req = {
            "operation": "get-jobs"
        }

        res = requests.put(self.url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        res = res.json()

        return res

    def delete_job(self, id):

        req = {
            "operation": "delete-job",
            "id": id
        }

        res = requests.put(self.url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"

    def delete_worker(self, id):

        req = {
            "operation": "delete-worker",
            "id": id
        }

        res = requests.put(self.url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        
    def get_job(self, id):

        req = {
            "operation": "get-job",
            "id": id
        }

        res = requests.put(self.url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"

        return res.json()
        
    def get_worker(self, id):

        req = {
            "operation": "get-workers",
            "id": id
        }

        res = requests.put(self.url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        
        return res.json()[id]

def parse_outputs(args):    

    sockets = {}

    ctxt = zmq.Context()

    for arg in args:
        name = arg.split(":", 1)[0]
        outs = arg.split(":", 1)[1].split(",")

        if not sockets.has_key(name):
            sockets[name] = []

        skt = ctxt.socket(zmq.PUSH)
        for v in outs:
            skt.connect(v)
        sockets[name].append(skt)

    return sockets

