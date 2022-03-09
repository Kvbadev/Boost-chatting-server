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
    int get_id();
    void opt_handler(const boost::system::error_code &e, size_t, char* opt);
    bool send_all_clients(const boost::system::error_code &e, size_t, int l, std::string clients);
    void send_clients_list(int l, std::string c);
    void send_clients_handler(const boost::system::error_code &e, size_t b);
    void get_msg_length_and_content(bool flag);
    void get_msg_length_handler(const boost::system::error_code &e, size_t, char* x, int len, bool flag);
    void getActMesHandler(const boost::system::error_code &e, size_t, char *mes, int len, bool flag);
    void writeToClients(int sender, int id, std::string message);
    void send_messages_to_client(int l, std::string &x);
    void send_messages(const boost::system::error_code &e, size_t, int len, std::string x);
    void send_messages_handler(const boost::system::error_code &e, size_t b);
    static ptr create(boost::asio::io_context& io_context){
        return ptr(new Connection(io_context));
    }
private:
    tcp::socket socket_;
    int id;
};

