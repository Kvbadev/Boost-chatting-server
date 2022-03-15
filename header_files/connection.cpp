#include "client.hpp"
#include "connection.hpp"
#include <boost/system/detail/error_code.hpp>

#define READ 0
#define SAVE 1

using boost::asio::ip::tcp;

Connection::Connection(boost::asio::io_context &io_context_) : socket_(tcp::socket(io_context_))
{}

tcp::socket& Connection::socket(){
    return socket_;
}
int Connection::get_id(){
    return this->id;
}
bool Connection::has_closed(const boost::system::error_code &e){
    try{
        if(e==boost::asio::error::eof){
            Client::remove_client(get_id());
            return 1;
        }
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
    return 0;
}
void Connection::set_id(size_t idd){
    this->id=idd;
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
    if(!has_closed(e)){
        if(e)
            std::cerr<<"opt_handler: "<<e.what()<<std::endl;
    }
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
            std::string *data = new std::string(Client::get_messages(shared_from_this()->get_id()));
            std::string *len = new std::string(std::to_string(data->size()));
            send_messages_to_client(len,data);
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
    if(!has_closed(e)){
        if(e)
            std::cerr<<e.what()<<std::endl;
    }
    try{
        delete len;
        boost::asio::async_write(socket(), boost::asio::buffer(*clients, clients->size()),
                                 boost::bind(&Connection::send_clients_handler, shared_from_this(),
                                 boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, clients));
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
        delete len;
        return 1;
    }
    return 0;
}
void Connection::send_clients_handler(const boost::system::error_code &e, size_t b, std::string *clients){
    if(!has_closed(e)){
        if(e)
            std::cerr<<e.what()<<std::endl;
    }
    delete clients;

    //loop
    read_opt();
}

void Connection::send_messages_to_client(std::string *len, std::string *messges){
    const int len_msg_length = 3;
    if(len->size()==1)
        len->insert(0, 2, '-');
    else if(len->size()==2)
        len->insert(0, 1, '-');
    boost::asio::async_write(socket(), boost::asio::buffer(*len, len_msg_length),
                            boost::bind(&Connection::send_messages, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, len, messges));
}
void Connection::send_messages(const boost::system::error_code &e, size_t, std::string *len, std::string *x){
    try{
        delete len;
        /* if(!has_closed(e)){ */
        if(e)
            std::cerr<<e.what()<<std::endl;
        /* } */
        else{
            boost::asio::async_write(socket(), boost::asio::buffer(*x, x->size()),
                                boost::bind(&Connection::send_messages_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, x));
        }
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
}
void Connection::send_messages_handler(const boost::system::error_code &e, size_t b, std::string *to_remove){
    try{
        delete to_remove;
        if(!has_closed(e)){
            if(e)
            std::cerr<<e.what()<<std::endl;
        }

        //loop
        read_opt();
    } catch(std::exception &e){
        std::cerr<<e.what()<<std::endl;
    }
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
        if(!has_closed(e)){
            if(e)
            std::cerr<<e.what()<<std::endl;
        }
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
    Client::getElemById(id)->get()->add_message(sender, message);
}
