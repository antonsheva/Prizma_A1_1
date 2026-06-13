#ifndef _DEFINE_
#define _DEFINE_

#define RS485_DIR_DRV 4

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
#define CMD_GET_JMMR_DATA  15
#define CMD_SET_ADDR_RM_1  16
#define CMD_SET_ADDR_RM_2  17
#define CMD_RESPONSE_DATA  18
#define CMD_GET_JMMR_LIST  19
#define CMD_SEARCH_DEVICES 20
#define CMD_SET_ADDRESSES  21
#define CMD_GEN_TEST_DATA  22
#define CMD_SET_JMMR_LIST  23
#define CMD_TEST           24     
#define CMD_SET_JMMR_DATA  25
#define CMD_SET_ADDR_ESP   26
#define CMD_SET_PWR        27



#define RESP_OK             1
#define RESP_ERROR          2
#define RESP_SEARCH_DEVICES 3
#define RESP_GET_JMMR_DATA  4
#define RESP_SET_JMMR_DATA  5
#define RESP_GET_JMMR_LIST  6
#define RESP_SET_JMMR_LIST  7




#define PARAM_CMD               "cmd"
#define PARAM_MOD_CODE          "mc"
#define PARAM_MOD_CODE_1        "mc1"
#define PARAM_MOD_CODE_2        "mc2"
#define PARAM_MASK              "msk"
#define PARAM_MASK_1            "msk1"
#define PARAM_MASK_2            "msk2"
#define PARAM_RM_NUM            "rm_num"
#define PARAM_RM_STATE          "rm_stt"
#define PARAM_PWR_1             "pwr1"
#define PARAM_PWR_2             "pwr2"

#define PARAM_SENDER            "sender"
#define PARAM_ADDR_ESP          "ad_esp"
#define PARAM_ADDR_RM_1         "ad_rm1"
#define PARAM_ADDR_RM_2         "ad_rm2"
#define PARAM_JMMR_LIST         "jmmr_list"
#define PARAM_JMMR_LIST_LEN     "jmmr_list_len"
#define PARAM_RESPONSE          "resp"
#define PARAM_MSG_DIR           "dir"
#define PARAM_TXT               "txt"
#define PARAM_TXT_LEN           "txt_len"


#define PWR_ON  1
#define PWR_OFF 2


#define MSG_DIR_REQUEST  0
#define MSG_DIR_RESPONSE 1

#define MAX_SERIAL_DATA_LEN 8192
#define MAX_DEVICE_QTY      5

#define SERIAL_SRC_USB       0
#define SERIAL_SRC_RM        1
#define SERIAL_SRC_485       2
#define SERIAL_SRC_BT        3

#endif