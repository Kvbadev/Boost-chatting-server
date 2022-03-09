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

    static std::string get_clients_data(int ownID){
        try{
            std::string text;
            for(int i=0;i<count;i++){
                text+=std::to_string(AllClients.at(i)->get_id());
                text+=" - ";
                text+=AllClients.at(i)->get_address();
                if(AllClients.at(i)->get_id()==ownID)
                    text+=" (Me)";
                text+='\n';
            }
            return text;
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
        return 0;
    }
    auto get_inbox(){
        return this->inbox;
    }
    static void create_client(Connection::ptr x){
        AllClients.push_back(Client::ptr(new Client(x)));
    }
    /* static void remove_client(int id){ */
    /*     auto x = AllClients.begin(); */
    /*     AllClients.erase((AllClients.begin()+id)); */
    /*     Client::count--; */
    /* } */
    static auto get_AllClients(){
        return AllClients;
    }
    static std::string get_messages(int id){
        try{
            std::string data;
            auto x = AllClients.at(id)->get_inbox();
            for(auto i = x.begin(); i!=x.end();i++){
                data+=std::to_string(i->sender_id);
                data+=" - ";
                data+=i->message;
                data+='\n';
            }
            return data;
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
        return 0;
    }
    static inline int count = 0;
private:
    Connection::ptr client_conn;
    std::vector<inbox_message> inbox;
    // inline to prevent reserving memory for variable in the other place in the program
    static inline std::vector<Client::ptr> AllClients;
};
