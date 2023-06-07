#include "user.hpp"

user::user (const string& login, const string& password, unsigned int balance)
	: login_(login), password_(password), balance_(balance) 
{}

const string& user::login() { return login_; }
const string& user::password() { return password_; }
unsigned int user::balance() { return balance_; }

bool user::decr_balance() 
{
	if (balance_ > 0)
	{
		--balance_;
		return true;
	}
	return false;
}
