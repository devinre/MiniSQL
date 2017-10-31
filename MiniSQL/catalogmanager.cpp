#include "catalogmanager.h"
int catalogmanager::hastable(string table)
{
	for (int i = 0;i < multytable.size();i++)
		if (multytable[i].tablename == table)
			return 1;
	return 0;
}
table catalogmanager::gettable(string tablename)
{
	table a;
	for (int i = 0;i < tablesize;i++)
		if (multytable[i].tablename == tablename)
			return multytable[i];
	return a;
}
int catalogmanager::getcolnum(table& tableinf, string first)
{
	for (int i = 0;i < tableinf.attributes.size();i++)
		if (tableinf.attributes[i].name == first)
			return i;
	return -1;
}
int catalogmanager::hasindex(string indexname)
{
	int i;
	for (i = 0;i < multyindex.size();i++)
		if (multyindex[i].indexna == indexname)
			break;
	if (i >= multyindex.size())
		return 0;
	else
		return 1;
}
int catalogmanager::hasindex(string tablena, int x)
{
	int i;
	for (i = 0;i < multyindex.size();i++)
		if (multyindex[i].tablena == tablena && multyindex[i].col == x)
			break;
	if (i >= multyindex.size())
		return 0;
	else
		return 1;
}
int catalogmanager::countkind(table& tableinf)
{
	return tableinf.attributes.size();
}
void catalogmanager::createtable(table& newtable)
{
	tablesize++;
	for (int i = 0;i < newtable.attributes.size();i++)
	{
		newtable.sumlength += newtable.attributes[i].length;
	}
	multytable.push_back(newtable);
}
index catalogmanager::getindex(string tablename, int col)
{
	int i;
	for (i = 0;i < multyindex.size();i++)
		if (multyindex[i].tablena == tablename&&multyindex[i].col == col)
			break;
	if (i >= indexsize)
	{
		index a;
		return a;
	}
	return multyindex[i];
}
void catalogmanager::droptable(table oldtable)
{
	for (int i = tablesize - 1;i >= 0;i--)
		if (multytable[i].tablename == oldtable.tablename)
		{
			multytable.erase(multytable.begin() + i);
			tablesize--;
		}
	for(int i = indexsize-1;i>=0;i--)
		if (multyindex[i].tablena == oldtable.tablename)
		{
			multyindex.erase(multyindex.begin() + i);
			indexsize--;
		}
}
void catalogmanager::update(table& newtable)
{
	for (int i = 0;i < tablesize;i++)
		if (multytable[i].tablename == newtable.tablename)
		{
			multytable[i].amount = newtable.amount;
			multytable[i].blocknum = newtable.blocknum;
			multytable[i].sumlength = newtable.sumlength;
			multytable[i].attributes = newtable.attributes;
		}
}
void catalogmanager::createindex(index tempindex)
{
	indexsize++;
	multyindex.push_back(tempindex);
}
void catalogmanager::update(index& tempindex)
{
	for (int i = 0;i < indexsize;i++)
		if (multyindex[i].indexna == tempindex.indexna)
		{
			multyindex[i].tablena = tempindex.tablena;
			multyindex[i].col = tempindex.col;
			multyindex[i].block = tempindex.block;
		}
}
index catalogmanager::getindex(string indexname)
{
	int i;
	for (i = 0;i <tablesize;i++)
		if (multyindex[i].indexna==indexname)
			break;
	if (i >= indexsize)
	{
		index a;
		return a;
	}
	return multyindex[i];
}
void catalogmanager::dropindex(string indexname)
{
	for(int i=indexsize-1;i>=0;i--)
		if (multyindex[i].indexna == indexname)
		{
			multyindex.erase(multyindex.begin()+ i);
			indexsize--;
		}
}

catalogmanager::catalogmanager()
{
	const string filename = "t.c";
	fstream fin(filename.c_str(), ios::in);
	fin >> tablesize;
	for (int i = 0;i < tablesize;i++)
	{
		table temptable;
		fin >> temptable.tablename;
		fin >> temptable.amount;
		fin >> temptable.blocknum;
		for (int j = 0;j < temptable.amount;j++)
		{
			attribute tempattribute;
			fin >> tempattribute.name;
			fin >> tempattribute.type;
			fin >> tempattribute.length;
			fin >> tempattribute.primary;
			fin >> tempattribute.unique;
			temptable.attributes.push_back(tempattribute);
			temptable.sumlength += tempattribute.length;
		}
		multytable.push_back(temptable);
	}
	fin.close();
	const string filename2 = "i.c";
	fstream fin2(filename2.c_str(), ios::in);
	fin2 >> indexsize;
	for (int i = 0;i < indexsize;i++)
	{
		index tempindex;
		fin2 >> tempindex.indexna;
		fin2 >> tempindex.tablena;
		fin2 >> tempindex.col;
		fin2 >> tempindex.collength;
		fin2 >> tempindex.block;
		multyindex.push_back(tempindex);
	}
	fin2.close();
}
catalogmanager::~catalogmanager()
{
	const string filename = "t.c";
	fstream fout(filename.c_str(), ios::out);
	fout << tablesize<<endl;
	for (int i = 0;i < tablesize;i++)
	{
		fout << multytable[i].tablename<<" ";
		fout << multytable[i].amount<<" ";
		fout << multytable[i].blocknum<<endl;
		for (int j = 0;j < multytable[i].amount;j++)
		{
			fout << multytable[i].attributes[j].name << " ";
			fout << multytable[i].attributes[j].type << " ";
			fout << multytable[i].attributes[j].length << " ";
			fout << multytable[i].attributes[j].primary << " ";
			fout << multytable[i].attributes[j].unique << endl;
		}
	}
	fout.close();
	const string filename2 = "i.c";
	fstream fout2(filename2.c_str(), ios::out);
	fout2 << indexsize<<endl;
	for (int i = 0;i < indexsize;i++)
	{
		fout2<<multyindex[i].indexna<<" ";
		fout2 << multyindex[i].tablena<<" ";
		fout2 << multyindex[i].col<<" ";
		fout2 << multyindex[i].collength<<" ";
		fout2 << multyindex[i].block<<endl;
	}
	fout2.close();
}