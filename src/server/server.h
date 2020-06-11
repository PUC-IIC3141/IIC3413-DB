#ifndef SERVER__SERVER_H_
#define SERVER__SERVER_H_

namespace db_server {
    // IMPORTANT: convention is
    // end_type = type + 1
    enum class MessageType : unsigned char {
        plain_text     = 0x00,
        end_success    = 0x01,
        end_fail       = 0x02,
    };

    constexpr auto DEFAULT_PORT = 8080;
    constexpr auto BUFFER_SIZE = 2048;
    constexpr auto BYTES_FOR_QUERY_LENGTH = 4;
}

#endif // SERVER__SERVER_H_
