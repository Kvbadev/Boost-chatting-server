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
                                                , acceptor_(tcp::acceptor(io_context,tcp::v4()))
                                                , socket_(tcp::socket(io_context))
    {
        std::cout<<"Server has been created!"<<std::endl;
    }
    sock_ptr start(){
        sock_ptr tmp = sock_gen(ios);
        /* acceptor_.async_accept(socket_, */
        /*                       boost::bind(&Server::AcHandler, this, boost::asio::placeholders::error) */
        /*                       ); */
        return tmp;
    }
    void AcHandler(const boost::system::error_code& er){
        if(er)
            std::cerr<<er.what()<<std::endl;
        else
            std::cout<<"Correct!";
    }
    sock_ptr sock_gen(boost::asio::io_context &io){
        return sock_ptr(new tcp::socket(io));
    }
private:
    boost::asio::io_context& ios;
    tcp::acceptor acceptor_;
    tcp::socket socket_;
};

void accept_handler(const boost::system::error_code& error){
    if (!error){
        /* x=1; */
        /* std::cerr<<error.what()<<std::endl; */
    /* else */
        std::cout<<"Okay!"<<std::endl;
    }
}

int main(){
    try{
        boost::asio::io_context io_con;
        Server server(io_con);
        /* Server::sock_ptr client_sock = server.start(); */
        tcp::socket sock(io_con);
        tcp::acceptor acptr(io_con, tcp::endpoint(tcp::v4(), 13));
        for(;;){
            acptr.async_accept(sock, boost::bind(accept_handler, std::placeholders::_1));
            io_con.run();
        }
    }
    catch(boost::system::error_code &e){
        throw e.what();
    }
    catch(...){
        throw ;
    }

    return 0;
}
