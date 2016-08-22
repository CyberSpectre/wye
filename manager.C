
#include "manager.h"
#include <iostream>

boost::property_tree::ptree
manager::handle(const boost::property_tree::ptree& p)
{

  std::string op;
  try {
      op = p.get<std::string>("operation");
  } catch (...) {
      throw std::runtime_error("Must supply an 'operation' value");
  }

  std::cerr << "Operation: " << op << std::endl;

  boost::property_tree::ptree r;

  r.put("one.two.three", "thingy");

  return r;

}

