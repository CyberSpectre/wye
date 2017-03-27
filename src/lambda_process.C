#include <lambda_process.h>

#include <boost/process.hpp>
#include <iostream>
#include <mutex>
#include <string>


#include <worker.h>


lambda_process::lambda_process()
{
}


void lambda_process::init_process(const boost::property_tree::ptree& p)
{
    std::string file = "lambda.py";
    std::string call;
    std::string lambda;

    try
    {
        lambda = p.get<std::string>("lambda");
    }
    catch (std::exception& e)
    {
        // Re-throw the error
        std::cerr << e.what() << std::endl;
        throw;
    }

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
        call = p.get<std::string>("call");
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
        // Uncomment if "name" is compulsary
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

    std::ostringstream buf;
    buf << "import wye.func; wye.func." << call << "('" << lambda << "', [";

    std::string psep = "";
    
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

            buf << psep;
            buf << "'" << outs << "'";
            psep = ",";
        }
    }

    buf << "])";

    exec = "/usr/bin/python";
    args = { {"python"}, {"-c"}, buf.str() };
}

