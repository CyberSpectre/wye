
#ifndef MANAGER_H
#define MANAGER_H

#include <boost/property_tree/ptree.hpp>
#include <thread>
#include <map>
#include <iostream>
#include <mutex>

#include <worker.h>

enum error_code {
    OK,
    NOT_IMPLEMENTED,
    INVALID_REQUEST,
    PROC_INIT_FAIL
};

extern std::map<error_code,std::string> error_string;

class job {
  private:
    static boost::uuids::random_generator uuidgen;
  public:
    virtual ~job() {}
    job();
    std::string id;
    std::string name;
    std::string description;
};

class manager {

  private:
    std::thread* background;
    boost::property_tree::ptree error(error_code, const std::string& msg);

    static bool py_initialised;

  public:

    manager() {
	background = nullptr;
    }

    typedef std::shared_ptr<worker> workptr;
    
    std::map<worker_id, workptr> workers;
    std::mutex workers_mutex;

    std::map<std::string, job> jobs;
    std::mutex jobs_mutex;
    
    boost::property_tree::ptree
	handle(const boost::property_tree::ptree&);

    boost::property_tree::ptree
	create_worker(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	create_python_worker(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	get_workers(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	delete_worker(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	create_job(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	get_job(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	delete_job(const boost::property_tree::ptree&);
    boost::property_tree::ptree
	get_jobs(const boost::property_tree::ptree&);

    void run();

    void stop();

};

#endif

