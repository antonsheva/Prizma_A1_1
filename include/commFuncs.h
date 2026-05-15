#ifndef _COMM_FUNCS_
#define _COMM_FUNCS_

#include "main.h"
#include "AN_jammAddr.h"

// #include "AN_cmd.h"
// extern AN_cmd cCmd; //

void AErrorLog(String error);

void ADebugLog(String msg);

void AReadSerialData(int src);

void getLocalAddresses(AN_jammAddr *addr);

void haveNewRebModData(String readData);

void fillDevInfoList(int paramQt, String *data);

void fillDevParams(int dataArrLen, String *data);

void getDevInfo(String data);

#endif

