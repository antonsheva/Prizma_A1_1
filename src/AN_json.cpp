#include "AN_json.h"

AN_json* AN_json::instance = nullptr;



String AN_json::packResponse(BYTE cmd, BYTE resp){
	char dataBuff[64];
	JsonDocument doc;		
	doc[PARAM_CMD]  	= cmd;
	doc[PARAM_RESPONSE] = resp;
    int len = serializeJson(doc, dataBuff);	
	return String(dataBuff);
}

String AN_json::packJmmrData(_MSG_PACK *msg){
	char dataBuff[1024];
	JsonDocument doc;	
    	
	doc[PARAM_CMD]    = CMD_GET_JMMR_LIST;
  doc[PARAM_SENDER] = G_lJmrStt.esp32Addr;
 
	doc["jmmr_data"][PARAM_DEV_ID    ] = msg->devId;
  doc["jmmr_data"][PARAM_GROUP_ID  ] = msg->groupId;
  doc["jmmr_data"][PARAM_DEV_TYPE  ] = msg->devType;
	doc["jmmr_data"][PARAM_DEV_RANGE ] = msg->devRange;
	

  doc["jmmr_data"][PARAM_ADDR_ESP  ] = msg->addrEsp32; 
  doc["jmmr_data"][PARAM_ADDR_RM_1 ] = msg->addrRm1; 
  doc["jmmr_data"][PARAM_ADDR_RM_2 ] = msg->addrRm2; 
  doc["jmmr_data"][PARAM_MOD_CODE_1] = msg->modCode1; 
  doc["jmmr_data"][PARAM_MOD_CODE_2] = msg->modCode2; 
  doc["jmmr_data"][PARAM_MASK_1    ] = msg->mask1; 
  doc["jmmr_data"][PARAM_MASK_2    ] = msg->mask2; 
  doc["jmmr_data"][PARAM_PWR_1     ] = msg->pwr1; 
  doc["jmmr_data"][PARAM_PWR_2     ] = msg->pwr2; 
  doc["jmmr_data"][PARAM_TXT       ] = G_msgTxtData;
  doc["jmmr_data"][PARAM_TXT_LEN   ] = G_msgTxtDataLen;

  int len = serializeJson(doc, dataBuff);	
	return String(dataBuff);
}

 

String AN_json::packRs485Data(_MSG_PACK *msg){
	char dataBuff[1024];
	JsonDocument doc;    

	doc[PARAM_SENDER    ] = G_lJmrStt.esp32Addr;        
	doc[PARAM_CMD       ]   = msg->cmd;
	doc[PARAM_ADDR_ESP  ]   = msg->addrEsp32;  
	doc[PARAM_MSG_DIR   ]   = msg->direction;
	doc[PARAM_RESPONSE  ]   = msg->response;      
	
	if(msg->direction == MSG_DIR_RESPONSE){
		doc[PARAM_DEV_ID    ] = msg->devId;
		doc[PARAM_GROUP_ID  ] = msg->groupId;
		doc[PARAM_DEV_TYPE  ] = msg->devType;
		doc[PARAM_DEV_RANGE ] = msg->devRange;		
	}

  if((msg->cmd == CMD_SET_STATE)||(msg->direction == MSG_DIR_RESPONSE)){
		
		doc[PARAM_MOD_CODE_1]   = msg->modCode1;
		doc[PARAM_MOD_CODE_2]   = msg->modCode2;
		doc[PARAM_MASK_1    ]   = msg->mask1;			
		doc[PARAM_MASK_2    ]   = msg->mask2;			
		doc[PARAM_PWR_1     ]   = msg->pwr1;			
		doc[PARAM_PWR_2     ]   = msg->pwr2;   						
	}
	if(msg->cmd == CMD_SET_STATE){
		doc[PARAM_RESPONSE  ]   = msg->response;
	}
	
	if((msg->response == RESP_GET_JMMR_DATA)&&(msg->direction == MSG_DIR_RESPONSE)){
		doc[PARAM_TXT       ]   = G_lJmrStt.info;
		doc[PARAM_TXT_LEN   ]   = G_lJmrStt.info.length();  						
	}

	if(msg->addrRm1 && msg->addrRm1 < 128)doc[PARAM_ADDR_RM_1 ] = msg->addrRm1;
	if(msg->addrRm2 && msg->addrRm2 < 128)doc[PARAM_ADDR_RM_2 ] = msg->addrRm2;   

	int len = serializeJson(doc, dataBuff);
	return String(dataBuff); 
}

