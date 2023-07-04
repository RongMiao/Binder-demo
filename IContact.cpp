#define LOG_NDEBUG 0
#define LOG_TAG "AIDL-DEMO-PROVIDER"
#include "IContact.h"

#include <binder/Parcel.h>

namespace android {

class BpContact : public BpInterface<IContact>
{
public:
    explicit BpContact(const sp<IBinder> impl)
        :BpInterface<IContact>(impl)
    {
    }

    status_t sendMessage(const String8 &orderNo)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IContact::getInterfaceDescriptor());
        data.writeString8(orderNo);
        remote()->transact(TRANSACT_SENDMESSAGE, data, &reply);
        // 这里的返回值可以不用检查
        return NO_ERROR;
    }
};

IMPLEMENT_META_INTERFACE(Contact, "vendor.demo.IContact");

status_t BnContact::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case TRANSACT_SENDMESSAGE: {
            CHECK_INTERFACE(IContact, data, reply);
            String8 orderNo = data.readString8();
            reply->writeInt32(sendMessage(orderNo));
            return NO_ERROR;
        } break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}


    
} // android

