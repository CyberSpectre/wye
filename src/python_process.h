
#ifndef PYTHON_PROCESS_H
#define PYTHON_PROCESS_H

#include <boost/property_tree/ptree.hpp>

#include <process.h>

class python_process : public process
{
public:
    python_process();

    virtual ~python_process()
    {
    }

    virtual void init_process(const boost::property_tree::ptree& p);

    virtual boost::property_tree::ptree run_process(
        const boost::property_tree::ptree& p);
};

#endif
