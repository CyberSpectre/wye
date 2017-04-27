#include <python_process.h>

#include <boost/process.hpp>
#include <iostream>
#include <mutex>
#include <string>

#include <worker.h>

python_process::python_process()
{
}

void python_process::init_process(const boost::property_tree::ptree& p)
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

    exec = "/usr/bin/python";
    args = {{"python"}, {file}};

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
        std::cerr << e.what();
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
