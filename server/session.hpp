#ifndef SESSION_HPP
#define SESSION_HPP

#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/asio/steady_timer.hpp>
#include <ctime>
#include <string>
#include <sstream>
#include <iostream>
#include <memory>
#include <chrono>
#include "protocol.hpp"
#include "db_handler.hpp"
#include "parser.hpp"

using namespace boost::asio;
using namespace boost::system;
using ip::tcp;
using boost::bind;
using std::cout;
using std::shared_ptr;
using std::array;
using std::string;

class session : public std::enable_shared_from_this<session>
{
public:
	session(io_service& io_service, io_service::strand& strand, db_users& db_u, db_operations& db_o, parser& parser);
	tcp::socket& socket();
	void start();
private:
	void stop(); 
	void handle_timer_wait(const error_code& ec);
	void handle_read_login(const error_code& ec);
	void handle_write_login_verification(bool login_verified, const error_code& ec);
	void handle_read_password(const error_code& ec);
	void handle_write_password_verification(bool password_verified, const error_code& ec);
	void handle_read(const error_code& ec);
	void handle_write(const error_code& ec);
private:
	tcp::socket socket_;
	io_service::strand& strand_;
	array<char, LOGIN_MAX_LENGTH> login_;
	array<char, PASSWORD_MAX_LENGTH> password_;
	array<char, MESSAGE_MAX_LENGTH> read_message_;
	array<char, MESSAGE_MAX_LENGTH> write_message_;
	shared_ptr<user> user_;
	db_users& db_u_;
	db_operations& db_o_;
	parser& parser_;
	bool stopped_;
	bool verified_;
	steady_timer timer_;
};

#endif
