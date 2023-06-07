#ifndef DB_HANDLER_HPP
#define DB_HANDLER_HPP

#include <ctime>
#include <string>
#include <iostream>
#include <memory>
#include <unordered_map>
#include <pqxx/pqxx>
#include "user.hpp"

using std::string;
using std::shared_ptr;
using std::unordered_map;
using std::cout;

// ***********Handler for 'Users' table*************
class db_users
{
public:
	db_users();
	~db_users();
	bool find_user(const string& login);
	shared_ptr<user> get_user(const string& login, const string& password);
	void remove_connected_user(const string& login);
	
private:
	unordered_map<string, shared_ptr<user>> connected_users_;
	pqxx::connection db_connection_;
	pqxx::work work_;
};


// ***********Handler for 'Oprations' table*************
class db_operations
{
public:
	db_operations();
	~db_operations();
	void write(const string& login, unsigned int balance, const string& expr, float result);
	const string get_curr_date_time();
private:
	
private:
	pqxx::connection db_connection_;
};

#endif
