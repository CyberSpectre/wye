
#ifndef LUA_PROCESS_H
#define LUA_PROCESS_H

#include <boost/property_tree/ptree.hpp>

#include <process.h>


class lua_process : public process
{
public:
    lua_process();

    virtual ~lua_process()
    {
    }

    virtual void init_process(const boost::property_tree::ptree& p);

    virtual boost::property_tree::ptree run_process(
        const boost::property_tree::ptree& p);

// TEMPORARY
//wye::wye_lua cml;

// cml constructor needs a path argument - this is usually the cybermon's lua config file
// Here I think that just needs to be the lua script filename - look at how python handler does it
//obs(const std::string& path) : cml(path) {}

};

#endif
