#ifndef SERVER__TCP_BUFFER_H_
#define SERVER__TCP_BUFFER_H_

#include <ostream>
#include <string>
#include <boost/asio.hpp>

#include "server/server.h"

using boost::asio::ip::tcp;

class TcpBuffer {
public:
    TcpBuffer(tcp::socket& sock);
    ~TcpBuffer();

    TcpBuffer& operator<<(std::string msg);
    void begin(db_server::MessageType msg_type);
    void set_error();
    void end();

private:
    int current_pos;
    bool error = false;
    unsigned char buffer[db_server::BUFFER_SIZE];
    tcp::socket& sock;
    void send();
};

#endif // SERVER__TCP_BUFFER_H_
