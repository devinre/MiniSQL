#ifndef __INDEXMANAGER_H__
#define __INDEXMANAGER_H__
#include <iostream>
#include <list>
#include <fstream>
#include "catalogmanager.h"
#include "buffermanager.h"
#include "recordmanager.h"
using namespace std;
extern buffermanager buffer;
const int POINTLENGTH = 5;
class indexleaf
{
public:
	string key;
	int fileoffset;
	int blockoffset;
	indexleaf():
		key(""),fileoffset(0),blockoffset(0){}
	indexleaf(string key,int fileoffset,int blockoffset):
		key(key),fileoffset(fileoffset),blockoffset(blockoffset){}
};
class indexbranch
{
public:
	string key;
	int child;
	indexbranch() :
		key(""), child(0) {}
	indexbranch(string key,int child):
		key(key),child(child){}
};
class bplustree
{
public:
	int judgeroot;
	int bufnumber;
	int getrecordnumber();
	int recordnum;
	int father;
	int getpoint(int position);
	int collength;
	bplustree(int number):bufnumber(number),recordnum(0){}
	bplustree(){}
};
class leaf :public bplustree
{
public:
	int nextsibling;
	int lastsibling;
	leaf(int number, const index& tempindex);
	void insert(indexleaf node);
	list<indexleaf> nodelist;
	leaf(int number);
	indexleaf pop();
	indexleaf getfront();
	~leaf();
};
class branch :public bplustree
{
public:
	branch(int number, const index& tempindex);
	branch(int number):bplustree(number){}
	void insert(indexbranch node);
	list<indexbranch> nodelist;
	indexbranch pop();
	indexbranch getfront();
	branch(){}
	~branch();
};
class indexmanager
{
public:
	void dropindex(index& tempindex);
	display selectequal(const table& temptable,const index& tempindex, string tempstring,int offset=0);
	value dealwithrecord(table temptable, string record);
	void createindex(const table& temptable, index& tempindex);
	indexbranch insertvalue(index& tempindex, indexleaf node, int blockoffset = 0);
private:
	string getcolkey(const table& temptable, const index& tempindex, string result);
};
#endif
