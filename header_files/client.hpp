#include <boost/asio.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/array.hpp>
#include <iterator>
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
            for(auto i=AllClients.begin();i!=AllClients.end();i++){
                text+=std::to_string(i->get()->get_id());
                text+=" - ";
                text+=i->get()->get_address();
                if(i->get()->get_id()==ownID)
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
        return inbox;
    }
    static void create_client(Connection::ptr x){
        AllClients.push_back(Client::ptr(new Client(x)));
        x->set_id(get_best_id());
        /* std::cout<<"new size (a):"<<AllClients.size()<<std::endl; */
        /* std::cout<<"new client id: "<<x->get_id()<<std::endl; */
    }
    static auto getElemById(int id){
        try{
            for(auto i=AllClients.begin(); i!=AllClients.end(); i++){
                if(i->get()->get_id()==id){
                    return i;
                }
            }
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
        return AllClients.end(); //error if it reaches that
    }
    static void remove_client(size_t id){
        /* std::cout<<"removed client id: "<<getElemById(id)->get()->get_id()<<std::endl; */
        AllClients.erase(getElemById(id));
        /* std::cout<<"new size (d):"<<AllClients.size()<<std::endl; */
    }
    static size_t get_best_id(){
        try{
            const int max_clients_num = 9;
            bool flag = true;
            if(AllClients.size()==0)
                return 0;
            else{
                for(int i=0;i<max_clients_num;i++){
                    for(auto j = AllClients.begin(); j!=AllClients.end()-1; j++){
                        if(j->get()->get_id()==i){
                            flag = false;
                            break;
                        }
                    }
                    if(flag)
                        return i;
                    flag = true;
                }
            }
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
        return -1;
    }
    static std::vector<Client::ptr> get_AllClients(){
        return AllClients;
    }
    static std::string get_messages(int id){
        try{
            std::string data;
            /* auto x = AllClients.at(getElemById(id))->get_inbox(); */
            auto x = getElemById(id)->get()->get_inbox();
            for(auto i = x.begin(); i!=x.end();i++){
                data+=std::to_string(i->sender_id);
                data+=" - ";
                data+=i->message;
                data+='\n';
            }
            if(!(data.size()))
                data += "No messages!\n";
            return data;
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
        return 0;
    }
private:
    Connection::ptr client_conn;
    std::vector<inbox_message> inbox;
    // inline to prevent reserving memory for variable in the other place in the program
    static inline std::vector<Client::ptr> AllClients;
};
