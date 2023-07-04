#ifndef __SERVER_H__
#define __SERVER_H__

#include "IServer.h"

namespace android {

class Server : public BnServer
{
public:
    Server(AString orderNo, AString productName, AString account, AString passwd);
    virtual status_t connect(const String8 &account, const String8 &passwd) const override;
    virtual status_t run(const String8 &cmd, String8 &result) override;

protected:
    virtual ~Server();

private:
    AString mOrderNo;
    AString mProductName;
    AString mAccount;
    AString mPasswd;
};
    
} // android

#endif // ! __SERVER_H__