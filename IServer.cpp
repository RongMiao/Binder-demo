

#include "IServer.h"
#include <binder/Parcel.h>


namespace android {

class BpServer : public BpInterface<IServer>
{
public: 
    explicit BpServer(const sp<IBinder> &impl)
        : BpInterface<IServer>(impl)
    {
    }

    status_t connect(const String8 &account, const String8 &passwd) const
    {
        Parcel data, reply;
        data.writeInterfaceToken(IServer::getInterfaceDescriptor());
        data.writeString8(account);
        data.writeString8(passwd);
        remote()->transact(TRANSACT_CONNECT, data, &reply);
        return reply.readInt32();
    }

    status_t run(const String8 &cmd, String8 &result)
    {
        Parcel data, reply;
        data.writeInterfaceToken(IServer::getInterfaceDescriptor());
        data.writeString8(cmd);
        remote()->transact(TRANSACT_RUN, data, &reply);
        status_t err = reply.readInt32();
        result = reply.readString8();
        return err;
    }
};

IMPLEMENT_META_INTERFACE(Server, "vendor.demo.IServer");

status_t BnServer::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case TRANSACT_CONNECT: {
            CHECK_INTERFACE(IServer, data, reply);
            String8 account = data.readString8();
            String8 passwd = data.readString8();
            reply->writeInt32(connect(account, passwd));
            return NO_ERROR;
        } break;
        case TRANSACT_RUN: {
            CHECK_INTERFACE(IServer, data, reply);
            String8 cmd = data.readString8();
            String8 result;
            reply->writeInt32(run(cmd, result));
            reply->writeString8(result);
            return NO_ERROR;
        } break;
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
}


}