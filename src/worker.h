
#ifndef WORKER_H
#define WORKER_H

#include <boost/uuid/uuid.hpp>
#include <boost/uuid/random_generator.hpp>
#include <boost/property_tree/ptree.hpp>
#include <list>
#include <map>
#include <string>


typedef std::string worker_id;


enum worker_state
{
    RUNNING, STOPPED
};


class worker
{
  private:
    static boost::uuids::random_generator uuidgen;

  public:

    worker();
    virtual ~worker() {}

    worker_id id;
    std::string name;
    std::string job_id;
    
    worker_state state;

    virtual void describe(boost::property_tree::ptree& p) = 0;
    virtual void check() = 0;

    typedef std::list<std::string> address;
    typedef std::list<address> address_list;
    std::map<std::string, address_list> outputs;

    std::map<std::string, std::string> inputs;
};

#endif

