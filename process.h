
#ifndef PROCESS_H
#define PROCESS_H

#include <vector>
#include <string>
#include <boost/process.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>

#include <worker.h>

class process : public worker {

  public:

    virtual void run();

    virtual void terminate();
    
    process() { proc = nullptr; }

    virtual ~process();

    boost::process::pistream& get_output() {
	return proc->get_stdout();
    }
    
    std::string exec;
    std::vector<std::string> args;
    
    std::shared_ptr<boost::process::child> proc;

    std::vector<std::string> inputs;
    std::vector<std::string> outputs;

    virtual void describe(boost::property_tree::ptree& p);

    virtual void check();
  
};

#endif

