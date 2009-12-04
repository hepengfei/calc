#include <iostream>
#include <string>
#include <stack>
#include <vector>
#include <map>

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
	ValueType value;
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


map <string, ValueType> varMap;


class Variable : public Expression {

public:
	Variable (std::string &n)
		: evaled(false)
		,  mName(n) {}

	ValueType operator() () {
		
		map<string,ValueType>::iterator it = varMap.find (mName);
		
		if (it != varMap.end() ) {
			value = it->second;
		} else {
			cout << "input " << mName << " value:";
			cin >> value;

			varMap[mName] = value;
		}

		return value;
	}

	string name () {
		return mName;
	}

private:
	bool evaled;
	ValueType value;
	string mName;
};

void set (string name, ValueType value) {
	varMap[name] = value;
}




//vector <Variable*> varTable;
stack <Expression*> theStack;
Expression *expression = 0;


/*表达式文法
  E=T{+|-T}.
  T=F{*|/F}.
  F=(E)|id|num.
  id=letter{letter|digit}
  num=digit{digit}[.{digit}]
  letter="A"|"B"|...|"Z"|a|b|...|z|
  digit=0|1|2|3|...|9
*/

#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>


//char letter[53]={0};
//char digit[10]={'0','1','2','3','4','5','6','7','8','9'};
char operators[6]={'+','-','*','/','(',')'};
char endchar='#';
double	ERRVAL=1;

struct itemtype{

	char name[4];
	double value;
	int eval;
};
		
enum tokencode {
	nul,
	ident,
	number,
	PLUS,
	MINUS,
	times,
	slash,
	lparen,
	rparen
};

	

/*void initletter(){
		
  letter[0]='A';
  letter[26]='a';
  int i;

  for(i=1;i<26;i++){

  letter[i]=letter[i-1]+1;
  letter[i+26]=letter[i+26-1]+1;
  }
  } */

	
char buffer[30]={0};					//存放表达式的缓冲区
int position;							//下一个字符的位置
char curChar;							//当前字符
enum tokencode curToken;				//当前词汇码
itemtype table[20];						//符号表
int total;								//符号表当前元素个数
int idindex;							//当前标识符在符号表中的位置
double numval;							//当前数对应的数值
int cntError;
double oldStack[19]; 
int top;								//错误总数
double a;
double b;								//上下限
double eps;								//误差

//函数声明
	
double push(double x);
double pop();
int lookup(char lexeme);
int insert(char lexeme);
void getIdentifier();
void getNumber();
void getToken();
enum tokencode getOpcode (char op);
int isoperators(char lexeme);
void initvar();
void E();
void T();
void F();
Expression *eval(const char expr[30]);
void error();
void act(enum tokencode tcode);
int isinte(char lexeme);


	

enum tokencode getOpcode (char op) {
	enum tokencode result;
	switch(op){
	case '+':
		result=PLUS;
		break;
	case '-':
		result=MINUS;
		break;
	case '*':
		result=times;
		break;
	case '/':
		result=slash;
		break;
	case '(':
		result=lparen;
		break;
	case ')':
		result=rparen;
		break;
	default:
		result=nul;
		break;
	}
	return(result);
}



double push(double x){
	top=top+1;
	oldStack[top]=x;
	return(x);
}

double pop(){
	double x;
	if (top>0){
		x=oldStack[top];
		top=top-1;
	}
	else
		x=-ERRVAL;
	return(x);
}
int namecmp(char n1[4],char n2[4]){
	return 
		(n1[0]==n2[0])
		&&(n1[1]==n2[1])
		&&(n1[2]==n2[2])
		&&(n1[3]==n2[3]);
}


			
int lookup(char lexeme[4]){
	int i;
	table[0].name[0]=lexeme[0];
	table[0].name[1]=lexeme[1];
	table[0].name[2]=lexeme[2];
	table[0].name[3]=lexeme[3];
	i=total;
	while(i!=0 && !namecmp(lexeme,table[i].name)){
		i--;
	}
	return(i);

}								//在符号表中查找lexeme，若找到则返回位置值，否则返回0
int insert(char lexeme[4]){			
	int i;
	i=lookup(lexeme);			//在符号表中查找lexeme
	if(i==0){					//lexeme不在表中
		total++;
		table[total].name[0]=lexeme[0];
		table[total].name[1]=lexeme[1];
		table[total].name[2]=lexeme[2];
		table[total].name[3]=lexeme[3];
		table[total].value=0;
		table[total].eval=0;
		return(total);
	}			
	else
		return(i);
}								//插入lexeme于符号表中，返回位置值
		
void getIdentifier(){			//取标识符
	char lexeme[4]={0};				//只认前四个标识符
	int i=0;
	while(isalnum(buffer[position])){
		if(i<4){
			lexeme[i]=buffer[position];
			i++;
		}
		position++;
	}
	idindex=insert(lexeme);
}



ValueType integ (ValueType a, ValueType b, Expression *exp, string x) {
	Expression &e = *exp;

	set(x, a);
	
	return e();
}


