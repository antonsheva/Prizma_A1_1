#ifndef _AN_CMD_
#define _AN_CMD_ 

#include "main.h"
#include "RmCtrl.h"
#include "AN_json.h"
#include "AN_shiftDataArr.h"


#define CMD_RD 0
#define CMD_WR 1

#define ATE0 1
#define ATE1 2









 




class AN_cmd : public AN_shiftDataArr{

private:
    static AN_cmd* instance;
	AN_cmd(){};
    ~AN_cmd(){instance = nullptr;}
    AN_cmd(const AN_cmd&) = delete;
    AN_cmd& operator=(const AN_cmd&) = delete;

  

    char serialData [2048] = "\0"; 
    bool waitResponse = 0;


    
    int  AT();
    /**
     * @brief Set addresses
     * {"cmd":21, "ad_esp":3, "ad_rm1":10, "ad_rm2":11}
     * @return int
    */
    int  setAddresses(_MSG_PACK *msg);
    /**
     * @brief Set ATC-param for one RebMod device.
     * {"cmd":4, "rm_num":1, "mc":2, "msk": 11223344}
     * rm_num - 1 or 2 else will be select current RebMod module
     * mc - the modulation code
     * msk - mask
     * @return int
    */
    int  setATC(_MSG_PACK *msg);
    int  getATBT();           
    int  getATC();
    int  getJammList();
    int  getAddresses();
    int  searchDevices(_MSG_PACK *msg);    
 
    void getDevParam();

    void setPrefData(_MSG_PACK *msg);
  
    void getJmmrData(_MSG_PACK *msg);
    void setJmmrData(_MSG_PACK *msg);

    
public:
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
    void AProcessCmd(_MSG_PACK *msg);



    void testFunc();
    void loadConfig();
    void updateLocalData(_MSG_PACK *msg);
    void generateTestData();
 
    void addJmmr(_MSG_PACK *msg);
    void addJmmr(JammerState *jmmr);
    int  processingResponseData(_MSG_PACK *msg);

     


    int setPwr();





    int setState(_MSG_PACK *msg);
    int getATI();
    int setATE();
	int ATZ();
    int setATW();
    int getInfo();
	int getState();
    void setPwrJmmr();
};

#endif


