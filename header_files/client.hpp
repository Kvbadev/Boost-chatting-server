#include <boost/asio.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include "connection.hpp"

struct inbox_message{
    int sender_id;
    std::string message;
};

class Client : public boost::enable_shared_from_this<Client>{
public:
    typedef boost::shared_ptr<Client> ptr;
    Client(Connection::ptr x);
    std::string get_address();
    int get_id();
    void add_message(int id, std::string message);

    static std::string get_clients_data(){
        std::string text;
        for(int i=0;i<count;i++){
            text+=std::to_string(AllClients.at(i)->id);
            text+=" - ";
            text+=AllClients.at(i)->get_address();
            text+='\n';
        }
        return text;
    }
    auto get_inbox(){
        return this->inbox;
    }
    static void create_client(Connection::ptr x){
        AllClients.push_back(Client::ptr(new Client(x)));
    }
    static auto get_AllClients(){
        return AllClients;
    }
    static void show_messages(int id){
        auto j = AllClients;
        auto x = AllClients.at(id)->get_inbox();
        for(auto i = x.begin(); i!=x.end();i++){
            
            std::cout<<i->sender_id<<":";
            std::cout<<i->message<<std::endl;
        }
    }
    static inline int count = 0;
private:
    Connection::ptr client_conn;
    int id;
    std::vector<inbox_message> inbox;
    // inline to prevent reserving memory for variable in the other place in the program
    static inline std::vector<Client::ptr> AllClients;
};
