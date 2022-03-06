#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/detail/error_code.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include "client.hpp"
#include "connection.hpp"

using boost::asio::ip::tcp;
typedef boost::shared_ptr<Connection> ptr;

Connection::Connection(boost::asio::io_context &io_context_) : socket_(tcp::socket(io_context_))
{}

tcp::socket& Connection::socket(){
    return socket_;
}
void Connection::read_opt(){
    try{
        char *option = new char[1];
        boost::asio::async_read(socket_, boost::asio::buffer(option, 1),
                                boost::bind(&Connection::opt_handler, shared_from_this(),
                                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, option)
                                );
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
}
void Connection::opt_handler(const boost::system::error_code &e, size_t, char* opt){
    if(e)
        std::cerr<<"opt_handler: "<<e.what()<<std::endl;
    else
        std::cout<<"You have chosen option: "<<*opt<<std::endl;

    //execute option
    switch(*opt){
        case 's':
            get_msg();
            break;
        case 'p':
            {
            std::string clients = Client::get_clients_data();
            send_clients_list(clients.length(), clients);
            }
            break;
        case 'S':
            break;
    }
    delete[] opt;
}
void Connection::send_clients_handler(const boost::system::error_code &e, size_t b){
    if(e)
        std::cerr<<e.what()<<std::endl;
    else
        std::cout<<"x sent correctly: "<<b<<std::endl;

    //loop
    read_opt();
}
bool Connection::send_all_clients(const boost::system::error_code &e, size_t, int l, std::string clients){
    if(e)
        std::cerr<<e.what()<<std::endl;
    try{
        boost::asio::async_write(socket(), boost::asio::buffer(clients, l),
                                 boost::bind(&Connection::send_clients_handler, shared_from_this(),
                                 boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
    return 0;
}
void Connection::send_clients_list(int l, std::string c){
    std::cout<<"Length: "<<std::to_string(l)<<'\n';
    boost::asio::async_write(socket(), boost::asio::buffer(std::to_string(l),c.size()), 
                            boost::bind(&Connection::send_all_clients, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, l, c));
}
void Connection::get_msg(){
    get_msg_length();
}
void Connection::get_msg_length(){
    auto x = new boost::array<char, 3>;
    boost::asio::async_read(socket_, boost::asio::buffer(x, 3), 
                            boost::bind(&Connection::get_msg_length_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, x));
}
void Connection::get_msg_length_handler(const boost::system::error_code &e, size_t bytes, boost::array<char, 3>* x){
    try{
        std::cout<<"bytes: "<<bytes<<std::endl;
        std::string b;
        for(auto i=x->begin(); i!=x->end(); i++){
            if(*i!='-')
                b+= *i;
        }
        delete x;
        int len = std::stoi(b);
        std::cout<<"len: "<<len<<std::endl;
        //get Message
        char *message = new char[len];
        boost::asio::async_read(socket_, boost::asio::buffer(message, len),
                                boost::bind(&Connection::getActMesHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, message, len));
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
}
void Connection::getActMesHandler(const boost::system::error_code &e, size_t, char *mes, int len){
    try{
        if(e)
            std::cerr<<e.what()<<std::endl;
        std::cout.write(mes, len);
        std::endl(std::cout);
        delete[] mes;

    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    } 

    //loop
    read_opt();
}



