#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <algorithm>
#include <bitset> 

using namespace std;

class SymbolTable {
	map<string, int> symtab;
public:
	SymbolTable();
	bool contains(string symbol){
		if(symtab.find(symbol)==symtab.end())
			return 0;
		return 1;
	}
	
	void addEntry(string symbol, int address){
		if(contains(symbol)==0)
			symtab[symbol]=address;
	}
	
	int getAddress(string symbol){
		if(contains(symbol)==1)
			return symtab[symbol];
		return 0;
	}
};

SymbolTable :: SymbolTable(){
	symtab["SP"]=0;
	symtab["LCL"]=1;
	symtab["ARG"]=2;
	symtab["THIS"]=3;
	symtab["THAT"]=4;
	symtab["R0"]=0;
	symtab["R1"]=1;
	symtab["R2"]=2;
	symtab["R3"]=3;
	symtab["R4"]=4;
	symtab["R5"]=5;
	symtab["R6"]=6;
	symtab["R7"]=7;
	symtab["R8"]=8;
	symtab["R9"]=9;
	symtab["R10"]=10;
	symtab["R11"]=11;
	symtab["R12"]=12;
	symtab["R13"]=13;
	symtab["R14"]=14;
	symtab["R15"]=15;
	symtab["SCREEN"]=16384;
	symtab["KBD"]=24576;

}

class Code {
	map<string, string> ctable;
	map<string, string> dtable;
	map<string, string> jtable;
public:
	Code();
	string dconverter(string dmnemo, int lNo);
	string cconverter(string cmnemo, int lNo);
	string jconverter(string jmnemo, int lNo);
};

Code :: Code() {
	ctable["0"] = "0101010";
	ctable["1"] = "0111111";
	ctable["-1"] = "0111010";
	ctable["!D"] = "0001101";
	ctable["!A"] = "0110001";
	ctable["-D"] = "0001111";
	ctable["-A"] = "0110011";
	ctable["D+1"] = "0011111";
	ctable["1+D"] = "0011111";
	ctable["M"] = "1110000";
	ctable["!M"] = "1110001";
	ctable["-M"] = "1110011";
	ctable["M+1"] = "1110111";
	ctable["A+1"] = "0110111";
	ctable["D-1"] = "0001110";
	ctable["A-1"] = "0110010";
	ctable["D+A"] = "0000010";
	ctable["D-A"] = "0010011";
	ctable["A-D"] = "0000111";
	ctable["D&A"] = "0000000";
	ctable["D|A"] = "0010101";
	ctable["1+M"] = "1110111";
	ctable["1+A"] = "0110111";
	ctable["-1+D"] = "0001110";
	ctable["-1+A"] = "0110010";
	ctable["A+D"] = "0000010";
	ctable["-A+D"] = "0010011";
	ctable["-D+A"] = "0000111";
	ctable["A&D"] = "0000000";
	ctable["A|D"] = "0010101";
	ctable["D"] = "0001100";	
	ctable["A"] = "0110000";
	ctable["M-1"] = "1110010";
	ctable["D+M"] = "1000010";
	ctable["D-M"] = "1010011";
	ctable["M-D"] = "1000111";
	ctable["D&M"] = "1000000";
	ctable["D|M"] = "1010101";
	ctable["-1+M"] = "1110010";
	ctable["M+D"] = "1000010";
	ctable["-M+D"] = "1010011";
	ctable["-D+M"] = "1000111";
	ctable["M&D"] = "1000000";
	ctable["M|D"] = "1010101";
	
	dtable[""] = "000";
	dtable["M"] = "001";
	dtable["D"] = "010";
	dtable["MD"] = "011";
	dtable["DM"] = "011";
	dtable["A"] = "100";
	dtable["AM"] = "101";
	dtable["AD"] = "110";
	dtable["AMD"] = "111";
	dtable["MA"] = "101";
	dtable["DA"] = "110";
	dtable["ADM"] = "111";
	dtable["DAM"] = "111";
	dtable["DMA"] = "111";
	dtable["MAD"] = "111";
	dtable["MDA"] = "111";
	
	jtable[""] = "000";
	jtable["JGT"] = "001";
	jtable["JLT"] = "100";
	jtable["JNE"] = "101";
	jtable["JGE"] = "011";
	jtable["JLE"] = "110";
	jtable["JEQ"] = "010";
	jtable["JMP"] = "111";
}

string Code :: dconverter(string dmnemo, int lNo){
	if(dtable.find(dmnemo)!=dtable.end())
		return dtable[dmnemo];
	cout << "Invalid Syntax At Line Number : " << lNo << endl;
	exit(1);
}

string Code :: cconverter(string cmnemo, int lNo){
	if(ctable.find(cmnemo)!=ctable.end())
		return ctable[cmnemo];
	cout << "Invalid Syntax At Line Number : " << lNo << endl;
	exit(1);
}

