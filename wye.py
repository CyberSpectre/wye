
import json
import requests
import sys

import wye

class Worker:

    def __init__(self, job, name, model, file, outputs=[]):
        self.name = name
        self.model = model
        self.file = file
        self.job = job
        self.outputs = outputs

    def implement(self):

        req = {
            "operation": "create-worker",
            "name": self.name,
            "model": self.model,
            "file": self.file,
            "job_id": self.job.id,
        }

        out = []
        for v in self.outputs:
            for v2 in v.get_inputs():
                out.append(v2)
        req["outputs"] = out

        res = requests.put(self.job.context.url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        res = res.json()
        if res.has_key("inputs"):
            self.inputs = res["inputs"]
        else:
            self.inputs = []

        sys.stderr.write("Worker %s (%s) added.\n" % (req["name"], res["id"]))

        self.id = res["id"]

        return self.id

    def get_inputs(self):
        return self.inputs

class Job:

    def __init__(self, context, name, description):
        self.context = context
        self.name = name
        self.description = description
        self.workers = []

    def define_python_worker(self, name, file, outputs=[]):
        return self.define_worker(name, "python", file, outputs)

#    def define_python_lambda_worker(self, name, lambda, outputs=[]):
#        command = ["python", "-c", 

    def define_worker(self, name, model, file, outputs=[]):
        worker = Worker(self, name, model, file, outputs)
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
        
class StreamingContext:

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

    
