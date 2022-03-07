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

#define READ 0
#define SAVE 1

using boost::asio::ip::tcp;

Connection::Connection(boost::asio::io_context &io_context_) : socket_(tcp::socket(io_context_))
{this->id = Client::count;}

tcp::socket& Connection::socket(){
    return socket_;
}
int Connection::get_id(){
    return this->id;
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
            get_msg_length_and_content(READ);
            break;
        case 'p':
            {
            std::string clients = Client::get_clients_data();
            send_clients_list(clients.length(), clients);
            }
            break;
        case 'S':
            get_msg_length_and_content(SAVE);
            break;
        case 'o':
            Client::show_messages(this->get_id());
    }
    delete opt;
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
void Connection::get_msg_length_and_content(bool flag){
    const int len = 3;
    char *x = new char[len];
    boost::asio::async_read(socket_, boost::asio::buffer(x, len), 
                            boost::bind(&Connection::get_msg_length_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, x, len, flag));
}
void Connection::get_msg_length_handler(const boost::system::error_code &e, size_t, char* x, int len, bool flag){
    try{
        std::string b;
        for(int i=0;i<len;i++){
            if(x[i]!='-')
                b+= x[i];
        }
        delete[] x;
        int len = std::stoi(b);

        std::cout<<"len: "<<len<<std::endl;
        //get Message
        char *message = new char[len];
        boost::asio::async_read(socket_, boost::asio::buffer(message, len),
                                boost::bind(&Connection::getActMesHandler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, message, len, flag));
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
}
void Connection::getActMesHandler(const boost::system::error_code &e, size_t, char *mes, int len, bool flag){
    try{
        if(e)
            std::cerr<<e.what()<<std::endl;
        if(flag==READ){
            std::cout.write(mes, len);
            std::endl(std::cout);
        }
        else{
            //idk why, but it must be done that way
            std::string tmp;
            tmp = mes[0];
            int clientID = std::stoi(tmp);
            //
            std::string message;
            for(int i=1;i<len;i++)
                message+=mes[i];
            writeToClients(this->id, clientID, message);
        }
        delete[] mes;

    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    } 

    //loop
    read_opt();
}
void Connection::writeToClients(int sender, int id, std::string message){
    Client::get_AllClients().at(id)->add_message(sender, message);
}
