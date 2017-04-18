
#include <boost/program_options.hpp>
#include <iostream>
#include <stdexcept>

#include <connection.h>
#include <manager.h>

using namespace std;
using namespace boost;

int main(int argc, char* argv[])
{
    namespace po = boost::program_options;

    // Assign a default port number
    unsigned int port = 8080;

    // Define the valid command line options
    po::options_description desc("Supported options");
    desc.add_options()("help,h", "Show options guidance")(
        "port,p", po::value<unsigned int>(&port), "port number to listen on");

    po::variables_map vm;

    try
    {
        // Process any command line arguments
        po::store(po::parse_command_line(argc, argv, desc), vm);
        po::notify(vm);
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
        std::cerr << desc << std::endl;
        return 1;
    }

    // Create the management TCP socket
    asio::io_service ios;
    asio::ip::tcp::acceptor acceptor(
        ios, asio::ip::tcp::endpoint(asio::ip::tcp::v6(), port));

    cout << "WYE service running on port " << port << endl;

    // Create the manager
    manager mgr;

    // Define lambda function
    auto signal_handler = [&mgr](const boost::system::error_code& error,
                                 int signal) {
        std::cerr << "Stopping..." << std::endl;
        mgr.stop();
        std::cerr << "Stopped." << std::endl;
        exit(1);
    };

    boost::asio::signal_set signals(ios, SIGINT, SIGTERM);

    signals.async_wait(signal_handler);

    // Define lambda function
    auto work = [&acceptor, &mgr](asio::yield_context yield) {
        // Count of incoming requests
        int counter = 0;

        // Infinite loop that blocks, waiting on a request
        for (; true; ++counter)
        {
            try
            {
                // Request a connection object is created and get given a shared_pointer in return
                auto connection = connection::make_connection(
                    acceptor.get_io_service(), counter, mgr);

                // Block waiting on a tcp connection event
                acceptor.async_accept(connection->tcp_layer(), yield);

                // Define lambda function
                auto handle_connection =
                    [connection](asio::yield_context yield) mutable {
                        (*connection)(yield);
                    };

                // Spawn a process in which to run the streaming process
                spawn(acceptor.get_io_service(), handle_connection);
            }
            catch (std::exception& e)
            {
                cerr << "Aborting on exception: " << e.what() << endl;
                std::exit(1);
            }
        }
    };

    // Run the manager that monitors the status of processes
    mgr.run();

    // Spawn the thread that process incoming requests
    spawn(ios, work);

    // The run() function blocks until all work has finished
    ios.run();

    return 0;
}
