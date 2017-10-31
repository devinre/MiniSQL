#include "indexmanager.h"
void indexmanager::dropindex(index& tempindex)
{
	string filename = tempindex.indexna + ".i";
	if (remove(filename.c_str()) != 0)
		perror("Delete index fail");
	else
		buffer.changevalid(filename);
}
display indexmanager::selectequal(const table& temptable, const index& tempindex, string tempstring, int offset)
{
	display afterselect;
	string filename = tempindex.indexna + ".i";
	int number = buffer.getbuffernumber(filename, offset);
	int judgeleaf = (buffer.bufblock[number].contents[1] == 'l');
	if (judgeleaf)
	{
		leaf templeaf(number, tempindex);
		list<indexleaf>::iterator i = templeaf.nodelist.begin();
		for (i = templeaf.nodelist.begin();i!=templeaf.nodelist.end();i++)
			if ((*i).key == tempstring)
			{
				filename = tempindex.tablena + ".t";
				int recordblock = buffer.getbuffernumber(filename, (*i).fileoffset);
				int pos = (temptable.sumlength + 1)*((*i).blockoffset);
				string result = buffer.bufblock[recordblock].getrecord(pos, pos + temptable.sumlength);
				if (result.c_str()[0] != '@')
				{
					result.erase(result.begin());
					value dealresult = dealwithrecord(temptable, result);
					afterselect.displays.push_back(dealresult);
					return afterselect;
				}
			}
	}
	else
	{
		branch tempbranch(number, tempindex);
		list<indexbranch>::iterator i = tempbranch.nodelist.begin();
		for (i = tempbranch.nodelist.begin();i != tempbranch.nodelist.end();i++)
		{
			if ((*i).key > tempstring)
			{
				i--;
				break;
			}
		}
		if (i == tempbranch.nodelist.end())
			i--;
		afterselect = selectequal(temptable, tempindex, tempstring, (*i).child);
	}
	return afterselect;
}
leaf::leaf(int number, const index& tempindex)
{
	bufnumber = number;
	judgeroot = (buffer.bufblock[bufnumber].contents[0] == 'r');
	int recordnumber = getrecordnumber();
	recordnum = 0;
	father = getpoint(6);
	lastsibling = getpoint(6 + POINTLENGTH);
	nextsibling = getpoint(6 + POINTLENGTH * 2);
	collength = tempindex.collength;
	int startpos = 6 + POINTLENGTH * 3;
	for (int i = 0;i < recordnumber;i++)
	{
		string key = "";
		for (int i = startpos;i < startpos + collength;i++)
		{
			if (buffer.bufblock[bufnumber].contents[i] == '@')
				break;
			else
				key += buffer.bufblock[bufnumber].contents[i];
		}
		startpos += collength;
		int fileoffset = getpoint(startpos);
		startpos += POINTLENGTH;
		int blockoffset = getpoint(startpos);
		startpos += POINTLENGTH;
		indexleaf node(key, fileoffset, blockoffset);
		insert(node);
	}
}
int bplustree::getrecordnumber()
{
	int a = 0;
	for (int i = 2;i < 6;i++)
	{
		if (buffer.bufblock[bufnumber].contents[i] == '@')
			break;
		a = 10 * a + buffer.bufblock[bufnumber].contents[i] - '0';
	}
	cout << "Recordnum: " << a << endl;
	return a;
}
int bplustree::getpoint(int position)
{
	int a = 0;
	for (int i = position;i < position + POINTLENGTH;i++)
		a = 10 * a + buffer.bufblock[bufnumber].contents[i] - '0';
	return a;
}
void leaf::insert(indexleaf node)
{
	recordnum++;
	list<indexleaf>::iterator i = nodelist.begin();
	if (nodelist.size() == 0)
	{
		nodelist.insert(i, node);
		return;
	}
	else
	{
		for (i = nodelist.begin();i != nodelist.end();i++)
			if ((*i).key > node.key)
				break;
	}
	nodelist.insert(i, node);
}
value indexmanager::dealwithrecord(table temptable, string record)
{
	value temp;
	int start = 0, end = 0;
	for (int i = 0;i < temptable.amount;i++)
	{
		start = end;
		end += temptable.attributes[i].length;
		string te_mp;
		for (int j = start;j < end;j++)
		{
			if (record[j] == '@')
				break;
			te_mp += record[j];
		}
		temp.newvalues.push_back(te_mp);
	}
	return temp;
}
branch::branch(int number, const index& tempindex)
{
	bufnumber = number;
	judgeroot = (buffer.bufblock[bufnumber].contents[0] == 'r');
	int recordnumber = getrecordnumber();
	recordnum = 0;
	father = getpoint(6);
	collength = tempindex.collength;
	int startpos = 6 + POINTLENGTH;
	for (int i = 0;i < recordnumber;i++)
	{
		string key = "";
		for (int i = startpos;i < startpos + collength;i++)
		{
			if (buffer.bufblock[bufnumber].contents[i] == '@')
				break;
			else
				key += buffer.bufblock[bufnumber].contents[i];
		}
		startpos += collength;
		int child = getpoint(startpos);
		startpos += POINTLENGTH;
		indexbranch node(key, child);
		insert(node);
	}
}
void branch::insert(indexbranch node)
{
	recordnum++;
	list<indexbranch>::iterator i = nodelist.begin();
	if (nodelist.size() == 0)
		nodelist.insert(i, node);
	else
	{
		for (i = nodelist.begin();i != nodelist.end();i++)
			if ((*i).key > node.key)
				break;
		nodelist.insert(i, node);
	}	
}
void indexmanager::createindex(const table& temptable, index& tempindex)
{
	string filename = tempindex.indexna + ".i";
	fstream fout(filename.c_str(), ios::out);
	fout.close();
	int blocknum = buffer.findblankbuffer();
	buffer.bufblock[blocknum].filename = filename;
	buffer.bufblock[blocknum].offset = 0;
	buffer.bufblock[blocknum].write = 1;
	buffer.bufblock[blocknum].valid = 1;
	buffer.bufblock[blocknum].contents[0] = 'r';
	buffer.bufblock[blocknum].contents[1] = 'l';
	memset(buffer.bufblock[blocknum].contents + 2, '0', 4);
	for (int i = 0;i < 3;i++)
	{
		memset(buffer.bufblock[blocknum].contents + 6 + POINTLENGTH*i, '0', POINTLENGTH);
		tempindex.block++;
	}
	filename = temptable.tablename + ".t";
	string result;
	string key;
	int length = temptable.sumlength + 1;
	const int recordamount = BLOCKSIZE / length;
	for (int i = 0;i < temptable.blocknum;i++)
	{
		int buffernum = buffer.judgeinbuffer(filename, i);
		if (buffernum == -1)
		{
			buffernum = buffer.findblankbuffer();
			buffer.importblock(filename, i, buffernum);
		}
		for (int j = 0;j < recordamount;j++)
		{
			int pos = j*length;
			result = buffer.bufblock[buffernum].getrecord(pos, pos + length);
			if (result.c_str()[0] == '@')
				continue;
			result.erase(result.begin());
			key = getcolkey(temptable, tempindex, result);
			indexleaf node(key, i, j);
			insertvalue(tempindex, node);
		}
	}
}
string indexmanager::getcolkey(const table& temptable, const index& tempindex, string result)
{
	string find;
	int start = 0, end = 0;
	for (int i = 0;i <= tempindex.col;i++)
	{
		start = end;
		end += temptable.attributes[i].length;
	}
	for (int j = start;j < end&&result[j] != '@';j++)
		find += result[j];
	return find;
}
indexbranch indexmanager::insertvalue(index& tempindex, indexleaf node, int blockoffset)
{
	indexbranch returnbranch;
	string filename = tempindex.indexna + ".i";
	int buffernum = buffer.getbuffernumber(filename, blockoffset);
	int judgeleaf = (buffer.bufblock[buffernum].contents[1] = 'l');
	if (judgeleaf)
	{
		leaf templeaf(buffernum, tempindex);
		templeaf.insert(node);
		const int recordlength = tempindex.collength + POINTLENGTH * 2;
		const int maxrecordnum = (BLOCKSIZE - 6 - POINTLENGTH * 3) / recordlength;
		if (templeaf.recordnum > maxrecordnum)
		{
			if (templeaf.judgeroot)
			{
				int rbuffernum = templeaf.bufnumber;
				templeaf.bufnumber = buffer.addblock(tempindex);
				int sbuffernum = buffer.addblock(tempindex);
				branch branchroot(rbuffernum);
				leaf leafadd(sbuffernum);
				branchroot.judgeroot = 1;
				leafadd.judgeroot = 0;
				templeaf.judgeroot = 0;
				branchroot.father = leafadd.father = templeaf.father = 0;
				branchroot.collength = leafadd.collength = leafadd.collength;
				leafadd.lastsibling = buffer.bufblock[templeaf.bufnumber].offset;
				templeaf.nextsibling = buffer.bufblock[leafadd.bufnumber].offset;
				while (leafadd.nodelist.size() < templeaf.nodelist.size())
				{
					indexleaf tnode = templeaf.pop();
					leafadd.insert(tnode);
				}
				indexbranch tempnode;
				tempnode.key = leafadd.getfront().key;
				tempnode.child = buffer.bufblock[leafadd.bufnumber].offset;
				branchroot.insert(tempnode);
				tempnode.key = templeaf.getfront().key;
				tempnode.child = buffer.bufblock[templeaf.bufnumber].offset;
				branchroot.insert(tempnode);
				return returnbranch;
			}
			else
			{
				int buffernum = buffer.addblock(tempindex);
				leaf leafadd(buffernum);
				leafadd.judgeroot = 0;
				leafadd.father = templeaf.father;
				leafadd.collength = templeaf.collength;
				leafadd.nextsibling = templeaf.nextsibling;
				leafadd.lastsibling = buffer.bufblock[templeaf.bufnumber].offset;
				templeaf.nextsibling = buffer.bufblock[leafadd.bufnumber].offset;
				if (leafadd.nextsibling != 0)
				{
					int buffernum = buffer.getbuffernumber(filename, leafadd.nextsibling);
					leaf leafnext(buffernum, tempindex);
					leafnext.lastsibling = buffer.bufblock[leafadd.bufnumber].offset;
				}
				while (leafadd.nodelist.size() < templeaf.nodelist.size())
				{
					indexleaf tnode = templeaf.pop();
					leafadd.insert(tnode);
				}
				returnbranch.key = leafadd.getfront().key;
				returnbranch.child = templeaf.nextsibling;
				return returnbranch;
			}
		}
		else
			return returnbranch;
	}
	else
	{
		branch tempbranch(buffernum, tempindex);
		list<indexbranch>::iterator i = tempbranch.nodelist.begin();
		if ((*i).key > node.key)
			(*i).key = node.key;
		else
		{
			for (i = tempbranch.nodelist.begin();i != tempbranch.nodelist.end();i++)
				if ((*i).key > node.key)
					break;
			i--;
		}
		indexbranch bnode = insertvalue(tempindex, node, (*i).child);
		if (bnode.key == "")
			return returnbranch;
		else
		{
			tempbranch.insert(bnode);
			const int recordlength = tempindex.collength + POINTLENGTH;
			const int maxrecordnum = (BLOCKSIZE - 6 - POINTLENGTH) / recordlength;
			if (tempbranch.recordnum > maxrecordnum)
			{
				if (tempbranch.judgeroot)
				{
					int rbuffernum = tempbranch.bufnumber;
					tempbranch.bufnumber = buffer.addblock(tempindex);
					int sbuffernum = buffer.addblock(tempindex);
					branch branchroot(rbuffernum);
					branch branchadd(sbuffernum);
					branchroot.judgeroot = 1;
					branchadd.judgeroot = 0;
					tempbranch.judgeroot = 0;
					branchroot.father = branchadd.father = tempbranch.father = 0;
					branchroot.collength = branchadd.collength = tempbranch.collength;
					while (branchadd.nodelist.size() < tempbranch.nodelist.size())
					{
						indexbranch tnode = tempbranch.pop();
						branchadd.insert(tnode);
					}
					indexbranch tempnode;
					tempnode.key = branchadd.getfront().key;
					tempnode.child = buffer.bufblock[branchadd.bufnumber].offset;
					branchroot.insert(tempnode);
					tempnode.key = tempbranch.getfront().key;
					tempnode.child = buffer.bufblock[tempbranch.bufnumber].offset;
					branchroot.insert(tempnode);
					return returnbranch;
				}
				else
				{
					int buffernum = buffer.addblock(tempindex);
					branch branchadd(buffernum);
					branchadd.judgeroot = 0;
					branchadd.father = tempbranch.father;
					branchadd.collength = tempbranch.collength;
					while (branchadd.nodelist.size() < tempbranch.nodelist.size())
					{
						indexbranch tnode = tempbranch.pop();
						branchadd.insert(tnode);
					}
					returnbranch.key = branchadd.getfront().key;
					returnbranch.child = buffer.bufblock[buffernum].offset;
					return returnbranch;
				}
			}
			else
				return returnbranch;
		}
	}
	return returnbranch;
}
leaf::leaf(int number)
{
bufnumber = number;
recordnum = 0;
nextsibling = lastsibling = 0;
}
indexleaf leaf::pop()
{
	recordnum--;
	indexleaf temp = nodelist.back();
	nodelist.pop_back();
	return temp;
}
indexleaf leaf::getfront()
{
	return nodelist.front();
}
indexbranch branch::pop()
{
	recordnum--;
	indexbranch temp = nodelist.back();
	nodelist.pop_back();
	return temp;
}
indexbranch branch::getfront()
{
	return nodelist.front();
}
branch::~branch()
{
	if (judgeroot)
		buffer.bufblock[bufnumber].contents[0] = 'r';
	else
		buffer.bufblock[bufnumber].contents[0] = '_';
	buffer.bufblock[bufnumber].contents[1] = ' ';
	char temp[5];
	sprintf(temp, "%d", recordnum);
	string strrecordnum = temp;
	while (strrecordnum.length() < 4)
		strrecordnum = '0' + strrecordnum;
	strncpy(buffer.bufblock[bufnumber].contents + 2, strrecordnum.c_str(), 4);
	if (nodelist.size() == 0)
	{
		cout << "ERROR" << endl;
		exit(0);
	}
	list<indexbranch>::iterator i;
	int pos = 6 + POINTLENGTH;
	for (i = nodelist.begin();i != nodelist.end();i++)
	{
		string key = (*i).key;
		while (key.length() < collength)
			key += '@';
		strncpy(buffer.bufblock[bufnumber].contents + pos, key.c_str(), collength);
		pos += collength;
		char temp[5];
		sprintf(temp, "%d", (*i).child);
		string ptrchild = temp;
		while (ptrchild.length() < POINTLENGTH)
			ptrchild = '0' + ptrchild;
		strncpy(buffer.bufblock[bufnumber].contents + pos, ptrchild.c_str(), POINTLENGTH);
		pos += POINTLENGTH;
	}
}
leaf::~leaf()
{
	if (judgeroot)
		buffer.bufblock[bufnumber].contents[0] = 'r';
	else
		buffer.bufblock[bufnumber].contents[0] = '_';
	buffer.bufblock[bufnumber].contents[1] = 'l';
	char temp[5];
	sprintf(temp, "%d", recordnum);
	string strrecordnum = temp;
	while (strrecordnum.length() < 4)
		strrecordnum = '0' + strrecordnum;
	int pos = 2;
	strncpy(buffer.bufblock[bufnumber].contents + pos, strrecordnum.c_str(), 4);
	pos += 4;
	sprintf(temp, "%d", father);
	string strptrfather = temp;
	while (strptrfather.length() < POINTLENGTH)
		strptrfather = '0' + strptrfather;
	strncpy(buffer.bufblock[bufnumber].contents + pos, strptrfather.c_str(), POINTLENGTH);
	pos += POINTLENGTH;
	sprintf(temp, "%d", lastsibling);
	string strlastsibling = temp;
	while (strlastsibling.length() < POINTLENGTH)
		strlastsibling = '0' + strlastsibling;
	strncpy(buffer.bufblock[bufnumber].contents + pos, strlastsibling.c_str(), POINTLENGTH);
	pos += POINTLENGTH;
	sprintf(temp, "%d", nextsibling);
	string strnexsibling = temp;
	while (strnexsibling.length() < POINTLENGTH)
		strnexsibling = '0' + strnexsibling;
	strncpy(buffer.bufblock->contents + pos, strnexsibling.c_str(), POINTLENGTH);
	pos += POINTLENGTH;
	if (nodelist.size() == 0)
	{
		cout << "ERROR" << endl;
		exit(0);
	}
	list<indexleaf>::iterator i;
	for (i = nodelist.begin();i != nodelist.end();i++)
	{
		string key = (*i).key;
		while (key.length() < collength)
			key += '@';
		strncpy(buffer.bufblock[bufnumber].contents + pos, key.c_str(), collength);
		pos += collength;
		sprintf(temp, "%d", (*i).blockoffset);
		string offsetinfile = temp;
		while (offsetinfile.length() < POINTLENGTH)
			offsetinfile = '0' + offsetinfile;
		strncpy(buffer.bufblock[bufnumber].contents + pos, offsetinfile.c_str(), POINTLENGTH);
		pos += POINTLENGTH;
		sprintf(temp, "%d", (*i).blockoffset);
		string offsetinblock = temp;
		while (offsetinblock.length() < POINTLENGTH)
			offsetinblock = '0' + offsetinblock;
		strncpy(buffer.bufblock[bufnumber].contents + pos, offsetinblock.c_str(), POINTLENGTH);
		pos += POINTLENGTH;
	}
}