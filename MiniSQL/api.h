#ifndef __API_H__
#define __API_H__
#include <iostream>
#include <string>
#include "interpreter.h"
#include "recordmanager.h"
#include "indexmanager.h"

using namespace std;
int inputend(char *input);
void run();
void visual(display afterselect,table temptable,vector<attribute> mulobject);
catalogmanager catalog;
interpreter analysis;
recordmanager record;
buffermanager buffer;
indexmanager ind;
#endif