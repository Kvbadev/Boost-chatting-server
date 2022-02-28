#include <boost/asio/placeholders.hpp>
#include <boost/asio/read_until.hpp>
#include <boost/system/detail/error_code.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <vector>

using boost::asio::ip::tcp;

class Client{
public:
    typedef boost::shared_ptr<Client> ptr;
    Client(std::string address){
        this->id = count++;
        this->address = address;
    }
    static void create(std::string addr){
        AllClients.push_back(ptr(new Client(addr)));
    }
    static int getCount(){
        return count;
    }
    int getId(){
        return this->id;
    }
    std::string getAddr(){
        return this->address;
    }
private:
    static inline int count = 0;
    static inline std::vector<ptr> AllClients;
    int id;
    std::string address;
};

class Connection : public boost::enable_shared_from_this<Connection>{

public:
    typedef boost::shared_ptr<Connection> ptr;

    static ptr create(boost::asio::io_context& io_context){
        return ptr(new Connection(io_context));
    }

    tcp::socket& socket(){
        return socket_;
    }
    void read_opt(){
        boost::asio::async_read(socket_, boost::asio::buffer(option, 1),
                                boost::bind(&Connection::opt_handler, shared_from_this(),
                                boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred)
                                );
    }
    void opt_handler(const boost::system::error_code &e, size_t){
        if(e)
            std::cerr<<"opt_handler: "<<e.what()<<std::endl;
        else{
            std::cout<<"You chose option: "<<option<<std::endl;
            opt_executer();
        }
    }
    void opt_executer(){
        switch(*option){
            case 's':
                get_msg();
                break;
        }
    }
    bool get_msg(){
        try{
            boost::asio::async_read_until(socket_, message, '\n', 
                         boost::bind(&Connection::read_handler, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
            return 1;
        }
        return 0;
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
        if(!error){
            Client::create(conn->socket().remote_endpoint().address().to_string());
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
        throw ;
    }
    return 0;
}
