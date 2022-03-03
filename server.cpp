#include <boost/asio/placeholders.hpp>
#include <boost/system/detail/error_code.hpp>
#include <exception>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/bind/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <string>
#include <vector>

using boost::asio::ip::tcp;

class Client{
public:
    typedef boost::shared_ptr<Client> ptr;
    Client(std::string add){
        id = 1;
        count++;
        address = add;
    }
    static void create(std::string addr){
        AllClients.push_back(ptr(new Client(addr)));
    }
    static int getCount(){
        return count;
    }
    /* static void get_clients_data(std::string &text){ */
    /*     text = "Hello World"; */
    /*     /1* for(int i=0;i<count;i++){ *1/ */
    /*     /1*     text+=AllClients[i]->address; *1/ */
    /*     /1*     text+='\n'; *1/ */
    /*     /1* } *1/ */
    /* } */
    int getId(){
        return this->id;
    }
    std::string getAddr(){
        return this->address;
    }
private:
    // inline to prevent reserving memory for variable in the other place in the program
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
            std::cout<<"You have chosen option: "<<option<<std::endl;
            opt_executer();
        }
    }
    void opt_executer(){
        switch(*option){
            case 's':
                get_msg();
                break;
            case 'p':
                clients = "Hi my name is slkfskldfjklsjlkfjsldjf and lollsjfkdsjlfs";
                send_msg_length(clients.length());
                break;
        }
    }
    bool send_all_clients(const boost::system::error_code &e, size_t, int l){
        if(e)
            std::cerr<<e.what()<<std::endl;
        try{
            boost::asio::async_write(socket_, boost::asio::buffer(clients, l),
                                     boost::bind(&Connection::send_clients_handler, shared_from_this(),
                                     boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred));
        } catch(std::exception &e){
            std::cerr<<e.what()<<std::endl;
            return 1;
        }
        return 0;
    }
    void send_msg_length(int l){
        std::cout<<"Length: "<<std::to_string(l)<<'\n';
        boost::asio::async_write(socket_, boost::asio::buffer(std::to_string(l),sizeof(int)), 
                                boost::bind(&Connection::send_all_clients, shared_from_this(), boost::asio::placeholders::error, boost::asio::placeholders::bytes_transferred, l));
    }
    void send_clients_handler(const boost::system::error_code &e, size_t b){
        if(e)
            std::cerr<<e.what()<<std::endl;
        else
            std::cout<<"x sent correctly: "<<b<<std::endl;
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
    std::string clients;
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
