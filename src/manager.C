
#include <manager.h>

#include <boost/uuid/uuid_io.hpp>
#include <iostream>
#include <mutex>
#include <thread>

#include <executable_process.h>
#include <invalid_request.h>
#include <lambda_process.h>
#include <lua_process.h>
#include <python_process.h>


manager::manager()
{
    background = nullptr;
}

boost::property_tree::ptree manager::handle(
    const boost::property_tree::ptree& p)
{
    std::string op;

    try
    {
        op = p.get<std::string>("operation");
    }
    catch (...)
    {
        throw std::invalid_request("Must supply an 'operation' value");
    }

    if (op == "create-worker")
    {
        return create_worker(p);
    }

    if (op == "get-workers")
    {
        return get_workers(p);
    }

    if (op == "delete-worker")
    {
        return delete_worker(p);
    }

    if (op == "create-job")
    {
        return create_job(p);
    }

    if (op == "get-job")
    {
        return get_job(p);
    }

    if (op == "get-jobs")
    {
        return get_jobs(p);
    }

    if (op == "delete-job")
    {
        return delete_job(p);
    }

    if (op == "delete-all-jobs")
    {
        return delete_all_jobs(p);
    }


    throw std::invalid_request("Operation '" + op + "' not recognised.");

    boost::property_tree::ptree r;
    return r;
}

boost::property_tree::ptree manager::get_workers(
    const boost::property_tree::ptree& p)
{
    boost::property_tree::ptree r;

    std::lock_guard<std::mutex> guard(workers_mutex);

    for (auto v : workers)
    {
        boost::property_tree::ptree w;

        v.second->describe(w);

        r.add_child(v.first, w);
    }

    return r;
}

boost::property_tree::ptree manager::create_worker(
    const boost::property_tree::ptree& p)
{
    std::string model;

    try
    {
        model = p.get<std::string>("model");
    }
    catch (...)
    {
        throw std::invalid_request("Must supply a 'model' value");
    }

    std::shared_ptr<process> proc;

    try
    {
        if (model == "executable")
        {
            proc = std::shared_ptr<process>(new executable_process());
            proc->init_process(p);
        }
        else if (model == "python")
        {
            proc = std::shared_ptr<process>(new python_process());
            proc->init_process(p);
        }
        else if (model == "lambda")
        {
            proc = std::shared_ptr<process>(new lambda_process());
            proc->init_process(p);
        }
        else if (model == "lua")
        {
            proc = std::shared_ptr<process>(new lua_process());
            proc->init_process(p);
        }
        else
        {
            throw std::invalid_request("Worker model '" + model +
                                       "' not known");
        }
    }
    catch (std::exception& e)
    {
        throw;
    }

    try
    {
        std::lock_guard<std::mutex> guard(workers_mutex);

        // Keep a map of the processes.
        workers[proc->id] = proc;

        std::cerr << "Running " + model + " process id: " << proc->id << std::endl;
        return proc->run_process(p);
    }
    catch (...)
    {
        throw std::runtime_error("Failed to create " + model + " process");
    }

    boost::property_tree::ptree r;
    return r;
}

boost::property_tree::ptree manager::delete_worker(
    const boost::property_tree::ptree& p)
{
    std::string id;

    try
    {
        id = p.get<std::string>("id");
    }
    catch (...)
    {
        throw std::invalid_request("Must supply an 'id' value");
    }

    std::lock_guard<std::mutex> guard(workers_mutex);

    if (workers.find(id) == workers.end())
    {
        throw std::invalid_request("Worker '" + id + "' not known.");
    }

    workers.erase(id);

    boost::property_tree::ptree r;
    return r;
}

boost::property_tree::ptree manager::create_job(
    const boost::property_tree::ptree& p)
{
    std::string name;
    std::string description;

    try
    {
        name = p.get<std::string>("name");
    }
    catch (...)
    {
        throw std::invalid_request("Must supply a 'name'.");
    }

    try
    {
        description = p.get<std::string>("description");
    }
    catch (...)
    {
        throw std::invalid_request("Must supply a 'description'.");
    }

    job j;
    j.name = name;
    j.description = description;

    std::lock_guard<std::mutex> guard(jobs_mutex);

    jobs[j.id] = j;

    boost::property_tree::ptree r;
    r.put("id", j.id);

    return r;
}

boost::property_tree::ptree manager::delete_job(
    const boost::property_tree::ptree& p)
{
    std::string id;

    try
    {
        id = p.get<std::string>("id");
    }
    catch (...)
    {
        throw std::invalid_request("Must supply an 'id' value");
    }

    std::lock_guard<std::mutex> jguard(jobs_mutex);

    if (jobs.find(id) == jobs.end())
    {
        throw std::invalid_request("Job '" + id + "' not known.");
    }

    std::lock_guard<std::mutex> wguard(workers_mutex);

    // Iteration isn't stable under change, so keep iterating until all items
    // deleted.
    while (1)
    {
        bool changed = false;
        for (auto v : workers)
        {
            if (v.second->job_id == id)
            {
                changed = true;
                workers.erase(v.first);
                break;
            }
        }

        if (!changed)
        {
            break;
        }
    }

    jobs.erase(id);

    boost::property_tree::ptree r;
    return r;
}

boost::property_tree::ptree manager::get_jobs(
    const boost::property_tree::ptree& p)
{
    boost::property_tree::ptree r;

    std::lock_guard<std::mutex> guard(jobs_mutex);

    for (auto v : jobs)
    {
        boost::property_tree::ptree e;
        if (v.second.name != "")
        {
            e.put("name", v.second.name);
        }

        if (v.second.description != "")
        {
            e.put("description", v.second.description);
        }

        r.add_child(v.first, e);
    }

    return r;
}

boost::property_tree::ptree manager::get_job(
    const boost::property_tree::ptree& p)
{
    std::string id;

    try
    {
        id = p.get<std::string>("id");
    }
    catch (...)
    {
        throw std::invalid_request("Must supply an 'id' value");
    }

    std::lock_guard<std::mutex> jguard(jobs_mutex);

    if (jobs.find(id) == jobs.end())
    {
        throw std::invalid_request("Job '" + id + "' not known.");
    }

    boost::property_tree::ptree w;

    std::lock_guard<std::mutex> wguard(workers_mutex);

    for (auto v : workers)
    {
        if (v.second->job_id == id)
        {
            boost::property_tree::ptree e;
            v.second->describe(e);

            w.add_child(v.first, e);
        }
    }

    boost::property_tree::ptree r;

    job& j = jobs[id];

    if (j.name != "")
    {
        r.put("name", j.name);
    }

    if (j.description != "")
    {
        r.put("description", j.description);
    }

    r.add_child("workers", w);

    return r;
}


boost::property_tree::ptree manager::delete_all_jobs(
    const boost::property_tree::ptree& p)
{
    boost::property_tree::ptree r;

    r = get_jobs(p);


    //boost::property_tree::ptree r;
    return r;
}

void manager::run()
{
    // Manager background thread body.
    // This occassionally checks the status of all worker processes.
    // Failed workers aren't deleted just marked as STOPPED

    auto body = [this]() {
        while (1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::lock_guard<std::mutex> guard(workers_mutex);

            for (auto v = workers.begin(); v != workers.end(); v++)
            {
                // Check the status of the worker process
                v->second->is_running();
            }
        }
    };

    try
    {
        background = new std::thread(body);
    }
    catch (...)
    {
        throw;        
    }
}

void manager::stop()
{
    std::lock_guard<std::mutex> guard(workers_mutex);

    // Destructors will kill processors.
    workers.clear();
}
