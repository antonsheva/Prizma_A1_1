#include "AN_json.h"

AN_json* AN_json::instance = nullptr;

void AN_json::printJmmrList(int jmmrListLen)
{
	if(!jmmrListLen){
		Serial.println("----------------");
		Serial.println("ESP addr-> "+String(G_lJmrStt.esp32Addr));
		Serial.println("addrRm1->  "+String(G_lJmrStt.rebMod[0].address));
		Serial.println("addrRm2->  "+String(G_lJmrStt.rebMod[1].address));
		Serial.println("mc1    ->  "+String(G_lJmrStt.rebMod[0].mc));
		Serial.println("mc2    ->  "+String(G_lJmrStt.rebMod[1].mc));
		Serial.println("mask1  ->  "+String(G_lJmrStt.rebMod[0].mask));
		Serial.println("mask2  ->  "+String(G_lJmrStt.rebMod[1].mask));	
		Serial.println("pwr1   ->  "+String(G_lJmrStt.rebMod[0].pwr));	
		Serial.println("pwr2   ->  "+String(G_lJmrStt.rebMod[1].pwr));
	}else{
		for(int i=0; i < jmmrListLen; i++){
			Serial.println("----------------");
			Serial.println("ESP addr-> "+String(G_jmrsList[i].esp32Addr));
			Serial.println("addrRm1-> "+String(G_jmrsList[i].rebMod[0].address));
			Serial.println("addrRm2-> "+String(G_jmrsList[i].rebMod[1].address));
			Serial.println("mc1    -> "+String(G_jmrsList[i].rebMod[0].mc));
			Serial.println("mc2    -> "+String(G_jmrsList[i].rebMod[1].mc));
			Serial.println("mask1  -> "+String(G_jmrsList[i].rebMod[0].mask));
			Serial.println("mask2  -> "+String(G_jmrsList[i].rebMod[1].mask));	
			Serial.println("pwr1   -> "+String(G_jmrsList[i].rebMod[0].pwr));	
			Serial.println("pwr2   -> "+String(G_jmrsList[i].rebMod[1].pwr));			
		}
	}
}

int AN_json::unpackData(String data, _MSG_PACK *msg)
{
    DeserializationError error = deserializeJson(jsonObj, data);
    Serial.println(data);
   
    if(error)return -1;
    
    msg->cmd         	= jsonObj[PARAM_CMD       		];
	// msg->modCode     	= jsonObj[PARAM_MOD_CODE  		];
    msg->direction      = jsonObj[PARAM_MSG_DIR    		];
	msg->response      	= jsonObj[PARAM_RESPONSE  		];
	msg->jmmrListLen	= jsonObj[PARAM_JMMR_LIST_LEN	];
	msg->sender 		= jsonObj[PARAM_SENDER   		];  
	msg->txtDataLen  	= jsonObj[PARAM_TXT_LEN 		];  
	msg->addrEsp32      = jsonObj[PARAM_ADDR_ESP 		];  
	msg->addrRm1   		= jsonObj[PARAM_ADDR_RM_1 		];  
	msg->addrRm2   		= jsonObj[PARAM_ADDR_RM_2 		];   
    msg->modCode1    	= jsonObj[PARAM_MOD_CODE_1		];
    msg->modCode2    	= jsonObj[PARAM_MOD_CODE_2		];
	msg->mask1       	= jsonObj[PARAM_MASK_1    		];
    msg->mask2       	= jsonObj[PARAM_MASK_2    		];
	msg->pwr1       	= jsonObj[PARAM_PWR_1    		];
	msg->pwr2       	= jsonObj[PARAM_PWR_2    		];

 
	txtData               = jsonObj[PARAM_TXT];
	if(txtData){
		G_msgTxtData    = String(txtData);
		G_msgTxtDataLen = G_msgTxtData.length();
	}
 



	// msg->txtData  = String(txtData);  	
  
	if(msg->jmmrListLen>0){
 
		G_jmrsList.clear();
		for(int i=0; i<msg->jmmrListLen; i++){
			JammerState jmmr; 
			jmmr.esp32Addr         = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDR_ESP  	];
			jmmr.rebMod[0].address = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDR_RM_1	];			
			jmmr.rebMod[0].mc      = jsonObj[PARAM_JMMR_LIST][i][PARAM_MOD_CODE_1	];			
			jmmr.rebMod[0].mask    = jsonObj[PARAM_JMMR_LIST][i][PARAM_MASK_1		];		
			jmmr.rebMod[1].address = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDR_RM_2	];			
			jmmr.rebMod[1].mc      = jsonObj[PARAM_JMMR_LIST][i][PARAM_MOD_CODE_2	];		
			jmmr.rebMod[1].mask    = jsonObj[PARAM_JMMR_LIST][i][PARAM_MASK_2		];		
			jmmr.rebMod[0].pwr     = jsonObj[PARAM_JMMR_LIST][i][PARAM_PWR_1		];		
			jmmr.rebMod[1].pwr     = jsonObj[PARAM_JMMR_LIST][i][PARAM_PWR_2		];		
			
			G_jmrsList.push_back(jmmr);
		}
		// printJmmrList(msg->jmmrListLen);
	}
    return 0;
}
 









