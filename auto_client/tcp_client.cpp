
#include <array>
#include <thread>
#include <iostream>
#include <cstring>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <chrono>
#include "protocol.hpp"

using namespace std::chrono_literals;
using namespace boost::asio;
using namespace boost::system;
using ip::tcp;
using std::cout;
using std::cin;
using std::getline;
using boost::bind;
using std::array;

class client
{
public:
	client(io_service& io_service, io_service::strand& strand, tcp::resolver::iterator endpoint_iter, 
			array<char, LOGIN_MAX_LENGTH> login, array<char, PASSWORD_MAX_LENGTH> password)
		: io_service_(io_service),
		  strand_(strand),
		  socket_(io_service),
		  stopped_(false),
		  login_(login),
		  password_(password)
	{
		//strcpy(login_.data(), login.data());
		//strcpy(password_.data(), password.data());
		//memset(read_message_.data(), '\0', MESSAGE_MAX_LENGTH);
		async_connect(socket_, endpoint_iter, bind(&client::handle_connect, this, _1));
		
	}
	
	void close()
	{
		io_service_.post(bind(&client::start_close, this));
	}
	
private:
	void handle_connect(const error_code& ec)
	{
		if (stopped_)
			return;
			
		if (!ec)
		{	
			start_write_login();
		}
		else
		{
			cout << "Connect error: " << ec.message() << "\n";
			close();
		}
	}
	
	void start_write_login()
	{
		if (stopped_)
			return;
		//cout << "Login: ";
		//if (!cin.getline(login_.data(), LOGIN_MAX_LENGTH))
		//	cin.clear();
		
		async_write(socket_, buffer(login_, login_.size()), 
			strand_.wrap(bind(&client::handle_write_login, this, _1)));
		
	}
	
	void handle_write_login(const error_code& ec)
	{
		if (stopped_)
			return;
			
		if (!ec)
		{
			async_read(socket_, buffer(read_message_, read_message_.size()),
				strand_.wrap(bind(&client::handle_read_verification_login, this, _1)));
		}
		else
		{
			cout << "Write login error: " << ec.message() << "\n";
			close();
		}
	}
	
	void handle_read_verification_login(const error_code& ec)
	{
		if (stopped_)
			return;
			
		if (!ec)
		{	
			if (!memcmp(read_message_.data(), "Login verification success", 26))
			{	
				start_write_password();			
			}
			else
			{
				cout << "Invalid login\n";
				close();
			}
		}
		else
		{
			cout << "Read verification login error: " << ec.message() << "\n";
			close();
		}
	}
	
	void start_write_password()
	{
		if (stopped_)
			return;
		//out << "Password: ";
		//memset(password_.data(), '\0', password_.size());
		//if (!cin.getline(password_.data(), PASSWORD_MAX_LENGTH))
		//	cin.clear();
		
		async_write(socket_, buffer(password_, password_.size()), 
			strand_.wrap(bind(&client::handle_write_password, this, _1)));
	}
	
	void handle_write_password(const error_code& ec)
	{
		if (stopped_)
			return;
		if (!ec)
		{
			async_read(socket_, buffer(read_message_, read_message_.size()),
				strand_.wrap(bind(&client::handle_read_verification_password, this, _1)));
		}
		else
		{
			cout << "Write password error: " << ec.message() << "\n";
			close();
		}
	}
	
	void handle_read_verification_password(const error_code& ec)
	{
		if (stopped_)
			return;
		if (!ec)
		{
			if (!memcmp(read_message_.data(), "Password verification success", 29))
			{
				//async_read(socket_, buffer(read_message_, read_message_.size()), 
				//	strand_.wrap(bind(&client::handle_read, this, _1)));
				start_write();
							
			}
			else
			{
				cout << "Invalid password\n";
				close();
			}
		}
		else
		{
			cout << "Read verification login error: " << ec.message() << "\n";
			close();
		}
	}
	
	void start_write()
	{	
		if (stopped_)
			return;
		cout << "Command: ";
		memset(write_message_.data(), '\0', write_message_.size());
		std::istringstream ist("calc 2 + 2 / 2 * 5\n");
		ist.getline(write_message_.data(), MESSAGE_MAX_LENGTH);
		
		//std::this_thread::sleep_for(50ms);
		
		async_write(socket_, buffer(write_message_, write_message_.size()), 
			strand_.wrap(bind(&client::handle_write, this, _1)));
	}
	
	void handle_write(const error_code& ec)
	{
		if (stopped_)
			return;
		if (!ec)
		{
			async_read(socket_, buffer(read_message_, read_message_.size()),
				strand_.wrap(bind(&client::handle_read, this, _1)));
		}
		else
		{
			cout << "Write error: " << ec.message() << "\n";
			close();
		}
	}
	
	void handle_read(const error_code& ec)
	{
		if (stopped_)
			return;
		if (!ec)
		{
			cout << "Server: " << read_message_.data() << std::endl;
			if (!memcmp(read_message_.data(), "Your balance is 0", 17))
				close();
			start_write();
		}
		else
		{
			cout << "Read error: " << ec.message() << "\n";
			close();
		}
	}
	
	void start_close()
	{
		stopped_ = true;
		socket_.close();
	}
	
private:
	io_service& io_service_;
	io_service::strand& strand_;
	tcp::socket socket_;
	array<char, MESSAGE_MAX_LENGTH> read_message_;
	array<char, MESSAGE_MAX_LENGTH> write_message_;
	array<char, LOGIN_MAX_LENGTH> login_;
	array<char, PASSWORD_MAX_LENGTH> password_;
	bool stopped_;
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 5)
		{
			std::cerr << "Usage: tcp_client <host> <port>\n";
			return 1;
		}
		
		io_service io_service;
		io_service::strand strand(io_service);
		
		tcp::resolver resolver(io_service);
		tcp::resolver::query query(argv[1], argv[2]);
		tcp::resolver::iterator iterator = resolver.resolve(query);
		
		array<char, LOGIN_MAX_LENGTH> login;
		array<char, PASSWORD_MAX_LENGTH> password;
		
		strcpy(login.data(), argv[3]);
		strcpy(password.data(), argv[4]);
		
		client c(io_service, strand, iterator, login, password);
		
		io_service.run();
	}
	catch(std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}
	
	return 0;
}





















