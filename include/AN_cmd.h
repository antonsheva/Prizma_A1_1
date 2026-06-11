#ifndef _AN_CMD_
#define _AN_CMD_ 

#include "main.h"
#include "RmCtrl.h"

#define CMD_RD 0
#define CMD_WR 1

#define ATE0 1
#define ATE1 2









 




class AN_cmd{

private:
    static AN_cmd* instance;
	AN_cmd(){};
    ~AN_cmd(){instance = nullptr;}
    AN_cmd(const AN_cmd&) = delete;
    AN_cmd& operator=(const AN_cmd&) = delete;



    bool waitResponse = 0;
    void sendJammListToBt();

    void sendBtResponse(BYTE cmd, uint32_t resp);

public:
    static AN_cmd* getI(){
        if(instance == nullptr){
            instance = new AN_cmd();
        }
        return instance;
    }

  String dataPackStr = "";
  
    int waitTimer        = 0;
    int lastCmd          = 0;
    int endOfDataPacks   = 0;
    int receiveDataPacks = 0;    

	float voltage1;
	float voltage2;
	float temp1;
	float temp2;
	
 
	
  


	void init();
    int AGetJson(String data, _MSG_PACK *msg);
    
    void AProcessCmd(_MSG_PACK *msg);
    
    void loadConfig();
    void loadMsgToJmrStt(_MSG_PACK *msg, JammerState *jmmr);
    void loadJmmrStateToMsg(_MSG_PACK *msg, JammerState *jmmrStt);
    void updateLocalData(_MSG_PACK *msg);
    void generateTestData();
    void printJmmrList(int src = 0);
    void APrintMsg(_MSG_PACK *msg);
    void sendBtData(JsonDocument doc);
    
    void sendResultToBt();
    void addJmmr(_MSG_PACK *msg);
    int processingResponseData(_MSG_PACK *msg);
    int searchDevices(BYTE addrSeneder);

    int getJammList();

    void resetDataPackProcess(String comment);

    int concatMsgPacks(String str);

    int getAddresses();
    /**
     * @brief Set the Addresses object
     * {"cmd":21, "ad_esp":3, "ad_rm1":10, "ad_rm2":11}
     * @return int
     */
    int setAddresses(_MSG_PACK *msg);
    int setAddrEsp32(BYTE addr, bool needRead = 1);
    int setAddrRm1(BYTE addr, bool needRead = 1);
    int setAddrRm2(BYTE addr, bool needRead = 1);

    int AT();
    int getATBT();           
    /**
     * @brief 
     * 
     * @return int 
     */
    int getATC();


    /**
     * @brief Set ATC-param for one RebMod device.
     * {"cmd":4, "rm_num":1, "mc":2, "msk": 11223344}
     * rm_num - 1 or 2 else will be select current RebMod module
     * mc - the modulation code
     * msk - mask
     * @return int
     */
    int setATC(_MSG_PACK *msg);
    int setState(_MSG_PACK *msg);
    int getATI();
    int setATE();
	int ATZ();
    int setATW();
    int getInfo();
	int getState();
};
 





#endif