
import json
import requests
import sys

url = "http://localhost:8080"

class Sink:

    def __init__(self):
        pass

    def run(self):
        req = {
            "operation":"create-worker",
            "model":"python",
            "file":"sink.py",
            "job_id":"asd"
        }

        res = requests.put(url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        res = res.json()
        self.inputs = res["inputs"]
        print self.inputs

    def get_inputs(self):
        return self.inputs

class T1:

    def __init__(self, outs):
        self.outputs = outs

    def run(self):

        for v in self.outputs:
            v.run()

        req = {
            "operation":"create-worker",
            "model":"python",
            "file":"t1.py",
            "job_id":"asd",
        }

        o = []
        for v in self.outputs:
            print v
            print v.get_inputs()
            for v2 in v.get_inputs():
                o.append(v2)
        req["outputs"] = o

        res = requests.put(url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        res = res.json()
        self.inputs = res["inputs"]

    def get_inputs(self):
        return self.inputs

class T2:

    def __init__(self, outs):
        self.outputs = outs

    def run(self):

        for v in self.outputs:
            v.run()

        req = {
            "operation":"create-worker",
            "model":"python",
            "file":"t2.py",
            "job_id":"asd",
        }

        o = []
        for v in self.outputs:
            for v2 in v.get_inputs():
                o.append(v2)
        req["outputs"] = o

        res = requests.put(url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        res = res.json()
        self.inputs = res["inputs"]

    def get_inputs(self):
        return self.inputs

class Source:

    def __init__(self, outs):
        self.outputs = outs

    def run(self):

        for v in self.outputs:
            v.run()

        req = {
            "operation":"create-worker",
            "model":"python",
            "file":"source.py",
            "job_id":"asd",
        }

        o = []
        for v in self.outputs:
            for v2 in v.get_inputs():
                o.append(v2)
        req["outputs"] = o

        res = requests.put(url, data=json.dumps(req))
        if res.status_code != 200:
            raise RuntimeError, "Bad request"
        res = res.json()

s1 = Sink()

s2 = Sink()

t1 = T1([s1])

t2 = T2([s2])

src = Source([t1, t2])

src.run()
