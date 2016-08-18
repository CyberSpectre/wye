
#ifndef MANAGER_H
#define MANAGER_H

#include <boost/property_tree/ptree.hpp>
#include <thread>
#include <map>
#include <boost/process.hpp>
#include <boost/uuid/random_generator.hpp>
#include <iostream>

enum error_code {
    OK,
    NOT_IMPLEMENTED,
    INVALID_REQUEST,
    PROC_INIT_FAIL
};

extern std::map<error_code,std::string> error_string;

typedef boost::uuids::uuid process_id;

class process {
  public:

    virtual void run();

    virtual void terminate();
    
    process() { proc = nullptr; }

    ~process() {
	if (proc) {
	    proc->terminate();
	    proc->wait();
	    delete proc;
	    proc = 0;
	    std::cerr << "Process killed" << std::endl;
	}
    }

    boost::process::pistream& get_output() {
	return proc->get_stdout();
    }
    
    process_id id;
    std::string job_id;

    std::string exec;
    std::vector<std::string> args;
    
    boost::process::child* proc;

    std::vector<std::string> inputs;
    std::vector<std::string> outputs;
    
};

class manager {

  private:
    std::thread* background;
    boost::property_tree::ptree error(error_code, const std::string& msg);

    static bool py_initialised;

    boost::uuids::random_generator uuidgen;

  public:

    manager() {
	background = nullptr;
    }

    std::map<process_id, process> processes;
    
    boost::property_tree::ptree
	handle(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	create_worker(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	create_python_worker(const boost::property_tree::ptree&);

    void run();

    void stop();

};

#endif

