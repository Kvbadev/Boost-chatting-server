#include <iostream>
#include <boost/array.hpp>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>

using boost::asio::ip::tcp;

class Connection : public boost::enable_shared_from_this<Connection>{

public:
    typedef boost::shared_ptr<Connection> ptr;

    static ptr create(boost::asio::io_context& io_context){
        return ptr(new Connection(io_context));
    }

    tcp::socket& socket(){
        return socket_;
    }
    void read(){
        boost::asio::async_read_until(socket_, message,'\n',
                                boost::bind(&Connection::read_handler, shared_from_this(),
                                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
                                );
    }
    void read_handler(const boost::system::error_code& e, size_t){
        if(e)
            std::cerr<<"read_handler: "<<e.what()<<std::endl;
        std::istream is(&message);
        std::string line;
        std::getline(is, line);
        std::cout<<line<<std::endl;
    }

private:
    Connection(boost::asio::io_context &io_context_) : socket_(tcp::socket(io_context_))
    {
    }
    tcp::socket socket_;
    boost::asio::streambuf message;
    char option[1];
};

class Server{ 

public:
    Server(boost::asio::io_context &io_context) : ios(io_context)
                                                , acceptor_(tcp::acceptor(io_context, tcp::endpoint(tcp::v4(), 13)))
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
        if(!error)
            conn->read();
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
        throw ;
    }
    return 0;
}
