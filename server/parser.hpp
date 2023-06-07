#ifndef PARSER_HPP
#define PARSER_HPP

#include <stack>
#include <string>
#include <cstdlib>

using std::string;
using std::stack;
using std::strtof;
using std::isspace;
using std::isdigit;


class parser
{
public:
	struct Operator
	{
		char simbol;
		char precidence;
		float (*function) (float operand1, float operand2);
	};
	
	float calc(const string& expr)
	{
		const char* str = expr.c_str();
		while(*str)
		{
			if (isspace(*str)) 
				++str;
			else if (isdigit(*str))
			{
				char* end_pos;
				float operand = strtof(str, &end_pos);
				str = end_pos;
				operands.push(operand);
			}
			else
			{
				// Find the corresponding operator
				for (auto& op : this->ops)
				{
					if (*str == op.simbol)
					{
						while (!operators.empty() && operators.top()->precidence >= op.precidence)
						{
							process_operation();
						}
						operators.push(&op);
						++str;
						break;
					}
				}
			}
		}
		
		while(!operators.empty())
		{
			process_operation();
		}
		
		return operands.top();
	}
	
private:
	void process_operation()
	{
		float operand2 = operands.top();
		operands.pop();
		float operand1 = operands.top();
		operands.pop();
		Operator* operator_ = operators.top();
		operators.pop();
		operands.push(operator_->function(operand1, operand2));
	}	
	
private:
	static const unsigned short OPER_NUMBER = 4;
	Operator ops[OPER_NUMBER]
	{
		{'+', 1, [](float op1, float op2) -> float {return op1 + op2;} },
		{'-', 1, [](float op1, float op2) -> float {return op1 - op2;} },
		{'*', 2, [](float op1, float op2) -> float {return op1 * op2;} },
		{'/', 2, [](float op1, float op2) -> float {return op1 / op2;} }
	};
	stack <Operator*> operators {};
  	stack <float> operands {};
};

#endif
