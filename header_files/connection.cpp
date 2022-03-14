#include <boost/asio.hpp>
#include <boost/asio/placeholders.hpp>
#include <boost/system/detail/error_code.hpp>
#include <boost/system/system_error.hpp>
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
    /* if(e==boost::asio::error::eof){ */
    /*     Client::remove_client(this->get_id()); */
    /* } */
    if(e)
        std::cerr<<"opt_handler: "<<e.what()<<std::endl;
    

    //execute option
    switch(*opt){
        case 's':
            get_msg_length_and_content(READ);
            break;
        case 'p':
            {
            std::string *clients = new std::string(Client::get_clients_data(shared_from_this()->get_id()));
            std::string *length = new std::string(std::to_string(clients->length()));
            send_clients_list(clients, length);
            }
            break;
        case 'S':
            get_msg_length_and_content(SAVE);
            break;
        case 'o':
            std::string data = Client::get_messages(shared_from_this()->get_id());
            send_messages_to_client(data.length(), data);
    }
    delete opt;
}
void Connection::send_clients_list(std::string *mes, std::string *len){
    const int len_msg_length = 3;
    if(len->size()==1)
        len->insert(0, 2, '-');
    else if(len->size()==2)
        len->insert(0, 1, '-');
    boost::asio::async_write(socket(), boost::asio::buffer(*len,len_msg_length), 
                            boost::bind(&Connection::send_all_clients, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, mes, len));
}
bool Connection::send_all_clients(const boost::system::error_code &e, size_t, std::string *clients, std::string *len){
    if(e)
        std::cerr<<e.what()<<std::endl;
    try{
        delete len;
        boost::asio::async_write(socket(), boost::asio::buffer(*clients, clients->size()),
                                 boost::bind(&Connection::send_clients_handler, shared_from_this(),
                                 boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, clients));
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
        return 1;
    }
    return 0;
}
void Connection::send_clients_handler(const boost::system::error_code &e, size_t b, std::string *clients){
    if(e)
        std::cerr<<e.what()<<std::endl;
    else
        delete clients;

    //loop
    read_opt();
}

void Connection::send_messages_to_client(int l, std::string &x){
    if(l>0)
        boost::asio::async_write(socket(), boost::asio::buffer(std::to_string(l), x.size()),
                                boost::bind(&Connection::send_messages, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, l, x));
    else{
        x = "No messages!";
        boost::asio::async_write(socket(), boost::asio::buffer(std::to_string(l), x.size()),
                                boost::bind(&Connection::send_messages, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, l, x));
    }

}
void Connection::send_messages(const boost::system::error_code &e, size_t, int len, std::string x){
    if(e)
        std::cerr<<e.what()<<std::endl;
    else{
        boost::asio::async_write(socket(), boost::asio::buffer(x, len),
                            boost::bind(&Connection::send_messages_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
    }
}
void Connection::send_messages_handler(const boost::system::error_code &e, size_t b){
    if(e)
        std::cerr<<e.what()<<std::endl;

    //loop
    read_opt();
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
            std::string message;
            for(int i=1;i<len;i++)
                message+=mes[i];
            int senderID = this->get_id();
            writeToClients(senderID, clientID, message);
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
