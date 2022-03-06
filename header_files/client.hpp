#include <boost/asio.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include "connection.hpp"

class Client{
public:
    typedef boost::shared_ptr<Client> ptr;
    Client(std::string addr, Connection::ptr x);
    std::string get_address();
    int getId();

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
    static void create_client(std::string addr, Connection::ptr x){
        AllClients.push_back(Client::ptr(new Client(addr, x)));
    }
private:
    Connection::ptr client_conn;
    int id;
    // inline to prevent reserving memory for variable in the other place in the program
    static inline int count = 0;
    static inline std::vector<Client::ptr> AllClients;
};
