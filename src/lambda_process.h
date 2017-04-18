
#ifndef LAMBDA_PROCESS_H
#define LAMBDA_PROCESS_H

#include <boost/property_tree/ptree.hpp>

#include <python_process.h>

class lambda_process : public python_process
{
public:
    lambda_process();

    void init_process(const boost::property_tree::ptree& p);
};

#endif
