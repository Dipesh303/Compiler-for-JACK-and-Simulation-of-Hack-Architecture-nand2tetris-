#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>

using namespace std;

ofstream ferr;

class JackTokenizer {
	ifstream fin;
	ofstream fout;
	string currToken;
	int lineNo;
	void keyword();
	void symbol();
	void identifier();
	void intVal();
	void stringVal();
public:
	JackTokenizer(string& filename);
	bool hasMoreTokens() {
		return (!fin.eof());
	}
	void advance();
	void tokenWrite();
	void closeToken();
};

JackTokenizer::JackTokenizer(string& filename) {
	if (fin.is_open())
		fin.close();
	fin.open(filename);
	if (!fin.is_open())
	{
		ferr << "ERROR: Could not open " << filename << endl; throw 0;
	}

	if (fout.is_open())
		fout.close();
	string str = filename.substr(0, filename.length() - 4) + "tok";
	fout.open(str);
	fout << "<tokens>" << endl;
}

void JackTokenizer::closeToken() {
	fout << "</tokens>" << endl;
	fout.close();
}

void JackTokenizer::advance() {
	//decomenting
	lineNo = 1;
	char c;
	while (fin.get(c)) {
		//if(c==10)
			//continue;
		//cout << c << "_";
		char p = fin.peek();
		if (c == '/' && p == '/') {
			while (fin.get(c)) {
				if (c == 10) { lineNo++; break; }
			}
			continue;
			//if (c == 10)lineNo++;
		}
		if (c == '/' && p == '*') {
			fin.get(c);
			while (fin.get(c)) {
				p = fin.peek();
				if (c == 10) lineNo++;
				if (c == '*' && p == '/') break;
			}
			fin.get(c);
			continue;
		}

		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c == '_') || (c >= '0' && c <= '9')) {
			string s = "";
			s = s + c;
			while (fin.get(c)) {
				if (c == ' ' || c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ',' || c == '.' || c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '<' || c == '>' || c == '=' || c == '~')
					break;
				if (c == 10)
				{
					lineNo++; break;
				}
				s = s + c;
			}
			currToken = s; tokenWrite();
		}

		if (c == '(' || c == ')' || c == '{' || c == '}' || c == '[' || c == ']' || c == ';' || c == ',' || c == '.' || c == '+' || c == '-' || c == '*' || c == '/' || c == '&' || c == '|' || c == '<' || c == '>' || c == '=' || c == '~')
		{
			currToken = c; tokenWrite();
		}

		if (c == 34) {
			string s = "";
			s = s + (char)34;
			while (fin.get(c)) {
				if (c == 10) { ferr << "ERROR: Expecting " << (char)34 << " at Line Number " << lineNo << endl; throw 0; }  // GIVE ERROR HERE
				s = s + c;
				if (c == 34)
					break;
			}
			currToken = s; tokenWrite();
		}
		if (c == 10) lineNo++;
	}

}

void JackTokenizer::tokenWrite() {
	if (currToken == "constructor" || currToken == "class" || currToken == "function" || currToken == "method" || currToken == "field" || currToken == "static" || currToken == "var" || currToken == "int" || currToken == "char" || currToken == "boolean" || currToken == "void" || currToken == "true" || currToken == "false" || currToken == "null" || currToken == "this" || currToken == "let" || currToken == "do" || currToken == "if" || currToken == "else" || currToken == "while" || currToken == "return")
		keyword();
	else if (currToken == "{" || currToken == "}" || currToken == "(" || currToken == ")" || currToken == "[" || currToken == "]" || currToken == "." || currToken == "," || currToken == ";" || currToken == "+" || currToken == "-" || currToken == "*" || currToken == "/" || currToken == "&" || currToken == "|" || currToken == "<" || currToken == ">" || currToken == "=" || currToken == "~")
		symbol();
	else if (currToken[0] >= '0' && currToken[0] <= '9')
		intVal();
	else if (currToken[0] == 34) //ascii value of double quotes is 34
		stringVal();
	else if (currToken[0] == '_' || (currToken[0] >= 'A' && currToken[0] <= 'Z') || (currToken[0] >= 'a' && currToken[0] <= 'z'))
		identifier();
}

void JackTokenizer::keyword() {
	fout << "<keyword> " << currToken << " </keyword>\n";
}

void JackTokenizer::symbol() {
	if (currToken == "<")
		fout << "<symbol> &lt; </symbol>\n";
	else if (currToken == ">")
		fout << "<symbol> &gt; </symbol>\n";
	else if (currToken == "&")
		fout << "<symbol> &amp; </symbol>\n";
	else
		fout << "<symbol> " << currToken << " </symbol>\n";
}

/*void JackTokenizer :: symbol(){
	fout << "<symbol> " << currToken << " </symbol>\n";
}*/

void JackTokenizer::intVal() {
	//check for example currToken == "54cs2"
	bool flag = 1;
	for (int i = 0; i < currToken.length(); i++) {
		if (!(currToken[i] >= '0' && currToken[i] <= '9'))
		{
			ferr << "ERROR: Invalid Token: " << currToken << " at Line Number " << lineNo << endl;  throw 0; flag = 0;   break;
		}
	}
	if (flag == 1) {
		int len = currToken.length(), j = 1, ans = currToken[0] - '0';
		while (j < len) {
			ans = ans * 10 + (currToken[j] - '0');
			j++;
		}
		fout << "<integerConstant> " << ans << " </integerConstant>\n";
	}
}

void JackTokenizer::stringVal() {
	string temp;
	temp = currToken.substr(1, currToken.length() - 2);
	fout << "<stringConstant> " << temp << " </stringConstant>\n";
}

void JackTokenizer::identifier() {
	fout << "<identifier> " << currToken << " </identifier>\n";
}

class JackAnalyzer {
	int tokenNo, spaces;
	ifstream fin;
	ofstream fout;
	string currTok;
	string TokTyp;
	map<string, bool> typTab;

	string tokErr();
	void printspace();
	void tokenTyp();
	void compileClass();
	void compileClassVarDec();
	void compileSubroutineDec();
	void compileParameterList();
	void compileSubroutineBody();
	void compileVarDec();
	void compileStatements();
	void compileLet();
	void compileIf();
	void compileWhile();
	void compileDo();
	void compileReturn();
	void compileExpression();
	void compileTerm();
	void compileExpressionList();
public:
	JackAnalyzer(string& filename);

};

