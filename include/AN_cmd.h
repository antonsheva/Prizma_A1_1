#ifndef _AN_CMD_
#define _AN_CMD_ 

#include "main.h"
#include "RebModCntrl.h"

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

#define CMD_GET_ADDRESSES  14
#define CMD_SET_ADDR_ESP32 15
#define CMD_SET_ADDR_RM_1  16
#define CMD_SET_ADDR_RM_2  17

#define CMD_GET_JAMM_LIST  18







#define PARAM_CMD         "cmd"
#define PARAM_MOD_CODE    "mc"
#define PARAM_MOD_CODE_1  "mc1"
#define PARAM_MOD_CODE_2  "mc2"
#define PARAM_MASK        "msk"
#define PARAM_MASK_1      "msk1"
#define PARAM_MASK_2      "msk2"
#define PARAM_DEV_NUM     "d_num"
#define PARAM_DEV_STATE   "d_stt"

#define PARAM_ADDR_ESP32  "ad_esp"
#define PARAM_ADDR_RM_1   "ad_rm1"
#define PARAM_ADDR_RM_2   "ad_rm2"
 




class AN_cmd{

private:
    static AN_cmd* instance;
	AN_cmd(){};
    ~AN_cmd(){instance = nullptr;}
    AN_cmd(const AN_cmd&) = delete;
    AN_cmd& operator=(const AN_cmd&) = delete;


public:

    static AN_cmd* getI(){
        if(instance == nullptr){
            instance = new AN_cmd();
        }
        return instance;
    }

 

  

	int cmd;
	int modCode;
	int modCode1;
	int modCode2;
	int mask;
	int mask1;
	int mask2;
 		
	BYTE addrEsp32;
	BYTE addrRm1;
	BYTE addrRm2;

	float voltage1;
	float voltage2;
	float temp1;
	float temp2;
	
	int devNum;	
	int devNum1;
	int devNum2;
	
  


	void init();
    int AGetJson();

    void AProcessCmd();

    int getJammList();

    int getAddresses();
    int setAddrEsp32();
    int setAddrRm1();
    int setAddrRm2();

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
 





#endif