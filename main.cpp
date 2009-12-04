#include <cctype>
#include <cstdlib>
#include <string>
#include <iostream>
#include <sstream>

using namespace std;

typedef double ValueType;


class Expression {

public:
	virtual ~Expression () {}

	virtual ValueType operator() () = 0;

};

class Number : public Expression {

public:
	Number (ValueType &v) : value(v) {}

	virtual ValueType operator() () {
		return value;
	}

private:
	ValueType &value;
};


class Plus : public Expression {

public:
	Plus (Expression *e1, Expression *e2)
		: exp1(*e1), exp2(*e2) {}

	virtual  ~Plus () {
		delete &exp1;
		delete &exp2;
	}
	
	ValueType operator() () {
		return exp1() + exp2();
	}

private:

	Expression &exp1, &exp2;
};


class Minus : public Expression {

public:
	Minus (Expression *e1, Expression *e2)
		: exp1 (*e1), exp2 (*e2) {}

	virtual ~Minus () {
		delete &exp1;
		delete &exp2;
	}
	
	ValueType operator() () {
		return exp1() - exp2();
	}

private:

	Expression &exp1, &exp2;
};


class Multiply : public Expression {

public:
	Multiply (Expression *e1, Expression *e2)
		: exp1(*e1), exp2(*e2) {}

	virtual ~Multiply () {
		delete &exp1;
		delete &exp2;
	}
	
	ValueType operator() () {
		return exp1() * exp2();
	}

private:

	Expression &exp1, &exp2;
};




class Division : public Expression {

public:
	Division (Expression *e1, Expression *e2)
		: exp1(*e1), exp2(*e2) {}

	virtual ~Division () {
		delete &exp1;
		delete &exp2;
	}
	
	ValueType operator() () {
		if (exp2() == 0)
			throw "division by zero";

		return exp1() / exp2();
	}

private:

	Expression &exp1, &exp2;
};


class Variable : public Expression {

public:
	Variable (std::string &n)
		: name(n) {}

	ValueType operator() () {
		std::cout << "input " << name << " value:";
		std::cin >> value; 

		return value;
	}

private:
	bool evaled;
	ValueType value;
	std::string name;
};




/*表达式文法
  E=T{+|-T}.
  T=F{*|/F}.
  F=(E)|id|num.
  id=letter{letter|digit}
  num=digit{digit}[.{digit}]
  letter="A"|"B"|...|"Z"|a|b|...|z|
  digit=0|1|2|3|...|9
*/


enum TokenCode {
	NUL,
	IDENT,
	NUMBER,
	PLUS,
	MINUS,
	MULTI,
	DIV,
	LPAREN,
	RPAREN
};

class Parser {

public:

	Parser (string &exp)
		: endChar('#')
		, operators("+-*/()")
		, expression(0)
		, buffer(exp) {
		position = 0;
	}

	~Parser () {
		if (expression)
			delete expression;
	}

	Expression &operator() () {
		getToken ();
		expression = E ();
		return *expression;
	}

protected:
	Expression *E () {
		Expression *e = T ();

		TokenCode op;
		while (curToken == PLUS
		       || curToken == MINUS) {
			op = curToken;
			getToken ();
			Expression *e1 = T();

			if (op==PLUS)
				e = new Plus (e, e1);
			else
				e = new Minus (e, e1);
		}
		return e;
	}
	
	Expression *T () {
		Expression *e = F ();

		TokenCode op;
		while (curToken == MULTI
		       || curToken == DIV) {
			op = curToken;
			getToken ();
			Expression *e1 = F();

			if (op==MULTI)
				e = new Multiply (e, e1);
			else
				e = new Division (e, e1);
		}
		return e;
	}
	
	Expression *F () {
		Expression *e;
		switch (curToken) {
		case LPAREN:
			getToken ();
			e = E ();
			if (curToken == RPAREN)
				getToken ();
			else
				throw "error";
			break;
		case IDENT:
			e = new Variable (varname);
			getToken ();
			break;
		case NUMBER:
			e = new Number (numval);
			getToken ();
			break;
		default:
			throw "error";
			break;
		}

		return e;
	}
	
	void getToken () {
		while (isSpace (buffer[position]))
			++ position;

		char ch=buffer[position];
		if (isOperator(ch)) {
			getOpcode(ch);
			++ position;
		} else if (isAlpha(ch)) {
			getVar ();
			curToken = IDENT;
		} else if (isDigit(ch)) {
			getNumber ();
			curToken = NUMBER;
		} else if (isEnd (ch)) {
			curToken = NUL;
		} else {
			throw "error";
		}
	}

	void getNumber () {
		stringstream str;
		str << buffer.substr(position);
		str >> numval;

		str >> buffer;
		position = 0;
	}

	void getVar () {
		size_t begin = position;
	
		while (isAlphaNum(buffer[position]))
			++ position;

		varname = buffer.substr(begin, position - begin);
	}

	void getOpcode (char ch) {
		TokenCode result;
		switch(ch){
		case '+':
			result=PLUS;
			break;
		case '-':
			result=MINUS;
			break;
		case '*':
			result=MULTI;
			break;
		case '/':
			result=DIV;
			break;
		case '(':
			result=LPAREN;
			break;
		case ')':
			result=RPAREN;
			break;
		default:
			result=NUL;
			break;
		}
		curToken = result;
	}

	bool isOperator (char ch) const {
		return operators.find (ch) != string::npos;
	}
	bool isDigit (char ch) const {
		return isdigit (ch);
	}
	bool isAlpha (char ch) const {
		return isalpha (ch);
	}
	bool isAlphaNum (char ch) {
		return isAlpha (ch) || isDigit(ch);
	}
	bool isEnd (char ch) const {
		return endChar == ch;
	}
	bool isSpace (char ch) const {
		return ch == ' ';
	}

private:
	const char endChar;
	const string operators;

	Expression *expression;

	string buffer;
	size_t position;

	TokenCode curToken;
	ValueType numval;
	string varname;
};

int main(){
	string buffer;

	do {
		cout << "expression:";
		cin >> buffer;
		buffer += "#";

		Parser parse (buffer);
		Expression &exp = parse ();

		cout << "result:" << exp() << endl;

	} while (buffer != "$");

	return 0;
}



