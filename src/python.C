
#include <manager.h>
#include <process.h>
#include <worker.h>
#include <boost/process.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <mutex>

boost::property_tree::ptree
manager::run_python(std::shared_ptr<process> proc,
		    const boost::property_tree::ptree& p)
{

    std::cerr << "Run..." << std::endl;
    proc->run();

    boost::process::pistream& out = proc->get_output();

    std::string line;
    
    while(std::getline(out, line)) {

	std::cerr << "Line: " << line << std::endl;

	if (line == "INIT") {
	    continue;
	}

	if (line.substr(0, 6) == "INPUT:") {

	    line = line.substr(6);

	    int pos = line.find(":");
	    if (pos == -1)
		return error(PROC_INIT_FAIL,
			     "Did not interpret process output INPUT:" + line);

	    std::string name = line.substr(0, pos);
	    std::string input = line.substr(pos + 1);

	    proc->inputs[name] = input;
		
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

	std::cerr << "Didn't understand: " << line << std::endl;

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
	    ins.put(i.first, i.second);
	}

	r.add_child("inputs", ins);
	
    }

    return r;

}

boost::property_tree::ptree
manager::create_python_worker(const boost::property_tree::ptree& p)
{

    std::string file;

    try {
	file = p.get<std::string>("file");
    } catch (std::exception& e) {
	return error(INVALID_REQUEST, e.what());
    }

    std::shared_ptr<process> proc =
	std::shared_ptr<process>(new process());

    proc->exec = "/usr/bin/python";
    proc->args = { {"python"}, {file} };

    try {
	proc->job_id = p.get<std::string>("job_id");
    } catch (std::exception& e) {
	return error(INVALID_REQUEST, e.what());
    }

    try {
      proc->name = p.get<std::string>("name");
    } catch (std::exception& e) {
    }
        
    try {
	for(auto i: p.get_child("outputs")) {

	    std::string name = i.first;

	    boost::property_tree::ptree& wal = i.second;

	    worker::address_list al;

	    for(auto j: wal) {

		worker::address a;

		for(auto k: j.second) {
		    a.push_back(k.second.get_value<std::string>());
		}

		al.push_back(a);

	    }

	    proc->outputs[name] = al;

	}
    } catch (...) {
    }

    for(auto i: proc->outputs) {

	std::string name = i.first;

	for(auto j: i.second) {

	    std::string outs = name + ":";
	    std::string sep = "";

	    for(auto k: j) {
		outs.append(sep);
		outs.append(k);
		sep = ",";
	    }

	    proc->args.push_back(outs);

	}

    }

    return run_python(proc, p);

}

boost::property_tree::ptree
manager::create_lambda_worker(const boost::property_tree::ptree& p)
{

    std::string file = "lambda.py";
    std::string lambda;

    std::shared_ptr<process> proc =
	std::shared_ptr<process>(new process());

    try {
	lambda = p.get<std::string>("lambda");
    } catch (std::exception& e) {
	return error(INVALID_REQUEST, e.what());
    }

    try {
	proc->job_id = p.get<std::string>("job_id");
    } catch (std::exception& e) {
	return error(INVALID_REQUEST, e.what());
    }

    try {
      proc->name = p.get<std::string>("name");
    } catch (std::exception& e) {
    }
    
    try {
	for(auto i: p.get_child("outputs")) {

	    std::string name = i.first;

	    boost::property_tree::ptree& wal = i.second;

	    worker::address_list al;

	    for(auto j: wal) {

		worker::address a;

		for(auto k: j.second) {
		    a.push_back(k.second.get_value<std::string>());
		}

		al.push_back(a);

	    }

	    proc->outputs[name] = al;

	}
    } catch (...) {
    }

    std::ostringstream buf;
    buf << "import wye.func; import sys; sys.stderr.write(str(wye.func)); sys.stderr.write('BUNCHY\\n'); wye.func.run('" << lambda << "', [";

    std::string psep = "";
    
    for(auto i: proc->outputs) {

	std::string name = i.first;

	for(auto j: i.second) {

	    std::string outs = name + ":";
	    std::string sep = "";

	    for(auto k: j) {
		outs.append(sep);
		outs.append(k);
		sep = ",";
	    }

	    buf << psep;
	    buf << "'" << outs << "'";
	    psep = ",";

	}

    }

    buf << "])";

    std::cerr << buf.str() << std::endl;

    proc->exec = "/usr/bin/python";
    proc->args = { {"python"}, {"-c"}, buf.str() };

    return run_python(proc, p);

}

