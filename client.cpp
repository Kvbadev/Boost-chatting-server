#include <boost/bind/bind.hpp>
#include <boost/system/detail/error_code.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/array.hpp>
#include <string>

using boost::asio::ip::tcp;

class Client{
public:

    Client(boost::asio::io_context &io_context_) : socket_(tcp::socket(io_context_))
    {}

    void print_help(){
        std::cout<<"h - to show that message\n";
        std::cout<<"p - to show all active clients\n";
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
                {
                std::string message = get_msg();
                send_msg_len(message.length(), message);
                }
                break;
            case 'p':
                send_opt();
                get_clients();
                break;
            default:
                std::cerr<<"Incorrect character!"<<std::endl;
                break;
        }
    }
    void get_clients(){
        char *msg_buf;
        try{
            int msg_len = std::stoi(getMsgLength());
            msg_buf = new char[msg_len];
            boost::system::error_code e;
            boost::asio::read(socket_, boost::asio::buffer(msg_buf, msg_len), e);
            if(e)
                std::cerr<<e.what()<<std::endl;
            else
                std::cout.write(msg_buf, msg_len);
        } catch(std::exception &e){
            std::cerr<<"get_clients: "<<e.what()<<std::endl;
        }
        delete[] msg_buf;
    }
    std::string getMsgLength(){
        boost::array<char, 4> x; boost::system::error_code e;
        socket_.read_some(boost::asio::buffer(x, sizeof(int)), e);
        if(e){
            if(e == boost::asio::error::eof){
                std::cerr<<"Server does not respond"<<std::endl;
                exit(0);
            }
            else
                std::cerr<<e.what()<<std::endl;
        }
            
        std::string b;
        for(auto i=x.begin(); i!=x.end(); i++)
            b += *i;
        return b;
    }
    bool connect_to_server(std::string ip, int port){
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
        return message;
    }
    void send_msg_len(int len, std::string message){
        try{
            std::cout<<"len: "<<len<<std::endl;
            boost::system::error_code e;
            boost::asio::write(socket_, boost::asio::buffer(std::to_string(len), sizeof(int)),e);
            if(e)
                std::cerr<<e.what()<<std::endl;
            send_msg(message);
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
    }
    void send_msg(std::string message){
        try{
            /* boost::asio::write(socket_, boost::asio::buffer(message)); */
            std::cout<<"message: '"<<message<<"' sent!\n";
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
    }
private:
    tcp::socket socket_;
    char option[1];
    boost::asio::streambuf clients_list_;
};

int main(){
    std::cout<<"Welcome in chat client!"<<std::endl;
    boost::asio::io_context ioc;
    std::string server_ip = "192.168.0.77";
    Client c(ioc);
    if((c.connect_to_server(server_ip, 13))==1)
        std::cerr<<"Can't connect to the server"<<std::endl;
    else{
        c.print_help();
        for(;;){
            c.set_opt();
            c.execute();
        }
    }
    return 0;
}
