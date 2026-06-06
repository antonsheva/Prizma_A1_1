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

public:
    static AN_cmd* getI(){
        if(instance == nullptr){
            instance = new AN_cmd();
        }
        return instance;
    }
    int waitTimer = 0;
    int lastCmd   = 0;

    

  

 
 

	float voltage1;
	float voltage2;
	float temp1;
	float temp2;
	
 
	
  


	void init();
    int AGetJson(char *data, _MSG_PACK *msg);

    void AProcessCmd(_MSG_PACK *msg);
 
    void loadConfig();
    void updateLocalData(int i);
    void generateTestData();
    void printJmmrList();
    void APrintMsg(_MSG_PACK *msg);
    void sendResultToBt();
    void addJmmr(_MSG_PACK *msg);
    int processingResponseData(_MSG_PACK *msg);
    int searchDevices(BYTE addrSeneder);

    int getJammList();

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
    int getATI();           
	int setATE();
	int ATZ();
    int setATW();
    int getInfo();
	int getState();
	int setState();
};
 





#endif