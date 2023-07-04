#define LOG_NDEBUG 0
#define LOG_TAG "AIDL-DEMO-SERVER"

#include <media/stagefright/foundation/AString.h>
#include <ctime>
#include <cstdlib>
#include <log/log.h>
#include "Server.h"

namespace android
{

Server::Server(AString orderNo, AString productName, AString account, AString passwd)
    : mOrderNo(orderNo), mProductName(productName), mAccount(account), mPasswd(passwd)
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
}

Server::~Server()
{
    ALOGV("[%s %s %d]", __FILE__, __FUNCTION__, __LINE__);
}

status_t Server::connect(const String8 &account, const String8 &passwd) const
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    if(mAccount != AString(account) || mPasswd != AString(passwd))
    {
        ALOGV("[%s %d %s] connect failed", __FILE__, __LINE__, __FUNCTION__);
        return BAD_VALUE;
    }
    ALOGV("[%s %d %s] %s connect OK", __FILE__, __LINE__, __FUNCTION__, mProductName.c_str());
    return NO_ERROR;
}


status_t Server::run(const String8 &cmd, String8 &result)
{
    ALOGV("[%s %d %s] cmd : [%s] is processing...wait...", __FILE__, __LINE__, __FUNCTION__, cmd.c_str());
    sleep(3);
    srand(time(0));
    int precision = rand() % 100;
    char ret[5];
    // %%表示转义
    sprintf(ret, "%03d%%", precision);
    result = ret;
    return NO_ERROR;
}
    
} // android