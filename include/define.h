#ifndef _DEFINE_
#define _DEFINE_

#define RS485_DIP_DRV 4

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
#define PARAM_PWR         "pwr"

#define PARAM_ADDRESSEE   "ad_esp"
#define PARAM_ADDR_RM_1   "ad_rm1"
#define PARAM_ADDR_RM_2   "ad_rm2"

#endif