
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
    try {
	file = p.get<std::string>("file");
	job_id = p.get<std::string>("job_id");
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

    std::ostringstream ofs;
    ofs << id;

    boost::property_tree::ptree r;
    r.put("id", ofs.str());

    return r;

}

