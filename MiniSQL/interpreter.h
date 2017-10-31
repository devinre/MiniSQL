#ifndef __INTERPRETER_H__
#define __INTERPRETER_H__
#include <iostream>
#include <vector>
#include "catalogmanager.h"
using namespace std;
const int EMPTY = 0;
const int QUIT = 1;
const int SELECT = 2;
const int TABLE = 3;
const int NOWHERE = 4;
const int COND = 5;
const int SELECT_WHERE = 6;
const int CREATETABLE = 7;
const int HASALREADYTABLE = 8;
const int INT = 9;
const int INTLENGTH = 11;
const int FLOAT = 12;
const int FLOATLENGTH = 10;
const int CHAR = 13;
const int CHARERROR = 14;
const int PRIMARY = 15;
const int UNIQUE = 16;
const int CREATET = 17;
const int CREATEINDEX = 18;
const int ALREADYINDEX = 19;
const int CREATEIND = 20;
const int DELETEWRONG = 21;
const int DELETESUCCESS = 22;
const int INSERTWRONG = 23;
const int NUMBERWRONG = 24;
const int INSERTSUCCESS = 25;
const int DROPTABLE = 26;
const int DROPTABLESUCCESS = 27;
const int DROPINDEX = 28;
const int NOINDEX = 29;
const int DROPINDEXSUCCESS = 30;
const int UNKNOWN = 31;
const int EXECFILE = 32;
extern catalogmanager catalog;
enum compareop {lessthan,lessthanequal,largerthan,largerthanequal,isequal,notequal};
class condition
{
public:
	compareop operation;
	int colnum;
	string value;
};

class interpreter
{
public:
	friend void run();
	void explain(char* cmd);
	int findfirst(string & sql, string & first);
	int op;
	interpreter() {
		op = UNKNOWN;
		tablename = "";
		indexname = "";
		attributename = "";
		primarycol = -1;
		uniquecol = -1;
	}
	~interpreter(){}
	vector<attribute> mulobject;
	string tablename;
	table tableinf;
	string attributename;
	vector<condition> conditions;
	string indexname;
	index indexinf;
	value values;
	int primarycol;
	int uniquecol;
	vector<condition> another;
	string filename;
protected:
	int judgeint(const char *);
	int findstring(string & sql, string & first);
};

#endif