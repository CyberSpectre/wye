
#include <iostream>
#include <thread>
#include <mutex>
#include <boost/uuid/uuid_io.hpp>

#include <manager.h>


std::map<error_code,std::string> error_string =
{
    {OK, "OK"},
    {NOT_IMPLEMENTED, "NOT_IMPLEMENTED"},
    {INVALID_REQUEST, "INVALID_REQUEST"},
    {PROC_INIT_FAIL, "PROCESSING_INIT_FAIL"}
};


manager::manager()
{
    background = nullptr;
}


boost::property_tree::ptree
manager::error(error_code c, const std::string& msg)
{
    boost::property_tree::ptree p;
    p.put("error_code", error_string[c]);
    p.put("error", msg);
    return p;
}

       
boost::property_tree::ptree
manager::handle(const boost::property_tree::ptree& p)
{
    std::string op;

    try
    {
        op = p.get<std::string>("operation");
    }
    catch (...)
    {
        throw std::runtime_error("Must supply an 'operation' value");
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

    return error(INVALID_REQUEST, "Operation '" + op + "' not recognised.");
}


boost::property_tree::ptree
manager::get_workers(const boost::property_tree::ptree& p)
{
    boost::property_tree::ptree r;

    std::lock_guard<std::mutex> guard(workers_mutex);

    for (auto v: workers)
    {
        boost::property_tree::ptree w;

        v.second->describe(w);

        r.add_child(v.first, w);
    }

    return r;
}


boost::property_tree::ptree
manager::create_worker(const boost::property_tree::ptree& p)
{
    std::string model;

    try
    {
	    model = p.get<std::string>("model");
    }
    catch (...)
    {
	    return error(INVALID_REQUEST, "Must supply a 'model' value");
    }

    if (model == "python")
    {
	    return create_python_worker(p);
    }

    if (model == "lambda")
    {
	    return create_lambda_worker(p);
    }

    if (model == "lua")
    {
	    return error(NOT_IMPLEMENTED, "Not implemented");
    }

    return error(INVALID_REQUEST, "Worker model '" + model + "' not known");
}


boost::property_tree::ptree
manager::delete_worker(const boost::property_tree::ptree& p)
{
    std::string id;

    try
    {
	    id = p.get<std::string>("id");
    }
    catch (...)
    {
	    return error(INVALID_REQUEST, "Must supply an 'id' value");
    }

    std::lock_guard<std::mutex> guard(workers_mutex);

    if (workers.find(id) == workers.end())
    {
	    return error(INVALID_REQUEST, "Worker '" + id + "' not known.");
    }

    workers.erase(id);

    boost::property_tree::ptree r;

    return r;
}


boost::property_tree::ptree
manager::create_job(const boost::property_tree::ptree& p)
{
    std::string name;
    std::string description;

    try
    {
	    name = p.get<std::string>("name");
    }
    catch (...)
    {
        return error(INVALID_REQUEST, "Must supply a 'name'.");
    }

    try
    {
	    description = p.get<std::string>("description");
    }
    catch (...)
    {
        return error(INVALID_REQUEST, "Must supply a 'description'.");
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


boost::property_tree::ptree
manager::delete_job(const boost::property_tree::ptree& p)
{
    std::string id;

    try
    {
	    id = p.get<std::string>("id");
    }
    catch (...)
    {
	    return error(INVALID_REQUEST, "Must supply an 'id' value");
    }

    std::lock_guard<std::mutex> jguard(jobs_mutex);

    if (jobs.find(id) == jobs.end())
    {
        return error(INVALID_REQUEST, "Job '" + id + "' not known.");
    }

    std::lock_guard<std::mutex> wguard(workers_mutex);

    // Iteration isn't stable under change, so keep iterating until all items
    // deleted.
    while (1)
    {
	    bool changed = false;
	    for(auto v: workers)
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


boost::property_tree::ptree
manager::get_jobs(const boost::property_tree::ptree& p)
{
    boost::property_tree::ptree r;

    std::lock_guard<std::mutex> guard(jobs_mutex);

    for(auto v: jobs)
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


boost::property_tree::ptree
manager::get_job(const boost::property_tree::ptree& p)
{
    std::string id;

    try
    {
        id = p.get<std::string>("id");
    }
    catch (...)
    {
        return error(INVALID_REQUEST, "Must supply an 'id' value");
    }

    std::lock_guard<std::mutex> jguard(jobs_mutex);

    if (jobs.find(id) == jobs.end())
    {
        return error(INVALID_REQUEST, "Job '" + id + "' not known.");
    }

    boost::property_tree::ptree w;

    std::lock_guard<std::mutex> wguard(workers_mutex);
    
    for(auto v: workers)
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


boost::property_tree::ptree
manager::run_python(std::shared_ptr<process> proc, const boost::property_tree::ptree& p)
{
    std::cerr << "Run python job..." << std::endl;
    proc->run();

    boost::process::pistream& out = proc->get_output();

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
    		    return error(PROC_INIT_FAIL, "Did not interpret process output INPUT:" + line);
            }

            std::string name = line.substr(0, pos);
            std::string input = line.substr(pos + 1);

            proc->inputs[name] = input;
		
            continue;
        }

        if (line.substr(0, 6) == "ERROR:")
        {
    	    return error(PROC_INIT_FAIL, "Process failed to start: " + line.substr(6));
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
        return error(PROC_INIT_FAIL, "Bad return string: " + line);
    }

    if (out.bad())
    {
        // Process is shared ptr, will go out of scope and be terminated.
        return error(PROC_INIT_FAIL, "Process didn't send RUNNING");
    }

    if (out.eof())
    {
        // Process is shared ptr, will go out of scope and be terminated.
        return error(PROC_INIT_FAIL, "Process didn't send RUNNING");
    }

    std::lock_guard<std::mutex> guard(workers_mutex);

    // This keeps a reference to process.
    workers[proc->id] = proc;

    boost::property_tree::ptree r;
    r.put("id", proc->id);

    if (proc->inputs.size() > 0)
    {
        boost::property_tree::ptree ins;

        for(auto i: proc->inputs)
        {
	        ins.put(i.first, i.second);
	    }

        r.add_child("inputs", ins);
    }

    return r;
}


boost::property_tree::ptree
manager::create_python_worker(const boost::property_tree::ptree& p)
{
    std::string file;

    try
    {
        file = p.get<std::string>("file");
    }
    catch (std::exception& e)
    {
        return error(INVALID_REQUEST, e.what());
    }

    // Create the worker process for python
    std::shared_ptr<process> proc = std::shared_ptr<process>(new process());

    proc->exec = "/usr/bin/python";
    proc->args = { {"python"}, {file} };

    try
    {
        proc->job_id = p.get<std::string>("job_id");
    }
    catch (std::exception& e)
    {
        return error(INVALID_REQUEST, e.what());
    }

    try
    {
        proc->name = p.get<std::string>("name");
    }
    catch (std::exception& e)
    {
        // Uncomment if "name" becomes compulsary 
        // return error(INVALID_REQUEST, e.what());
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

            proc->outputs[name] = al;
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }

    for(auto i: proc->outputs)
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

            proc->args.push_back(outs);
        }
    }

    return run_python(proc, p);
}


boost::property_tree::ptree
manager::create_lambda_worker(const boost::property_tree::ptree& p)
{
    std::string file = "lambda.py";
    std::string call;
    std::string lambda;

    std::shared_ptr<process> proc =
	std::shared_ptr<process>(new process());

    try
    {
        lambda = p.get<std::string>("lambda");
    }
    catch (std::exception& e)
    {
        return error(INVALID_REQUEST, e.what());
    }

    try
    {
        proc->job_id = p.get<std::string>("job_id");
    } 
    catch (std::exception& e)
    {
        return error(INVALID_REQUEST, e.what());
    }

    try
    {
        call = p.get<std::string>("call");
    }
    catch (std::exception& e)
    {
        return error(INVALID_REQUEST, e.what());
    }

    try
    {
        proc->name = p.get<std::string>("name");
    }
    catch (std::exception& e)
    {
        // Uncomment if "name" becomes compulsary
        // return error(INVALID_REQUEST, e.what());
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

            proc->outputs[name] = al;
        }
    }
    catch (std::exception& e)
    {
        std::cout << e.what();
    }

    std::ostringstream buf;
    buf << "import wye.func; wye.func." << call << "('" << lambda << "', [";

    std::string psep = "";
    
    for(auto i: proc->outputs)
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

    proc->exec = "/usr/bin/python";
    proc->args = { {"python"}, {"-c"}, buf.str() };

    return run_python(proc, p);
}


void manager::run()
{
    // Manager background thread body.
    // This occassionally checks the status of all worker processes
    // It doesn't do anything else (or handle any failures) at this time. 

    auto body = [this] ()
    {
        while (1)
        {
            std::this_thread::sleep_for(std::chrono::seconds(1));

            std::lock_guard<std::mutex> guard(workers_mutex);

            for(auto v: workers)
            {
                // Check the worker process is still alive
                v.second->check();
            }
        }
    };

    background = new std::thread(body);
}


void manager::stop()
{

    std::lock_guard<std::mutex> guard(workers_mutex);

    // Destructors will kill processors.
    workers.clear();

}


