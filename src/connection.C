#include <connection.h>

#include <boost/http/algorithm.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


using namespace std;
using namespace boost;


connection::connection(asio::io_service &ios, int counter, manager& mgr)
    : socket(ios),
      counter(counter),
      mgr(mgr)
{
}


std::shared_ptr<connection> connection::make_connection(asio::io_service &ios, int counter, manager& m)
{
    return std::shared_ptr<connection>{new connection{ios, counter, m}};
}


void connection::operator()(asio::yield_context yield)
{
    auto self = shared_from_this();
    try
    {
        while (self->socket.is_open())
        {
            // Read request.
            self->socket.async_read_request(self->method,
                                            self->path,
                                            self->message,
                                            yield);

            if (http::request_continue_required(self->message))
            {
                // 100-CONTINUE
                self->socket.async_write_response_continue(yield);
            }

            while (self->socket.read_state() != http::read_state::empty)
            {
                switch (self->socket.read_state())
                {
                    case http::read_state::message_ready:
                        // Read some body.
                        self->socket.async_read_some(self->message, yield);
                        break;
                    case http::read_state::body_ready:
                        // Read trailers.
                        self->socket.async_read_trailers(self->message, yield);
                        break;
                    default:;
                }
            }

            property_tree::ptree pt;

            {
                std::string body;
                body.assign(self->message.body().begin(), self->message.body().end());
                std::istringstream in(body);

                try
                {
                    property_tree::read_json(in, pt);
                }
                catch (std::exception& e)
                {
                    // Write reply.
                    http::message reply;

                    const char body[] = "Your client sent a message this server could not interpret.";
                    std::copy(body, body + sizeof(body) - 1, std::back_inserter(reply.body()));

                    self->socket.async_write_response(400,
                                                         string_ref("Bad Request"),
                                                         reply,
                                                         yield);

                    return;
                }

                try
                {
                    pt = mgr.handle(pt);
                }
                catch (std::exception& e)
                {
                    // Write reply.
                    http::message reply;

                    std::string errormsg = e.what();

                    std::cerr << "Error: " << errormsg << std::endl;

                    std::copy(errormsg.begin(),
                              errormsg.end(),
                              std::back_inserter(reply.body()));

                    self->socket.async_write_response(500,
                                                        string_ref("Internal Server Error"),
                                                        reply,
                                                        yield);

                    return;
                }

                std::ostringstream out;
                property_tree::write_json(out, pt);

                // Write reply.
                http::message reply;
                reply.body().clear();

                std::string repbody = out.str();

                std::copy(repbody.begin(),
                          repbody.end(),
                          std::back_inserter(reply.body()));

                self->socket.async_write_response(200,
                                                    string_ref("OK"),
                                                    reply,
                                                    yield);

                return;
            }
        }

        cout << "socket closed" << std::endl;
    }
    catch (system::system_error &e)
    {
        if (e.code() != system::error_code{asio::error::eof})
        {
            throw e;
        }

        return;
    }
    catch (std::exception &e)
    {
        throw e;
    }
}


asio::ip::tcp::socket &connection::tcp_layer()
{
    return socket.next_layer();
}

