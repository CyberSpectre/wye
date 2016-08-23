
#include <manager.h>
#include <process.h>
#include <boost/process.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <mutex>

bool manager::py_initialised = false;

boost::property_tree::ptree
manager::create_python_worker(const boost::property_tree::ptree& p)
{

    std::string file;
    std::string job_id;
    std::string name;
    std::vector<std::string> outputs;
    try {
	file = p.get<std::string>("file");
	job_id = p.get<std::string>("job_id");
    } catch (std::exception& e) {
	return error(INVALID_REQUEST, e.what());
    }

    try {
	name = p.get<std::string>("name");
    } catch (std::exception& e) {
    }

    try {
	for(auto i: p.get_child("outputs")) {
	    outputs.push_back(i.second.get_value<std::string>());
	}
    } catch (...) {
    }

    std::shared_ptr<process> proc =
	std::shared_ptr<process>(new process());

    proc->name = name;
    proc->job_id = job_id;
    proc->exec = "/usr/bin/python";
    proc->args = { {"python"}, {file} };
    proc->outputs = outputs;


    for(auto i: outputs) {
	proc->args.push_back(i);
    }
    
    std::cerr << "Run..." << std::endl;
    proc->run();

    boost::process::pistream& out = proc->get_output();

    std::string line;
    
    while(std::getline(out, line)) {


	std::cerr << line << std::endl;

	if (line == "INIT") {
	    continue;
	}

	if (line.substr(0, 6) == "INPUT:") {
	    proc->inputs.push_back(line.substr(6));
	    std::cout << "Input " << line.substr(6) << std::endl;
	    continue;
	}

	if (line.substr(0, 6) == "ERROR:") {

	    return error(PROC_INIT_FAIL,
			 "Process failed to start: " + line.substr(6));
	}

	if (line.substr(0, 7) == "NOTICE:") {
	    std::cerr << "Notice: " << line.substr(7) << std::endl;
	    continue;
	}

	if (line == "RUNNING") {
	    std::cerr << "Process started successfully." << std::endl;
	    break;
	}

	// This 'waits' the process.
	return error(PROC_INIT_FAIL, "Bad return string: " + line);

    }

    if (out.bad()) {
	// Process is shared ptr, will go out of scope and be terminated.
	return error(PROC_INIT_FAIL, "Process didn't send RUNNING");
    }

    if (out.eof()) {
	// Process is shared ptr, will go out of scope and be terminated.
	return error(PROC_INIT_FAIL, "Process didn't send RUNNING");
    }

    std::lock_guard<std::mutex> guard(workers_mutex);

    // This keeps a reference to process.
    workers[proc->id] = proc;

    boost::property_tree::ptree r;
    r.put("id", proc->id);

    if (proc->inputs.size() > 0) {
	boost::property_tree::ptree ins;

	for(auto i: proc->inputs) {
	    boost::property_tree::ptree elt;
	    elt.put_value(i);
	    ins.push_back(std::make_pair("", elt));
	}

	r.add_child("inputs", ins);
	
    }

    return r;

}

