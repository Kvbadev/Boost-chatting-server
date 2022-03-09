#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/array.hpp>
#include <string>
#include <vector>

#define SERVER_PORT 3033

/* #include "header_files/connection.hpp" //not required */
#include "header_files/client.hpp"

using boost::asio::ip::tcp;

class Server{ 

public:
    Server(boost::asio::io_context &io_context) : ios(io_context)
                                                , acceptor_(tcp::acceptor(io_context, tcp::endpoint(tcp::v4(), SERVER_PORT)))
    {
        std::cout<<"Server has been created!"<<std::endl;
    }
    void start(){
        Connection::ptr new_connection = Connection::create(ios);
        acceptor_.async_accept(new_connection->socket(),
                              boost::bind(&Server::accept_handler, this, new_connection, boost::asio::placeholders::error)
                              );
    }
private:
    void accept_handler(Connection::ptr conn, const boost::system::error_code& error){
        if(!error){
            Client::create_client(conn);
            conn->read_opt();
        }
        else
            std::cerr<<error.what()<<std::endl;
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
        throw;
    }
    return 0;
}


