#include <job.h>

#include <boost/uuid/uuid_io.hpp>
#include <iostream>


boost::uuids::random_generator job::uuidgen;


job::job()
{

    // UUID
    boost::uuids::uuid u = uuidgen();

    std::ostringstream ofs;
    ofs << u;
    id = ofs.str();
}

