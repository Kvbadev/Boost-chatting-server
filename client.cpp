#include <boost/bind/bind.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>

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
    void execute(){
        switch(option){
            case 'h':
                print_help();
                break;
            case 's':
                send_to_server();
                break;
            default:
                std::cerr<<"Incorrect character!"<<std::endl;
        }
    }
    void send_to_server(){
        std::string message;
        boost::system::error_code e;
        std::cout<<"Enter your message: ";
        std::getline(std::cin, message);
        message += '\n';
        try{
            boost::asio::ip::address ip = boost::asio::ip::address::from_string("192.168.1.241", e);
            tcp::endpoint end = tcp::endpoint(ip, 13);
            socket_.connect(end, e);
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
    char option;
    tcp::socket socket_;
};

int main(){

    std::cout<<"Welcome in chat client!"<<std::endl;
    boost::asio::io_context ioc;
    Client c(ioc);
    for(;;){
        c.print_help();
        c.set_opt();
        c.execute();
    }
    std::string h;
    std::getline(std::cin, h);
    std::cout<<h;

    return 0;
}
