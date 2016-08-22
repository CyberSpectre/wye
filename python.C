
#include <manager.h>
#include <boost/process.hpp>
#include <vector>
#include <string>
#include <iostream>
#include <boost/archive/text_oarchive.hpp>
#include <boost/uuid/uuid_io.hpp>

bool manager::py_initialised = false;

boost::property_tree::ptree
manager::create_python_worker(const boost::property_tree::ptree& p)
{

    std::string file;
    std::string job_id;
    std::vector<std::string> outputs;
    try {
	file = p.get<std::string>("file");
	job_id = p.get<std::string>("job_id");
    } catch (std::exception& e) {
	return error(INVALID_REQUEST, e.what());
    }

    try {
	for(auto i: p.get_child("outputs")) {
	    outputs.push_back(i.second.get_value<std::string>());
	}
    } catch (...) {
    }

    // UUID
    boost::uuids::uuid id = uuidgen();

    processes[id].id = id;
    processes[id].job_id = "n/a";
    processes[id].exec = "/usr/bin/python";
    processes[id].args = { {"python"}, {file} };
    processes[id].outputs = outputs;

    for(auto i: outputs) {
	processes[id].args.push_back(i);
    }
    
    std::cerr << "Run..." << std::endl;
    processes[id].run();

    boost::process::pistream& out = processes[id].get_output();

    std::string line;
    
    while(std::getline(out, line)) {


	std::cerr << line << std::endl;

	if (line == "INIT") {
	    continue;
	}

	if (line.substr(0, 6) == "INPUT:") {
	    processes[id].inputs.push_back(line.substr(6));
	    std::cout << "Input " << line.substr(6) << std::endl;
	    continue;
	}

	if (line.substr(0, 6) == "ERROR:") {

	    // This 'waits' the process.
	    processes.erase(id);
	    
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
	processes.erase(id);
	return error(PROC_INIT_FAIL, "Bad return string: " + line);

    }

    if (out.bad()) {
	// This 'waits' the process.
	processes.erase(id);
	return error(PROC_INIT_FAIL, "Process didn't send RUNNING");
    }

    if (out.eof()) {
	// This 'waits' the process.
	processes.erase(id);
	return error(PROC_INIT_FAIL, "Process didn't send RUNNING");
    }
	
    std::ostringstream ofs;
    ofs << id;

    boost::property_tree::ptree r;
    r.put("id", ofs.str());

    if (processes[id].inputs.size() > 0) {
	boost::property_tree::ptree ins;

	for(auto i: processes[id].inputs) {
	    boost::property_tree::ptree elt;
	    elt.put_value(i);
	    ins.push_back(std::make_pair("", elt));
	}

	r.add_child("inputs", ins);
	
    }

    return r;

}

