#include <boost/bind/bind.hpp>
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
            default:
                std::cerr<<"Incorrect character!"<<std::endl;
        }
    }
    bool connect_to_server(std::string ip, int port){
        boost::system::error_code e;
        try{
            tcp::endpoint end = tcp::endpoint(boost::asio::ip::address::from_string(ip, e), port);
            socket_.connect(end, e);
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
    char option[1];
    tcp::socket socket_;
};

int main(){
    std::cout<<"Welcome in chat client!"<<std::endl;
    boost::asio::io_context ioc;
    std::string server_ip = "192.168.1.241";
    Client c(ioc);
    c.connect_to_server(server_ip, 13);
    if(c.connect_to_server(server_ip, 13))
        std::cerr<<"Can't connect to the server"<<std::endl;
    else{
        for(;;){
            c.print_help();
            c.set_opt();
            c.execute();
        }
    }
    return 0;
}
