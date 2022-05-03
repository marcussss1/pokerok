#pragma once // NOLINT

#include <exception>
#include <string>

class ServerException: public std::exception {
public:
    explicit ServerException(const std::string& msg) : m_msg(msg) {}
    const char* what() const noexcept override {
        return m_msg.c_str();
    }
private:
    std::string m_msg;
};

class SocketException: public ServerException {
public:
    SocketException()
        : ServerException("Error when creating a socket\n") {}
};

class BindException: public ServerException {
public:
    BindException()
        : ServerException("Error when trying to bind\n") {}
};

class ListenException: public ServerException {
public:
    ListenException()
        : ServerException("Error listening to socket\n") {}
};


