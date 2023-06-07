#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <iostream>
#include <string>
#include <exception>
#include <pqxx/pqxx>
#include "server.hpp"

using namespace boost::asio;
using namespace boost::system;
using ip::tcp;
using std::string;
using std::cout;

int main(int argc, char* argv[])
{
	try
	{
		if (argc < 2)
		{
			std::cerr << "Usage: tcp_server <port> [<port> ...]\n";
			return 1;
		}
		
		io_service io_service;
		io_service::strand strand(io_service);
		
		cout << "Server starts\n";
		tcp::endpoint endpoint(tcp::v4(), std::atoi(argv[1]));;
		server server(io_service, strand, endpoint);
		
		io_service.run();
		
	}
	catch(const std::exception& ex)
	{
		std::cerr << "Exception: " << ex.what() << "\n";
	}
	
	
	cout << "End of programm\n";
	return 0;
}
