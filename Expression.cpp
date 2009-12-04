#include <iostream>


typedef double ValueType;


class Expression {

public:

	virtual ValueType operator() () = 0;

};

class Number : public Expression {

public:
	Number (ValueType &v) : value(v) {}

	ValueType operator() () {
		return value;
	}

private:
	ValueType &value;
};


class Plus : public Expression {

public:
	Plus (Expression &e1, Expression &e2)
		: exp1(e1), exp2(e2) {}
	
	ValueType operator() () {
		return exp1() + exp2();
	}

private:

	Expression &exp1, &exp2;
};


class Minus : public Expression {

public:
	Minus (Expression &e1, Expression &e2)
		: exp1 (e1), exp2 (e2) {}
	
	ValueType operator() () {
		return exp1() - exp2();
	}

private:

	Expression &exp1, &exp2;
};


class Multiply : public Expression {

public:
	Multiply (Expression &e1, Expression &e2)
		: exp1(e1), exp2(e2) {}
	
	ValueType operator() () {
		return exp1() * exp2();
	}

private:

	Expression &exp1, &exp2;
};




class Division : public Expression {

public:
	Division (Expression &e1, Expression &e2)
		: exp1(e1), exp2(e2) {}
	
	ValueType operator() () {
		if (exp2() == 0)
			throw "division by zero";

		return exp1() / exp2();
	}

private:

	Expression &exp1, &exp2;
};


class Integ : public Expression {

public:
	Integ (Expression &a, Expression &b, Expression &exp)
		: ma(a), mb(b), mexp(exp) {}

	ValueType operator() () {
		
	}

private:

	Expression &ma, &mb, &mexp;

	ValueType eps;
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
	ValueType value;
	std::string name;
};



Expression stack[256];

Expression &parse (std::string &exp) {
	switch () {
	case NUMBER:
		push Number(v);
	case PLUS:
		push(pop()+pop());
		
	}
}

int main ()
{
	// 3+5*6-8/2 = 29
	ValueType v1(3), v2(5), v3(6), v4(8), v5(2);

	Number n1 (v1);
	Number n2 (v2);
	Number n3 (v3);
	Number n4 (v4);
	Number n5 (v5);

	Multiply o1 (n2, n3);
	Division o2 (n4, n5);
	Plus o3 (n1, o1);
	Minus o4 (o3, o2);

	std::cout << "3+5*6-8/2 = " << o4 () << std::endl;
	o4.eval();
	
	return 0;
}
