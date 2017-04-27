
#ifndef APPLICATION_PROCESS_H
#define APPLICATION_PROCESS_H

#include <boost/property_tree/ptree.hpp>

#include <process.h>

class application_process : public process
{
public:
    application_process();

    virtual ~application_process()
    {
    }

    virtual void init_process(const boost::property_tree::ptree& p);

    virtual boost::property_tree::ptree run_process(
        const boost::property_tree::ptree& p);
};

#endif
