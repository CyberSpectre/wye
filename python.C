
#include <manager.h>
#include <boost/process.hpp>
#include <vector>
#include <string>
#include <iostream>

bool manager::py_initialised = false;

boost::property_tree::ptree
manager::create_python_worker(const boost::property_tree::ptree& p)
{

    std::string file;
    try {
	file = p.get<std::string>("file");
    } catch (...) {
	return error(INVALID_REQUEST, "Must supply a 'file' value");
    }

    std::string exec = "/usr/bin/python";
    std::vector<std::string> args = { {"python"}, {file} };

    boost::process::context ctx;
    ctx.stdout_behavior = boost::process::inherit_stream();
    ctx.stderr_behavior = boost::process::inherit_stream();

    std::cerr << "Exec..." << std::endl;
    boost::process::child c = boost::process::launch(exec, args, ctx);

}

