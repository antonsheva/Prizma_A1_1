#ifndef _COMM_FUNCS_
#define _COMM_FUNCS_

#include "main.h"
#include "AN_jammAddr.h"

// #include "AN_cmd.h"
// extern AN_cmd cCmd; //

void AErrorLog(String error);

void ADebugLog(String msg);
 
void getLocalAddresses();

void initLocalAddresses();

void haveNewRebModData(String readData);


void fillDevParams(int dataArrLen, String *data);

void getDevInfo(String data);

#endif

