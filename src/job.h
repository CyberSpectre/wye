
#ifndef JOB_H
#define JOB_H

#include <process.h>
#include <worker.h>
#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <thread>

class job
{
private:
    static boost::uuids::random_generator uuidgen;

public:
    virtual ~job()
    {
    }

    job();

    std::string id;
    std::string name;
    std::string description;
};

#endif
