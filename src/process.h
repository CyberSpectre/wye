
#ifndef PROCESS_H
#define PROCESS_H

#include <boost/process.hpp>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <string>
#include <vector>


#include <worker.h>


class process : public worker
{
  public:
    process();

    virtual ~process();

    virtual void run();

    virtual void terminate();

    boost::process::pistream& get_output();
    
    std::string exec;
    std::vector<std::string> args;
    
    std::shared_ptr<boost::process::posix_child> proc;

    virtual void describe(boost::property_tree::ptree& p);

    virtual void check();
};

#endif

