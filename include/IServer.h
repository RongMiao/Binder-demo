#ifndef __ISERVER_H__
#define __ISERVER_H__

#include <binder/IInterface.h>

namespace android {

class IServer : public IInterface {
public:
    DECLARE_META_INTERFACE(Server);

    virtual status_t connect(const String8 &account, const String8 &passwd) const = 0;
    virtual status_t run(const String8 &cmd, String8 &result) = 0;

protected:
    enum {
        TRANSACT_CONNECT = IBinder::FIRST_CALL_TRANSACTION,
        TRANSACT_RUN,
    };
}; // IServer


class BnServer : public BnInterface<IServer>
{
public:
    virtual status_t onTransact(uint32_t code, 
                                const Parcel& data,
                                Parcel* reply,
                                uint32_t flags = 0);
};

} // android

#endif // !__ISERVER_H__




