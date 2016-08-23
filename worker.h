
#ifndef WORKER_H
#define WORKER_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/property_tree/ptree.hpp>

typedef std::string worker_id;

enum worker_state {
    RUNNING, STOPPED
};

class worker {
  private:
    static boost::uuids::random_generator uuidgen;

  public:

    worker_id id;
    std::string name;
    std::string job_id;
    
    worker_state state;

    virtual ~worker() {}
    worker();
    
    virtual void describe(boost::property_tree::ptree& p) = 0;
    virtual void check() = 0;
};

#endif

