
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
};

#endif
