#ifndef _AN_CMD_
#define _AN_CMD_ 

#include "main.h"

#define CMD_RD 0
#define CMD_WR 1

#define ATE0 1
#define ATE1 2


#define CMD_AT        1             
#define CMD_GET_ATBT  2             
#define CMD_GET_ATC   3             
#define CMD_SET_ATC   4             
#define CMD_SET_ATE0  5              
#define CMD_SET_ATE1  6              
#define CMD_GET_ATI   7             
#define CMD_ATZ       8             
#define CMD_GET_ATW   9             
#define CMD_SET_ATW   10             
#define CMD_GET_STATE 11
#define CMD_SET_STATE 12
#define CMD_GET_INFO  13


#define PARAM_CMD         "cmd"
#define PARAM_MOD_CODE    "mc"
#define PARAM_MOD_CODE_1  "mc1"
#define PARAM_MOD_CODE_2  "mc2"
#define PARAM_MASK        "msk"
#define PARAM_MASK_1      "msk1"
#define PARAM_MASK_2      "msk2"
#define PARAM_DEV_NUM     "d_num"
#define PARAM_DEV_STATE   "d_stt"

#include "AN_rebMod.h"

class AN_cmd{

	public:

	AN_cmd(void);
	~AN_cmd(void);


	AN_rebMod *cRebMod;

	int cmd;
	int modCode;
	int modCode1;
	int modCode2;
	int mask;
	int mask1;
	int mask2;	
	float voltage1;
	float voltage2;
	float temp1;
	float temp2;
	
	int devNum;	
	int devNum1;
	int devNum2;
	
	
	
    int AGetJson();

    void AProcessCmd();
    int AT();       				
	int getATBT();           
    int getATC();           
    int setATC();           
    int getATI();           
	int setATE();
	int ATZ();
    int setATW();
    int getInfo();
	int getState();
	int setState();
 
	
};

extern AN_cmd cCmd;





#endif