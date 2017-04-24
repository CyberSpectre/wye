#include <lua_process.h>

#include <boost/process.hpp>
#include <iostream>
#include <mutex>
#include <string>

#include <worker.h>

lua_process::lua_process()
{
}

void lua_process::init_process(const boost::property_tree::ptree& p)
{
    std::string file;

    try
    {
        file = p.get<std::string>("file");
    }
    catch (std::exception& e)
    {
        // Re-throw the error
        throw;
    }

    exec = "/usr/bin/lua";
    args = {{"lua"}, {file}};

    try
    {
        job_id = p.get<std::string>("job_id");
    }
    catch (std::exception& e)
    {
        // Re-throw the error
        throw;
    }

    try
    {
        name = p.get<std::string>("name");
    }
    catch (std::exception& e)
    {
        // Uncomment if "name" becomes compulsary
        // throw;
    }

    try
    {
        for (auto i : p.get_child("outputs", boost::property_tree::ptree()))
        {
            std::string name = i.first;

            boost::property_tree::ptree& wal = i.second;

            worker::address_list al;

            for (auto j : wal)
            {
                worker::address a;

                for (auto k : j.second)
                {
                    a.push_back(k.second.get_value<std::string>());
                }

                al.push_back(a);
            }

            outputs[name] = al;
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }

    for (auto i : outputs)
    {
        std::string name = i.first;

        for (auto j : i.second)
        {
            std::string outs = name + ":";
            std::string sep = "";

            for (auto k : j)
            {
                outs.append(sep);
                outs.append(k);
                sep = ",";
            }

            args.push_back(outs);
        }
    }
}

boost::property_tree::ptree lua_process::run_process(
    const boost::property_tree::ptree& p)
{
    // Call the base class run()
    run();

    boost::property_tree::ptree r;
    r.put("id", id);

    if (inputs.size() > 0)
    {
        boost::property_tree::ptree ins;

        for (auto i : inputs)
        {
            ins.put(i.first, i.second);
        }

        r.add_child("inputs", ins);
    }

    return r;
}
