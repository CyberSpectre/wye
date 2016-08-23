
#include <process.h>
#include <boost/process.hpp>
#include <iostream>
#include <boost/uuid/uuid_io.hpp>

void process::run() {
  boost::process::context ctx;
  ctx.stdout_behavior = boost::process::capture_stream();
  ctx.stderr_behavior = boost::process::inherit_stream();

  boost::process::child c = boost::process::launch(exec, args, ctx);
  proc = std::shared_ptr<boost::process::child>(new boost::process::child(c));

}

void process::terminate() {
    std::cerr << "Terminate..." << std::endl;
    proc->terminate();
    std::cerr << "join..." << std::endl;
    proc->wait();
    std::cerr << "Stopped" << std::endl;
    proc = nullptr;
}

void process::describe(boost::property_tree::ptree& p)
{

    p.put("id", id);
    if (name != "")
	p.put("name", name);

    p.put("job_id", job_id);
    p.put("exec", exec);

    if (state == RUNNING)
	p.put("state", "RUNNING");
    else
	p.put("state", "STOPPED");
    
    if (args.size() > 0) {
	boost::property_tree::ptree as;
	for(auto i: args) {
	    boost::property_tree::ptree elt;
	    elt.put_value(i);
	    as.push_back(std::make_pair("", elt));
	}
	p.add_child("args", as);
    }

    if (inputs.size() > 0) {
	boost::property_tree::ptree ins;
	for(auto i: inputs) {
	    boost::property_tree::ptree elt;
	    elt.put_value(i);
	    ins.push_back(std::make_pair("", elt));
	}
	p.add_child("inputs", ins);
    }

    if (outputs.size() > 0) {
	boost::property_tree::ptree outs;
	for(auto i: outputs) {
	    boost::property_tree::ptree elt;
	    elt.put_value(i);
	    outs.push_back(std::make_pair("", elt));
	}
	p.add_child("outputs", outs);
    }

}

void process::check()
{

    if (state != RUNNING) return;

    if (proc == nullptr)
	throw std::runtime_error("Process null pointer!");

    try {
	boost::process::status stat = proc->wait_nohang();

	if (stat.exited()) {
	    std::cerr << "Process " << id << " has stopped." << std::endl;
	    state = STOPPED;
	    proc = nullptr;
	}
	
    } catch (...) {
	// Assume exception is that process no longer exists.
	std::cerr << "Process " << id << " has stopped." << std::endl;
	state = STOPPED;
	proc = nullptr;
    }

}

process::~process()
{
    if (proc) {
	proc->terminate();
	proc->wait();
	proc = nullptr;
	std::cerr << "Process killed" << std::endl;
    }
}

