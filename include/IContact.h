#ifndef __ICONTACT_H__
#define __ICONTACT_H__

#include <binder/IInterface.h>
#include <utils/KeyedVector.h>

namespace android {

class IContact : public IInterface
{
public:
    DECLARE_META_INTERFACE(Contact);

    virtual status_t sendMessage(const String8 &orderNo) = 0;

protected:
    enum {
        TRANSACT_SENDMESSAGE = IBinder::FIRST_CALL_TRANSACTION,
    };
};

class BnContact : public BnInterface<IContact>
{
public:
    virtual status_t onTransact(uint32_t code, 
                            const Parcel& data,
                            Parcel* reply,
                            uint32_t flags = 0);
};

} // android
#endif // ! __ICONTACT_H__