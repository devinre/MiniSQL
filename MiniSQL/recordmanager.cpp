#include "recordmanager.h"
void recordmanager::createtable(table newtable)
{
	string filename = newtable.tablename + ".t";
	fstream fout(filename.c_str(), ios::out);
	fout.close();
}
void recordmanager::droptable(table oldtable)
{
	string filename = oldtable.tablename + ".t";
	if (remove(filename.c_str()) != 0)
		perror("Delete fail");
	else
		buffer.changevalid(filename);
}
void recordmanager::insert(table& tableinf, value& values)
{
	string result = tabletovalue(tableinf, values);
	insertposition insp = buffer.findinsertposition(tableinf);
buffer.bufblock[insp.bufnum].contents[insp.position] = '1';
for (int i = 0;i < tableinf.sumlength;i++)
	buffer.bufblock[insp.bufnum].contents[insp.position + i + 1] = result.c_str()[i];
buffer.bufblock[insp.bufnum].write = 1;
}
string recordmanager::tabletovalue(table tableinf, value values)
{
	string tempvalue;
	string result;
	for (int i = 0;i < values.newvalues.size();i++)
	{
		tempvalue = values.newvalues[i];
		for (;tempvalue.length() < tableinf.attributes[i].length;tempvalue += '@');
		result += tempvalue;
	}
	return result;
}
display recordmanager::select(table temptable, vector<condition> conditions)
{
	display temp;
	if (conditions.size() == 0)
	{
		temp = select(temptable);
		return temp;
	}
	string filename = temptable.tablename + ".t";
	string record;
	value row;
	int length = temptable.sumlength + 1;
	const int recordnumber = BLOCKSIZE / length;
	for (int i = 0;i < temptable.blocknum;i++)
	{
		int number = buffer.judgeinbuffer(filename, i);
		if (number == -1)
		{
			number = buffer.findblankbuffer();
			buffer.importblock(filename, i, number);
		}
		for (int j = 0;j < recordnumber;j++)
		{
			int pos = j*length;
			record = buffer.bufblock[number].getrecord(pos, pos + length);
			if (record.c_str()[0] != '@')
			{
				record.erase(record.begin());
				row = dealwithrecord(temptable, record);
				if (selectcondition(temptable, row, conditions))
					temp.displays.push_back(row);
			}
		}
	}
	return temp;
}
display recordmanager::select(table& temptable)
{
	string filename = temptable.tablename + ".t";
	string record;
	value row;
	display rows;
	int length = temptable.sumlength + 1;
	const int recordnumber = BLOCKSIZE / length;
	for (int i = 0;i < temptable.blocknum;i++)
	{
		int number = buffer.judgeinbuffer(filename, i);
		if (number == -1)
		{
			number = buffer.findblankbuffer();
			buffer.importblock(filename, i, number);
		}
		for (int j = 0;j < recordnumber;j++)
		{
			int pos = j*length;
			record = buffer.bufblock[number].getrecord(pos, pos + length);
			if (record.c_str()[0] == '@')
				continue;
			record.erase(record.begin());
			row = dealwithrecord(temptable, record);
			rows.displays.push_back(row);
		}
	}
	return rows;
}
value recordmanager::dealwithrecord(table temptable, string record)
{
	value temp;
	int start = 0, end = 0;
	for (int i = 0;i < temptable.amount;i++)
	{
		start = end;
		end += temptable.attributes[i].length;
		string te_mp;
		for (int j = start;j < end;j++)
			te_mp += record[j];
		temp.newvalues.push_back(te_mp);
	}
	return temp;
}
int recordmanager::selectcondition(table temptable, value row, vector<condition> conditions)
{
	for (int i = 0;i < conditions.size();i++)
	{
		int col = conditions[i].colnum;
		string a = "";
		string b = conditions[i].value;
		int c = 0;
		int d = b.length();
		for (int j = 0;j < row.newvalues[col].length();j++)
		{
			if (row.newvalues[col].c_str()[j] == '@')
			{
				c = j;
				break;
			}
			a += row.newvalues[col].c_str()[j];
		}
		float e = atof(a.c_str());
		float f = atof(b.c_str());
		switch (temptable.attributes[col].type)
		{
		case CHAR:
			switch (conditions[i].operation)
			{
			case lessthan:
				if (a >= b)
					return 0;
				break;
			case lessthanequal:
				if (a > b)
					return 0;
				break;
			case largerthan:
				if (a <= b)
					return 0;
				break;
			case largerthanequal:
				if (a < b)
					return 0;
				break;
			case isequal:
				if (a != b)
					return 0;
				break;
			case notequal:
				if (a == b)
					return 0;
				break;
			}
			break;
		case INT:
			switch (conditions[i].operation)
			{
			case lessthan:
				if (c > d)
					return 0;
				else if (c < d)
					break;
				else if (a >= b)
					return 0;
				break;
			case lessthanequal:
				if (c > d)
					return 0;
				else if (c < d)
					break;
				else if (a > b)
					return 0;
				break;
			case largerthan:
				if (c < d)
					return 0;
				else if (c > d)
					break;
				else if (a <= b)
					return 0;
				break;
			case largerthanequal:
				if (c < d)
					return 0;
				else if (c > d)
					break;
				else if (a < b)
					return 0;
				break;
			case isequal:
				if (c != d)
					return 0;
				else if (a != b)
					return 0;
				break;
			case notequal:
				if (c != d)
					break;
				else if (a == b)
					return 0;
				break;
			}
			break;
		case FLOAT:
			switch (conditions[i].operation)
			{
			case lessthan:
				if (e >= f)
					return 0;
				break;
			case lessthanequal:
				if (e > f)
					return 0;
				break;
			case largerthan:
				if (e <= f)
					return 0;
				break;
			case largerthanequal:
				if (e < f)
					return 0;
				break;
			case isequal:
				if (e != f)
					return 0;
				break;
			case notequal:
				if (e == f)
					return 0;
				break;
			}
		}
	}
	return 1;
}
int recordmanager::deletev(table tableinf, vector<condition>conditions)
{
	string filename = tableinf.tablename + ".t";
	string result;
	value newvalue;
	int a = 0;
	const int amount = BLOCKSIZE / (tableinf.sumlength + 1);
	for (int i = 0;i < tableinf.blocknum;i++)
	{
		int buffernum = buffer.judgeinbuffer(filename, i);
		if (buffernum == -1)
		{
			buffernum = buffer.findblankbuffer();
			buffer.importblock(filename, i, buffernum);
		}
		for (int j = 0;j < amount;j++)
		{
			int pos = j*(tableinf.sumlength + 1);
			result = buffer.bufblock[buffernum].getrecord(pos, pos + tableinf.sumlength + 1);
			if (result.c_str()[0] != '@')
			{
				result.erase(result.begin());
				newvalue = dealwithrecord(tableinf, result);
				if (selectcondition(tableinf, newvalue, conditions))
				{
					buffer.bufblock[buffernum].contents[pos] = '@';
					a++;
				}
			}
		}
		buffer.bufblock[buffernum].write = 1;
	}
	return a;
}