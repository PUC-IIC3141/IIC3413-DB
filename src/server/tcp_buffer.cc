#include "tcp_buffer.h"

TcpBuffer::TcpBuffer(tcp::socket& sock)
    : sock(sock) { }


TcpBuffer::~TcpBuffer() = default;


void TcpBuffer::begin(db_server::MessageType msg_type) {
    buffer[0] = static_cast<unsigned char>(msg_type);
    // skip first byte for type and 2 bytes for length
    current_pos = 3;
}


void TcpBuffer::set_error() {
    error = true;
}


TcpBuffer& TcpBuffer::operator<<(std::string msg) {
    auto msg_len = msg.size();
    unsigned available_space = db_server::BUFFER_SIZE - current_pos; // unsigned instead of auto to avoid warning in comparison
    if (msg_len < available_space) { // msg fits in buffer
        memcpy( &buffer[current_pos], msg.data(), msg_len );
        current_pos += msg_len;
        if (current_pos == db_server::BUFFER_SIZE) {
            send();
        }
    } else { // msg needs to be splitted, could need multiple sends
        for (auto it = msg.begin(); it != msg.end(); ++it) {
            buffer[current_pos++] = *it;
            if (current_pos == db_server::BUFFER_SIZE) {
                send();
            }
        }
    }
    return *this;
}


void TcpBuffer::end() {
    if (error) {
        buffer[0] = static_cast<unsigned char>(db_server::MessageType::end_fail);
    } else {
        buffer[0] = static_cast<unsigned char>(db_server::MessageType::end_success);
    }
    send();
}


void TcpBuffer::send() {
    unsigned char b1 = current_pos & 0xFF;
    unsigned char b2 = (current_pos >> 8) & 0xFF;

    buffer[1] = b1;
    buffer[2] = b2;

    boost::asio::write(sock, boost::asio::buffer(buffer, db_server::BUFFER_SIZE));
    current_pos = 3;
}
