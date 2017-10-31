# MiniSQL设计报告


**1.引言**

**1.1 项目背景**

本项目为浙江大学2016-2017学年春夏学期数据库系统课程大程作业，目标设计完成一个MiniSQL的小型系统。完成本项目需要具有一定数据库实现和操作的基本知识以及一门较熟练的编程语言，本项目采用C++编程语言进行实现。

**1.2 项目目的**

设计并实现一个精简型单用户SQL引擎(DBMS)MiniSQL，允许用户通过字符界面输入SQL语句实现表的建立/删除；索引的建立/删除以及表记录的插入/删除/查找。

通过对MiniSQL的设计与实现，提高学生的系统编程能力，加深对数据库系统原理的理解。

**1.3 本文档目的**

本文档将从该项目实现的几个模块入手分析本项目实现的关键细节和技术，展示项目实现的效果。

**2.项目需求**

**2.1 运行环境**

Windows平台，Pentimu II或更高，500MB内存及以上。

**2.2 需求概述**

- **数据类型**

只要求支持三种基本数据类型：int，char(n)，float，其中char(n)满足 1 &lt;= n &lt;= 255 。

- **表定义**

一个表最多可以定义32个属性，各属性可以指定是否为unique；支持单属性的主键定义。

- **索引的建立和删除**

对于表的主属性自动建立B+树索引，对于声明为unique的属性可以通过SQL语句由用户指定建立/删除B+树索引（因此，所有的B+树索引都是单属性单值的）。

- **查找记录**

可以通过指定用and连接的多个条件进行查询，支持等值查询和区间查询。

- **插入和删除记录**

支持每次一条记录的插入操作；支持每次一条或多条记录的删除操作。

**2.3 语法说明**

MiniSQL支持标准的SQL语句格式，每一条SQL语句以分号结尾，一条SQL语句可写在一行或多行。为简化编程，要求所有的关键字都为小写。在以下语句的语法说明中，用黑体显示的部分表示语句中的原始字符串，如 **create** 就严格的表示字符串&quot;create&quot;，否则含有特殊的含义，如 表名 并不是表示字符串 &quot;表名&quot;，而是表示表的名称。

- **创建表语句**

该语句的语法如下：

**create**** table **表名** (**

        列名 类型 **,**

        列名 类型 **,**

        列名 类型 **,**

        **primary key**** ( **列名** )**

**);**

其中类型的说明见第二节&quot;功能需求&quot;。

若该语句执行成功，则输出执行成功信息；若失败，必须告诉用户失败的原因。

示例语句：

**create table student (**

**                sno char(8),**

**                sname char(16) unique,**

**                sage int,**

**                sgender char (1),**

**                primary key ( sno )**

**);**

- **删除表语句**

该语句的语法如下：

**drop table** 表名 **;**

若该语句执行成功，则输出执行成功信息；若失败，必须告诉用户失败的原因。

示例语句：

**drop table student;**

- **创建索引语句**

该语句的语法如下：

**create index** 索引名 **on** 表名 ( 列名 ) **;**

若该语句执行成功，则输出执行成功信息；若失败，必须告诉用户失败的原因。

示例语句：

**create index stunameidx on student ( sname );**

- **删除索引语句**

该语句的语法如下：

**drop index** 索引名 **;**

若该语句执行成功，则输出执行成功信息；若失败，必须告诉用户失败的原因。

示例语句：

**drop index stunameidx;**

- **选择语句**

该语句的语法如下：

**select \* from** 表名 **;**

或：

**select \* from** 表名 **where** 条件 **;**

其中&quot;条件&quot;具有以下格式：列 op 值 and 列 op 值 … and 列 op 值。

op是算术比较符：=        &lt;&gt;        &lt;        &gt;        &lt;=        &gt;=

若该语句执行成功且查询结果不为空，则按行输出查询结果，第一行为属性名，其余每一行表示一条记录；若查询结果为空，则输出信息告诉用户查询结果为空；若失败，必须告诉用户失败的原因。

示例语句：

**select \* from student;**

**select \* from student where sno = &#39;88888888&#39;;**

