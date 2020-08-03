#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
//fileName is xxx. from xxx.vm
//funcName is current function name and is initialized in writeFunction(function function_name k)
//2 modifications :  -Parser constructor replaced by setFile   -currCmd is made public to check if currCmd=""

using namespace std;

int str2num(string str) {
	int len = str.length(), j = 1, ans = str[0] - '0';
	while (j < len) {
		ans = ans * 10 + (str[j] - '0');
		j++;
	}
	return ans;
}

class Parser {
	ifstream fin;
	string currCmd;
		public:
	string getcurrCmd();
	void setFile(string& filename);
	bool moreCmd(){
		return (!fin.eof());
	}
	void advance();
	string cmdTyp();
	string arg1();
	int arg2();
};

string Parser :: getcurrCmd(){
	return currCmd;
}

void Parser :: setFile(string& filename){
	if(fin.is_open())
		fin.close();
	fin.open(filename);
	if(!fin.is_open())
		cerr << "Error : Could not open " << filename << endl;
}

void Parser :: advance(){
	int cmtpos;
	string currLine = "";
	while(getline(fin,currLine)){
		
		cmtpos=currLine.find("//");
		if (cmtpos != string::npos) {
			currLine.erase(cmtpos, currLine.length() - cmtpos);
			int end = currLine.length() - 1;
			while ((end >= 0) && (currLine[end] == ' ' || currLine[end] == '\t'))
				end = end - 1;
			if (end != currLine.length() - 1)
				currLine.erase(end + 1, currLine.length() - end - 1);
		}
		if(!currLine.empty())
			break;
	}
	currCmd=currLine;	
}

string Parser :: cmdTyp(){
	if(currCmd.substr(0,3)=="add" || currCmd.substr(0,3)=="sub" || currCmd.substr(0,3)=="neg" || currCmd.substr(0,2)=="eq" || currCmd.substr(0,2)=="lt" || currCmd.substr(0,2)=="gt"  || currCmd.substr(0,3)=="and" || currCmd.substr(0,2)=="or" || currCmd.substr(0,3)=="not")
		return "C_ARITHMETIC";
	else if(currCmd.substr(0,4)=="push")
		return "C_PUSH";
	else if(currCmd.substr(0,3)=="pop")
		return "C_POP";
	else if(currCmd.substr(0,5)=="label")
		return "C_LABEL";
	else if(currCmd.substr(0,4)=="goto")
		return "C_GOTO";
	else if(currCmd.substr(0,2)=="if")
		return "C_IF";
	else if(currCmd.substr(0,8)=="function")
		return "C_FUNCTION";
	else if(currCmd.substr(0,4)=="call")
		return "C_CALL";
	else if(currCmd.substr(0,6)=="return")
		return "C_RETURN";
	else{
		//why not cerr here???
		cout << "Invalid Syntax" << endl;
	}
}
	
string Parser :: arg1(){
	string C=cmdTyp();
	if(C!="C_RETURN"){
		size_t space1=currCmd.find(" ",0);
		if(space1!=string::npos){
			size_t space2 = currCmd.find(" ", space1 + 1);
			if(space2!=string::npos){
				return currCmd.substr(space1+1, space2-space1-1);
			}
			else{
				return currCmd.substr(space1+1, currCmd.length()-space1-1);
			}
		}
		else{
			return currCmd.substr(0, currCmd.length());
		}
	}
	else
		return "";
}

int Parser :: arg2(){
	string C=cmdTyp();
	if(C=="C_PUSH" || C=="C_POP" || C=="C_FUNCTION" || C=="C_CALL"){
		size_t space1=currCmd.find(" ",0);
		if(space1!=string::npos){
			size_t space2=currCmd.find(" ",space1+1);
			if(space2!=string::npos){
				size_t space3=currCmd.find(" ",space2+1);
				if(space3!=string::npos)
					return str2num(currCmd.substr(space2+1, space3-space2-1));
				else
					return str2num(currCmd.substr(space2+1, currCmd.length()-space2-1));
			}
			else{
				cout << "Invalid Syntax" << endl;
			}
				
		}
		else{
			cout << "Invalid Syntax" << endl;
		}
	}
}  	

