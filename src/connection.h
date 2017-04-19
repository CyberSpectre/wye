#ifndef CONNECTION_H
#define CONNECTION_H

#include <boost/asio.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/http/buffered_socket.hpp>

#include <iostream>
#include <memory>
#include <sstream>

#include <manager.h>

using namespace std;
using namespace boost;

class connection : public std::enable_shared_from_this<connection>
{
private:
    manager& mgr;

public:
    void operator()(asio::yield_context yield);

    asio::ip::tcp::socket& tcp_layer();

    static std::shared_ptr<connection> make_connection(asio::io_service& ios,
                                                       int counter, manager& m);

private:
    connection(asio::io_service& ios, int counter, manager& mgr);

    http::buffered_socket socket;
    int counter;

    std::string method;
    std::string path;
    http::message message;
};

#endif
