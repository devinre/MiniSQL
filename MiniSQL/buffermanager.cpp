#include "buffermanager.h"
void buffermanager::changevalid(string filename)
{
	for (int i = 0;i < MAXBLOCKAMOUNT;i++)
		if (bufblock[i].filename == filename)
		{
			bufblock[i].valid = 0;
			bufblock[i].write = 0;
		}
}
insertposition buffermanager::findinsertposition(table& tableinf)
{
	insertposition insp;
	if (tableinf.blocknum == 0)
	{
		insp.bufnum = addblock(tableinf);
		insp.position = 0;
		return insp;
	}
	string filename = tableinf.tablename + ".t";
	int length = tableinf.sumlength + 1;
	int blockoffset = tableinf.blocknum - 1;
	int buffernum = judgeinbuffer(filename, blockoffset);
	if (buffernum == -1)
	{
		buffernum = findblankbuffer();
		importblock(filename, blockoffset, buffernum);
	}
	const int recordamount = BLOCKSIZE / length;
	for (int i = 0;i < recordamount;i++)
	{
		int address = i*length;
		char temp = bufblock[buffernum].contents[address];
		if (temp == '@')
		{
			insp.bufnum = buffernum;
			insp.position = address;
			return insp;
		}
	}
	insp.bufnum = addblock(tableinf);
	insp.position = 0;
	return insp;
}
int buffermanager::addblock(table& tableinf)
{
	int i = findblankbuffer();
	bufblock[i].initial();
	bufblock[i].valid = 1;
	bufblock[i].write = 1;
	bufblock[i].filename = tableinf.tablename + ".t";
	bufblock[i].offset = tableinf.blocknum++;
	return i;
}
int buffermanager::findblankbuffer()
{
	int number = 0;
	int maxlru = bufblock[0].lru;
	for (int i = 0;i < MAXBLOCKAMOUNT;i++)
	{
		if (!bufblock[i].valid)
		{
			bufblock[i].initial();
			bufblock[i].valid = 1;
			return i;
		}
		else if (maxlru < bufblock[i].lru)
		{
			maxlru = bufblock[i].lru;
			number = i;
		}
	}
	storedisk(number);
	bufblock[number].valid = 1;
	return number;
}
void buffermanager::storedisk(int number)
{
	if (!bufblock[number].write)
		return;
	string filename = bufblock[number].filename;
	fstream fout(filename.c_str(), ios::in | ios::out);
	fout.seekp(BLOCKSIZE*bufblock[number].offset, fout.beg);
	fout.write(bufblock[number].contents, BLOCKSIZE);
	bufblock[number].initial();
	fout.close();
}
int buffermanager::judgeinbuffer(string filename, int blockoffset)
{
	for (int i = 0;i < MAXBLOCKAMOUNT;i++)
		if (bufblock[i].filename == filename&&bufblock[i].offset == blockoffset)
			return i;
	return -1;
}
void buffermanager::importblock(string filename, int blockoffset, int buffernum)
{
	bufblock[buffernum].valid = 1;
	bufblock[buffernum].write = 0;
	bufblock[buffernum].filename = filename;
	bufblock[buffernum].offset = blockoffset;
	fstream  fin(filename.c_str(), ios::in);
	fin.seekp(BLOCKSIZE*blockoffset, fin.beg);
	fin.read(bufblock[buffernum].contents, BLOCKSIZE);
	fin.close();
}
int buffermanager::getbuffernumber(string filename, int offset)
{
	int number = judgeinbuffer(filename, offset);
	if (number == -1)
	{
		number = findblankbufferfile(filename);
		importblock(filename, offset, number);
	}
	return number;
}
int buffermanager::findblankbufferfile(string filename)
{
	int number = -1;
	int maxlru = bufblock[0].lru;
	for (int i = 0;i < MAXBLOCKAMOUNT;i++)
	{
		if (!bufblock[i].valid)
		{
			bufblock[i].valid = 1;
			return i;
		}
		else if (maxlru < bufblock[i].lru&&bufblock[i].filename!=filename)
		{
			maxlru = bufblock[i].lru;
			number = i;
		}
	}
	if (number == -1)
	{
		cout << "ERROR:All the buffer has been used" << endl;
		exit(0);
	}
	storedisk(number);
	bufblock[number].valid = 1;
	return number;
}
int buffermanager::addblock(index& indexinf)
{
	string filename = indexinf.indexna + ".i";
	int i = findblankbufferfile(filename);
	bufblock[i].initial();
	bufblock[i].valid = 1;
	bufblock[i].write = 1;
	bufblock[i].filename = filename;
	bufblock[i].offset = indexinf.block++;
	return i;
}
buffermanager::buffermanager()
{
	for (int i = 0;i < MAXBLOCKAMOUNT;i++)
		bufblock[i].initial();
}
buffermanager::~buffermanager()
{
	for (int i = 0;i < MAXBLOCKAMOUNT;i++)
		storedisk(i);
}