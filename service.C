#include <iostream>
#include <algorithm>

#include <boost/utility/string_ref.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/spawn.hpp>
#include <boost/http/buffered_socket.hpp>
#include <boost/http/algorithm.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <sstream>
#include <stdexcept>

using namespace std;
using namespace boost;

class connection: public std::enable_shared_from_this<connection>
{
public:
    void operator()(asio::yield_context yield) {
	auto self = shared_from_this();
        try {
            while (self->socket.is_open()) {

		// Read request.
                self->socket.async_read_request(self->method, self->path,
                                                self->message, yield);
		
                if (http::request_continue_required(self->message)) {
		    // 100-CONTINUE
                    self->socket.async_write_response_continue(yield);
                }

                while (self->socket.read_state() != http::read_state::empty) {
                    switch (self->socket.read_state()) {
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

		std::cout << "Body length " << self->message.body().size()
			  << std::endl;

		boost::property_tree::ptree pt;

		{
		    std::string body;
		    body.assign(self->message.body().begin(),
				self->message.body().end());
		    std::istringstream in(body);

		    try {
			boost::property_tree::read_json(in, pt);
		    } catch (std::exception& e) {
			
			// Write reply.
			http::message reply;

			const char body[] = "Your client sent a message this server could not interpret.";
			std::copy(body, body + sizeof(body) - 1,
				  std::back_inserter(reply.body()));

			self->socket.async_write_response(400,
							  string_ref("Bad Request"),
							  reply,
							  yield);
			return;
		    }

		    std::string thing;
		    std::cerr << "Get" << std::endl;

		    thing = pt.get<std::string>("one.two");

		    std::cerr << "Thing: " << thing << std::endl;

		}

		// Message received.
                cout << "Method: " << self->method << std::endl;
                cout << "Path: " << self->path << std::endl;

                {
                    auto host = self->message.headers().find("host");
                    if (host != self->message.headers().end())
                        cout << "Host: " << host->second << endl;
                }

		std::cout << "---" << std::endl;

		// Write reply.
		http::message reply;

                const char body[] = "Hello World\n";
                std::copy(body, body + sizeof(body) - 1,
                          std::back_inserter(reply.body()));

                self->socket.async_write_response(200, string_ref("OK"), reply,
                                                  yield);
            }

        } catch (system::system_error &e) {
            if (e.code() != system::error_code{asio::error::eof}) {
		throw e;
            }

//            cout << '[' << self->counter << "] Error: " << e.what() << endl;
	    return;
	    
        } catch (std::exception &e) {
	    throw e;
        }
    }

    asio::ip::tcp::socket &tcp_layer()
    {
        return socket.next_layer();
    }

    static std::shared_ptr<connection> make_connection(asio::io_service &ios,
                                                       int counter)
    {
        return std::shared_ptr<connection>{new connection{ios, counter}};
    }

private:

    connection(asio::io_service &ios, int counter)
        : socket(ios)
        , counter(counter) {}

    http::buffered_socket socket;
    int counter;

    std::string method;
    std::string path;
    http::message message;
};

int main()
{
    asio::io_service ios;
    asio::ip::tcp::acceptor acceptor(ios,
                                     asio::ip::tcp
                                     ::endpoint(asio::ip::tcp::v6(), 8080));

    auto work = [&acceptor](asio::yield_context yield) {
        int counter = 0;
        for ( ; true ; ++counter ) {
            try {
                auto connection
                    = connection::make_connection(acceptor.get_io_service(),
                                                  counter);
                acceptor.async_accept(connection->tcp_layer(), yield);

                auto handle_connection
                    = [connection](asio::yield_context yield) mutable {
                    (*connection)(yield);
                };
                spawn(acceptor.get_io_service(), handle_connection);
            } catch (std::exception &e) {
                cerr << "Aborting on exception: " << e.what() << endl;
                std::exit(1);
            }
        }
    };

    spawn(ios, work);
    ios.run();

    return 0;
}
