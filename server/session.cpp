#include "session.hpp"

session::session(io_service& io_service, io_service::strand& strand, db_users& db_u, db_operations& db_o, parser& parser)
	: socket_(io_service), 
	  strand_(strand), 
	  db_u_(db_u), 
	  db_o_(db_o), 
	  parser_(parser), 
	  stopped_(false), 
	  verified_(false), 
	  timer_(io_service)
{
	 memset(login_.data(), '\0', LOGIN_MAX_LENGTH);
	 memset(password_.data(), '\0', PASSWORD_MAX_LENGTH);
	 memset(read_message_.data(), '\0', MESSAGE_MAX_LENGTH);
	 memset(write_message_.data(), '\0', MESSAGE_MAX_LENGTH);
}

tcp::socket& session::socket() { return socket_; }

void session::start()
{
	// Start async operations
	
	// Timer which waits login and password from user
	timer_.expires_after(std::chrono::seconds(10));
	timer_.async_wait(strand_.wrap(bind(&session::handle_timer_wait, this, _1)));
	
	// Read login
	async_read(socket_, buffer(login_, login_.size()), 
		strand_.wrap(bind(&session::handle_read_login, shared_from_this(), _1)));
}

void session::stop()
{
	// This method closes connection on socket and deletes connected user from database handler
	stopped_ = true;
	socket_.close();
	db_u_.remove_connected_user(string(login_.data()));
}

void session::handle_timer_wait(const error_code& ec)
{
	if (stopped_)
		return;
		
	if (!ec)
	{
		if (!verified_)
		{
			// Timeout for user verification
			cout << "Verification timeout\n";
			stop();
		}
	}
}

void session::handle_read_login(const error_code& ec)
{
	if (stopped_) 
		return;
	
	if (!ec)
	{
		cout << "Login: " << login_.data() << "\n";
		
		// Check the login correction and form answer
		if (db_u_.find_user(string(login_.data())))
		{
			// Login is correct
			std::istringstream ist("Login verification success\n");
			ist.getline(write_message_.data(), MESSAGE_MAX_LENGTH);
		
			async_write(socket_, buffer(write_message_, write_message_.size()), 
				strand_.wrap(bind(&session::handle_write_login_verification, 
					shared_from_this(), true, _1)));
		}
		else
		{
			// Login is invalid
			std::istringstream ist("Login verification failed\n");
			ist.getline(write_message_.data(), MESSAGE_MAX_LENGTH);
		
			async_write(socket_, buffer(write_message_, write_message_.size()), 
				strand_.wrap(bind(&session::handle_write_login_verification, 
					shared_from_this(), false, _1)));
		}
		
	}
	else
	{
		cout << "Read login error: " << ec.message() << "\n";
		stop();
	}
}

void session::handle_write_login_verification(bool login_verified, const error_code& ec)
{
	if (stopped_) 
		return;
		
	if (!ec)
	{
		if (login_verified)
			// Start reading password if login is verified
			async_read(socket_, buffer(password_, password_.size()),
				strand_.wrap(bind(&session::handle_read_password, shared_from_this(), _1)));
		else
			stop();
	}
	else
	{
		cout << "Write login verification error: " << ec.message() << "\n";
		stop();
	}
}

void session::handle_read_password(const error_code& ec)
{
	if (stopped_) 
		return;
		
	if (!ec)
	{
		cout << "Password: " << password_.data() << "\n";
		
		// Check the password correction and from answer
		user_ = db_u_.get_user(string(login_.data()), string(password_.data())); // Get user info from database
		if (user_)
		{
			// Password is correct
			verified_ = true;
			cout << "User [" << login_.data() << "] verified\n";
			
			std::istringstream ist("Password verification success\n");
			ist.getline(write_message_.data(), MESSAGE_MAX_LENGTH);
		
			async_write(socket_, buffer(write_message_, write_message_.size()), 
				strand_.wrap(bind(&session::handle_write_password_verification, 
					shared_from_this(), true, _1)));
		}
		else
		{
			// Password is invalid
			cout << "Get user error\n";
			std::istringstream ist("Password verification failed\n");
			ist.getline(write_message_.data(), MESSAGE_MAX_LENGTH);
		
			async_write(socket_, buffer(write_message_, write_message_.size()), 
				strand_.wrap(bind(&session::handle_write_password_verification, 
					shared_from_this(), false, _1)));
		}
		
	}
	else
	{
		cout << "Read password error: " << ec.message() << "\n";
		stop();
	}
}

void session::handle_write_password_verification(bool password_verified, const error_code& ec)
{
	if (stopped_) 
		return;
		
	if (!ec)
	{
		if (password_verified)
			// If password is verified start read command 'calc' of 'logout'
			async_read(socket_, buffer(read_message_, read_message_.size()),
				strand_.wrap(bind(&session::handle_read, shared_from_this(), _1)));
		else
			stop();
	}
	else
	{
		cout << "Write login verification error: " << ec.message() << "\n";
		stop();
	}
}


void session::handle_read(const error_code& ec)
{
	if (stopped_) 
		return;
		
	if (!ec)
	{
		cout << user_->login() << ": " << read_message_.data() << "\n";
		
		// Recognize command
		if (!memcmp(read_message_.data(), "calc ", 5))
		{
			// 'Calc' command recognized
			if (user_->balance())
			{
				user_->decr_balance();
				
				string expr(read_message_.data() + 5);
				float result = parser_.calc(expr);
				
				std::istringstream ist("Result: " + std::to_string(result) + "\n");
				ist.getline(write_message_.data(), MESSAGE_MAX_LENGTH);
				
				db_o_.write(user_->login(), user_->balance(), expr, result); // Write operation to databasae
				cout << "result is ready\n";
			}
			else
			{
				std::istringstream ist("Your balance is 0\n");
				ist.getline(write_message_.data(), MESSAGE_MAX_LENGTH);
			}
			
		}
		else if (!memcmp(read_message_.data(), "logout", 6))
		{
			// 'Logout' command recognized
			cout << "Disconnect user [" << user_->login() << "]\n";
			stop();
			return;
		}
		else
		{
			// Invalid command
			std::istringstream ist("Invalid command\n");
			ist.getline(write_message_.data(), MESSAGE_MAX_LENGTH);
		}
		
		// Write answer
		async_write(socket_, buffer(write_message_, write_message_.size()), 
			strand_.wrap(bind(&session::handle_write, 
				shared_from_this(), _1)));
		
	}
	else
	{
		cout << "Read error: " << ec.message() << "\n";
		stop();
	}
}

void session::handle_write(const error_code& ec)
{
	if (stopped_) 
		return;
		
	if (!ec)
	{
		// After successful writing start read message from client
		async_read(socket_, buffer(read_message_, read_message_.size()), 
			strand_.wrap(bind(&session::handle_read, 
				shared_from_this(), _1)));	
	}
	else
	{
		cout << "Write error: " << ec.message() << "\n";
		stop();
	}
}
























