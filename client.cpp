#include <boost/bind/bind.hpp>
#include <boost/system/detail/error_code.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <string>

using boost::asio::ip::tcp;

class Client{
public:

    Client(boost::asio::io_context &io_context_) : socket_(tcp::socket(io_context_))
    {}

    void print_help(){
        std::cout<<"h - to show that message\n";
        std::cout<<"p - to show all active clients(in construction)\n";
        std::cout<<"S - to send message to particular client(in construction)\n";
        std::cout<<"s - to send message to server\n";
        std::cout<<"Select: ";
    }
    void set_opt(){
        std::cin>>option;
        std::cin.ignore();
    }
    bool send_opt(){
        try{
            boost::asio::write(socket_, boost::asio::buffer(option));
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
            return 1;
        }
        return 0;
    }
    void execute(){
        switch(*option){
            case 'h':
                print_help();
                break;
            case 's':
                send_opt();
                send_msg(get_msg());
                break;
            case 'p':
                send_opt();
                get_clients();
                //curly braces because jumping initialization error without because of switch
                {
                /* std::istream is(&clients_list_); */
                /* std::string line; */
                /* std::getline(is, line); */
                std::cout<<clients_list_<<std::endl;
                }
                break;
            default:
                std::cerr<<"Incorrect character!"<<std::endl;
                break;
        }
    }
    void get_clients(){
        try{
            boost::asio::read(socket_, boost::asio::buffer(clients_list_, 11));
        } catch(std::exception &e){
            std::cerr<<"get_clients: "<<e.what()<<std::endl;
        }
    }
    bool connect_to_server(std::string ip, int port){
        boost::system::error_code e;
        try{
            tcp::endpoint end = tcp::endpoint(boost::asio::ip::address::from_string(ip), port);
            socket_.connect(end);
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
            return 1;
        }
        return 0;
    }
    std::string get_msg(){
        std::string message;
        std::cout<<"Enter your message: ";
        std::getline(std::cin, message);
        return (message+='\n');
    }
    void send_msg(std::string message){
        try{
            boost::asio::write(socket_, boost::asio::buffer(message));
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
    }
    void handler(boost::system::error_code &e, size_t){
        if(e)
            std::cerr<<e.what()<<std::endl;
        else
            std::cout<<"Message sent!"<<std::endl;
    }
private:
    tcp::socket socket_;
    char option[1];
    char clients_list_[11];
};

int main(){
    std::cout<<"Welcome in chat client!"<<std::endl;
    boost::asio::io_context ioc;
    std::string server_ip = "192.168.1.241";
    Client c(ioc);
    if((c.connect_to_server(server_ip, 13))==1)
        std::cerr<<"Can't connect to the server"<<std::endl;
    else{
        c.print_help();
        for(;;){
            c.set_opt();
            c.execute();
        }
        /* char mes[6]; */
        /* c.set_opt(); */
        /* c.send_opt(); */
        /* boost::asio::read(c.socket_, boost::asio::buffer(mes, 6)); */ 
        /* std::cout<<mes; */
    }
    return 0;
}