JackAnalyzer::JackAnalyzer(string& filename) {
	//initializing
	typTab["int"] = 1;
	typTab["char"] = 1;
	typTab["boolean"] = 1;
	//
	if (fin.is_open())
		fin.close();
	fin.open(filename);
	if (!fin.is_open()) {
		ferr << "ERROR: Could not open " << filename << endl; throw 0;
	}

	if (fout.is_open())
		fout.close();
	string str = filename.substr(0, filename.length() - 3) + "xml";
	fout.open(str);


	getline(fin, currTok);   tokenNo = 0;   spaces = 0;
	if (currTok != "<tokens>") {
		ferr << "ERROR: Expecting <tokens> at line Number " << tokenNo << " in place of " << currTok << endl; throw 0;
	}
	getline(fin, currTok);   tokenNo++; tokenTyp();
	if (currTok != "<keyword> class </keyword>") {
		if (TokTyp == "keyword") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <keyword> but " << tokErr() << endl; throw 0;
		}
	}
	else {
		printspace(); fout << "<class>" << endl;
		compileClass();
		printspace(); fout << "</class>" << endl;
	}
	//check for </tokens>
}

void JackAnalyzer::printspace() {
	if (spaces < 0) return;
	for (int i = 0; i < spaces; i++) {
		fout << " ";
	}
}

string JackAnalyzer::tokErr() {
	int s, e;
	s = currTok.find(">");
	e = currTok.find("<", s);
	string serr = currTok.substr(s + 2, e - s - 3);
	return serr;
}

void JackAnalyzer::tokenTyp() {
	int s, e;
	s = currTok.find("<");
	e = currTok.find(">");
	TokTyp = currTok.substr(s + 1, e - s - 1);
}

