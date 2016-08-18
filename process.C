
#include <manager.h>
#include <boost/process.hpp>
#include <iostream>

void process::run() {
  boost::process::context ctx;
  ctx.stdout_behavior = boost::process::inherit_stream();
  ctx.stderr_behavior = boost::process::inherit_stream();

  boost::process::child c = boost::process::launch(exec, args, ctx);
  proc = new boost::process::child(c);

}

void process::terminate() {
    std::cerr << "Terminate..." << std::endl;
    proc->terminate();
    std::cerr << "join..." << std::endl;
    proc->wait();
    std::cerr << "Stopped" << std::endl;
    delete proc;
    proc = nullptr;
}

