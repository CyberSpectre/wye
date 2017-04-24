
#include <process.h>

#include <boost/process.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <iostream>

process::process()
{
    proc = nullptr;
}

process::~process()
{
    if (proc)
    {
        proc->terminate();
        proc->wait();
        proc = nullptr;

        std::cerr << "Process id: " << id << " killed" << std::endl;
    }
}

void process::run()
{
    boost::process::posix_context ctx;

    ctx.output_behavior.insert(boost::process::behavior_map::value_type(
        STDOUT_FILENO, boost::process::inherit_stream()));

    ctx.output_behavior.insert(boost::process::behavior_map::value_type(
        STDERR_FILENO, boost::process::inherit_stream()));

    ctx.output_behavior.insert(boost::process::behavior_map::value_type(
        3, boost::process::capture_stream()));

    ctx.environment = boost::process::self::get_environment();

    boost::process::posix_child c =
        boost::process::posix_launch(exec, args, ctx);

    proc = std::shared_ptr<boost::process::posix_child>(
        new boost::process::posix_child(c));
}

void process::terminate()
{
    std::cerr << "Terminate..." << std::endl;
    proc->terminate();

    std::cerr << "join..." << std::endl;
    proc->wait();

    std::cerr << "Stopped" << std::endl;

    proc = nullptr;
}

boost::process::pistream& process::get_output()
{
    return proc->get_output(3);
}

void process::describe(boost::property_tree::ptree& p)
{
    p.put("id", id);

    if (name != "")
    {
        p.put("name", name);
    }

    p.put("job_id", job_id);
    p.put("exec", exec);

    if (state == RUNNING)
    {
        p.put("state", "RUNNING");
    }
    else
    {
        p.put("state", "STOPPED");
    }

    if (args.size() > 0)
    {
        boost::property_tree::ptree as;
        for (auto i : args)
        {
            boost::property_tree::ptree elt;
            elt.put_value(i);
            as.push_back(std::make_pair("", elt));
        }
        p.add_child("args", as);
    }

    if (inputs.size() > 0)
    {
        boost::property_tree::ptree ins;
        for (auto i : inputs)
        {
            ins.put(i.first, i.second);
        }
        p.add_child("inputs", ins);
    }

    if (outputs.size() > 0)
    {
        boost::property_tree::ptree outs;
        for (auto i : outputs)
        {
            std::string name = i.first;

            boost::property_tree::ptree wal;

            for (auto j : i.second)
            {
                boost::property_tree::ptree al;

                for (auto k : j)
                {
                    boost::property_tree::ptree elt;
                    elt.put_value(k);
                    al.push_back(std::make_pair("", elt));
                }

                wal.push_back(std::make_pair("", al));
            }

            outs.add_child(name, wal);
        }

        p.add_child("outputs", outs);
    }
}

bool process::is_running()
{
    if (state != RUNNING)
    {
        std::cerr << "Process " << id << " not RUNNING." << std::endl;
        return false;
    }

    if (proc == nullptr)
    {
        throw std::runtime_error("Process null pointer!");
    }

    try
    {
        int pid_status;

        if (::waitpid(proc->get_id(), &pid_status, WNOHANG) == -1)
        {
            // Do not call proc-get_id() in here or it'll seg fault
           
            throw std::runtime_error("Process id: " + id + " failed");
        }
    }
    catch (...)
    {
        // Assume exception is that process no longer exists.
        std::cerr << "Process id: " << id << " appears to no longer exist."
                  << std::endl;
        state = STOPPED;
        proc = nullptr;

        return false;
    }

    return true;
}
