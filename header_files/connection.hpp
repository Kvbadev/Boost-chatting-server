#include <boost/asio.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>

#pragma once
class Client;

using boost::asio::ip::tcp;
class Connection : public boost::enable_shared_from_this<Connection>{

public:
    typedef boost::shared_ptr<Connection> ptr;
    Connection(boost::asio::io_context &io_context_);
    tcp::socket& socket();
    void read_opt();
    void opt_handler(const boost::system::error_code &e, size_t);
    void opt_executer();
    bool send_all_clients(const boost::system::error_code &e, size_t, int l, std::string clients);
    void send_clients_list(int l, std::string c);
    void send_clients_handler(const boost::system::error_code &e, size_t b);
    void get_msg();
    void get_msg_length();
    void get_msg_length_handler(const boost::system::error_code &e, size_t bytes, boost::array<char, 3>* x);
    void getActMesHandler(const boost::system::error_code &e, size_t, char *mes);
    static ptr create(boost::asio::io_context& io_context){
        return ptr(new Connection(io_context));
    }

private:
    tcp::socket socket_;
    char option[1];
};

