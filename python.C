
#include <manager.h>
#include <boost/python.hpp>

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
	py_initialised = true;
    }

    boost::python::object main_module = boost::python::import("__main__");
    boost::python::object main_namespace = main_module.attr("__dict__");

    boost::python::object ret = boost::python::exec_file("thing.py", main_namespace);

}

