#include "AN_shiftDataArr.h"


AN_shiftDataArr::AN_shiftDataArr(/* args */)
{
}

AN_shiftDataArr::~AN_shiftDataArr()
{
}


void AN_shiftDataArr::loadMsgToJmrStt(_MSG_PACK *msg, JammerState *jmmr)
{
	jmmr->devId							= msg->devId;      
	jmmr->groupId						= msg->groupId; 
	jmmr->devType						= msg->devType;  	
	jmmr->rebMod[0].mc      = msg->modCode1;
	jmmr->rebMod[1].mc      = msg->modCode2;
	jmmr->rebMod[0].mask    = msg->mask1;
	jmmr->rebMod[1].mask    = msg->mask2;
	jmmr->rebMod[0].pwr     = msg->pwr1;
	jmmr->rebMod[1].pwr     = msg->pwr2;
	jmmr->info        			= msg->txtData;
	
  if(msg->addrRm1 && msg->addrRm1 < 128)jmmr->rebMod[0].address = msg->addrRm1;
  if(msg->addrRm2 && msg->addrRm2 < 128)jmmr->rebMod[1].address = msg->addrRm2;
}

void AN_shiftDataArr::loadJmmrStateToMsg(_MSG_PACK *msg, JammerState *jmmr){
	msg->devId      = jmmr->devId;        
	msg->groupId    = jmmr->groupId;             
	msg->devType    = jmmr->devType;  	

	msg->addrRm1   	= jmmr->rebMod[0].address;
  msg->modCode1  	= jmmr->rebMod[0].mc;
	msg->mask1     	= jmmr->rebMod[0].mask;
	msg->pwr1      	= jmmr->rebMod[0].pwr;

	msg->addrRm2   	= jmmr->rebMod[1].address;
  msg->modCode2  	= jmmr->rebMod[1].mc;	
	msg->mask2     	= jmmr->rebMod[1].mask;
	msg->pwr2      	= jmmr->rebMod[1].pwr;         
}

void AN_shiftDataArr::copyJmmr(JammerState *jmmr1, JammerState *jmmr2){
	jmmr1->devId   								= jmmr2->devId								;
	jmmr1->groupId 								= jmmr2->groupId							;
	jmmr1->devType 								= jmmr2->devType							;
	jmmr1->esp32Addr 							= jmmr2->esp32Addr						;
	jmmr1->info 					  			= jmmr2->info									;

  jmmr1->rebMod[0].mc          	= jmmr2->rebMod[0].mc         ;    			
  jmmr1->rebMod[0].mask        	= jmmr2->rebMod[0].mask       ;      				
  jmmr1->rebMod[0].address     	= jmmr2->rebMod[0].address    ;         				
  jmmr1->rebMod[0].echo        	= jmmr2->rebMod[0].echo       ;      				
  jmmr1->rebMod[0].pwr         	= jmmr2->rebMod[0].pwr        ;     				
  jmmr1->rebMod[0].vcpu        	= jmmr2->rebMod[0].vcpu       ;      					
  jmmr1->rebMod[0].temp        	= jmmr2->rebMod[0].temp       ;      					
  jmmr1->rebMod[0].info        	= jmmr2->rebMod[0].info       ;      					
  jmmr1->rebMod[0].infoDataQty 	= jmmr2->rebMod[0].infoDataQty; 
	
  jmmr1->rebMod[1].mc          	= jmmr2->rebMod[1].mc         ;    			
  jmmr1->rebMod[1].mask        	= jmmr2->rebMod[1].mask       ;      				
  jmmr1->rebMod[1].address     	= jmmr2->rebMod[1].address    ;         				
  jmmr1->rebMod[1].echo        	= jmmr2->rebMod[1].echo       ;      				
  jmmr1->rebMod[1].pwr         	= jmmr2->rebMod[1].pwr        ;     				
  jmmr1->rebMod[1].vcpu        	= jmmr2->rebMod[1].vcpu       ;      					
  jmmr1->rebMod[1].temp        	= jmmr2->rebMod[1].temp       ;      					
  jmmr1->rebMod[1].info        	= jmmr2->rebMod[1].info       ;      					
  jmmr1->rebMod[1].infoDataQty 	= jmmr2->rebMod[1].infoDataQty; 
}








void AN_shiftDataArr::printJmmrData(JammerState *jmmr){
		Serial.println("----------------");
		Serial.println("devId   ->  "+String(jmmr->devId));
		Serial.println("groupId ->  "+String(jmmr->groupId));
		Serial.println("devType ->  "+String(jmmr->devType));
		Serial.println("ESP addr->  "+String(jmmr->esp32Addr));
		Serial.println("addrRm1 ->  "+String(jmmr->rebMod[0].address));
		Serial.println("addrRm2 ->  "+String(jmmr->rebMod[1].address));
		Serial.println("mc1     ->  "+String(jmmr->rebMod[0].mc));
		Serial.println("mc2     ->  "+String(jmmr->rebMod[1].mc));
		Serial.println("mask1   ->  "+String(jmmr->rebMod[0].mask));
		Serial.println("mask2   ->  "+String(jmmr->rebMod[1].mask));	
		Serial.println("pwr1    ->  "+String(jmmr->rebMod[0].pwr));	
		Serial.println("pwr2    ->  "+String(jmmr->rebMod[1].pwr));	
}

void AN_shiftDataArr::APrintMsg(_MSG_PACK *msg)
{
	Serial.println(" --- Message  --- ");

	Serial.println("devId   -> "+String(msg->devId));
	Serial.println("groupId -> "+String(msg->groupId));
	Serial.println("devType -> "+String(msg->devType));

	Serial.println("ESP addr-> "+String(msg->sender));
	Serial.println("RM1 addr-> "+String(msg->addrRm1));
	Serial.println("mc1     -> "+String(msg->modCode1));
	Serial.println("mask1   -> "+String(msg->mask1));
	
	Serial.println("RM2 addr-> "+String(msg->addrRm2));
	Serial.println("mc2     -> "+String(msg->modCode2));
	Serial.println("mask2   -> "+String(msg->mask2));

	Serial.println(msg->txtData);
}


 void AN_shiftDataArr::printJmmrList(int len)
{
    Serial.println("--------printJmmrList--------");

	if(!len){
		printJmmrData(&G_lJmrStt);
	}else{
		for(int i=0; i<len; i++){
			printJmmrData(&G_jmmrsList[i]);		
		}
	}
}

