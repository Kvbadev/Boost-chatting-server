#include "client.hpp"
#include "connection.hpp"

Client::Client(std::string addr, Connection::ptr x){
    id = count++;
    client_conn = x;
}
std::string Client::get_address(){
    return this->client_conn->socket().remote_endpoint().address().to_string();
}
int Client::getId(){
    return this->id;
}
