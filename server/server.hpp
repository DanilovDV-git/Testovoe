#ifndef SERVER_HPP
#define SERVER_HPP

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <memory>
#include <iostream>
#include <string>
#include <pqxx/pqxx>
#include "db_handler.hpp"
#include "session.hpp"
#include "parser.hpp"


using namespace boost::asio;
using namespace boost::system;
using ip::tcp;
using boost::bind;
using std::cout;
using std::shared_ptr;

class server
{
public:
	server(io_service& io_service, io_service::strand& strand, tcp::endpoint endpoint);
private:
	void start_accept();
	void handle_accept(shared_ptr<session> new_session, const error_code& ec);
private:
	io_service& io_service_;
	io_service::strand& strand_;
	tcp::acceptor acceptor_;
	db_users db_u_;
	db_operations db_o_;
	parser parser_;
};

#endif