String AN_json::packJmmrList(){
	char dataBuff[4096];
	JsonDocument doc;		
	doc[PARAM_CMD]    = CMD_GET_JMMR_LIST;
  doc[PARAM_SENDER] = G_lJmrStt.esp32Addr;

	for(size_t i=0; i<G_jmmrsList.size(); i++){
		doc["jmmr_list"][i][PARAM_DEV_ID    ] = G_jmmrsList[i].devId;
    doc["jmmr_list"][i][PARAM_GROUP_ID  ] = G_jmmrsList[i].groupId;
    doc["jmmr_list"][i][PARAM_DEV_TYPE  ] = G_jmmrsList[i].devType;
		doc["jmmr_list"][i][PARAM_DEV_RANGE ] = G_jmmrsList[i].devRange;
		 
		doc["jmmr_list"][i][PARAM_ADDR_ESP  ] = G_jmmrsList[i].esp32Addr;
		doc["jmmr_list"][i][PARAM_ADDR_RM_1 ] = G_jmmrsList[i].rebMod[0].address;
		doc["jmmr_list"][i][PARAM_ADDR_RM_2 ] = G_jmmrsList[i].rebMod[1].address;
		doc["jmmr_list"][i][PARAM_MOD_CODE_1] = G_jmmrsList[i].rebMod[0].mc;
		doc["jmmr_list"][i][PARAM_MOD_CODE_2] = G_jmmrsList[i].rebMod[1].mc;
		doc["jmmr_list"][i][PARAM_MASK_1    ] = G_jmmrsList[i].rebMod[0].mask;
		doc["jmmr_list"][i][PARAM_MASK_2    ] = G_jmmrsList[i].rebMod[1].mask;
		doc["jmmr_list"][i][PARAM_PWR_1     ] = G_jmmrsList[i].rebMod[0].pwr;
		doc["jmmr_list"][i][PARAM_PWR_2     ] = G_jmmrsList[i].rebMod[1].pwr;

    doc["jmmr_list"][i][PARAM_TXT       ] = G_jmmrsList[i].info;
    doc["jmmr_list"][i][PARAM_TXT_LEN   ] = G_jmmrsList[i].info.length();
	}
  int len = serializeJson(doc, dataBuff);	
	
	return String(dataBuff);
}

int AN_json::unpackData(String data, _MSG_PACK *msg)
{
    DeserializationError error = deserializeJson(jsonObj, data);
    Serial.println(data);
   
    if(error)return -1;

	msg->devId  		= jsonObj[PARAM_DEV_ID    		];        
    msg->groupId		= jsonObj[PARAM_GROUP_ID  		];     
    msg->devType		= jsonObj[PARAM_DEV_TYPE  		];     
    msg->devRange		= jsonObj[PARAM_DEV_RANGE  		];  	    
    
    msg->cmd         	= jsonObj[PARAM_CMD       		];
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
   
	if(msg->jmmrListLen>0){
 
		G_jmmrsList.clear();
		for(int i=0; i<msg->jmmrListLen; i++){
			JammerState jmmr; 
			jmmr.devId  		   = jsonObj[PARAM_JMMR_LIST][i][PARAM_DEV_ID   	];        
			jmmr.groupId		   = jsonObj[PARAM_JMMR_LIST][i][PARAM_GROUP_ID 	];     
			jmmr.devType		   = jsonObj[PARAM_JMMR_LIST][i][PARAM_DEV_TYPE 	];     
			jmmr.devRange		   = jsonObj[PARAM_JMMR_LIST][i][PARAM_DEV_RANGE	]; 

			jmmr.esp32Addr         = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDR_ESP  	];
			jmmr.rebMod[0].address = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDR_RM_1	];			
			jmmr.rebMod[0].mc      = jsonObj[PARAM_JMMR_LIST][i][PARAM_MOD_CODE_1	];			
			jmmr.rebMod[0].mask    = jsonObj[PARAM_JMMR_LIST][i][PARAM_MASK_1		];		
			jmmr.rebMod[1].address = jsonObj[PARAM_JMMR_LIST][i][PARAM_ADDR_RM_2	];			
			jmmr.rebMod[1].mc      = jsonObj[PARAM_JMMR_LIST][i][PARAM_MOD_CODE_2	];		
			jmmr.rebMod[1].mask    = jsonObj[PARAM_JMMR_LIST][i][PARAM_MASK_2		];		
			jmmr.rebMod[0].pwr     = jsonObj[PARAM_JMMR_LIST][i][PARAM_PWR_1		];		
			jmmr.rebMod[1].pwr     = jsonObj[PARAM_JMMR_LIST][i][PARAM_PWR_2		];		
			
			G_jmmrsList.push_back(jmmr);
		}
 
	}
    return 0;
}
 













