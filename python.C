
#include <manager.h>
#include <boost/python.hpp>

/*
stackoverflow.com/questions/26061298/python-multi-thread-multi-interpreter-c-api
*/

bool manager::py_initialised = false;

boost::property_tree::ptree
manager::create_python_worker(const boost::property_tree::ptree& p)
{

#ifdef ASDASD
    std::string module;
    try {
	module = p.get<std::string>("module");
    } catch (...) {
	return error(INVALID_REQUEST, "Must supply a 'module' value");
    }

    return error(NOT_IMPLEMENTED, "Not implemented");

#endif

    if (py_initialised == false) {
	// Don't fool with signal handler.
	Py_InitializeEx(false);

	PyEval_InitThreads();

	PyEval_ReleaseLock();
	
	py_initialised = true;
    }

    auto body = [] () {

	PyEval_AcquireLock();

	PyThreadState* ts = Py_NewInterpreter();

	PyThreadState* ts2 =  PyThreadStateNew(ts->interp);

	PyThreadState_Swap(ts2);

	// Do stuff

	

    }

    boost::python::object main_module = boost::python::import("__main__");
    boost::python::object main_namespace = main_module.attr("__dict__");

    boost::python::object ret = boost::python::exec_file("thing.py", main_namespace);

}

