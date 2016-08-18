
#include "manager.h"
#include <iostream>
#include <thread>

std::map<error_code,std::string> error_string = {
    {OK, "OK"}, {NOT_IMPLEMENTED, "NOT_IMPLEMENTED"},
    {INVALID_REQUEST, "INVALID_REQUEST"},
    {PROC_INIT_FAIL, "PROCESSING_INIT_FAIL"}
};

boost::property_tree::ptree
manager::error(error_code c, const std::string& msg)
{
    boost::property_tree::ptree p;
    p.put("status", "ERROR");
    p.put("error_code", error_string[c]);
    p.put("error", msg);
    return p;
}
       


boost::property_tree::ptree
manager::handle(const boost::property_tree::ptree& p)
{

  std::string op;
  try {
      op = p.get<std::string>("operation");
  } catch (...) {
      throw std::runtime_error("Must supply an 'operation' value");
  }

  std::cerr << "Operation: " << op << std::endl;

  if (op == "create-worker")
      return create_worker(p);

  return error(INVALID_REQUEST, "Operation '" + op + "' not recognised.");

}

boost::property_tree::ptree
manager::create_worker(const boost::property_tree::ptree& p)
{

    std::string model;
    try {
	model = p.get<std::string>("model");
    } catch (...) {
	return error(INVALID_REQUEST, "Must supply a 'model' value");
    }

    if (model == "python")
	return create_python_worker(p);

    if (model == "lua")
	return error(NOT_IMPLEMENTED, "Not implemented");

    return error(INVALID_REQUEST, "Worker model '" + model + "' not known");

}

void
manager::run()
{

    // Manager background thread body.
    auto body = [] () {
	while (1) {
	    std::this_thread::sleep_for(std::chrono::seconds(1));
	}
    };

    background = new std::thread(body);

}

void manager::stop()
{

//    for(auto p : processes) {
//	p.second.terminate();
//    }

    // Destructors will kill processors.
    processes.clear();

}

