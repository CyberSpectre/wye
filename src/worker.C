
#include <worker.h>
#include <boost/uuid/uuid_io.hpp>

boost::uuids::random_generator worker::uuidgen;

worker::worker() {

    // UUID
    boost::uuids::uuid u = uuidgen();

    std::ostringstream ofs;
    ofs << u;
    id = ofs.str();

    state = RUNNING;
    
}

