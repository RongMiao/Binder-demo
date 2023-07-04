

#include "ICloudServiceProvider.h"
#include <binder/Parcel.h>
#include "IContact.h"
#include "IServer.h"

namespace android {

class BpCloudServiceProvider : public BpInterface<ICloudServiceProvider>
{
public:
    explicit BpCloudServiceProvider(const sp<IBinder>& impl)
        : BpInterface<ICloudServiceProvider>(impl)
    {
    }

    status_t getProducts(KeyedVector<String8, int32_t> &sales)
    {
        Parcel data, reply;
        // 传入暗号（discriptor），Bn端收到消息会检查这个暗号，判断bp端是否发错人
        data.writeInterfaceToken(ICloudServiceProvider::getInterfaceDescriptor());
        // no data need transact
        remote()->transact(TRANSACT_GETPRODUCT, data, &reply);
        // 检查远程调用的返回值
        status_t err = reply.readInt32();
        if(err == OK) {
            // 按照约定好的格式读取远程调用的返回值
            // 读取产品的数量
            int32_t num = reply.readInt32();
            for(int i = 0; i < num; i++) {
                // 依次读取到产品的名称以及对应的价格
                String8 key = reply.readString8();
                int32_t value = reply.readInt32();
                // 组装为keyedvector
                sales.add(key, value);
            }
        }
        return err;
    }

    status_t bargain(const String8 &product, int32_t price, const sp<IContact> &contact)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICloudServiceProvider::getInterfaceDescriptor());
        data.writeString8(product);
        data.writeInt32(price);
        // 获取到BpBinder / BnBinder对象中存储的IBinder
        sp<IBinder> b(IInterface::asBinder(contact));
        // 写入binder对象，如果是BpBinder则传递的是Handle，如果是BBinder则传递这个Binder实体以及其引用计数
        data.writeStrongBinder(b);
        remote()->transact(TRANSACT_BARGAIN, data, &reply);
        return reply.readInt32();
    }

    status_t subscribe(const String8 &orderNo, const String8 &account, const String8 &passwd, sp<IServer> &server)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICloudServiceProvider::getInterfaceDescriptor());
        data.writeString8(orderNo);
        data.writeString8(account);
        data.writeString8(passwd);
        remote()->transact(TRANSACT_SUBSCRIBE, data, &reply);
        status_t err = reply.readInt32();
        if(err == OK) {
            // 读取返回的IBinder对象（BpBinder）
            sp<IBinder> b = reply.readStrongBinder();
            // 转换成为BpServer
            server = interface_cast<IServer>(b);
        }
        return err;
    }
    
    status_t unsubscribe(const String8 &orderNo)
    {
        Parcel data, reply;
        data.writeInterfaceToken(ICloudServiceProvider::getInterfaceDescriptor());
        data.writeString8(orderNo);
        remote()->transact(TRANSACT_UNSUBSCRIBE, data, &reply);
        return reply.readInt32();
    }
};

IMPLEMENT_META_INTERFACE(CloudServiceProvider, "vendor.demo.ICloudServiceProvider");

status_t BnCloudServiceProvider::onTransact(
    uint32_t code, const Parcel& data, Parcel* reply, uint32_t flags)
{
    switch (code) {
        case TRANSACT_GETPRODUCT: {
            // 检查暗号
            CHECK_INTERFACE(ICloudServiceProvider, data, reply);
            KeyedVector<String8, int32_t> sales;
            status_t err = getProducts(sales);
            // 先写入函数调用返回值
            reply->writeInt32(err);
            if(err == OK) {
                // system/core/libutils/include/utils/Errors.h
                // 按照约定好的格式写入返回值
                // 这里需要将KeyedVector进行拆分然后再组装
                reply->writeInt32(sales.size());
                for(int i = 0; i < sales.size(); i++) {
                    reply->writeString8(sales.keyAt(i));
                    reply->writeInt32(sales.valueAt(i));
                }
            }
            return NO_ERROR;
        } break;
        case TRANSACT_BARGAIN: {
            CHECK_INTERFACE(ICloudServiceProvider, data, reply);
            // 读取传入参数
            String8 product = data.readString8();
            int32_t price = data.readInt32();
            // 读取传过来的IBinder，这里应该都是BpBinder
            sp<IBinder> b = data.readStrongBinder();
            // 使用BpBinder创建BpContact，可以使用interface_cast<>，也可以使用IContact::asInterface
            sp<IContact> contact = interface_cast<IContact>(b);
            status_t err = bargain(product, price, contact);
            reply->writeInt32(err);
            return NO_ERROR;
        } break;
        case TRANSACT_SUBSCRIBE: {
            CHECK_INTERFACE(ICloudServiceProvider, data, reply);
            String8 orderNo = data.readString8();
            String8 account = data.readString8();
            String8 passwd = data.readString8();
            sp<IServer> server;
            status_t err = subscribe(orderNo, account, passwd, server);
            reply->writeInt32(err);
            if(err == OK) {
                // 获取创建的BnServer中的BBinder对象
                sp<IBinder> b(IInterface::asBinder(server));
                // 封装并进行传输
                reply->writeStrongBinder(b);
            }
            return NO_ERROR;
        } break;
        case TRANSACT_UNSUBSCRIBE: {
            CHECK_INTERFACE(ICloudServiceProvider, data, reply);
            String8 orderNo = data.readString8();
            reply->writeInt32(unsubscribe(orderNo));
            return NO_ERROR;
        }
        default:
            return BBinder::onTransact(code, data, reply, flags);
    }
};


}   // android