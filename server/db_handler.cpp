#include "db_handler.hpp"

// ***********Handler for 'Users' table*************
db_users::db_users()
	: db_connection_("user=daniil password=1234 hostaddr=127.0.0.1 dbname=serverdb"),
	  work_(db_connection_)
{
	cout << "Connected to " << db_connection_.dbname() << "\n";
}

db_users::~db_users() {}

bool db_users::find_user(const string& login)
{
	// Firstly search in connected_users_ (for multiple connection for one user)
	if (auto search = connected_users_.find(login); search != connected_users_.end())
	{
		return true;
	}
	else
	{
		// Search in real database using libqxx classes
		for (auto [str] : work_.query<string>("SELECT Login FROM Users"))
        {
        	if (str == login)
        		return true;   
        }
		return false;
	}
}

shared_ptr<user> db_users::get_user(const string& login, const string& password)
{
	// Firstly try to get from connected_users_ (for multiple connection for one user)
	if (auto search = connected_users_.find(login); search != connected_users_.end())
		if (search->second->password() == password)
    		return search->second;
		else 
			return shared_ptr<user>(nullptr); 
	else
	{
		// Here we start search in real database using libpqxx classes
		for (auto [l, p, b] : work_.query<string, string, int>(
            "SELECT Login, Password, Balance FROM Users"))
        {
        	if (l == login && p == password)
        	{
        		connected_users_.insert({l, std::make_shared<user>(l, p, b)});
        		return connected_users_.at(l);
    		} 
        }
		return shared_ptr<user>(nullptr);
	}
}

void db_users::remove_connected_user(const string& login)
{
	connected_users_.erase(login);
}





// ***********Handler for 'Oprations' table*************
db_operations::db_operations()
	: db_connection_("user=daniil password=1234 hostaddr=127.0.0.1 dbname=serverdb")
	  //work_(db_connection_)
{
	cout << "Connected to " << db_connection_.dbname() << "\n";
}

db_operations::~db_operations()
{
	//work_.commit();
}

void db_operations::write(const string& login, unsigned int balance, const string& expr, float result)
{
	// Start transaction
	pqxx::work work_(db_connection_);
	work_.exec("INSERT INTO Operations (Login, Balance, ExecuteTime, Expression, Result) VALUES(\'" + login + "\', " + std::to_string(balance) + ", TO_TIMESTAMP(\'" + get_curr_date_time() + "\', \'YYYY-MM-DD HH24:MI:SS\'), \'" + expr + "\', " + std::to_string(result) + ")");
	
	// Commit changes in database and close transaction
	work_.commit();
}


const string db_operations::get_curr_date_time() 
{
	// String view for Date and time for TIMESTAMP value in database 
    std::time_t time = std::time({});
    char time_string[std::size("yyyy-mm-dd hh:mm:ss")];
    std::strftime(std::data(time_string), std::size(time_string), "%F %T", std::gmtime(&time));
    cout << "Current time: " << time_string << "\n";
    return time_string;
}




















