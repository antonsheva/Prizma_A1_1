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
#define CMD_SET_ADDRESSEE  15
#define CMD_SET_ADDR_RM_1  16
#define CMD_SET_ADDR_RM_2  17

#define CMD_RESPONSE_DATA  18
#define CMD_GET_JAMM_LIST  19
#define CMD_SEARCH_DEVICES 20
#define CMD_SET_ADDRESSES  21

#define CMD_GEN_TEST_DATA  22
#define CMD_LOAD_CONFIG    23





#define PARAM_CMD         "cmd"
#define PARAM_MOD_CODE    "mc"
#define PARAM_MOD_CODE_1  "mc1"
#define PARAM_MOD_CODE_2  "mc2"
#define PARAM_MASK        "msk"
#define PARAM_MASK_1      "msk1"
#define PARAM_MASK_2      "msk2"
#define PARAM_RM_NUM      "rm_num"
#define PARAM_RM_STATE    "rm_stt"

#define PARAM_ADDRESSEE   "addr"
#define PARAM_ADDR_RM_1   "ad_rm1"
#define PARAM_ADDR_RM_2   "ad_rm2"
 




class AN_cmd{

private:
    static AN_cmd* instance;
	AN_cmd(){};
    ~AN_cmd(){instance = nullptr;}
    AN_cmd(const AN_cmd&) = delete;
    AN_cmd& operator=(const AN_cmd&) = delete;

    bool waitResponse = 0;
    
public:
    int waitTimer = 0;
    static AN_cmd* getI(){
        if(instance == nullptr){
            instance = new AN_cmd();
        }
        return instance;
    }

    

  

 
 

	float voltage1;
	float voltage2;
	float temp1;
	float temp2;
	
 
	
  


	void init();
    int AGetJson();

    void AProcessCmd(_MSG_PACK *msg);
    void loadConfig();
    void updateLocalData(int i);
    void generateTestData();
    void printJmmrList();
    void APrintMsg(_MSG_PACK *msg);
    void addJrmr(_MSG_PACK *msg);
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