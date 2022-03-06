#include <boost/asio.hpp>
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
        boost::asio::async_read(socket_, boost::asio::buffer(option, 1),
                                boost::bind(&Connection::opt_handler, shared_from_this(),
                                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
                                );
    }
    void Connection::opt_handler(const boost::system::error_code &e, size_t){
        if(e)
            std::cerr<<"opt_handler: "<<e.what()<<std::endl;
        else{
            std::cout<<"You have chosen option: "<<option<<std::endl;
            opt_executer();
        }
    }
    void Connection::opt_executer(){
        switch(*option){
            case 's':
                get_msg();
                break;
            case 'p':
                {
                std::string clients = Client::get_clients_data();
                send_msg_length(clients.length(), clients);
                }
                break;
            case 'S':
                break;
        }
        //loop
        read_opt();
    }
    void Connection::send_clients_handler(const boost::system::error_code &e, size_t b){
        if(e)
            std::cerr<<e.what()<<std::endl;
        else
            std::cout<<"x sent correctly: "<<b<<std::endl;
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
    void Connection::send_msg_length(int l, std::string c){
        std::cout<<"Length: "<<std::to_string(l)<<'\n';
        boost::asio::async_write(socket(), boost::asio::buffer(std::to_string(l),sizeof(int)), 
                                boost::bind(&Connection::send_all_clients, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, l, c));
    }
    void Connection::get_msg(){
        get_msg_length();
    }
    void Connection::get_msg_length(){
        auto x = new boost::array<char, 4>;
        boost::asio::async_read(socket(), boost::asio::buffer(x, 2), 
                                boost::bind(&Connection::get_msg_length_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, x));
    }
    void Connection::get_msg_length_handler(const boost::system::error_code &e, size_t, boost::array<char, 4>* x){
        try{
            std::string b;
            for(auto i=x->begin(); i!=x->end(); i++)
                b+= *i;
            delete x;
            int len = std::stoi(b);
            std::cout<<len<<'\n';
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
    }

