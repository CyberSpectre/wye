
#ifndef MANAGER_H
#define MANAGER_H

#include <boost/property_tree/ptree.hpp>
#include <thread>
#include <map>

enum error_code {
    OK,
    NOT_IMPLEMENTED,
    INVALID_REQUEST
};

extern std::map<error_code,std::string> error_string;

class process {
  public:
    boost::process::child proc;
};

class manager {

  private:
    std::thread* background;
    boost::property_tree::ptree error(error_code, const std::string& msg);

    static bool py_initialised;

  public:

    manager() {
	background = nullptr;
    }
    
    boost::property_tree::ptree
	handle(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	create_worker(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	create_python_worker(const boost::property_tree::ptree&);

    void run();

};

#endif

