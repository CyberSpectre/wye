#!/usr/bin/env python

import sys
import wye

url = "http://localhost:8080"

context = wye.StreamingContext(url)

job = context.define_job(name="job1", description="A test job")

def sum_pi(x):
    if not "parts" in globals():
        globals()["parts"] = {}
    if not "last" in globals():
        globals()["last"] = 0
    global last
    global parts
    
    last = last + 1
    parts[(x[0],x[1])] = x[2]
    pi = 0
    for v in parts:
        pi = pi + parts[v]

    if last % 100 == 0:
        sys.stderr.write("PI is %-2.10f\n" % pi)
        
    return None

sink = job.define_lambda_worker("sink", sum_pi)

power = job.define_lambda_worker("power",
                                 lambda x: (x[0], x[1], x[2] * 16 ** (-x[0])),
                                 parallelism=1,
                                 outputs={
                                     "output": [(sink,"input")]
                                 })

f1 = job.define_lambda_worker("frac1",
                              lambda x: (x, 1, 4.0 / (8.0 * x + 1)),
                              parallelism=1,
                              outputs={
                                  "output": [(power,"input")]
                              })

f2 = job.define_lambda_worker("frac2",
                              lambda x: (x, 2, - (2.0 / (8.0 * x + 4.0))),
                              parallelism=1,
                              outputs={
                                  "output": [(power,"input")]
                              })

f3 = job.define_lambda_worker("frac3",
                              lambda x: (x, 3, - (1.0 / (8.0 * x + 5.0))),
                              parallelism=1,
                              outputs={
                                  "output": [(power,"input")]
                              })

f4 = job.define_lambda_worker("frac4",
                              lambda x: (x, 4, - (1.0 / (8.0 * x + 6.0))),
                              parallelism=1,
                              outputs={
                                  "output": [(power,"input")]
                              })

src = job.define_python_worker("source", "source_nums.py",
                               {
                                   "output": [
                                       (f1,"input"),
                                       (f2,"input"),
                                       (f3,"input"),
                                       (f4,"input")
                                   ]
                               })

job_id = job.implement()

sys.stderr.write("Job %s is running.\n" % job_id)

