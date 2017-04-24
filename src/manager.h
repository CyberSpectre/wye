
#ifndef MANAGER_H
#define MANAGER_H

#include <boost/property_tree/ptree.hpp>
#include <iostream>
#include <map>
#include <mutex>
#include <thread>

#include <job.h>
#include <process.h>
#include <worker.h>

class manager
{
private:
    std::thread* background;

public:
    manager();

    typedef std::shared_ptr<worker> workptr;

    std::map<worker_id, workptr> workers;
    std::mutex workers_mutex;

    std::map<std::string, job> jobs;
    std::mutex jobs_mutex;

    boost::property_tree::ptree handle(const boost::property_tree::ptree&);

    boost::property_tree::ptree create_worker(
        const boost::property_tree::ptree&);

    boost::property_tree::ptree get_workers(const boost::property_tree::ptree&);

    boost::property_tree::ptree delete_worker(
        const boost::property_tree::ptree&);

    boost::property_tree::ptree create_job(const boost::property_tree::ptree&);

    boost::property_tree::ptree get_job(const boost::property_tree::ptree&);

    boost::property_tree::ptree delete_job(const boost::property_tree::ptree&);

    boost::property_tree::ptree get_jobs(const boost::property_tree::ptree&);

    void run();

    void stop();
};

#endif
