// ���������� ������� � ������� ��� ���������� �������������� ���������

#include "../include/arithmetic.h"
#include <algorithm>
#include <iostream>

Lexem::Lexem(std::string value, Token token)
{
	_value = value;
	_token = token;
}

std::string& Lexem::value()
{
	return _value;
}

Token Lexem::token()
{
	return _token;
}

int Expression::priority(char op)
{
	switch (op)
	{
	case '(': // � ����������� ������ ���������� ���������
		return 1;
	case '+': // � + � - ����
	case '-':
		return 2;
	case '*': // � * � / ����� ������� ���������
	case '/':
		return 3;
	default:
		return 0;
	}
}

// ���� �������� ����������
void Expression::input(std::map<std::string, double>& vars, std::string& op, double& d)
{
	if (isalpha(op[0]) || (op[0] == '-' && isalpha(op[1]))) // ���� ������ - ����������
	{
		if (vars.find(op) == vars.end()) // ���� �� ���������� � ������������� �������
		{
			std::cout << "������� �������� ���������� " << ((op[0] == '-') ? op.substr(1) : op) << ": ";
			std::cin >> vars[op]; // ���� �������� � ������������� ������
			if (op[0] == '-') // ���� ������� �����
				vars[op] *= -1; // �� �������� �� -1
		}
		d = vars[op]; // ������ � ����������
	}
	else // �����
		d = std::atof(op.c_str()); // �������������� ������ � �����
}

// ������� � �������� �������� ������
std::string Expression::toRPN()
{
	std::string rpn; // �������������� ������
	Stack<std::string> operands; // ���� ��� ���������
	for (size_t i = 0; i < lexems.size(); i++) // ���� �� ���� ���������
	{
		Token token = lexems[i].token();
		std::string value = lexems[i].value();
		if (token == Token::VAR || token == Token::NUM) // ���� ����� - ���������� ��� �����
			rpn += value + " "; // �� ������ ��������� � ������
		else if (token == Token::OP) // ���� ����� - ��������
		{
			if (operands.IsEmpty()) // ���� ���� ����
				operands.push(value); // �� ������ ��������� � ����
			else
			{
				// ���� ���� �� ���� � ��������� ���������� �������� ����� ���� ��������
				while (!operands.IsEmpty() && priority(operands.Value()[0]) >= priority(value[0]))
					rpn += operands.pop() + " "; // �� ��������� � ������ �� �����
				operands.push(value); // ��������� �������� � ����
			}
		}
		else if (token == Token::LP) // ���� ����� - ����� ������
			operands.push(value); // ��������� ������ � ����
		else // ���� ����� - ������ ������
		{
			while (operands.Value() != "(") // ���� �� ������� ����� ������
				if (operands.Value() != ")") // ��� ����������� �������� ��������� � ������
					rpn += operands.pop() + " ";
			operands.pop(); // ������� ����� ������
		}
	}

	while (!operands.IsEmpty()) // ������� ����� ��������� � ������
		rpn += operands.pop() + " ";

	rpn.pop_back();
	return rpn;
}

// ���������� �������� �� �������� �������� ������
double Expression::eval(std::string rpn)
{
	size_t cur = 0; // ������� ������
	Stack<std::string> st; // ���� ���������� � �����
	std::map<std::string, double> vars; // ������ ��� "����������-��������"
	while (cur < rpn.size()) // ���� ������ �� �����������
	{
		if (rpn[cur] == ' ') // ���� ������ ������
			cur++; // �� ��������� �� ��������� ������
		else if (isdigit(rpn[cur]) || (rpn[cur] == '-' && isdigit(rpn[cur + 1]))) // ���� ������� �����
		{
			std::string number;
			while (isdigit(rpn[cur]) || rpn[cur] == '.' || rpn[cur] == '-') // �������� �����
				number += rpn[cur++];
			st.push(number); // ��������� � ���� �����
		}
		else if (isalpha(rpn[cur]) || (rpn[cur] == '-' && isalpha(rpn[cur + 1]))) // ���� ������� ����������
		{
			std::string variable;
			while (isalpha(rpn[cur]) || rpn[cur] == '-') // �������� ����������
				variable += rpn[cur++];
			st.push(variable); // ��������� � ���� ����������
		}
		else
		{
			std::string op2 = st.pop(), // �������� ��� ��������
				op1 = st.pop();
			double d1, d2;
			input(vars, op2, d2); // ���� ����������, �������� �� ��������
			input(vars, op1, d1);

			switch (rpn[cur]) // ��������� ������� ��������
			{
			case '+':
				st.push(std::to_string(d1 + d2));
				break;
			case '-':
				st.push(std::to_string(d1 - d2));
				break;
			case '*':
				st.push(std::to_string(d1 * d2));
				break;
			case '/':
				st.push(std::to_string(d1 / d2));
				break;
			}
			cur++;
		}
	}

	if (isalpha(st.Value()[0]))
	{
		double d;
		input(vars, st.Value(), d);
		return d;
	}
	else
		return std::atof(st.pop().c_str());
}

