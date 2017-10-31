#ifndef __BUFFERMANAGER_H__
#define __BUFFERMANAGER_H__
#include <iostream>
#include "catalogmanager.h"
using namespace std;
const int MAXBLOCKAMOUNT = 1000;
const int BLOCKSIZE = 4096;
class insertposition
{
public:
	int bufnum;
	int position;
};
class buf
{
public:
	string filename;
	int valid;
	int write;
	int lru;
	int offset;
	char contents[BLOCKSIZE + 1];
	buf() 
	{
		initial();
	}
	void initial()
	{
		write = 0;
		valid = 0;
		filename = "NULL";
		lru = 0;
		offset = 0;
		for (int i = 0;i < BLOCKSIZE;i++) contents[i] = '@';
		contents[BLOCKSIZE] = '\0';
	}
	string getrecord(int begin, int end)
	{
		string temp = "";
		if (begin >= 0 && begin <= end&&end <= BLOCKSIZE)
			for (int i = begin;i < end;i++)
				temp += contents[i];
		return temp;
	}
};
class buffermanager
{
public:
	void changevalid(string filename);
	buf bufblock[MAXBLOCKAMOUNT];
	insertposition findinsertposition(table& tableinf);
	int addblock(table& tableinf);
	int findblankbuffer();
	void storedisk(int number);
	int judgeinbuffer(string filename, int blockoffset);
	void importblock(string filename,int blockoffset,int buffernum);
	int getbuffernumber(string filename, int offset);
	int findblankbufferfile(string filename);
	int addblock(index& indexinf);
	friend class recordmanager;
	friend class indexmanager;
	friend class leaf;
	friend class branch;
	friend class bplustree;
	buffermanager();
	~buffermanager();
};
#endif