int main(){

	
	do{
		printf("Input expression:");
		cin>>buffer;

		if (buffer[0] == '$')
			break;

		if (buffer[0]=='{') {
			ValueType a,b;
			cin >> a ;
			cin >> b;

			string exp;
			cin >> exp;

			string name;
			cin >> name;

			memset (buffer, 0, 30);
			memcpy (buffer, exp.c_str(), exp.size ());
			Expression *e = eval (buffer);

			cout << "value:" << integ (a, b, e, name) << endl;
		} else {

		Expression *expression=eval(buffer);
		cout << "value=" << (*expression) () << endl;

		set( "a", 100 );
		cout << "value=" << (*expression) () << endl;
		
		delete expression;
		}

		if((curToken==nul)&&(cntError!=0)){
			error();
			printf("Error total=%d\n",cntError);
		}

	}while(true);

	return 0;
}




Expression *eval(const char expr[30]){
	initvar();
	getToken();
	E();

	Expression *expression = theStack.top ();
	theStack.pop ();

	return expression;
		
}

void initvar(){
	top=0;			//initoldStack
	position=0;
	total=0;
	cntError=0;
		
}
void getToken(){
	while(buffer[position]==' ')//过滤空格
		position++;
	curChar=buffer[position];
	if(isoperators(curChar)==1){
		curToken=getOpcode(curChar);
		position++;
	}
	else if(isalpha(curChar)!=0){
		getIdentifier();
		curToken=ident;
	}
	else if(isdigit(curChar)!=0){
		getNumber();
		curToken=number;
	}
	else if(curChar==endchar)
		curToken=nul;
}

void getNumber(){
	double v;
	double radix;
	numval=0;
	while(isdigit(buffer[position])){
		v=buffer[position]-'0';
		numval=10*numval+v;
		position++;
	}							//拼整数
	if(buffer[position]=='.'){
		position++;
		radix=0.1;
		while(isdigit(buffer[position])){
			v=radix*(buffer[position]-'0');
			numval=numval+v;
			radix=radix*0.1;
			position++;
		}
	}							//拼小数
}


int isoperators(char lexeme){
	int result;
	switch (lexeme) {
	case '+':		
	case '-':
	case '*':
	case '/':
	case '(':
	case ')':
		result=1;
		break;
	default:
		result=0;
		break;
	}
	return(result);
}

void error(){
	printf("Error:%c",curChar);
	printf("Position:%d",position-1);
	cntError=cntError+1;
}


void act(enum tokencode tcode){
	//double x;
	switch(tcode){
	case ident:
	{
		string name=table[idindex].name;
		Variable *var = new Variable(name);
		theStack.push (var);

		/*
		if(table[idindex].eval==1)
			x=table[idindex].value;
		else {
			//if(isinte(table[idindex].name)){
			//	getInte();
				
				
			//	}	
			//else{

			printf("Input %s:",table[idindex].name);
			cin>>x;	
			//getchar();
			//scanf(" %f",&x);
			table[idindex].value=x;
			table[idindex].eval=1;
			//	}
		}*/
		//push(x);
		break;
	}
	case number:
	{
		Number *num = new Number (numval);
		theStack.push (num);
		//push(numval);
		break;
	}
	case PLUS:
	{
		Expression *a = theStack.top ();
		theStack.pop ();
		Expression *b = theStack.top ();
		theStack.pop ();
		Plus *p = new Plus ( a, b);
		theStack.push (p);
			
		//push( pop () + pop () );
		break;
	}
	case MINUS:
	{
		Expression *a = theStack.top ();
		theStack.pop ();
		Expression *b = theStack.top ();
		theStack.pop ();
		Minus *p = new Minus ( b, a);
		theStack.push (p);
				
		//x=pop();
		//push(pop()-x);
		break;
	}
	case times:
	{
		Expression *a = theStack.top ();
		theStack.pop ();
		Expression *b = theStack.top ();
		theStack.pop ();
		Multiply *p = new Multiply ( a, b);
		theStack.push (p);
			
			
		//push(pop()*pop());
		break;
	}
	case slash:
	{
		Expression *a = theStack.top ();
		theStack.pop ();
		Expression *b = theStack.top ();
		theStack.pop ();
		Division *p = new Division ( b, a);
		theStack.push (p);

				
		/*x=pop();
		if(x!=0)
			push(pop()/x);
		else{
			error();
			printf("Zero divisor.\n");
		}*/
		break;
	}
	}
		
}

void F(){							//F=(E)|id|num
	//double x;
	switch(curToken){
	case lparen:
		getToken();
		E();
		if(curToken==rparen)
			getToken();
		else
			error();
		break;
	case ident:
	case number:
		act(curToken);
		getToken();
		break;
	default:
		error();
		break;
	}
}

void T(){							//T=F{*|/F}
	enum tokencode op;
	//double x;
	F();
	while(curToken==times||curToken==slash){
		op=curToken;
		getToken();
		F();		
		act(op);
	}
}

void E(){							//E=T{+|-T}
	enum tokencode op;
	//double x;
	T();
	while(curToken==PLUS||curToken==MINUS){
		op=curToken;
		getToken();
		T();
		act(op);
	}
}
/*
  int isinte(char lexeme[4]){
  return 
  (lexeme[0]=='i')
  &&(lexeme[1]=='n)
  &&(lexeme[2]=='t')
  &&(lexeme[3]=='e');
  }


*/
