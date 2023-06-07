#include "server.hpp"

server::server(io_service& io_service, io_service::strand& strand, tcp::endpoint endpoint)
	: io_service_(io_service),
	  strand_(strand),
	  acceptor_(io_service, endpoint)	  
{
	start_accept();
}

void server::start_accept()
{
	shared_ptr<session> new_session = std::make_shared<session>(io_service_, strand_, db_u_, db_o_, parser_);
		acceptor_.async_accept(new_session->socket(), 
			strand_.wrap(bind(&server::handle_accept, this, new_session, _1)));
}

void server::handle_accept(shared_ptr<session> new_session, const error_code& ec)
{
	if (!ec)
	{	cout << "Starting new session\n";
		new_session->start();
	}
	else
	{
		cout << "Accept error: " << ec.message() << "\n";
	}
	
	start_accept();
}