class Code{
	ofstream fout;
	map<string, string> SegCode;
	int labno, retno;
	string fileName, funcName;
		public:
	Code(string& filename);
	void setFileName(string& filename);
	void writeArithmetic(string cmd);
	void writePushPop(string cmd, string seg, int index);
	void writeInit();
	void writeLabel(string label);
	void writeGoto(string label);
	void writeIf(string label);
	void writeCall(string func_name, int numArgs);
	void writeReturn();
	void writeFunction(string func_name, int numLcls);
};

Code :: Code(string& filename){
	fout.open(filename);
	
	writeInit();
	
	SegCode["local"]="LCL";
	SegCode["argument"]="ARG";
	SegCode["this"]="THIS";
	SegCode["that"]="THAT";
	SegCode["pointer"]="3";
	SegCode["temp"]="5";
	
}

void Code :: setFileName(string& filename){
	fileName=filename;
	labno=0;
	retno=0;
}

void Code :: writeArithmetic(string cmd){
	if(cmd=="add" || cmd=="sub" || cmd=="and" || cmd=="or"){
		fout<<"@SP\nAM=M-1\nD=M\nA=A-1\n";
		if(cmd=="add")
			fout<<"M=M+D\n";
		if(cmd=="sub")
			fout<<"M=M-D\n";
		if(cmd=="and")
			fout<<"M=M&D\n";
		if(cmd=="or")
			fout<<"M=M|D\n";
	}
	else if(cmd=="not")
		fout<<"@SP\nA=M-1\nM=!M\n";
	else if(cmd=="neg")
		fout<<"@SP\nA=M-1\nM=-M\n";
	else if(cmd=="eq" || cmd=="lt" || cmd=="gt"){
		fout << "@SP\nAM=M-1\nD=M\nA=A-1\nD=M-D\n" ;
		//cout << "wefwqfwegwge";
		fout << "@Label." << fileName << labno << endl;
		labno++;
		fout<<"D;";
		if(cmd=="eq")
			fout<<"JEQ\nD=0\n";
		if(cmd=="lt")
			fout<<"JLT\nD=0\n";
		if(cmd=="gt")
			fout<<"JGT\nD=0\n";
		fout<<"@Label."<< fileName << labno << endl;
		labno++;
		fout<<"0;JMP\n(Label."<< fileName << labno-2 <<")\nD=-1\n(Label."<<  fileName << labno-1 <<")\n@SP\nA=M-1\nM=D\n";
	}
}

void Code :: writePushPop(string cmd, string seg, int index){
	if(cmd=="C_PUSH"){
		if(SegCode.find(seg) != SegCode.end()){
			fout << "@" << SegCode[seg] << endl;
			if(seg=="temp" || seg=="pointer")
				fout << "D=A\n";
			else
				fout << "D=M\n";
			fout << "@" << index << endl;
			fout << "A=A+D\nD=M\n";
			fout << "@SP\nM=M+1\nA=M-1\nM=D\n";
		}
		else if(seg=="constant"){
			fout << "@" << index << endl << "D=A\n";
			fout << "@SP\nM=M+1\nA=M-1\nM=D\n";
		}
		else if(seg=="static"){
			fout << "@" << fileName << index << endl;
			fout << "D=M\n";
			fout << "@SP\nM=M+1\nA=M-1\nM=D\n";
		}
		else
			cout << "Invalid Segment in push: " << seg << endl; 
	}
	else{
		if(seg=="constant")
			cout << "Error: Segment of pop cannot be constant" << endl;
		else if(SegCode.find(seg) != SegCode.end()){
			fout << "@" << SegCode[seg] << endl;
			if(seg=="temp" || seg=="pointer")
				fout << "D=A\n";
			else
				fout << "D=M\n";
			fout << "@" << index << endl;
			fout << "D=A+D\n@R13\nM=D\n@SP\nAM=M-1\nD=M\n@R13\nA=M\nM=D\n";
		}
		else if(seg=="static"){
			fout << "@SP\nAM=M-1\nD=M\n@" << fileName << index << endl << "M=D\n";
		}
		else
			cout << "Invalid Segment in pop: " << seg << endl;
	}
}

void Code :: writeInit(){
	fout << "@256\nD=A\n@SP\nM=D\n";
}

void Code :: writeLabel(string label){
	fout << "(" << funcName << "$" << label << ")" << endl;
}

void Code :: writeGoto(string label){
	fout << "@" << funcName << "$" << label << endl;
	fout << "0;JMP\n";
}

