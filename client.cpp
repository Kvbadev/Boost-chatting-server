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
        std::cout<<"o - check your messages\n";
        std::cout<<"Select: ";
    }
    char* set_opt(){
        try{
            char *opt = new char[1];
            std::cin>>opt;
            return opt;
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
            return 0;
        }
    }
    bool send_opt(char *option){
        try{
            boost::asio::write(socket_, boost::asio::buffer(option, 1));
            delete option;
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
            return 1;
        }
        return 0;
    }
    void execute(char* opt){
        switch(*opt){
            case 'h':
                print_help();
                break;
            case 's':
                send_opt(opt);
                send_len_and_msg(get_msg());
                break;
            case 'p':
                send_opt(opt);
                get_clients();
                break;
            case 'S':
                send_opt(opt);
                {
                //max 9 at the moment
                int n = getClientNum();
                std::string message = get_msg();
                message = std::to_string(n) + message;
                send_len_and_msg(message);
                break;
                }
            case 'o':
                send_opt(opt);
                break;
            default:
                std::cerr<<"Incorrect character!"<<std::endl;
                break;
        }
    }
    int getClientNum(){
        int num;
        std::cout<<"Select client that You want to send message to: ";
        std::cin>>num;
        return num;
    }
    void get_clients(){
        char *msg_buf;
        try{
            int msg_len = std::stoi(getMsgLength());
            msg_buf = new char[msg_len];
            boost::system::error_code e;
            socket_.read_some(boost::asio::buffer(msg_buf, msg_len), e);
            if(e)
                std::cerr<<e.what()<<std::endl;
            else{
                std::cout.write(msg_buf, msg_len);
                std::flush(std::cout);
            }
            delete[] msg_buf;
        } catch(std::exception &e){
            std::cerr<<"get_clients: "<<e.what()<<std::endl;
        }
    }
    std::string getMsgLength(){
        boost::array<char, 4> x; boost::system::error_code e;
        socket_.read_some(boost::asio::buffer(x), e);
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
        std::cin.ignore();
        std::cout<<"Enter your message: ";
        std::getline(std::cin, message);
        return message;
    }
    void send_len_and_msg(std::string message){
        try{
            std::string len = std::to_string(message.length());
            changeLen(len);
            std::cout<<"len: "<<len<<std::endl;
            boost::system::error_code e;
            boost::asio::write(socket_, boost::asio::buffer(len, 3),e);
            send_msg(message);
            if(e)
                std::cerr<<e.what()<<std::endl;
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
    }
    void changeLen(std::string &mes){
        if(mes.size()==1)
            mes.insert(0, 2, '-');
        else if(mes.size()==2)
            mes.insert(0,1,'-');
    }
    void send_msg(std::string message){
        try{
            boost::asio::write(socket_, boost::asio::buffer(message));
            std::cout<<"message: '"<<message<<"' sent!\n";
        }
        catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
        }
    }
private:
    tcp::socket socket_;
};

int main(){
    std::cout<<"Welcome in chat client!"<<std::endl;
    boost::asio::io_context ioc;
    std::string server_ip = "192.168.93.144";
    Client c(ioc);
    if((c.connect_to_server(server_ip, 13))==1)
        std::cerr<<"Can't connect to the server"<<std::endl;
    else{
        c.print_help();
        for(;;){
            c.execute(c.set_opt());
        }
    }
    return 0;
}
