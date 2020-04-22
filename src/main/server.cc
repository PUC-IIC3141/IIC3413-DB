/*
 * server is a executable that listens for tcp conections asking for queries,
 * and it send the results to the client.
 *
 * There are 3 methods:
 *
 * 1) main: parses the program options (e.g: buffer size, port, database folder).
 *    Then initialize the `file_manager`, `buffer_manager` and the `RelationalModel`
 *    calls the method `server` to start the server.
 *
 * 2) server: infinite loop, waiting for a new TCP connection. When a connection
 *    is established, it calls the `session` method in a different thread, and the loop
 *    starts again in the main thread, waiting for another connection.
 *
 * 3) session: executes the query and send the result to the client via TcpBuffer,
 *    who might break the result into multiple tcp messages.
 */

#include <chrono>
#include <fstream>
#include <iostream>
#include <iterator>
#include <memory>
#include <thread>

#include <boost/asio.hpp>
#include <boost/program_options.hpp>

#include "base/parser/logical_plan/exceptions.h"
#include "base/binding/binding.h"
#include "base/binding/binding_iter.h"
#include "base/parser/logical_plan/op/op.h"
#include "base/parser/logical_plan/op/op_select.h"
#include "relational_model/graph/relational_graph.h"
#include "relational_model/query_optimizer/physical_plan_generator.h"
#include "relational_model/relational_model.h"
#include "storage/buffer_manager.h"
#include "storage/file_manager.h"
#include "server/tcp_buffer.h"

using namespace std;
using boost::asio::ip::tcp;
namespace po = boost::program_options;

void session(tcp::socket sock) {
    try {
        unsigned char query_size_b[db_server::BYTES_FOR_QUERY_LENGTH];
        boost::asio::read(sock, boost::asio::buffer(query_size_b, db_server::BYTES_FOR_QUERY_LENGTH));

        int query_size = 0;
        for (int i = 0, offset = 0; i < db_server::BYTES_FOR_QUERY_LENGTH; i++, offset += 8) {
            query_size += query_size_b[i] << offset;
        }
        std::string query;
        query.resize(query_size);
        boost::asio::read(sock, boost::asio::buffer(query.data(), query_size));
        cout << "Query received:\n";
        cout << query << "\n";

        TcpBuffer tcp_buffer = TcpBuffer(sock);
        tcp_buffer.begin(db_server::MessageType::plain_text);

        // start timer
        auto start = chrono::system_clock::now();
        try {
            // get logical plan
            auto select_plan = Op::get_select_plan(query);

            // get physical plan
            PhysicalPlanGenerator plan_generator { };
            auto root = plan_generator.exec(*select_plan);

            // prepare to start the execution
            root->begin();

            // get all results
            auto binding = root->next();
            int count = 0;
            while (binding != nullptr) {
                tcp_buffer << binding->to_string();
                binding = root->next();
                count++;
            }

            auto end = chrono::system_clock::now();
            chrono::duration<float, std::milli> duration = end - start;
            tcp_buffer << "Found " << std::to_string(count) << " results.\n";
            tcp_buffer << "Execution time: " << std::to_string(duration.count()) << " milliseconds.\n";
        }
        catch (QueryException& e) {
            tcp_buffer << "Query exception: " << e.what() << "\n";
            tcp_buffer.set_error();
        }
        tcp_buffer.end();
    }
    catch (std::exception& e) {
        std::cerr << "Exception in thread: " << e.what() << "\n";
    }
}


void server(boost::asio::io_service& io_service, unsigned short port) {
    tcp::acceptor a(io_service, tcp::endpoint(tcp::v4(), port));
    while (true) {
        tcp::socket sock(io_service);
        a.accept(sock);
        std::thread(session, std::move(sock)).detach();
    }
}


int main(int argc, char **argv) {
    int port;
    try {
        // Parse arguments
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help,h", "show this help message")
            ("db-folder,d", po::value<string>()->required(), "set database folder path")
            ("buffer-size,b", po::value<int>(), "set buffer pool size")
            ("port,p", po::value<int>(&port)->default_value(db_server::DEFAULT_PORT), "database server port")
        ;

        po::positional_options_description p;
        p.add("db-folder", -1);

        po::variables_map vm;
        po::store(po::command_line_parser(argc, argv).options(desc).positional(p).run(), vm);

        if (vm.count("help")) {
            cout << "Usage: server [options] DB_FOLDER\n";
            cout << desc << "\n";
            return 0;
        }
        po::notify(vm);

        file_manager.init(vm["db-folder"].as<string>());

        if (vm.count("buffer-size")) {
            buffer_manager.init(vm["buffer-size"].as<int>());
        } else {
            buffer_manager.init();
        }
        RelationalModel::init();

        boost::asio::io_service io_service;
        boost::asio::deadline_timer t(io_service, boost::posix_time::seconds(5));

        server(io_service, port);
    }
    catch(exception& e) {
        cerr << "Exception: " << e.what() << "\n";
        return 1;
    }
    catch(...) {
        cerr << "Exception of unknown type!\n";
    }
}
