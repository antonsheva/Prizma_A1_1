#ifndef _AN_SHIFT_
#define _AN_SHIFT_


#include "objects.h"

class AN_shiftDataArr{
private:
    /* data */
public:
    AN_shiftDataArr(/* args */);
    ~AN_shiftDataArr();
    void loadMsgToJmrStt(_MSG_PACK *msg, JammerState *jmmr);
    void loadJmmrStateToMsg(_MSG_PACK *msg, JammerState *jmmr);
    void copyJmmr(JammerState *jmmr1, JammerState *jmmr2);
    void printJmmrData(JammerState *jmmr);
    void APrintMsg(_MSG_PACK *msg);
    void printJmmrList(int len = 0);
};

#endif