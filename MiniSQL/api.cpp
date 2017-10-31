#include "api.h"
int inputend(char *input)
{
	int a = strlen(input) - 1;
	char b = ' ';
	while (b == '\t' || b == ' ')
	{
		b = input[a];
		a--;
	}
	if (b == ';')
	{
		input[a + 1] = '\0';
		return 1;
	}
	return 0;
}
void run()
{
	index tempindex;
	table temptable;
	display afterselect;
	int pripos;
	int rownum;
	string tempstring;
	switch (analysis.op)
	{
	case CREATET:
		analysis.tableinf.amount = analysis.tableinf.attributes.size();
		catalog.createtable(analysis.tableinf);
		record.createtable(analysis.tableinf);
		cout << "MiniSQL has created table: " << analysis.tableinf.tablename << endl;
		break;
	case HASALREADYTABLE:
		cout << "ERROR:The table has already exiested in the database" << endl;
		break;
	case DROPTABLESUCCESS:
		record.droptable(analysis.tableinf);
		for (int i = 0;i < analysis.tableinf.amount;i++)
		{
			tempindex = catalog.getindex(analysis.tableinf.tablename, i);
			if (tempindex.indexna != "")
				ind.dropindex(tempindex);
		}
		catalog.droptable(analysis.tableinf);
		cout << "MiniSQL has dropped table: " << analysis.tableinf.tablename << endl;
		break;
	case INSERTSUCCESS:
		temptable = analysis.tableinf;
		if (analysis.primarycol == -1 && analysis.uniquecol == -1)
		{
			record.insert(temptable, analysis.values);
			catalog.update(temptable);
			cout << "MiniSQL has inserted the value successfully" << endl;
		}
		if (analysis.primarycol != -1)
		{
			afterselect = record.select(temptable, analysis.conditions);
			if (afterselect.displays.size() > 0)
			{
				cout << "ERROR:The primary key has already existed" << endl;
				break;
			}
		}
		if (analysis.uniquecol != -1)
		{
			afterselect = record.select(temptable, analysis.another);
			if (afterselect.displays.size() > 0)
			{
				cout << "ERROR:The unique value has already existed" << endl;
				break;
			}
		}
		record.insert(temptable, analysis.values);
		catalog.update(temptable);
		cout << "MiniSQL has inserted the value successfully" << endl;
		break;
	case INSERTWRONG:
		cout << "ERROR:Wrong input after \"insert into\"" << endl;
		break;
	case NOWHERE:
		temptable = analysis.tableinf;
		afterselect = record.select(temptable);
		if (afterselect.displays.size() != 0)
			visual(afterselect, temptable, analysis.mulobject);
		else
			cout << "MiniSQL has found nothing" << endl;
		break;
	case SELECT_WHERE:
		temptable = analysis.tableinf;
		if (analysis.conditions.size() == 1)
		{
			for (int i = 0;i < analysis.tableinf.attributes.size();i++)
			{
				if ((analysis.tableinf.attributes[i].primary == 1 || analysis.tableinf.attributes[i].unique == 1) && analysis.tableinf.attributes[i].name == analysis.attributename)
				{
					pripos = i;
					tempindex = catalog.getindex(temptable.tablename, i);
					break;
				}
			}
			if (pripos = analysis.conditions[0].colnum&&analysis.conditions[0].operation == isequal&&tempindex.tablena != "")
			{
				tempstring = analysis.conditions[0].value;
				afterselect = ind.selectequal(temptable, tempindex, tempstring);
			}
			else
				afterselect = record.select(temptable, analysis.conditions);
		}
		else
			afterselect = record.select(temptable, analysis.conditions);
		if (afterselect.displays.size() != 0)
			visual(afterselect, temptable, analysis.mulobject);
		else
			cout << "MiniSQL found nothing" << endl;
		break;
	case DELETESUCCESS:
		rownum = record.deletev(analysis.tableinf, analysis.conditions);
		cout << "MiniSQL has deleted " << rownum << "records" << endl;
		break;
	case CREATEIND:
		temptable = analysis.tableinf;
		tempindex = analysis.indexinf;
		if (temptable.attributes[tempindex.col].primary && !temptable.attributes[tempindex.col].unique)
		{
			cout << "ERROR:attribute " << temptable.attributes[tempindex.col].name << "is not unique" << endl;
			break;
		}
		catalog.createindex(tempindex);
		ind.createindex(temptable, tempindex);
		catalog.update(tempindex);
		cout << "MiniSQL has created index: " << tempindex.indexna << endl;
		break;
	case ALREADYINDEX:
		cout << "ERROR:The index has already existed" << endl;
	case DROPINDEXSUCCESS:
		tempindex = catalog.getindex(analysis.indexname);
		if (tempindex.indexna == "")
			cout << "ERROR:Index" << analysis.indexname << "doesn't exist" << endl;
		ind.dropindex(tempindex);
		catalog.dropindex(analysis.indexname);
		cout << "MiniSQL has dropped the index successfully" << endl;
		break;
	case CREATEINDEX:
		cout << "ERROR:wrong input after \"create index\"" << endl;
		break;
	case QUIT:
		cout << "You can close the cmd" << endl;
		getchar();
		exit(0);
		break;
	case EMPTY:
		cout << "ERROR:you input nothing" << endl;
		break;
	case UNKNOWN:
		cout << "ERROR:can not support your command" << endl;
		break;
	case SELECT:
		cout << "ERROR:wrong input after \"select\"" << endl;
		break;
	case CREATETABLE:
		cout<< "ERROR:wrong input after \"create table\"" << endl;
		break;
	case DELETEWRONG:
		cout << "ERROR:wrong input after \"delete\"" << endl;
		break;
	case DROPTABLE:
		cout << "ERROR:wrong input after \"drop table\"" << endl;
		break;
	case DROPINDEX:
		cout << "ERROR:wrong input after \"drop index\"" << endl;
		break;
	case PRIMARY:
		cout << "ERROR: wrong primary key" << endl;
		break;
	case UNIQUE:
		cout << "ERROR: wrong unique key" << endl;
		break;
	case CHARERROR:
		cout << "ERROR:\"char\" is to large" << endl;
		break;
	case TABLE:
		cout << "ERROR:the table does not exist" << endl;
		break;
	case NOINDEX:
		cout << "ERROR:the index does not exist" << endl;
		break;
	case COND:
		cout << "ERROR:an attritube does not exist" << endl;
		break;
	case NUMBERWRONG:
		cout << "ERROR:insert values wrong" << endl;
		break;
	}
}
void visual(display afterselect, table temptable, vector<attribute> mulobject)
{
	if (mulobject[0].name == "*")
	{
		cout << endl << "+";
		for (int i = 0;i < temptable.amount;i++)
		{
			for (int j = 0;j < temptable.attributes[i].length + 1;j++)
				cout << "-";
			cout << "+";
		}
		cout << endl;
		cout << "|";
		for (int i = 0;i < temptable.amount;i++)
		{
			cout << temptable.attributes[i].name;
			int fillblank = temptable.attributes[i].length - temptable.attributes[i].name.length();
			for (int j = 0;j < fillblank;j++)
				cout << ' ';
			cout << "|";
		}
		cout << endl;
		cout << "+";
		for (int i = 0;i < temptable.amount;i++)
		{
			for (int j = 0;j < temptable.attributes[i].length + 1;j++)
				cout << "-";
			cout << "+";
		}
		cout << endl;
		for (int i = 0;i < afterselect.displays.size();i++)
		{
			cout << "|";
			for (int j = 0;j < temptable.amount;j++)
			{
				int fillblank = temptable.attributes[j].length;
				for (int k = 0;k < afterselect.displays[i].newvalues[j].length();k++)
				{
					if (afterselect.displays[i].newvalues[j].c_str()[k] == '@')
						break;
					else
					{
						cout << afterselect.displays[i].newvalues[j].c_str()[k];
						fillblank--;
					}
				}
				for (int k = 0;k < fillblank;k++)
					cout << " ";
				cout << "|";
			}
			cout << endl;
		}
			cout << "+";
			for (int j = 0;j < temptable.amount;j++)
			{
				for (int k = 0;k < temptable.attributes[j].length + 1;k++)
					cout << "-";
				cout << "+";
			}
			cout << endl;		
	}
	else
	{
		cout << endl << "+";
		for (int i = 0;i < mulobject.size();i++)
		{
			int selected;
			for (selected = 0;selected < temptable.attributes.size();selected++)
				if (temptable.attributes[selected].name == mulobject[i].name)
					break;
			for (int j = 0;j < temptable.attributes[selected].length + 1;j++)
				cout << "-";
			cout << "+";
		}
		cout << endl;
		cout << "|";
		for (int i = 0;i < mulobject.size();i++)
		{
			int selected;
			for (selected = 0;selected < temptable.attributes.size();selected++)
				if (temptable.attributes[selected].name == mulobject[i].name)
					break;
			cout << temptable.attributes[selected].name;
			int fillblank = temptable.attributes[selected].length - temptable.attributes[selected].name.length();
			for (int j = 0;j < fillblank;j++)
				cout << " ";
			cout << "|";
		}
		cout << endl;
		cout << "+";
		for (int i = 0;i < mulobject.size();i++)
		{
			int selected;
			for (selected = 0;selected < temptable.attributes.size();selected++)
				if (temptable.attributes[selected].name == mulobject[i].name)
					break;
			for (int j = 0;j < temptable.attributes[selected].length + 1;j++)
				cout << "-";
			cout << "+";
		}
		cout << endl;
		for (int i = 0;i < afterselect.displays.size();i++)
		{
			cout << "|";
			for (int j = 0;j < mulobject.size();j++)
			{
				int selected;
				for (selected = 0;selected < temptable.attributes.size();selected++)
					if (temptable.attributes[selected].name == mulobject[i].name)
						break;
				int fillblank = temptable.attributes[selected].length;
				for (int k = 0;k < afterselect.displays[i].newvalues[selected].length();k++)
				{
					if (afterselect.displays[i].newvalues[selected].c_str()[k] == '@')
						break;
					else
					{
						cout << afterselect.displays[i].newvalues[selected].c_str()[k];
						fillblank--;
					}
				}
				for (int k = 0;k < fillblank;k++)
					cout << " ";
				cout << "|";
			}
			cout << endl;
		}
		cout << "+";
		for (int i = 0;i < mulobject.size();i++)
		{
			int selected;
			for (selected = 0;selected < temptable.attributes.size();selected++)
				if (temptable.attributes[selected].name == mulobject[i].name)
					break;
			for (int j = 0;j < temptable.attributes[selected].length + 1;j++)
				cout << "-";
			cout << "+";
		}
		cout << endl;
	}
	cout << "MiniSQL has displayed " << afterselect.displays.size() << "records that it found" << endl;
}
