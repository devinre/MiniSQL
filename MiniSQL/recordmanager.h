#ifndef __RECORDMANAGER_H__
#define __RECORDMANAGER_H__
#include "catalogmanager.h"
#include "buffermanager.h"
#include "interpreter.h"
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
extern buffermanager buffer;
class display
{
public:
	vector<value> displays;
};
class recordmanager
{
public:
	void createtable(table newtable);
	void droptable(table oldtable);
	void insert(table& tableinf, value& values);
	display select(table temptable, vector<condition> conditions);
	display select(table& temptable);
	int deletev(table tableinf, vector<condition>conditions);
private:
	string tabletovalue(table tableinf, value values);
	value dealwithrecord(table temptable, string record);
	int selectcondition(table temptable, value row, vector<condition> conditions);
};
#endif
