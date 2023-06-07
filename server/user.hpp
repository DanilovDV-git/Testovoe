#ifndef USER_HPP
#define USER_HPP


#include <string>

using std::string;

class user
{
public:
	user(const string& login, const string& password, unsigned int balance);
		
	const string& login();
	const string& password();
	unsigned int balance();
	bool decr_balance();

private:
	string login_;
	string password_;
	unsigned int balance_;
};
#endif