void Code :: writeIf(string label){
	fout << "@SP\nAM=M-1\nD=M\n";
	fout << "@" << funcName << "$" << label << endl;
	fout << "D;JNE\n";
}

void Code :: writeCall(string func_name, int numArgs){
	//push return address
	fout << "@Return." << fileName << retno << endl;
	fout << "D=A\n@SP\nAM=M+1\nA=A-1\nM=D\n";
	//push LCL
	fout << "@LCL\nD=M\n@SP\nAM=M+1\nA=A-1\nM=D\n";
	//push ARG
	fout << "@ARG\nD=M\n@SP\nAM=M+1\nA=A-1\nM=D\n";
	//push THIS
	fout << "@THIS\nD=M\n@SP\nAM=M+1\nA=A-1\nM=D\n";
	//push THAT
	fout << "@THAT\nD=M\n@SP\nAM=M+1\nA=A-1\nM=D\n";
	//ARG=SP-numArgs-5
	fout << "@SP\nD=M\n@" << numArgs << endl;
	fout << "D=D-A\n@5\nD=D-A\n@ARG\nM=D\n";
	//LCL=SP
	fout << "@SP\nD=M\n@LCL\nM=D\n";
	//goto func_name
	fout << "@" << func_name << endl;
	fout << "0;JMP\n";
	fout << "(Return." << fileName << retno << ")" << endl;
	retno++;
}

void Code :: writeReturn(){
	//FRAME=LCL(new)
	fout << "@LCL\nD=M\n@FRAME\nM=D\n";
	//RETADD=*(FRAME-5)
	fout << "@5\nD=A\n@FRAME\nA=M-D\nD=M\n@RETADD\nM=D\n";
	//*ARG=pop()
	fout << "@SP\nAM=M-1\nD=M\n@ARG\nA=M\nM=D\n";
	//SP=ARG+1
	fout << "@ARG\nD=M+1\n@SP\nM=D\n";
	//THAT=*(FRAME-1)
	fout << "@FRAME\nA=M-1\nD=M\n@THAT\nM=D\n";
	//THIS=*(FRAME-2)
	fout << "@2\nD=A\n@FRAME\nA=M-D\nD=M\n@THIS\nM=D\n";
	//ARG=*(FRAME-3)
	fout << "@3\nD=A\n@FRAME\nA=M-D\nD=M\n@ARG\nM=D\n";
	//LCL=*(FRAME-4)
	fout << "@4\nD=A\n@FRAME\nA=M-D\nD=M\n@LCL\nM=D\n";
	//goto RETADD
	fout << "@RETADD\nA=M\n0;JMP\n";
}

void Code :: writeFunction(string func_name, int numLcls){
	funcName=func_name;
	fout << "(" << func_name << ")" << endl;
	for(int i=0; i<numLcls; i++){
		writePushPop("C_PUSH", "constant", 0);
	}
}

int main(int argc, char *argv[]){
	string outfil=argv[1];
	Code C(outfil);
	Parser P;
	bool x=false;

	for (int j = 2; j < argc; j++) {
		string s = argv[j];
		if (s == "Sys.vm")
			x = true;
	}
	
	if(x==true)
		C.writeCall("Sys.init", 0);
	//cout << x << endl;
	for(int i=2; i<argc; i++){
		string infil=argv[i];
		P.setFile(infil);
		C.setFileName(infil.substr(0, infil.length()-2));
		while(P.moreCmd()){
			P.advance();
			if(P.getcurrCmd()=="")
				break;
			string Cmd=P.cmdTyp();
			if(Cmd=="C_ARITHMETIC")
				C.writeArithmetic(P.arg1());
			if(Cmd=="C_PUSH")
				C.writePushPop(Cmd, P.arg1(), P.arg2());
			if(Cmd=="C_POP")
				C.writePushPop(Cmd, P.arg1(), P.arg2());
			if(Cmd=="C_LABEL")
				C.writeLabel(P.arg1());
			if(Cmd=="C_GOTO")
				C.writeGoto(P.arg1());
			if(Cmd=="C_IF")
				C.writeIf(P.arg1());
			if(Cmd=="C_CALL")
				C.writeCall(P.arg1(), P.arg2());
			if(Cmd=="C_RETURN")
				C.writeReturn();
			if(Cmd=="C_FUNCTION")
				C.writeFunction(P.arg1(), P.arg2());
		}
	}
	return 0;
}	