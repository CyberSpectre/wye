
#ifndef EXECUTABLE_PROCESS_H
#define EXECUTABLE_PROCESS_H

#include <boost/property_tree/ptree.hpp>

#include <process.h>

class executable_process : public process
{
public:
    executable_process();

    virtual ~executable_process()
    {
    }

    virtual void init_process(const boost::property_tree::ptree& p);
};

#endif
