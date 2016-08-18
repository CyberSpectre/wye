
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
    int inputs;
    int outputs;
    try {
	file = p.get<std::string>("file");
	job_id = p.get<std::string>("job_id");
	inputs = p.get<unsigned int>("inputs", 0);
	outputs = p.get<unsigned int>("outputs", 0);
    } catch (std::exception& e) {
	return error(INVALID_REQUEST, e.what());
    }

    // UUID
    boost::uuids::uuid id = uuidgen();

    processes[id].id = id;
    processes[id].job_id = "n/a";
    processes[id].exec = "/usr/bin/python";
    processes[id].args = { {"python"}, {file} };
    
    std::cerr << "Run..." << std::endl;
    processes[id].run();

    boost::process::pistream& out = processes[id].get_output();
    
    while(1) {
	std::string line;
	std::getline(out, line);

	if (line == "INIT") continue;

	if (line.substr(0, 6) == "INPUT:") {
	    int input_id;
	    std::string name;
	    std::istringstream buf(line.substr(6));
	    buf >> input_id;
	    buf.get();
	    buf >> name;

	    processes[id].inputs[input_id] = name;
	    
	    std::cout << "Input " << id << " " << name << std::endl;

	}

	if (line.substr(0, 6) == "ERROR:") {
	    return error(PROC_INIT_FAIL,
			 "Process failed to start: " + line.substr(6));
	}

	if (line == "RUNNING") {
	    std::cerr << "Process started successfully." << std::endl;
	    break;
	}

    }

    std::ostringstream ofs;
    ofs << id;

    boost::property_tree::ptree r;
    r.put("id", ofs.str());

    for (auto i : processes[id].inputs) {
	std::ostringstream buf;
	buf << "inputs." << i.first;
	r.put(buf.str(), i.second);
    }

    return r;

}