void JackAnalyzer::compileClass() {
	spaces = spaces + 2;
	printspace();   fout << currTok << endl;  //printing white spaces and the class token : <keyword> class </keyword>

	getline(fin, currTok);   tokenNo++; tokenTyp();
	if (TokTyp == "identifier") {
		printspace();   fout << currTok << endl;  //className
	}
	else {
		ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> { </symbol>") {
		printspace();   fout << currTok << endl;  // {
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	while (currTok == "<keyword> static </keyword>" || currTok == "<keyword> field </keyword>") {
		printspace();   fout << "<classVarDec>" << endl;  // classVarDec
		compileClassVarDec();
		printspace();   fout << "</classVarDec>" << endl;
		getline(fin, currTok); tokenNo++; tokenTyp();
	}

	while (currTok == "<keyword> constructor </keyword>" || currTok == "<keyword> function </keyword>" || currTok == "<keyword> method </keyword>") {
		printspace(); fout << "<subroutineDec>" << endl;  // subroutineDec
		compileSubroutineDec();
		printspace(); fout << "</subroutineDec>" << endl;
		getline(fin, currTok); tokenNo++; tokenTyp();
	}

	if (currTok == "<symbol> } </symbol>") {
		printspace();   fout << currTok << endl;  // }
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}
	spaces = spaces - 2;
}

void JackAnalyzer::compileClassVarDec() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <keyword> static|field </keyword>

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (TokTyp == "keyword" || TokTyp == "identifier") {
		if (currTok == "<keyword> constructor </keyword>" || currTok == "<keyword> class </keyword>" || currTok == "<keyword> function </keyword>" || currTok == "<keyword> method </keyword>" || currTok == "<keyword> field </keyword>" || currTok == "<keyword> static </keyword>" || currTok == "<keyword> var </keyword>" || currTok == "<keyword> void </keyword>" || currTok == "<keyword> true </keyword>" || currTok == "<keyword> false </keyword>" || currTok == "<keyword> null </keyword>" || currTok == "<keyword> this </keyword>" || currTok == "<keyword> let </keyword>" || currTok == "<keyword> do </keyword>" || currTok == "<keyword> if </keyword>" || currTok == "<keyword> else </keyword>" || currTok == "<keyword> while </keyword>" || currTok == "<keyword> return </keyword>") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			printspace(); fout << currTok << endl; // type
		}
	}
	else {
		ferr << "ERROR: Expecting <keyword> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (TokTyp == "identifier") {
		printspace(); fout << currTok << endl; // VarName
	}
	else {
		ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	while (currTok == "<symbol> , </symbol>") {
		printspace(); fout << currTok << endl; // ,
		getline(fin, currTok); tokenNo++; tokenTyp();
		if (TokTyp == "identifier") {
			printspace(); fout << currTok << endl; // VarName
		}
		else {
			ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
		}
		getline(fin, currTok); tokenNo++; tokenTyp();
	}

	if (currTok == "<symbol> ; </symbol>") {
		printspace(); fout << currTok << endl; // ; 
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}
	spaces = spaces - 2;
}

void JackAnalyzer::compileSubroutineDec() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <keyword> method|function|constructor </keyword>

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (TokTyp == "keyword" || TokTyp == "identifier") {
		if (currTok == "<keyword> constructor </keyword>" || currTok == "<keyword> class </keyword>" || currTok == "<keyword> function </keyword>" || currTok == "<keyword> method </keyword>" || currTok == "<keyword> field </keyword>" || currTok == "<keyword> static </keyword>" || currTok == "<keyword> var </keyword>" || currTok == "<keyword> true </keyword>" || currTok == "<keyword> false </keyword>" || currTok == "<keyword> null </keyword>" || currTok == "<keyword> this </keyword>" || currTok == "<keyword> let </keyword>" || currTok == "<keyword> do </keyword>" || currTok == "<keyword> if </keyword>" || currTok == "<keyword> else </keyword>" || currTok == "<keyword> while </keyword>" || currTok == "<keyword> return </keyword>") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			printspace(); fout << currTok << endl; // type|void
		}
	}
	else {
		ferr << "ERROR: Expecting <keyword> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (TokTyp == "identifier") {
		printspace(); fout << currTok << endl; // SubroutineName
	}
	else {
		ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> ( </symbol>") {
		printspace(); fout << currTok << endl; // (
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	printspace(); fout << "<parameterList>" << endl; // parameterList
	compileParameterList();
	printspace(); fout << "</parameterList>" << endl;

	if (currTok == "<symbol> ) </symbol>") {
		printspace(); fout << currTok << endl; // )
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> { </symbol>") {
		printspace(); fout << "<subroutineBody>" << endl; // subroutineBody
		compileSubroutineBody();
		printspace(); fout << "</subroutineBody>" << endl;
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}
	spaces = spaces - 2;
}

void JackAnalyzer::compileParameterList() {
	spaces = spaces + 2;

	if (TokTyp == "keyword" || TokTyp == "identifier") {
		if (currTok == "<keyword> constructor </keyword>" || currTok == "<keyword> class </keyword>" || currTok == "<keyword> function </keyword>" || currTok == "<keyword> method </keyword>" || currTok == "<keyword> field </keyword>" || currTok == "<keyword> static </keyword>" || currTok == "<keyword> var </keyword>" || currTok == "<keyword> void </keyword>" || currTok == "<keyword> true </keyword>" || currTok == "<keyword> false </keyword>" || currTok == "<keyword> null </keyword>" || currTok == "<keyword> this </keyword>" || currTok == "<keyword> let </keyword>" || currTok == "<keyword> do </keyword>" || currTok == "<keyword> if </keyword>" || currTok == "<keyword> else </keyword>" || currTok == "<keyword> while </keyword>" || currTok == "<keyword> return </keyword>") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			printspace(); fout << currTok << endl; // type
		}

		getline(fin, currTok); tokenNo++; tokenTyp();
		if (TokTyp == "identifier") {
			printspace(); fout << currTok << endl; // varName
		}
		else {
			ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
		}

		getline(fin, currTok); tokenNo++; tokenTyp();
		while (currTok == "<symbol> , </symbol>") {
			printspace(); fout << currTok << endl; // ,

			getline(fin, currTok); tokenNo++; tokenTyp();
			if (TokTyp == "keyword" || TokTyp == "identifier") {
				if (currTok == "<keyword> constructor </keyword>" || currTok == "<keyword> class </keyword>" || currTok == "<keyword> function </keyword>" || currTok == "<keyword> method </keyword>" || currTok == "<keyword> field </keyword>" || currTok == "<keyword> static </keyword>" || currTok == "<keyword> var </keyword>" || currTok == "<keyword> void </keyword>" || currTok == "<keyword> true </keyword>" || currTok == "<keyword> false </keyword>" || currTok == "<keyword> null </keyword>" || currTok == "<keyword> this </keyword>" || currTok == "<keyword> let </keyword>" || currTok == "<keyword> do </keyword>" || currTok == "<keyword> if </keyword>" || currTok == "<keyword> else </keyword>" || currTok == "<keyword> while </keyword>" || currTok == "<keyword> return </keyword>") {
					ferr << "ERROR: " << tokErr() << endl; throw 0;
				}
				else {
					printspace(); fout << currTok << endl; // type
				}
			}
			else {
				ferr << "ERROR: Expecting <keyword> but " << tokErr() << endl; throw 0;
			}

			getline(fin, currTok); tokenNo++; tokenTyp();
			if (TokTyp == "identifier") {
				printspace(); fout << currTok << endl; // varName
			}
			else {
				ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
			}

			getline(fin, currTok); tokenNo++; tokenTyp();
		}
	}
	//here curr token should be ')' and it is always checked just after calling this parameter function
	spaces = spaces - 2;
}

void JackAnalyzer::compileSubroutineBody() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <symbol> { </symbol>

	getline(fin, currTok); tokenNo++; tokenTyp();
	while (currTok == "<keyword> var </keyword>") {
		printspace();   fout << "<varDec>" << endl;  // varDec
		compileVarDec();
		printspace();   fout << "</varDec>" << endl;
		getline(fin, currTok); tokenNo++; tokenTyp();
	}

	printspace();   fout << "<statements>" << endl;  // statements
	compileStatements();
	printspace();   fout << "</statements>" << endl;

	if (currTok == "<symbol> } </symbol>") {
		printspace(); fout << currTok << endl; // }
	}
	else if (currTok == "<keyword> else </keyword>") {
		ferr << "ERROR: else without a previous if" << endl; throw 0;
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	} //give error that invalid statement or } missing 
	spaces = spaces - 2;
}

void JackAnalyzer::compileVarDec() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <keyboard> var </keyboard>

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (TokTyp == "keyword" || TokTyp == "identifier") {
		if (currTok == "<keyword> constructor </keyword>" || currTok == "<keyword> class </keyword>" || currTok == "<keyword> function </keyword>" || currTok == "<keyword> method </keyword>" || currTok == "<keyword> field </keyword>" || currTok == "<keyword> static </keyword>" || currTok == "<keyword> var </keyword>" || currTok == "<keyword> void </keyword>" || currTok == "<keyword> true </keyword>" || currTok == "<keyword> false </keyword>" || currTok == "<keyword> null </keyword>" || currTok == "<keyword> this </keyword>" || currTok == "<keyword> let </keyword>" || currTok == "<keyword> do </keyword>" || currTok == "<keyword> if </keyword>" || currTok == "<keyword> else </keyword>" || currTok == "<keyword> while </keyword>" || currTok == "<keyword> return </keyword>") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			printspace(); fout << currTok << endl; // type
		}
	}
	else {
		ferr << "ERROR: Expecting <keyword> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (TokTyp == "identifier") {
		printspace(); fout << currTok << endl; // VarName
	}
	else {
		ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	while (currTok == "<symbol> , </symbol>") {
		printspace(); fout << currTok << endl; // ,
		getline(fin, currTok); tokenNo++; tokenTyp();
		if (TokTyp == "identifier") {
			printspace(); fout << currTok << endl; // VarName
		}
		else {
			ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
		}
		getline(fin, currTok); tokenNo++; tokenTyp();
	}

	if (currTok == "<symbol> ; </symbol>") {
		printspace(); fout << currTok << endl; // ; 
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	spaces = spaces - 2;
}

void JackAnalyzer::compileStatements() {
	spaces = spaces + 2;
	while (currTok == "<keyword> let </keyword>" || currTok == "<keyword> if </keyword>" || currTok == "<keyword> while </keyword>" || currTok == "<keyword> do </keyword>" || currTok == "<keyword> return </keyword>") {
		if (currTok == "<keyword> let </keyword>") {
			printspace(); fout << "<letStatement>" << endl; // letStatement
			compileLet();
			printspace(); fout << "</letStatement>" << endl;
		}
		else if (currTok == "<keyword> if </keyword>") {
			printspace(); fout << "<ifStatement>" << endl; // ifStatement
			compileIf();
			printspace(); fout << "</ifStatement>" << endl;
		}
		else if (currTok == "<keyword> while </keyword>") {
			printspace(); fout << "<whileStatement>" << endl; // whileStatement
			compileWhile();
			printspace(); fout << "</whileStatement>" << endl;
		}
		else if (currTok == "<keyword> do </keyword>") {
			printspace(); fout << "<doStatement>" << endl; // doStatement
			compileDo();
			printspace(); fout << "</doStatement>" << endl;
		}
		else if (currTok == "<keyword> return </keyword>") {
			printspace(); fout << "<returnStatement>" << endl; // returnStatement
			compileReturn();
			printspace(); fout << "</returnStatement>" << endl;
		}
	}
	spaces = spaces - 2;
}

void JackAnalyzer::compileLet() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <keyword> let </keyword>

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (TokTyp == "identifier") {
		printspace(); fout << currTok << endl; // VarName
	}
	else {
		ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> [ </symbol>") {
		printspace(); fout << currTok << endl; // [

		getline(fin, currTok); tokenNo++; tokenTyp();
		printspace(); fout << "<expression>" << endl; // expression
		compileExpression();
		printspace(); fout << "</expression>" << endl;

		if (currTok == "<symbol> ] </symbol>") {
			printspace(); fout << currTok << endl; // ]
		}
		else {
			if (TokTyp == "symbol") {
				ferr << "ERROR: " << tokErr() << endl; throw 0;
			}
			else {
				ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
			}
		}
		getline(fin, currTok); tokenNo++; tokenTyp();
	}

	if (currTok == "<symbol> = </symbol>") {
		printspace(); fout << currTok << endl; // =
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	printspace(); fout << "<expression>" << endl; // expression
	compileExpression();
	printspace(); fout << "</expression>" << endl;

	if (currTok == "<symbol> ; </symbol>") {
		printspace(); fout << currTok << endl; // ;
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	spaces = spaces - 2;
}

void JackAnalyzer::compileIf() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <keyword> if </keyword>

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> ( </symbol>") {
		printspace(); fout << currTok << endl; // (
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	printspace(); fout << "<expression>" << endl; // expression
	compileExpression();
	printspace(); fout << "</expression>" << endl;

	if (currTok == "<symbol> ) </symbol>") {
		printspace(); fout << currTok << endl; // )
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> { </symbol>") {
		printspace(); fout << currTok << endl; // {
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	printspace();   fout << "<statements>" << endl;  // statements
	compileStatements();
	printspace();   fout << "</statements>" << endl;

	if (currTok == "<symbol> } </symbol>") {
		printspace(); fout << currTok << endl; // }
	}
	else if (currTok == "<keyword> else </keyword>") {
		ferr << "ERROR: else without a previous if" << endl; throw 0;
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<keyword> else </keyword>") {
		printspace(); fout << currTok << endl; // else

		getline(fin, currTok); tokenNo++; tokenTyp();
		if (currTok == "<symbol> { </symbol>") {
			printspace(); fout << currTok << endl; // {
		}
		else {
			if (TokTyp == "symbol") {
				ferr << "ERROR: " << tokErr() << endl; throw 0;
			}
			else {
				ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
			}
		}

		getline(fin, currTok); tokenNo++; tokenTyp();
		printspace();   fout << "<statements>" << endl;  // statements
		compileStatements();
		printspace();   fout << "</statements>" << endl;

		if (currTok == "<symbol> } </symbol>") {
			printspace(); fout << currTok << endl; // }
		}
		else if (currTok == "<keyword> else </keyword>") {
			ferr << "ERROR: else without a previous if" << endl;
		}
		else {
			if (TokTyp == "symbol") {
				ferr << "ERROR: " << tokErr() << endl; throw 0;
			}
			else {
				ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
			}
		}
		getline(fin, currTok); tokenNo++; tokenTyp();
	}
	spaces = spaces - 2;
}

void JackAnalyzer::compileWhile() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <keyword> while </keyword>

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> ( </symbol>") {
		printspace(); fout << currTok << endl; // (
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	printspace(); fout << "<expression>" << endl; // expression
	compileExpression();
	printspace(); fout << "</expression>" << endl;

	if (currTok == "<symbol> ) </symbol>") {
		printspace(); fout << currTok << endl; // )
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> { </symbol>") {
		printspace(); fout << currTok << endl; // {
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	printspace();   fout << "<statements>" << endl;  // statements
	compileStatements();
	printspace();   fout << "</statements>" << endl;

	if (currTok == "<symbol> } </symbol>") {
		printspace(); fout << currTok << endl; // }
	}
	else if (currTok == "<keyword> else </keyword>") {
		ferr << "ERROR: else without a previous if" << endl;
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	spaces = spaces - 2;
}

void JackAnalyzer::compileDo() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <keyword> do </keyword>

	//--------------------------------------------------------------------------------------------------------------------------------SubRoutineCall---------------------------------------\\ 
	getline(fin, currTok); tokenNo++; tokenTyp();
	if (TokTyp == "identifier") {
		printspace(); fout << currTok << endl; // SubroutineName or className or varName
		// type of identifier1 = subroutineName||className||varName;
	}
	else {
		ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> . </symbol>") {
		//if type of identifier1 == subroutineName GIVE ERROR HERE
		printspace(); fout << currTok << endl; // .

		getline(fin, currTok); tokenNo++; tokenTyp();
		if (TokTyp == "identifier") {
			// type of identifier2 = subroutineName||className||varName;  if type of identifier2 != subroutineName GIVE ERROR HERE 
			printspace(); fout << currTok << endl; // SubroutineName	
		}
		else {
			ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
		}
		getline(fin, currTok); tokenNo++; tokenTyp();
	}

	//if type of identifier1==className||varName && type of identifier2==uninialized  GIVE ERROR HERE  

	if (currTok == "<symbol> ( </symbol>") {
		printspace(); fout << currTok << endl; // (
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	printspace();   fout << "<expressionList>" << endl;  // expressionList
	compileExpressionList();
	printspace();   fout << "</expressionList>" << endl;

	if (currTok == "<symbol> ) </symbol>") {
		printspace(); fout << currTok << endl; // )
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	//--------------------------------------------------------------------------------------------------------------------------------SubRoutineCall---------------------------------------\\ 
	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok == "<symbol> ; </symbol>") {
		printspace(); fout << currTok << endl; // ;
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}
	getline(fin, currTok); tokenNo++; tokenTyp();
	spaces = spaces - 2;
}

void JackAnalyzer::compileReturn() {
	spaces = spaces + 2;
	printspace(); fout << currTok << endl; //printing white spaces and the token : <keyword> return </keyword>

	getline(fin, currTok); tokenNo++; tokenTyp();
	if (currTok != "<symbol> ; </symbol>") {
		printspace(); fout << "<expression>" << endl; // expression
		compileExpression();
		printspace(); fout << "</expression>" << endl;
	}
	if (currTok == "<symbol> ; </symbol>") {
		printspace(); fout << currTok << endl; // ;
	}
	else {
		if (TokTyp == "symbol") {
			ferr << "ERROR: " << tokErr() << endl; throw 0;
		}
		else {
			ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
		}
	}

	getline(fin, currTok); tokenNo++; tokenTyp();
	spaces = spaces - 2;
}

void JackAnalyzer::compileExpression() {
	spaces = spaces + 2;
	printspace();   fout << "<term>" << endl;  // term
	compileTerm();
	printspace();   fout << "</term>" << endl;

	while (currTok == "<symbol> + </symbol>" || currTok == "<symbol> - </symbol>" || currTok == "<symbol> * </symbol>" || currTok == "<symbol> / </symbol>" || currTok == "<symbol> &amp; </symbol>" || currTok == "<symbol> | </symbol>" || currTok == "<symbol> &lt; </symbol>" || currTok == "<symbol> &gt; </symbol>" || currTok == "<symbol> = </symbol>") {
		printspace(); fout << currTok << endl; // op

		getline(fin, currTok); tokenNo++; tokenTyp();
		printspace();   fout << "<term>" << endl;  // term
		compileTerm();
		printspace();   fout << "</term>" << endl;
	}
	spaces = spaces - 2;
}

void JackAnalyzer::compileTerm() {
	spaces = spaces + 2;
	if (TokTyp == "integerConstant" || TokTyp == "stringConstant" || currTok == "<keyword> true </keyword>" || currTok == "<keyword> false </keyword>" || currTok == "<keyword> null </keyword>" || currTok == "<keyword> this </keyword>") {
		printspace(); fout << currTok << endl; //integerConstant | stringConstant | keywordConstant
		getline(fin, currTok); tokenNo++; tokenTyp();
	}
	else if (TokTyp == "identifier") {
		printspace(); fout << currTok << endl; //varName or varName'['expression']' or SubRoutineCall
		getline(fin, currTok); tokenNo++; tokenTyp();

		if (currTok == "<symbol> [ </symbol>") {
			//here type of identifier should be varName else GIVE ERROR HERE
			printspace(); fout << currTok << endl; // [

			getline(fin, currTok); tokenNo++; tokenTyp();
			printspace(); fout << "<expression>" << endl; // expression
			compileExpression();
			printspace(); fout << "</expression>" << endl;

			if (currTok == "<symbol> ] </symbol>") {
				printspace(); fout << currTok << endl; // ]
			}
			else {
				if (TokTyp == "symbol") {
					ferr << "ERROR: " << tokErr() << endl; throw 0;
				}
				else {
					ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
				}
			}
			getline(fin, currTok); tokenNo++; tokenTyp();
		}
		else if (currTok == "<symbol> ( </symbol>") {
			//here type of identifier should be SubRoutineName
			printspace(); fout << currTok << endl; // (

			getline(fin, currTok); tokenNo++; tokenTyp();
			printspace();   fout << "<expressionList>" << endl;  // expressionList
			compileExpressionList();
			printspace();   fout << "</expressionList>" << endl;

			if (currTok == "<symbol> ) </symbol>") {
				printspace(); fout << currTok << endl; // )
			}
			else {
				if (TokTyp == "symbol") {
					ferr << "ERROR: " << tokErr() << endl; throw 0;
				}
				else {
					ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
				}
			}
			getline(fin, currTok); tokenNo++; tokenTyp();
		}
		else if (currTok == "<symbol> . </symbol>") {
			//here type of identifier should be className or varName else GIVE ERROR HERE
			printspace(); fout << currTok << endl; // .

			getline(fin, currTok); tokenNo++; tokenTyp();
			if (TokTyp == "identifier") {
				//if type of identifier2 != subroutineName GIVE ERROR HERE 
				printspace(); fout << currTok << endl; // SubroutineName	
			}
			else {
				ferr << "ERROR: Expecting <identifier> but " << tokErr() << endl; throw 0;
			}

			getline(fin, currTok); tokenNo++; tokenTyp();
			if (currTok == "<symbol> ( </symbol>") {
				printspace(); fout << currTok << endl; // (
			}
			else {
				if (TokTyp == "symbol") {
					ferr << "ERROR: " << tokErr() << endl; throw 0;
				}
				else {
					ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
				}
			}

			getline(fin, currTok); tokenNo++; tokenTyp();
			printspace();   fout << "<expressionList>" << endl;  // expressionList
			compileExpressionList();
			printspace();   fout << "</expressionList>" << endl;

			if (currTok == "<symbol> ) </symbol>") {
				printspace(); fout << currTok << endl; // )
			}
			else {
				if (TokTyp == "symbol") {
					ferr << "ERROR: " << tokErr() << endl; throw 0;
				}
				else {
					ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
				}
			}
			getline(fin, currTok); tokenNo++; tokenTyp();
		}
	}
	else if (currTok == "<symbol> ( </symbol>") {
		printspace(); fout << currTok << endl; // (

		getline(fin, currTok); tokenNo++; tokenTyp();
		printspace(); fout << "<expression>" << endl; // expression
		compileExpression();
		printspace(); fout << "</expression>" << endl;

		if (currTok == "<symbol> ) </symbol>") {
			printspace(); fout << currTok << endl; // )
		}
		else {
			if (TokTyp == "symbol") {
				ferr << "ERROR: " << tokErr() << endl; throw 0;
			}
			else {
				ferr << "ERROR: Expecting <symbol> but " << tokErr() << endl; throw 0;
			}
		}
		getline(fin, currTok); tokenNo++; tokenTyp();
	}
	else if (currTok == "<symbol> - </symbol>" || currTok == "<symbol> ~ </symbol>") {
		printspace(); fout << currTok << endl; // -|~
		getline(fin, currTok); tokenNo++; tokenTyp();
		printspace();   fout << "<term>" << endl;  // term
		compileTerm();
		printspace();   fout << "</term>" << endl;
	}
	else {
		ferr << "ERROR: " << tokErr() << endl; throw 0;
	}
	spaces = spaces - 2;
}

void JackAnalyzer::compileExpressionList() {
	spaces = spaces + 2;
	if (currTok != "<symbol> ) </symbol>") {
		printspace(); fout << "<expression>" << endl; // expression
		compileExpression();
		printspace(); fout << "</expression>" << endl;
		while (currTok == "<symbol> , </symbol>") {
			printspace(); fout << currTok << endl; // ,

			getline(fin, currTok); tokenNo++; tokenTyp();
			printspace(); fout << "<expression>" << endl; // expression
			compileExpression();
			printspace(); fout << "</expression>" << endl;
		}
	}
	spaces = spaces - 2;
}

struct details {
	string type, kind;
	int index;
};

class SymbolTable {
	map<string, details> symtab;
public:
	int Count1, Count2, Count; //Count1 is: fieldcount for class symbol table | argumentcount for subroutine symbol table
							   //Count2 is: staticcount for class symbol table | localcount for subroutine symbol table
	bool contains(string name) {
		if (symtab.find(name) == symtab.end())
			return 0;
		return 1;
	}
	string getKind(string name) {
		if (contains(name) == 0)
			return "";
		return (symtab[name].kind);
	}
	string getType(string name) {
		if (contains(name) == 0)
			return "";
		return (symtab[name].type);
	}
	int getIndex(string name) {
		if (contains(name) == 0)
			return -1;
		return (symtab[name].index);
	}
	void init() {
		symtab.clear();
		Count1 = 0; Count2 = 0; Count = 0;
	}
	void addEntry(string name, string typ, string kin) {
		details temp;
		temp.kind = kin; temp.type = typ;
		if (kin == "field" || kin == "argument") {
			temp.index = Count1;
			symtab[name] = temp;
			Count1++; Count++;
		}
		else if (kin == "static" || kin == "local") {
			temp.index = Count2;
			symtab[name] = temp;
			Count2++; Count++;
		}
	}
};

class compileEngine {
	ifstream fin;
	ofstream fout;
	int labnum;
	string className, currentSubroutineName, currentSubroutineType;
	string Token, TT;
	SymbolTable class_sym_tab, sub_sym_tab;

	void extract();
	int call();
	string opHelp(string s);
	void compileClass();
	void compileClassVarDec();
	void compileSubroutineDec();
	void compileParameterList();
	void compileSubroutineBody();
	void compileVarDec();
	void compileStatements();
	void compileLet();
	void compileIf();
	void compileWhile();
	void compileDo();
	void compileReturn();
	void compileExpression();
	void compileTerm();
	int compileExpressionList();
public:
	compileEngine(string& filename);
};

compileEngine::compileEngine(string& filename) {
	labnum = 0;

	if (fin.is_open())
		fin.close();
	fin.open(filename);
	if (!fin.is_open())
		cerr << "ERROR: Could not open " << filename << endl;


	if (fout.is_open())
		fout.close();
	string str = filename.substr(0, filename.length() - 3) + "vm";
	fout.open(str);

	getline(fin, Token); extract();
	call();
	if (Token != "</class>") {/*error*/ }
}

void compileEngine::extract() {
	int s, e, check;
	s = Token.find("<");
	e = Token.find(">");
	check = Token.find("<", s + 1);
	if (check != string::npos) {
		TT = Token.substr(s + 1, e - s - 1);
		Token = Token.substr(e + 2, check - e - 3);
	}
	else {
		TT = "";
		Token = Token.substr(s + 1, e - s - 1);
	}
}

int compileEngine::call() {
	if (Token == "class") {
		compileClass(); return 0;
	}
	else if (Token == "classVarDec") {
		compileClassVarDec(); return 0;
	}
	else if (Token == "subroutineDec") {
		compileSubroutineDec(); return 0;
	}
	else if (Token == "parameterList") {
		compileParameterList(); return 0;
	}
	else if (Token == "subroutineBody") {
		compileSubroutineBody(); return 0;
	}
	else if (Token == "varDec") {
		compileVarDec(); return 0;
	}
	else if (Token == "statements") {
		compileStatements(); return 0;
	}
	else if (Token == "letStatement") {
		compileLet(); return 0;
	}
	else if (Token == "ifStatement") {
		compileIf(); return 0;
	}
	else if (Token == "whileStatement") {
		compileWhile(); return 0;
	}
	else if (Token == "doStatement") {
		compileDo(); return 0;
	}
	else if (Token == "returnStatement") {
		compileReturn(); return 0;
	}
	else if (Token == "expression") {
		compileExpression(); return 0;
	}
	else if (Token == "term") {
		compileTerm(); return 0;
	}
	else if (Token == "expressionList") {
		return (compileExpressionList());
	}
	else {/*error*/ }
}

string compileEngine::opHelp(string s) {
	if (s == "+")
		return "add";
	if (s == "-")
		return "sub";
	if (s == "*")
		return "call Math.multiply 2";
	if (s == "/")
		return "call Math.divide 2";
	if (s == "&amp;")
		return "and";
	if (s == "|")
		return "or";
	if (s == "&lt;")
		return "lt";
	if (s == "&gt;")
		return "gt";
	if (s == "=")
		return "eq";
	return "";
}

void compileEngine::compileClass() {
	getline(fin, Token); extract(); // class
	getline(fin, Token); extract(); // className

	className = Token;
	class_sym_tab.init();

	getline(fin, Token); extract(); // {
	getline(fin, Token); extract();

	while (Token != "}") {
		call();
		getline(fin, Token); extract();
	}
	getline(fin, Token); extract();
}

void compileEngine::compileClassVarDec() {
	string tempn, tempt, tempk;
	getline(fin, Token); extract();
	tempk = Token;
	getline(fin, Token); extract();
	tempt = Token;
	getline(fin, Token); extract();
	tempn = Token;
	class_sym_tab.addEntry(tempn, tempt, tempk);
	getline(fin, Token); extract(); // , or ;
	while (Token == ",") {
		getline(fin, Token); extract();
		tempn = Token;
		class_sym_tab.addEntry(tempn, tempt, tempk);
		getline(fin, Token); extract();
	}
	getline(fin, Token); extract();
}

void compileEngine::compileSubroutineDec() {
	getline(fin, Token); extract();
	currentSubroutineType = Token;
	getline(fin, Token); extract();
	getline(fin, Token); extract();
	currentSubroutineName = Token;
	sub_sym_tab.init();
	if (currentSubroutineType == "method") {
		sub_sym_tab.addEntry("this", className, "argument");
	}
	getline(fin, Token); extract(); // (
	getline(fin, Token); extract();
	call();
	getline(fin, Token); extract(); // )
	getline(fin, Token); extract();
	call();
	getline(fin, Token); extract();
}

void compileEngine::compileParameterList() {
	string tempt, tempn;
	getline(fin, Token); extract();
	if (Token != "/parameterList") {
		tempt = Token;
		getline(fin, Token); extract();
		tempn = Token;
		sub_sym_tab.addEntry(tempn, tempt, "argument");
		getline(fin, Token); extract();
		while (Token == ",") {
			getline(fin, Token); extract();
			tempt = Token;
			getline(fin, Token); extract();
			tempn = Token;
			sub_sym_tab.addEntry(tempn, tempt, "argument");
			getline(fin, Token); extract();
		}
	}
}

void compileEngine::compileSubroutineBody() {
	getline(fin, Token); extract();
	getline(fin, Token); extract();
	while (Token == "varDec") {
		call();
		getline(fin, Token); extract();
	}

	fout << "function " << className << "." << currentSubroutineName << " " << sub_sym_tab.Count2 << endl;
	if (currentSubroutineType == "constructor") {
		fout << "push constant " << class_sym_tab.Count1 << endl;
		fout << "call Memory.alloc 1" << endl;
		fout << "pop pointer 0" << endl;
	}
	if (currentSubroutineType == "method") {
		fout << "push argument 0" << endl;
		fout << "pop pointer 0" << endl;
	}

	call(); //statements
	getline(fin, Token); extract(); // }
	getline(fin, Token); extract();
}

void compileEngine::compileVarDec() {
	string tempt, tempn;
	getline(fin, Token); extract();
	getline(fin, Token); extract();
	tempt = Token;
	getline(fin, Token); extract();
	tempn = Token;
	sub_sym_tab.addEntry(tempn, tempt, "local");
	getline(fin, Token); extract();
	while (Token == ",") {
		getline(fin, Token); extract();
		tempn = Token;
		sub_sym_tab.addEntry(tempn, tempt, "local");
		getline(fin, Token); extract();
	}
	getline(fin, Token); extract();
}

void compileEngine::compileStatements() {
	getline(fin, Token); extract();
	while (Token == "letStatement" || Token == "ifStatement" || Token == "whileStatement" || Token == "doStatement" || Token == "returnStatement") {
		call();
		getline(fin, Token); extract();
	}
}

void compileEngine::compileLet() {
	getline(fin, Token); extract(); // let
	getline(fin, Token); extract(); // varName

	string varnam = Token, kin_varnam;
	int ind_varnam;
	if (sub_sym_tab.contains(varnam)) {
		kin_varnam = sub_sym_tab.getKind(varnam);	ind_varnam = sub_sym_tab.getIndex(varnam);
	}
	else if (class_sym_tab.contains(varnam)) {
		kin_varnam = class_sym_tab.getKind(varnam);	ind_varnam = class_sym_tab.getIndex(varnam);
		if (kin_varnam == "field")
			kin_varnam = "this";
	}
	else {
		ferr << "Declaration error: " << varnam << " undeclared." << endl; throw 0;
	}

	getline(fin, Token); extract(); // [ or =
	if (Token == "=") {
		getline(fin, Token); extract();
		call();
		fout << "pop " << kin_varnam << " " << ind_varnam << endl;
		getline(fin, Token); extract(); // ;
	}
	else if (Token == "[") {
		getline(fin, Token); extract(); // expression
		call();
		getline(fin, Token); extract(); // ]
		fout << "push " << kin_varnam << " " << ind_varnam << endl;
		fout << "add" << endl;
		getline(fin, Token); extract(); // =
		getline(fin, Token); extract();
		call();
		fout << "pop temp 0" << endl;
		fout << "pop pointer 1" << endl;
		fout << "push temp 0" << endl;
		fout << "pop that 0" << endl;
		getline(fin, Token); extract(); // ;
	}
	getline(fin, Token); extract();
}

void compileEngine::compileIf() {
	int l = labnum; labnum = labnum + 2;
	getline(fin, Token); extract(); //if
	getline(fin, Token); extract(); // (
	getline(fin, Token); extract();
	call();
	getline(fin, Token); extract(); // )
	getline(fin, Token); extract(); // {
	fout << "not" << endl;
	fout << "if-goto " << className << "." << l << endl;
	getline(fin, Token); extract();
	call();
	getline(fin, Token); extract(); // }
	fout << "goto " << className << "." << (l + 1) << endl;
	fout << "label " << className << "." << l << endl;

	getline(fin, Token); extract();
	if (Token == "else") {
		getline(fin, Token); extract(); // {
		getline(fin, Token); extract();
		call();
		getline(fin, Token); extract(); // }
		getline(fin, Token); extract();
	}
	fout << "label " << className << "." << (l + 1) << endl;

}

void compileEngine::compileWhile() {
	int l = labnum; labnum = labnum + 2;
	getline(fin, Token); extract(); // while
	getline(fin, Token); extract(); // (
	fout << "label " << className << "." << l << endl;
	getline(fin, Token); extract();
	call();
	getline(fin, Token); extract(); // )
	fout << "not" << endl;
	fout << "if-goto " << className << "." << (l + 1) << endl;
	getline(fin, Token); extract(); // {
	getline(fin, Token); extract();
	call();
	getline(fin, Token); extract(); // }
	fout << "goto " << className << "." << l << endl;
	fout << "label " << className << "." << (l + 1) << endl;
	getline(fin, Token); extract();
}

void compileEngine::compileDo() {
	getline(fin, Token); extract(); // do
	getline(fin, Token); extract(); // id1
	string id1 = Token;
	int nP;
	getline(fin, Token); extract(); // ( or .
	if (Token != ".") {
		fout << "push pointer 0" << endl;
		getline(fin, Token); extract();
		if (Token != "expressionList") { cout << "expression list expected" << endl; }
		nP = call();
		getline(fin, Token); extract(); // )
		getline(fin, Token); extract(); // ;
		fout << "call " << className << "." << id1 << " " << (nP + 1) << endl;
		fout << "pop temp 0" << endl;
	}
	else {
		getline(fin, Token); extract(); // id2
		string tempk, tempt, id2 = Token;
		int tempi;
		if (sub_sym_tab.contains(id1)) {
			//id1 is an object
			tempk = sub_sym_tab.getKind(id1);
			tempt = sub_sym_tab.getType(id1);
			tempi = sub_sym_tab.getIndex(id1);
			fout << "push " << tempk << " " << tempi << endl;
		}
		else if (class_sym_tab.contains(id1)) {
			//id1 is an object
			tempk = class_sym_tab.getKind(id1);
			if (tempk == "field")
				tempk = "this";
			tempt = class_sym_tab.getType(id1);
			tempi = class_sym_tab.getIndex(id1);
			fout << "push " << tempk << " " << tempi << endl;
		}
		else {/*id1 is a classname eg:array, string, keyboard */ }
		getline(fin, Token); extract(); // (
		getline(fin, Token); extract();
		if (Token != "expressionList") { cout << "expression list expected" << endl; }
		nP = call();
		getline(fin, Token); extract(); // )
		getline(fin, Token); extract(); // ;
		if (sub_sym_tab.contains(id1) || class_sym_tab.contains(id1))
			fout << "call " << tempt << "." << id2 << " " << (nP + 1) << endl;
		else
			fout << "call " << id1 << "." << id2 << " " << nP << endl;
		fout << "pop temp 0" << endl;
	}
	getline(fin, Token); extract();
}

void compileEngine::compileReturn() {
	getline(fin, Token); extract(); // return
	getline(fin, Token); extract(); // expression or ;
	if (Token == "expression") {
		call();
		getline(fin, Token); extract(); // ;
		fout << "return" << endl;
	}
	else {
		fout << "push constant 0" << endl;
		fout << "return" << endl;
	}
	getline(fin, Token); extract();
}

void compileEngine::compileExpression() {
	getline(fin, Token); extract();
	call();
	getline(fin, Token); extract(); // op or /expression
	while (Token == "+" || Token == "-" || Token == "*" || Token == "/" || Token == "&amp;" || Token == "|" || Token == "&lt;" || Token == "&gt;" || Token == "=") {
		string op = opHelp(Token);
		getline(fin, Token); extract();
		call();
		fout << op << endl;
		getline(fin, Token); extract(); // op or /expression			
	}
}

void compileEngine::compileTerm() {
	getline(fin, Token); extract();
	//check for unaryOp
	if (Token == "-" || Token == "~") {
		string op = Token;
		getline(fin, Token); extract();
		call();
		if (op == "-")
			fout << "neg" << endl;
		if (op == "~")
			fout << "not" << endl;
		getline(fin, Token); extract();
	}
	//check for '('expression')'
	else if (Token == "(") {
		getline(fin, Token); extract();
		call();
		getline(fin, Token); extract(); // )
		getline(fin, Token); extract();
	}
	//check for integerConstant
	else if (TT == "integerConstant") {
		fout << "push constant " << Token << endl;
		getline(fin, Token); extract();
	}
	//check for keywordConstant
	else if (Token == "true" || Token == "false" || Token == "null" || Token == "this") {
		if (Token == "true")
			fout << "push constant 0" << endl << "not" << endl;
		else if (Token == "false")
			fout << "push constant 0" << endl;
		else if (Token == "null")
			fout << "push constant 0" << endl;
		else if (Token == "this")
			fout << "push pointer 0" << endl;
		getline(fin, Token); extract();
	}
	//check for stringConstant
	else if (TT == "stringConstant") {
		fout << "push constant " << Token.length() << endl;
		fout << "call String.new 1" << endl;
		for (int i = 0; i < Token.length(); i++) {
			fout << "push constant " << (int)Token[i] << endl;
			fout << "call String.appendChar 2" << endl;
		}
		getline(fin, Token); extract();
	}
	else if (TT == "identifier") {
		string varnam = Token;
		int nP; bool iii = 0;
		string kin_varnam;
		int ind_varnam;
		if (sub_sym_tab.contains(varnam)) {
			kin_varnam = sub_sym_tab.getKind(varnam);	ind_varnam = sub_sym_tab.getIndex(varnam);
		}
		else if (class_sym_tab.contains(varnam)) {
			kin_varnam = class_sym_tab.getKind(varnam);	ind_varnam = class_sym_tab.getIndex(varnam);
			if (kin_varnam == "field")
				kin_varnam = "this";
		}
		else { iii = 1;/*error  may be these is not an error*/ }

		getline(fin, Token); extract(); // /term or [ or ( or .

		//check for varName
		if (Token == "/term") {
			if (iii == 0)
				fout << "push " << kin_varnam << " " << ind_varnam << endl;
			else {
				ferr << "Declaration error: " << varnam << " undeclared." << endl; throw 0;
			}
		}
		//check for varName[expression]
		else if (Token == "[") {
			if (iii == 0) {
				getline(fin, Token); extract();
				call();
				getline(fin, Token); extract(); // ]
				fout << "push " << kin_varnam << " " << ind_varnam << endl;
				fout << "add" << endl;
				fout << "pop pointer 1" << endl;
				fout << "push that 0" << endl;
				getline(fin, Token); extract();
			}
			else {
				ferr << "Declaration error: " << varnam << " undeclared." << endl; throw 0;
			}
		}
		//check for SubroutineCall
		else if (Token == "(") {
			string id1 = varnam;
			fout << "push pointer 0" << endl;
			getline(fin, Token); extract();
			if (Token != "expressionList") { cout << "expression list expected" << endl; }
			nP = call();
			getline(fin, Token); extract(); // )
			fout << "call " << className << "." << id1 << " " << (nP + 1) << endl;
			getline(fin, Token); extract();
		}
		else if (Token == ".") {
			string id1 = varnam;
			getline(fin, Token); extract(); // id2
			string tempk, tempt, id2 = Token;
			int tempi;
			if (sub_sym_tab.contains(id1)) {
				//id1 is an object
				tempk = sub_sym_tab.getKind(id1);
				tempt = sub_sym_tab.getType(id1);
				tempi = sub_sym_tab.getIndex(id1);
				fout << "push " << tempk << " " << tempi << endl;
			}
			else if (class_sym_tab.contains(id1)) {
				//id1 is an object
				tempk = class_sym_tab.getKind(id1);
				if (tempk == "field")
					tempk = "this";
				tempt = class_sym_tab.getType(id1);
				tempi = class_sym_tab.getIndex(id1);
				fout << "push " << tempk << " " << tempi << endl;
			}
			else {/*id1 is a classname eg:array, string, keyboard */ }
			getline(fin, Token); extract(); // (
			getline(fin, Token); extract();
			if (Token != "expressionList") { cout << Token << endl; }
			nP = call();
			getline(fin, Token); extract(); // )
			if (sub_sym_tab.contains(id1) || class_sym_tab.contains(id1))
				fout << "call " << tempt << "." << id2 << " " << (nP + 1) << endl;
			else
				fout << "call " << id1 << "." << id2 << " " << nP << endl;
			getline(fin, Token); extract();
		}
	}
}

int compileEngine::compileExpressionList() {
	int nP = 0;
	getline(fin, Token); extract(); // expression or /expressionList
	if (Token == "expression") {
		call(); nP++;
		getline(fin, Token); extract(); // , or /expressionList
		while (Token == ",") {
			getline(fin, Token); extract();
			call(); nP++;
			getline(fin, Token); extract(); // , or /expressionList
		}
	}
	return nP;
}

int main(int argc, char* argv[]) {
	for (int i = 2; i < argc; i++) {
		try {
			string infil = argv[i];
			string Error = infil.substr(0, infil.length() - 4) + "err";
			if (ferr.is_open()) { ferr.close(); }
			ferr.open(Error);
			JackTokenizer JT(infil);
			JT.advance();
			JT.closeToken();
			string tokfil = infil.substr(0, infil.length() - 4) + "tok";
			JackAnalyzer JA(tokfil);
			string xmlfil = infil.substr(0, infil.length() - 4) + "xml";
			compileEngine CE(xmlfil);
		}
		catch(...){/*Do Nothing*/}
	}
	return 0;
}