#ifndef __CATALOGMANAGER_H__
#define __CATALOGMANAGER_H__
#include <iostream>
#include <vector>
#include <fstream>
#include <string>
using namespace std;
class value
{
public:
	vector<string> newvalues;
};
class index
{
public:
	string indexna;
	string tablena;
	int col;
	int block;
	int collength;
	index():col(0),block(0){}
};
class attribute
{
public:
	string name;
	int type;
	int length;
	int unique;
	int primary;
	attribute() 
	{
		unique = 0;
		primary = 0;
	}
	attribute(string n,int t,int l,int u,int p)
		:name(n),type(t),length(l),unique(u),primary(p){}
};
class table
{
public:
	string tablename;
	vector<attribute> attributes;
	int sumlength;
	int amount;
	int blocknum;
	table():blocknum(0),amount(0),sumlength(0){}
};
class catalogmanager
{
public:
	int hastable(string table);
	table gettable(string tablename);
	int getcolnum(table& tableinf, string first);
	int hasindex(string indexname);
	int hasindex(string tablena, int x);
	int countkind(table& tableinf);
	void createtable(table& newtable);
	index getindex(string tablename, int col);
	void droptable(table oldtable);
	void update(table& newtable);
	void createindex(index tempindex);
	void update(index& tempindex);
	index getindex(string indexname);
	void dropindex(string indexname);
	catalogmanager();
	~catalogmanager();
private:
	vector<table> multytable;
	int tablesize;
	vector<index> multyindex;
	int indexsize;
};


#endif // !

