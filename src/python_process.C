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
    args = { {"python"}, {file} };

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
        for(auto i: p.get_child("outputs", boost::property_tree::ptree()))
        {
            std::string name = i.first;

            boost::property_tree::ptree& wal = i.second;

            worker::address_list al;

            for(auto j: wal)
            {
                worker::address a;

                for(auto k: j.second)
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

    for(auto i: outputs)
    {
        std::string name = i.first;

        for(auto j: i.second)
        {
            std::string outs = name + ":";
            std::string sep = "";

            for(auto k: j)
            {
                outs.append(sep);
                outs.append(k);
                sep = ",";
            }

            args.push_back(outs);
        }
    }
}


boost::property_tree::ptree
python_process::run_process(const boost::property_tree::ptree& p)
{
    // Call the base class run()
    run();

    boost::process::pistream& out = get_output();

    std::string line;

    while(std::getline(out, line))
    {
        if (line == "INIT")
        {
            continue;
        }

        if (line.substr(0, 6) == "INPUT:")
        {
           line = line.substr(6);

            int pos = line.find(":");
            if (pos == -1)
            {
                throw std::runtime_error("Did not interpret process output INPUT:" + line);
            }

            std::string name = line.substr(0, pos);
            std::string input = line.substr(pos + 1);

            inputs[name] = input;
		
            continue;
        }

        if (line.substr(0, 6) == "ERROR:")
        {
    	    throw std::runtime_error("Process failed to start: " + line.substr(6));
        }

        if (line.substr(0, 7) == "NOTICE:")
        {
            std::cerr << "Notice: " << line.substr(7) << std::endl;
            continue;
        }

        if (line == "RUNNING")
        {
            std::cerr << "Process started successfully." << std::endl;
            break;
        }

        std::cerr << "Didn't understand: " << line << std::endl;

        // This 'waits' the process.
        throw std::runtime_error("Bad return string: " + line);
    }

    if (out.bad())
    {
        // Process is shared ptr, will go out of scope and be terminated.
        throw std::runtime_error("Process didn't send RUNNING");
    }

    if (out.eof())
    {
        // Process is shared ptr, will go out of scope and be terminated.
        throw std::runtime_error("Process didn't send RUNNING");
    }

    boost::property_tree::ptree r;
    r.put("id", id);

    if (inputs.size() > 0)
    {
        boost::property_tree::ptree ins;

        for(auto i: inputs)
        {
	        ins.put(i.first, i.second);
	    }

        r.add_child("inputs", ins);
    }

    return r;
}