// ������� ���������
bool Expression::parse(std::string expr, bool print)
{
	std::string::iterator end_pos = std::remove(expr.begin(), expr.end(), ' '); // �������� ��������
	expr.erase(end_pos, expr.end());
	lexems.clear();

	size_t cur = 0; // ������� ������
	std::string number; // ������ ��� ����� � ����������
	Stack<int> indexes; // ���� ��������
	bool isParsed = true; // ��������� �� ������
	int parCount = -1;
	while (cur < expr.size()) // ���� ������ �� �����������
	{
		if (expr[cur] == '(' || expr[cur] == ')'
			|| expr[cur] == '+' || expr[cur] == '*' || expr[cur] == '/')
		{
			if (expr[cur] == '(') // ���� ��������� ����������� ������
			{
				if (parCount != -1)
					parCount++;
				// ���� ������ ������ ��� ���������� ����� - �������� ��� (
				if (lexems.size() == 0 || lexems[lexems.size() - 1].token() == Token::OP
					|| lexems[lexems.size() - 1].token() == Token::LP)
				{
					lexems.push_back(Lexem("(", Token::LP));
					indexes.push(cur); // ��������� � ���� ������
				}
				else // ����� ��������� �� ������
				{
					if (print)
						std::cout << "������ " << cur + 1 << ": " << "�������� ��������!\n";
					isParsed = false;
				}
			}
			else if (expr[cur] == ')') // ���� ��������� ����������� ������
			{
				if (parCount != -1)
					parCount--;
				// ���� �� ������ ������ � ���������� ����� - �����, ���������� ��� )
				if (lexems.size() > 0 && (lexems[lexems.size() - 1].token() == Token::NUM
					|| lexems[lexems.size() - 1].token() == Token::VAR
					|| lexems[lexems.size() - 1].token() == Token::RP))
				{
					if (indexes.IsEmpty()) // ���� ���� ����
					{
						// ��������� �� ������
						if (print)
							std::cout << "������ " << cur + 1 << ": " << "�������� ������� ������!\n";
						isParsed = false;
					}
					else // �����
					{
						indexes.pop(); // �������� �� �����
						lexems.push_back(Lexem(")", Token::RP));
						if (parCount == 0)
						{
							lexems.push_back(Lexem(")", Token::RP));
							parCount = -1;
						}
					}
				}
				else // ����� ��������� �� ������
				{
					if (print)
						std::cout << "������ " << cur + 1 << ": " << "��������� ����� ��� ����������!\n";
					isParsed = false;
				}
			}
			else // ����� ���� �������� + * /
			{
				// ���� ������ ������ ��� ���������� ����� - ��������
				if (lexems.size() == 0 || (lexems[lexems.size() - 1].token() == Token::OP))
				{
					// ��������� �� ������
					if (print)
						std::cout << "������ " << cur + 1 << ":" << "��������� �����, ���������� ��� ������!\n";
					isParsed = false;
				}
				lexems.push_back(Lexem(std::string() + expr[cur], Token::OP));
			}
			cur++;
		}
		else if (expr[cur] == '-') // ���� �������� -
		{
			if (cur < expr.size() - 1 && expr[cur + 1] == '(')
			{
				parCount = 0;
				lexems.push_back(Lexem("(", Token::LP));
				lexems.push_back(Lexem("-1", Token::NUM));
				lexems.push_back(Lexem("*", Token::OP));
			}
			else if (lexems.size() == 0 ||
				lexems[lexems.size() - 1].token() == OP || lexems[lexems.size() - 1].token() == LP)
				number.push_back('-');
			else
				lexems.push_back(Lexem(std::string() + expr[cur], Token::OP));
			cur++;

			while (cur < expr.size() - 1 && expr[cur] == '-' && expr[cur + 1] == '-')
				cur += 2;
		}
		else if (isdigit(expr[cur])) // ���� ��������� �����
		{
			// ���� ������ ������ ��� ���������� ����� - �������� ��� (
			if (lexems.size() > 0 && lexems[lexems.size() - 1].token() != Token::LP
				&& lexems[lexems.size() - 1].token() != Token::OP)
				//tokens.push_back(Token::NUM); // ��������� ����� �����
				//else
			{
				// ����� ��������� �� ������
				if (print)
					std::cout << "������ " << cur + 1 << ":" << "��������� ������ ������ ��� ��������!\n";
				isParsed = false;
			}

			int count = 0; // ���������� �����
			while (isdigit(expr[cur]) || expr[cur] == '.') // ���� ����� ��� �����
			{
				if (expr[cur] == '.') // ���� �����
				{
					count++;
					if (count == 2) // ���� ������ ����� �����, �� ������
						isParsed = false;
				}
				number.push_back(expr[cur++]); // ���������� ����� � �����
			}

			lexems.push_back(Lexem(number, Token::NUM));
			number = ""; // ������� ������
		}
		else if (isalpha(expr[cur])) // ���� ��������� �����
		{
			// ���� ������ ������ ��� ���������� ����� - �������� ��� (
			if (lexems.size() > 0 && lexems[lexems.size() - 1].token() != Token::LP
				&& lexems[lexems.size() - 1].token() != Token::OP)
			{
				if (print)
					std::cout << "������ " << cur + 1 << ": " << "��������� ������ ������ ��� ��������!\n";
				isParsed = false;
			}

			while (isalpha(expr[cur]) || isdigit(expr[cur]))
				number.push_back(expr[cur++]);// ���������� ����� � �����

			lexems.push_back(Lexem(number, Token::VAR));
			number = ""; // ������� ������
		}
		else if (expr[cur++] == '.')
		{
			number = "0.";
		}
		else // ����� ��������� �� ������
		{
			if (print)
				std::cout << "������ " << cur + 1 << ": " << "������������ ������!\n";
			isParsed = false;
			cur++;
		}
	}

	if (!indexes.IsEmpty()) // ���� ���� ������ �� ����, �� ������
	{
		if (print)
			std::cout << "������ " << cur << ": " << "�������� ������� ������!\n";
		isParsed = false;
	}

	return isParsed;
}