string Code :: jconverter(string jmnemo, int lNo){
	if(jtable.find(jmnemo)!=jtable.end())
		return jtable[jmnemo];
	cout << "Invalid Syntax At Line Number : " << lNo << endl;
	exit(1);
}

class Parser {
	ifstream fin;
	string currComm;
	map<char, char> CommTab;
public:
	Parser(string& filename);
	bool moreComm() {
		return (!fin.eof());
	}
	void moveOn(int& lineNo);
	char commaTyp(int lineNo);
	string Psym();
	string Pcomp();
	string Pjump();
	string Pdest();
	
};

Parser :: Parser(string& filename) {
		fin.open(filename);
		
		CommTab['A']='C';
		CommTab['M']='C';
		CommTab['D']='C';
		CommTab['0']='C';
		CommTab['1']='C';
		CommTab['-']='C';
		CommTab['!']='C';
		CommTab['(']='L';
		CommTab['@']='A';
}

void Parser :: moveOn(int& lineNo) {
	string currLine;
	bool commandFound = false;
	int commentpos;
	
	while((!commandFound) && getline(fin, currLine)){
		lineNo++;
		currLine.erase(remove(currLine.begin(), currLine.end(), ' '), currLine.end());
		commentpos=currLine.find("//");
		if(commentpos!=string::npos){
			currLine.erase(commentpos, (currLine.length()-commentpos));
		}
		commandFound=!currLine.empty();
	}
	currComm=currLine;
}

char Parser :: commaTyp(int lineNo){
	if(CommTab.find(currComm[0]) != CommTab.end())
		return CommTab[currComm[0]];
	cout << "Invalid Syntax At Line Number : " << lineNo << endl;
	exit(1);
}

string Parser :: Psym() {
	int st, end;
	st=currComm.find('(');
	end=currComm.find(')');
	if(currComm[0]=='@')
		return currComm.substr(1, currComm.length()-1);
	else if(st!=string::npos && end!=string::npos)
		return currComm.substr(st+1, end-st-1);
	return "";
}

string Parser :: Pdest() {
	int equalto;
	equalto=currComm.find('=');
	if(equalto != string::npos)
		return currComm.substr(0, equalto);
	return "";
}

string Parser :: Pjump() {
	int semic=currComm.find(';');
	if(semic != string::npos)
		return currComm.substr(semic+1, currComm.length()-semic-1);
	return "";
}

string Parser :: Pcomp() {
	int equalto, semic;
	equalto=currComm.find('=');
	semic=currComm.find(';');
	if(equalto != string::npos){
		if(semic != string::npos){
			return currComm.substr(equalto+1, semic-equalto-1);
		}
	return currComm.substr(equalto+1, currComm.length()-equalto-1);
	}
	else if(semic != string::npos){
		return currComm.substr(0,semic);
	}
	return "";
}

int str2num(string a){
	int len=a.length(), j=1, ans=a[0]-'0';
	while(j<len){
		ans=ans*10+(a[j]-'0');
		j++;
	}
	return ans;
}

int main(int argc ,char *argv[]){
	string inpfil, outfil;
	int lNoRom=0, lNoErr=0;
	ofstream fout;
	
	inpfil=argv[1];
	outfil=inpfil.substr(0, inpfil.length()-3) + "hack";
	Parser pass1(inpfil);
	fout.open(outfil);
	SymbolTable sym_table;
	while(1){
		pass1.moveOn(lNoErr);
		if(!pass1.moreComm())
			break;
		char comm_type =pass1.commaTyp(lNoErr);
		if((comm_type=='A') || (comm_type=='C'))
			lNoRom++;
		else if (!sym_table.contains(pass1.Psym()))
			sym_table.addEntry(pass1.Psym(), lNoRom);
		
	}
	lNoErr=0;
	
	Code conv;
	Parser pass2(inpfil);
	int ramAdd=16;
	
	while(1){
		pass2.moveOn(lNoErr);
		if(!pass2.moreComm())
			break;
		char comm_type=pass2.commaTyp(lNoErr);
		if(comm_type=='A'){
			fout << '0';
			if(((pass2.Psym())[0]) <= '9') {
				fout << bitset<15>(str2num(pass2.Psym())).to_string();
			}
			else {
				if(!sym_table.contains(pass2.Psym())){
					sym_table.addEntry(pass2.Psym(), ramAdd);
					ramAdd++;
				}
				fout << bitset<15>(sym_table.getAddress(pass2.Psym())).to_string();
			}
			fout << endl;
		}
		else if(comm_type=='C'){
			fout << "111";
			fout << conv.cconverter(pass2.Pcomp(), lNoErr);
			fout << conv.dconverter(pass2.Pdest(), lNoErr);
			fout << conv.jconverter(pass2.Pjump(), lNoErr);
			fout << endl;
		}
	}		
	return 0;
}