**select \* from student where sage &gt; 20 and sgender = &#39;F&#39;;**

- **插入记录语句**

该语句的语法如下：

**insert into** 表名 **values (** 值1 , 值2 , … , 值n **);**

若该语句执行成功，则输出执行成功信息；若失败，必须告诉用户失败的原因。

示例语句：

**insert into student values (&#39;12345678&#39;,&#39;wy&#39;,22,&#39;M&#39;);**

- **删除记录语句**

该语句的语法如下：

**delete from** 表名 **;**

或：

**delete from** 表名 **where** 条件 **;**

若该语句执行成功，则输出执行成功信息，其中包括删除的记录数；若失败，必须告诉用户失败的原因。

示例语句：

**delete from student;**

**delete from student where sno = &#39;88888888&#39;;**

- **退出MiniSQL系统语句**

该语句的语法如下：

**quit;**

- **执行SQL脚本文件语句**

该语句的语法如下：

**execfile** 文件名 **;**

SQL脚本文件中可以包含任意多条上述8种SQL语句，MiniSQL系统读入该文件，然后按序依次逐条执行脚本中的SQL语句。

**3.系统体系结构**

本项目的结构设计主要参考MiniSQL实验指导书中给定的结构图进行设计，在一定程度上遵照了各个模块应实现的效果和层次结构，各个模块的具体设计思路将会在下一模块中进行介绍。

 ![](data:image/*;base64,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)

图片1 MiniSQL体系结构

**4.模块设计**

**4.1 Interpreter模块**

**4.1.1 功能需求**

Interpreter模块直接与用户交互，主要实现以下功能：

1.        程序流程控制，即&quot;启动并初始化-&gt;&#39;接收命令、处理命令、显示命令结果&#39;循环-&gt;退出&quot;流程。

2.        接收并解释用户输入的命令，生成命令的内部数据结构表示，同时检查命令的语法正确性和语义正确性，对正确的命令调用API层提供的函数执行并显示执行结果，对不正确的命令显示错误信息。

**4.1.2 流程控制**

用户在命令行中输入要执行的SQL语句，每条指令的结束以&quot;；&quot;为标志，主程序将会把要解析的SQL命令传入到Interpreter模块。Interpreter模块将会根据字符串的顺序读入命令、数据、符号等，并同时判断语法的正确性。如过在逐步解析语句过程中发现错误，将会抛出异常提醒，输出到主界面中并终止语句分析。如果语法检查通过后，语句的解析也相应完成，并传入到API模块进行系统内部的处理。

程序将会循环等待读取命令，一条指令完成后，将会等待下一条指令的录入。

图片 2 Interpreter流程控制

**4.1.3 功能列表**

- **创建表语句**

**create**** table **表名** (**

        列名 类型 **,**

        列名 类型 **,**

        列名 类型 **,**

        **primary key**** ( **列名** )**

**);**

语法检查：验证表名、列名、数据类型是否有效，验证列属性如（unique和primary），验证能否转换为内部数据表示，验证是否有重复表名。

- **删除表语句**

**drop table** 表名 **;**

语法检查：验证&quot;表名&quot;是否存在于数据库中。

- **创建索引语句**

**create index** 索引名 **on** 表名 ( 列名 ) **;**

语法检查：验证索引名和表名、列名是否合法，验证是否存在索引名冲突，验证表名、列名是否存在。

- **删除索引语句**

**drop index** 索引名 **;**

语法检查：验证索引名是否存在数据库中。

- **选择语句**

**select \* from** 表名 **;**

语法检查：验证表名是否存在，即是否存在该表。

或：

**select \* from** 表名 **where** 条件 **;**

语法检查：验证表名是否存在，验证条件是否合法，如列名是否合法，比较是否合法。

- **插入记录语句**

**insert into** 表名 **values (** 值1 , 值2 , … , 值n **);**

语法检查：验证表名和列名是否存在和有效。

- **删除记录语句**

**delete from** 表名 **;**

语法检查：验证表名是否存在和有效。

或：

**delete from** 表名 **where** 条件 **;**

语法检查：验证表名、列名、条件是否有效和合法。

- **退出MiniSQL系统语句**

**quit;**

语法检查：输入合法即可。

- **执行SQL脚本文件语句**

**execfile** 文件名 **;**

语法检查：为合法文件格式，以及文件合法存在。

**4.1.4 解决思路**

本模块采取逐个单词依次分析的办法，从给定字符串中取出一个首单词决定下一步分叉，因此总体呈现命令树状分析的模型。

图片 3 命令分析树

**4.1.5 函数功能**

表格 1 conditions类：用于存储条件

| 变量或函数 | 功能 |
| --- | --- |
| enum compareop {lessthan,lessthanequal,largerthan,largerthanequal,isequal,notequal}; | 五种比较 |
| compareop operation; | Where语句中的比较类型 |
| int colnum; | 列名在表中的序号 |
| string value; | 存储比较值 |

表格2 interpreter类

| 变量或函数 | 功能 |
| --- | --- |
| friend void run(); | 从命令提示行读取命令 |
| void explain(char\* cmd); | 解释读取的SQL命令 |
| int findfirst(string &amp; sql, string &amp; first); | 读取对应字符串sql的第一个单词，供explain函数调用 |
| int op; | 指示错误序号 |
| interpreter() | 初始化各变量 |
| ~interpreter() | 无 |
| vector&lt;attribute&gt; mulobject; | 存储表的各个列名及其属性 |
| string tablename; | 存储表名 |
| table tableinf; | Table类，存储表的详细信息 |
| string attributename; | 存储列名 |
| vector&lt;condition&gt; conditions; | 存储多个比较条件的详细信息 |
| string indexname; | 存储索引名 |
| index indexinf; | 存储索引详细信息 |
| value values; | Value类，为表每一行的记录 |
| int primarycol; | Primary key的列号 |
| int uniquecol; | Unique key的列号 |
| vector&lt;condition&gt; another; | 比较条件存储 |
| string filename; | 要打开的文件名 |
| int judgeint(const char \*); | 判断命令字符串中的整数 |
| int findstring(string &amp; sql, string &amp; first); | 特殊情况下，取命令字符串中的单引号之间的语句，如&#39;brook app&#39; |

**4.2 API模块**

**4.2.1 功能需求**

API模块是整个系统的核心，其主要功能为提供执行SQL语句的接口，供Interpreter层调用。该接口以Interpreter层解释生成的命令内部表示为输入，根据Catalog Manager提供的信息确定执行规则，并调用Record Manager、Index Manager和Catalog Manager提供的相应接口进行执行，最后返回执行结果给Interpreter模块。

**4.2.2 执行原理**

API模块首先通过Interpreter层获得输入的SQL语句的内部命令形式，然后通过Catalog Manager层进行语句的验证和对应信息的查询，根据获取的结果调用record manager、index manager进行语句的执行。API模块为Interpreter、catalog manager、record manager、index manager提供了沟通的枢纽，可以说是整个程序的关键核心所在。

**4.3.3 执行函数**

第一步：main函数读取SQL命令语句

                while (!inend)

                {

                        cin.getline(input, 500);

                        ……………………

strcat(cmd, input);

                        cmd[a] = &#39; &#39;;

                        cmd[a + 1] = &#39;\0&#39;;

                }

得到cmd存储SQL命令。

第二步：explain(cmd)

Explain函数调用的是interpreter层的函数，此为API层与interpreter层的交互。

第三步：run（）

Run函数为api层的函数，此函数分析interpreter传回来的内部数据，在此过程中与catalog、record、index进行交互。

交互调用情况：

catalogmanager catalog;

interpreter analysis;

recordmanager record;

buffermanager buffer;

indexmanager ind;

第四部：visual（）

此函数为API层的函数，提供查询结果的显示，打印在命令提示符窗口。

**4.3 catalogmanager 模块**

**4.3.1 功能需求**

Catalog Manager负责管理数据库的所有模式信息，包括：

1.        数据库中所有表的定义信息，包括表的名称、表中字段（列）数、主键、定义在该表上的索引。

2.        表中每个字段的定义信息，包括字段类型、是否唯一等。

3.        数据库中所有索引的定义，包括所属表、索引建立在那个字段上等。

Catalog Manager还必需提供访问及操作上述信息的接口，供Interpreter和API模块使用。

以及

4.        数据表中的记录条数及空记录串的头记录号。

5.        数据库内已建的表的数目。

**4.3.2 信息组织形式**

**4.3.2.1 Table：**

| tablesize |
| --- |

其中

Tablesize:表的数量

针对每张表再进行存储：

| tablename | amount | blocknum |
| --- | --- | --- |

其中

Tablename:存储的表名

Amount：表的列名的数量

Blocknum:表在文件中的block数量

针对列名再进行存储：

| name | type | length | primary | unique |
| --- | --- | --- | --- | --- |

其中

Name：列名

Type：列的数据属性

Length：列的数据长度

Primary：是否为primary key

Unique：是否为unique

**4.3.2.2 index**

| indexsize |
| --- |

其中indexsize为index的数量

再针对每个index进行存储：

| indexna | tablena | col | collength | block |
| --- | --- | --- | --- | --- |

其中

Indexna:为index的名称

Tablena：为对应的table的名字

Col：为对应的列名的序号

Collength：为每个列对应的长度

Block：为存储的block区域

**4.3.3 功能清单**

此清单为其他层调用该层可能会用到的一些功能。

- 创建表

根据Interpreter处理生成的初步内部数据形式，提取表名，验证是否有重名现象。若无则建立表文件，同时将表的各种信息记录在数据字典内，若有主键定义，应对主键生成索引，并将索引信息记录在数据字典内。若有重名表，则创建表命令失败，并打印出错信息。

- 创建索引

根据Interpreter处理生成的初步内部数据形式，提取表名及列名，在数据字典中查找有无该表及属性的定义，且验证是否可对该属性建立索引。若可建，则创建索引文件，同时将索引信息记录在数据字典内。若不可建索引，则根据不同类型打印出错信息供用户参考。

- 选择语句

根据Interpreter处理生成的初步内部数据形式，提取表名及各列名，在数据字典中查找有无该表及各属性的定义，同时须查找条件中属性名是否在表中有定义，且常量值的数据类型与属性的数据类型是否可比。若有条件，须先根据条件及条件中已建索引的属性名来选择一个合适的索引供Record Manager使用。任何的不符都将打印出相应的出错信息供用户参考。

- 插入记录

根据Interpreter处理生成的初步内部数据形式，提取表名及各属性值，在数据字典中查找该表的定义，并验证各属性值是否满足各属性的数据类型定义。同时将各属性值转化为记录形式。任何的不符都将打印出相应的出错信息供用户参考。

- 删除记录

根据Interpreter处理生成的初步内部数据形式，提取表名及WHERE条件，，在数据字典中查找有无该表的定义，同时须查找条件中属性名是否在表中有定义，且常量值的数据类型与属性的数据类型是否可比。若有条件，须先根据条件及条件中已建索引的属性名来选择一个合适的索引供Record Manager 使用。任何的不符都视为语句的语义错误，并打印出相应的错误信息供用户参考。

- 删除索引

根据Interpreter处理生成的初步内部数据形式，提取索引名，在数据字典中查找有无该索引的定义。若有，则删除该索引在数据字典中的信息，并删除索引文件。若无，则打印出相应的出错信息供用户参考。

- 删除表

根据Interpreter处理生成的初步内部数据形式，提取表名，在数据字典中查找有无该表的定义。若有，则先查找该表上的所有索引并删除，然后删除该表在数据字典中的信息，最后删除该表文件。若在数据字典中无该表的定义，则打印出相应的出错信息供用户参考。

**4.4 recordmanager模块**

**4.4.1 功能需求**

Record Manager负责管理记录表中数据的数据文件。主要功能为实现数据文件的创建与删除（由表的定义与删除引起）、记录的插入、删除与查找操作，并对外提供相应的接口。其中记录的查找操作要求能够支持不带条件的查找和带一个条件的查找（包括等值查找、不等值查找和区间查找）。

数据文件由一个或多个数据块组成，块大小应与缓冲区块大小相同。一个块中包含一条至多条记录，为简单起见，只要求支持定长记录的存储，且不要求支持记录的跨块存储。

**4.4.2 函数功能**

表格3 recordmanager

| 函数 | 功能 |
| --- | --- |
| void createtable(table newtable); | 创建对应表格的.t后缀名文件 |
| void droptable(table oldtable); | 删除对应表格 |
| void insert(table&amp; tableinf, value&amp; values); | 插入对应值到表格中 |
| display select(table temptable, vector&lt;condition&gt; conditions); | 有条件选择若无可用的索引，则须对表文件进行遍历，并对每条记录，进行WHERE 条件匹配，若符合，则打印相应属性的值。若有可用的索引，则可调用Index Manater模块的功能，查找符合WHERE 条件的记录，并打印相应属性的值。在最后打印出被选择的记录的条数。 |
| display select(table&amp; temptable); | 无条件选择 |
| int deletev(table tableinf, vector&lt;condition&gt;conditions); | 有条件删除 |

**4.5 indexmanager模块**

**4.5.1 功能需求**

Index Manager负责B+树索引的实现，实现B+树的创建和删除（由索引的定义与删除引起）、等值查找、插入键值、删除键值等操作，并对外提供相应的接口。

B+树中节点大小应与缓冲区的块大小相同，B+树的叉数由节点大小与索引键大小计算得到。

**4.5.2 解决方案**

块的设计：把4kb的block作为B＋树的节点，在每个block的开始用一个不同字符代替叶子块和中间块，同时紧跟block中的value的数目以及value值，同时对索引文件中的空块链成单链表，由索引文件信息结构体的freenum保存第一个空块的块号，其余每块头三个字节保存下一个空块的块号，最后一个保存字符&#39;000&#39;。

**4.5.3 功能清单**

- 查找：

查找一个值等于所要查找的的记录在table表中的记录号

思路：按照b＋树的原理，遍历中间和叶子节点，直到找到其值，返回。

- 有条件查找

思路：分两类考虑，按照b＋树的原理，遍历中间和叶子节点，直到找到起始值或终止值所在的块时，返回。

- 插入

1.思路：先调用search\_one函数，找到要插入的叶子块块号，并判断是否值已经存在，若不存在，当叶子未满时，则找到位子插入，否则，调用insert\_divide函数分割，再插入。

2.思路：当节点满时，由insert\_one函数调用insert\_divide函数，先找出所有要分割的节点的父节点，自上而下进行分割，然后由insert\_divide函数再递归调用insert\_one函数进行插入（这时肯定能插入）。

- 获取空节点

思路：判断块数是否&gt;256，若没有，判断空块链表是否为空，为空则得到（现有总块号数

＋1）的空块，否则，读取空块链表首，修改链表；并使总块数＋1。

**4.6 buffermanager模块**

**4.6.1 功能需求**

Buffer Manager负责缓冲区的管理，主要功能有：

1.        根据需要，读取指定的数据到系统缓冲区或将缓冲区中的数据写出到文件

2.        实现缓冲区的替换算法，当缓冲区满时选择合适的页进行替换

3.        记录缓冲区中各页的状态，如是否被修改过等

4.        提供缓冲区页的pin功能，及锁定缓冲区的页，不允许替换出去

为提高磁盘I/O操作的效率，缓冲区与文件系统交互的单位是块，块的大小应为文件系统与磁盘交互单位的整数倍，一般可定为4KB或8KB。

**4.6.2 总体思路**

记录管理模块（Record Manager）和索引管理模块（Index Manager）向缓冲区管理申请所要的数据，缓冲区管理器首先在缓冲区中查看数据是否存在，若存在，直接返回，否则，从磁盘中将数据读入缓冲区，然后返回。

最近最少使用(LRU)算法：用一个链表记录所有的缓冲块，每次访问到一个缓冲块就将它插入到链表的头部，这样链表尾的缓冲块就是最近最少使用的块，在需要的时候就可以替换出去。

**4.7 测试**

通过验收，但针对大数据较慢，有待改进。
