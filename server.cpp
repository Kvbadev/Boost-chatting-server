#include <boost/asio/registered_buffer.hpp>
#include <boost/system/detail/error_code.hpp>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>

using boost::asio::ip::tcp;

class Server{ 

public:
    typedef boost::shared_ptr<tcp::socket> sock_ptr;
    Server(boost::asio::io_context &io_context) : ios(io_context)
                                                , acceptor_(tcp::acceptor(io_context, tcp::endpoint(tcp::v4(), 13)))
    {
        std::cout<<"Server has been created!"<<std::endl;
    }
    void start(){
        sock_ptr sockt = sock_gen(ios);
        acceptor_.async_accept(*sockt.get(),
                              boost::bind(&Server::accept_handler, this, sockt, boost::asio::placeholders::error)
                              );
    }
private:
    sock_ptr sock_gen(boost::asio::io_context &io){
        return sock_ptr(new tcp::socket(io));
    }
    void accept_handler(sock_ptr sock, const boost::system::error_code& error){
        if(!error){
            std::cout<<"Connection Established!"<<std::endl;
            std::cout<<"Sending message..."<<std::endl;  
            boost::asio::async_write(*sock.get(),  boost::asio::buffer("Test message\n"), 
                                    [](const boost::system::error_code& e, size_t bytes){if(!e) std::cout<<"Message has been sent!\n";}
                                    );
            (*sock.get()).close();
        } else{
            std::cerr<<error.what()<<std::endl;
        }
        start();
            
    }
    boost::asio::io_context& ios;
    tcp::acceptor acceptor_;
};


int main(){
    try{
        boost::asio::io_context io_con;
        Server server(io_con);
        server.start();
        io_con.run();
    }
    catch(boost::system::error_code &e){
        throw e.what();
    }
    catch(...){
        throw ;
    }
    return 0;
}
