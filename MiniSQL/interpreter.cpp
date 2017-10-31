#include "interpreter.h"

void interpreter::explain(char* cmd)
{
	string first="";
	string sql = cmd;
	attribute object;
	condition cond;
	int findprimary = 0, findunique = 0;
	int flag;
	string newvalue;
	op = UNKNOWN;
	tablename = "";
	indexname = "";
	if (mulobject.size() > 0)
		mulobject.clear();
	if (conditions.size() > 0)
		conditions.clear();
	if (values.newvalues.size() > 0)
		values.newvalues.clear();
	tableinf.tablename = "";
	tableinf.amount = tableinf.blocknum = tableinf.sumlength = 0;
	tableinf.attributes.clear();
	indexinf.block = indexinf.col = -1;
	indexinf.indexna = "";
	flag = findfirst(sql, first);
	if (!flag)
	{
		op = EMPTY;
		return;
	}
	else if (strcmp(first.c_str(), "quit") == 0)
	{
		flag = findfirst(sql,first);
		if (!flag)
			op = QUIT;
		return;
	}
	else if (strcmp(first.c_str(), "select") == 0)
	{
		op = SELECT;
		flag = findfirst(sql, first);
		if (!flag)
			return;
		object.name = first;
		mulobject.push_back(object);
		flag = findfirst(sql, first);
		if (!flag)
			return;
		while (strcmp(first.c_str(), ",") == 0)
		{
			flag = findfirst(sql, first);
			if (!flag)
				return;
			object.name = first;
			mulobject.push_back(object);
			flag = findfirst(sql, first);
			if (!flag)
				return;
		}
		if (strcmp(first.c_str(), "from") != 0)
			return;
		flag = findfirst(sql, first);
		if (!flag)
			return;
		tablename = first;
		if (!catalog.hastable(first))
		{
			op = TABLE;
			return;
		}
		tableinf = catalog.gettable(tablename);
		flag = findfirst(sql, first);
		if (!flag)
		{
			op = NOWHERE;
			return;
		}
		if (strcmp(first.c_str(), "where") != 0)
			return;
		flag = findfirst(sql,first);
		if (!flag)
			op = SELECT;
		cond.colnum = catalog.getcolnum(tableinf, first);
		attributename = first;
		if (cond.colnum == -1)
		{
			op = COND;
			return;
		}
		flag = findfirst(sql, first);
		if (!flag)
			return;
		if (strcmp(first.c_str(), "<") == 0)
			cond.operation = lessthan;
		else if (strcmp(first.c_str(), "<=") == 0)
			cond.operation = lessthanequal;
		else if (strcmp(first.c_str(), ">") == 0)
			cond.operation = largerthan;
		else if (strcmp(first.c_str(), ">=") == 0)
			cond.operation = largerthanequal;
		else if (strcmp(first.c_str(), "=") == 0)
			cond.operation = isequal;
		else if (strcmp(first.c_str(), "<>") == 0)
			cond.operation = notequal;
		else return;
		flag = findfirst(sql, first);
		if (!flag)
			return;
		if (strcmp(first.c_str(), "\'") == 0)
		{
			flag = findstring(sql, first);
			if (!flag)
				return;
			cond.value = first;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "\'") != 0)
				return;
		}
		else
			cond.value = first;
		conditions.push_back(cond);
		flag = findfirst(sql, first);
		if (!flag)
		{
			op = SELECT_WHERE;
			return;
		}
		while (strcmp(first.c_str(), "and") == 0)
		{
			flag = findfirst(sql, first);
			if (!flag)
				return;
			cond.colnum = catalog.getcolnum(tableinf, first);
			if (cond.colnum == -1)
			{
				op = COND;
				return;
			}
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "<") == 0)
				cond.operation = lessthan;
			else if (strcmp(first.c_str(), "<=") == 0)
				cond.operation = lessthanequal;
			else if (strcmp(first.c_str(), ">") == 0)
				cond.operation = largerthan;
			else if (strcmp(first.c_str(), ">=") == 0)
				cond.operation = largerthanequal;
			else if (strcmp(first.c_str(), "=") == 0)
				cond.operation = isequal;
			else if (strcmp(first.c_str(), "<>") == 0)
				cond.operation = notequal;
			else return;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "\'") == 0)
			{
				flag = findstring(sql, first);
				if (!flag)
					return;
				cond.value = first;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "\'") != 0)
					return;
			}
			else
				cond.value = first;
			conditions.push_back(cond);
			flag = findfirst(sql, first);
			if (!flag)
			{
				op = SELECT_WHERE;
				return;
			}
		}
	}
	else if (strcmp(first.c_str(), "create") == 0)
	{
		flag = findfirst(sql, first);
		if (!flag)
			return;
		if (strcmp(first.c_str(), "table") == 0)
		{
			op = CREATETABLE;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			tablename = first;
			if (catalog.hastable(tablename))
			{
				op = HASALREADYTABLE;
				return;
			}
			tableinf.tablename = first;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "(") != 0)
				return;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "unique") == 0 || strcmp(first.c_str(), "primary") == 0)
				return;
			object.name = first;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "int") == 0)
			{
				object.type = INT;
				object.length = INTLENGTH;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "unique") == 0)
				{
					object.unique = 1;
					flag = findfirst(sql, first);
					if (!flag)
						return;
				}
			}
			else if (strcmp(first.c_str(), "float") == 0)
			{
				object.type = FLOAT;
				object.length = FLOATLENGTH;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "unique") == 0)
				{
					object.unique = 1;
					flag = findfirst(sql, first);
					if (!flag)
						return;
				}
			}
			else if (strcmp(first.c_str(), "char") == 0)
			{
				object.type = CHAR;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "(") != 0)
					return;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (!judgeint(first.c_str()))
					return;
				object.length = atoi(first.c_str()) + 1;
				if (object.length > 256 || object.length < 2)
				{
					op = CHARERROR;
					return;
				}
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), ")") != 0)
					return;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "unique") == 0)
				{
					object.unique = 1;
					flag = findfirst(sql, first);
					if (!flag)
						return;
				}
			}
			else return;
			tableinf.attributes.push_back(object);
			object.unique = object.primary = 0;
			while (strcmp(first.c_str(), ",") == 0)
			{
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "primary") == 0)
				{
					flag = findfirst(sql, first);
					if (!flag)
						return;
					if (strcmp(first.c_str(), "key") != 0)
						return;
					flag = findfirst(sql, first);
					if (!flag)
						return;
					if (strcmp(first.c_str(), "(") != 0)
						return;
					flag = findfirst(sql, first);
					if (!flag)
						return;
					for (int i = 0;i < tableinf.attributes.size();i++)
					{
						if (strcmp(tableinf.attributes[i].name.c_str(), first.c_str()) == 0)
						{
							findprimary = 1;
							tableinf.attributes[i].primary = 1;
							tableinf.attributes[i].unique = 1;
						}
					}
					if (!findprimary)
					{
						op = PRIMARY;
						return;
					}
					findprimary = 0;
					flag = findfirst(sql, first);
					if (!flag)
						return;
					if (strcmp(first.c_str(), ")") != 0)
						return;
					flag = findfirst(sql, first);
					if (!flag)
						return;
				}
				else if (strcmp(first.c_str(), "unique") == 0)
				{
					flag = findfirst(sql, first);
					if (!flag)
						return;
					if (strcmp(first.c_str(), "(") != 0)
						return;
					flag = findfirst(sql, first);
					if (!flag)
						return;
					for (int i = 0;i < tableinf.attributes.size();i++)
					{
						if (strcmp(tableinf.attributes[i].name.c_str(), first.c_str()) == 0)
						{
							findunique = 1;
							tableinf.attributes[i].unique = 1;
						}
					}
					if (!findunique)
					{
						op = UNIQUE;
						return;
					}
					findunique = 0;
					flag = findfirst(sql, first);
					if (!flag)
						return;
					if (strcmp(first.c_str(), ")") != 0)
						return;
					flag = findfirst(sql, first);
					if (!flag)
						return;
				}
				else
				{
					object.name = first;
					flag = findfirst(sql, first);
					if (!flag)
						return;
					if (strcmp(first.c_str(), "int") == 0)
					{
						object.type = INT;
						object.length = INTLENGTH;
						flag = findfirst(sql, first);
						if (!flag)
							return;
						if (strcmp(first.c_str(), "unique") == 0)
						{
							object.unique = 1;
							flag = findfirst(sql, first);
							if (!flag)
								return;
						}
					}
					else if (strcmp(first.c_str(), "float") == 0)
					{
						object.type = FLOAT;
						object.length = FLOATLENGTH;
						flag = findfirst(sql, first);
						if (!flag)
							return;
						if (strcmp(first.c_str(), "unique") == 0)
						{
							object.unique = 1;
							flag = findfirst(sql, first);
							if (!flag)
								return;
						}
					}
					else if (strcmp(first.c_str(), "char") == 0)
					{
						object.type = CHAR;
						flag = findfirst(sql, first);
						if (!flag)
							return;
						if (strcmp(first.c_str(), "(") != 0)
							return;
						flag = findfirst(sql, first);
						if (!flag)
							return;
						if (!judgeint(first.c_str()))
							return;
						object.length = atoi(first.c_str()) + 1;
						if (object.length > 256 || object.length < 2)
						{
							op = CHARERROR;
							return;
						}
						flag = findfirst(sql, first);
						if (!flag)
							return;
						if (strcmp(first.c_str(), ")") != 0)
							return;
						flag = findfirst(sql, first);
						if (!flag)
							return;
						if (strcmp(first.c_str(), "unique") == 0)
						{
							object.unique = 1;
							flag = findfirst(sql, first);
							if (!flag)
								return;
						}
					}
					else return;
					tableinf.attributes.push_back(object);
					object.unique = object.primary = 0;
				}
			}
			if (strcmp(first.c_str(), ")") != 0)
				return;
			flag = findfirst(sql, first);
			if (!flag)
				op = CREATET;
		}
		else if (strcmp(first.c_str(), "index") == 0)
		{
			op = CREATEINDEX;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			indexname = first;
			indexinf.indexna = first;
			if (catalog.hasindex(indexname))
			{
				op = ALREADYINDEX;
				return;
			}
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "on") != 0)
				return;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			tablename = first;
			if (!catalog.hastable(first))
			{
				op = TABLE;
				return;
			}
			tableinf = catalog.gettable(tablename);
			indexinf.tablena = first;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "(") != 0)
				return;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			int x;
			x = catalog.getcolnum(tableinf, first);
			if (x == -1)
			{
				op = COND;
				return;
			}
			if (catalog.hasindex(tablename, x))
			{
				op = ALREADYINDEX;
				return;
			}
			indexinf.col = x;
			indexinf.block = 0;
			indexinf.collength = tableinf.attributes[x].length;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), ")") != 0)
				return;
			flag = findfirst(sql, first);
			if (!flag)
				op = CREATEIND;
		}
	}
	else if (strcmp(first.c_str(), "delete") == 0)
	{
		flag = findfirst(sql, first);
		if (!flag)
			return;
		if (strcmp(first.c_str(), "from") == 0)
		{
			op = DELETEWRONG;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			tablename = first;
			if (!catalog.hastable(first))
			{
				op = TABLE;
				return;
			}
			flag = findfirst(sql, first);
			tableinf = catalog.gettable(tablename);
			if (!flag)
			{
				op = DELETESUCCESS;
				return;
			}
			if (strcmp(first.c_str(), "where") != 0)
				return;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			int y;
			y = catalog.getcolnum(tableinf, first);
			if (y == -1)
			{
				op = COND;
				return;
			}
			cond.colnum = y;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "<") == 0)
				cond.operation = lessthan;
			else if (strcmp(first.c_str(), "<=") == 0)
				cond.operation = lessthanequal;
			else if (strcmp(first.c_str(), ">") == 0)
				cond.operation = largerthan;
			else if (strcmp(first.c_str(), ">=") == 0)
				cond.operation = largerthanequal;
			else if (strcmp(first.c_str(), "=") == 0)
				cond.operation = isequal;
			else if (strcmp(first.c_str(), "<>") == 0)
				cond.operation = notequal;
			else return;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "\'") == 0)
			{
				flag = findstring(sql, first);
				if (!flag)
					return;
				cond.value = first;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "\'") != 0)
					return;
			}
			else
				cond.value = first;
			conditions.push_back(cond);
			flag = findfirst(sql, first);
			if (!flag)
			{
				op = DELETESUCCESS;
				return;
			}
			while (strcmp(first.c_str(), "and") == 0)
			{
				flag = findfirst(sql, first);
				if (!flag)
					return;
				int z;
				z = catalog.getcolnum(tableinf, first);
				if (z == -1)
				{
					op = COND;
					return;
				}
				cond.colnum = z;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "<") == 0)
					cond.operation = lessthan;
				else if (strcmp(first.c_str(), "<=") == 0)
					cond.operation = lessthanequal;
				else if (strcmp(first.c_str(), ">") == 0)
					cond.operation = largerthan;
				else if (strcmp(first.c_str(), ">=") == 0)
					cond.operation = largerthanequal;
				else if (strcmp(first.c_str(), "=") == 0)
					cond.operation = isequal;
				else if (strcmp(first.c_str(), "<>") == 0)
					cond.operation = notequal;
				else return;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "\'") == 0)
				{
					flag = findstring(sql, first);
					if (!flag)
						return;
					cond.value = first;
					flag = findfirst(sql, first);
					if (!flag)
						return;
					if (strcmp(first.c_str(), "\'") != 0)
						return;
				}
				else
					cond.value = first;
				conditions.push_back(cond);
				flag = findfirst(sql, first);
				if (!flag)
				{
					op = DELETESUCCESS;
					return;
				}
			}
		}
	}
	else if (strcmp(first.c_str(), "insert") == 0)
	{
		flag = findfirst(sql, first);
		if (!flag)
			return;
		if (strcmp(first.c_str(), "into") == 0)
		{
			op = INSERTWRONG;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			tablename = first;
			if (!catalog.hastable(first))
			{
				op = TABLE;
				return;
			}
			tableinf = catalog.gettable(tablename);
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "values") != 0)
				return;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (first != "(")
				return;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			if (strcmp(first.c_str(), "\'") == 0)
			{
				flag = findstring(sql, first);
				if (!flag)
					return;
				newvalue = first;
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "\'") != 0)
					return;
			}
			else
			{
				newvalue = first;
			}
			flag = findfirst(sql, first);
			if (!flag)
				return;
			values.newvalues.push_back(newvalue);
			while (strcmp(first.c_str(), ",") == 0)
			{
				flag = findfirst(sql, first);
				if (!flag)
					return;
				if (strcmp(first.c_str(), "\'") == 0)
				{
					flag = findstring(sql, first);
					if (!flag)
						return;
					newvalue = first;
					flag = findfirst(sql, first);
					if (!flag)
						return;
					if (strcmp(first.c_str(), "\'") != 0)
						return;
				}
				else
				{
					newvalue = first;
				}
				values.newvalues.push_back(newvalue);
				flag = findfirst(sql, first);
				if (!flag)
					return;				
			}
			if (first != ")")
				return;
			if (values.newvalues.size() != catalog.countkind(tableinf))
			{
				op = NUMBERWRONG;
				return;
			}
			flag = findfirst(sql, first);
			for (int i = 0;i < tableinf.attributes.size();i++)
			{
				if (tableinf.attributes[i].primary)
				{
					primarycol = i;
					cond.colnum = i;
					cond.operation = isequal;
					cond.value = values.newvalues[i];
					conditions.push_back(cond);
				}
				if (tableinf.attributes[i].primary != 1 && tableinf.attributes[i].unique)
				{
					uniquecol = i;
					cond.colnum = i;
					cond.operation = isequal;
					cond.value = values.newvalues[i];
					another.push_back(cond);
				}
			}
			if (!flag)
			{
				op = INSERTSUCCESS;
				return;
			}
		}
	}
	else if (strcmp(first.c_str(), "drop") == 0)
	{
		flag = findfirst(sql, first);
		if (!flag)
			return;
		if (strcmp(first.c_str(), "table") == 0)
		{
			op = DROPTABLE;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			tablename = first;
			if (!catalog.hastable(tablename))
			{
				op = TABLE;
				return;
			}
			tableinf = catalog.gettable(tablename);
			flag = findfirst(sql, first);
			if (!flag)
				op = DROPTABLESUCCESS;
		}
		else if (strcmp(first.c_str(), "index") == 0)
		{
			op = DROPINDEX;
			flag = findfirst(sql, first);
			if (!flag)
				return;
			indexname = first;
			if (!catalog.hasindex(indexname))
			{
				op = NOINDEX;
				return;
			}
			flag = findfirst(sql, first);
			if (!flag)
				op = DROPINDEXSUCCESS;
		}
	}
	else if (strcmp(first.c_str(), "execfile") == 0)
	{
		flag = findfirst(sql, first);
		if (!flag)
			return;
		filename = first;
		flag = findfirst(sql, first);
		if (!flag)
		op = EXECFILE;
	}
	return;
}
int interpreter::findfirst(string & sql, string & first)
{
	first.clear();
	int a = 0, b = 0;
	char single = ' ';
	for (;a < sql.length();a++)
	{
		if (single == ' ' || single == '\t' || single == 10 || single == 13)
			single = sql[a];
		else break;
	}
	if (a == sql.length() && (single == ' ' || single == '\t' || single == 10 || single == 13))
		return 0;
	switch (single)
	{
	case ',':
	case '(':
	case ')':
	case '*':
	case '=':
	case '\'':
		first += single;
		sql.erase(0, a);
		break;
	case '<':
		first += single;
		single = sql[a++];
		if (single == '=' || single == '>')
		{
			first += single;
			sql.erase(0, a);
		}
		else
			sql.erase(0, a - 1);
		break;
	case '>':
		first += single;
		single = sql[a++];
		if (single == '=')
		{
			first += single;
			sql.erase(0, a);
		}
		else
			sql.erase(0, a - 1);
		break;
	default:
		do {
			first += single;
			if (a < sql.length())
				single = sql[a++];
			else
			{
				sql.erase(0, a);
				first[b++] = '\0';
				return 1;
			}
		} while (single != '*'&&single != ','&&single != '('&&single != ')'&&single != ' '&&single != '\t'&&single != '='&&single != '>'&&single != '<'&&single != '\''&&single != 10 && single != 13);
		sql.erase(0, a - 1);
	}
	return 1;
}
int interpreter::judgeint(const char *first)
{
	if (!isdigit(first[0]) && !(first[0] == '-'))
		return 0;
	for (int i = 1;i < strlen(first);i++)
		if (!isdigit(first[i]))
			return 0;
	return 1;
}
int interpreter::findstring(string& sql, string& first)
{
	int a=0;
	first.clear();
	char b;
	if (first[0] == '\'')
	{
		first = "";
		return 1;
	}
	else for (a = 0;a < sql.length();a++)
	{
		if (sql[a] != '\'')
		{
			b = sql[a];
			first += b;
		}
		else
		{
			sql.erase(0, a);
			return 1;
		}
	}
	return 0;
}