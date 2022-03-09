#include "client.hpp"
#include "connection.hpp"

Client::Client(Connection::ptr x){
    client_conn = x;
    count++;
}
std::string Client::get_address(){
    return this->client_conn->socket().remote_endpoint().address().to_string();
}
int Client::get_id(){
    return this->client_conn->get_id();
}

void Client::add_message(int id, std::string message){
    inbox.push_back(inbox_message{id, message});
}
