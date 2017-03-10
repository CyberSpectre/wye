
#ifndef JOB_H
#define JOB_H

#include <boost/property_tree/ptree.hpp>
#include <thread>
#include <iostream>
#include <process.h>
#include <worker.h>

class job
{
private:
    static boost::uuids::random_generator uuidgen;

public:
    virtual ~job() {}
    job();
    std::string id;
    std::string name;
    std::string description;
};

#endif

