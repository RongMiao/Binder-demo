#ifndef __ICLOUD_SERVER_Provider_H__
#define __ICLOUD_SERVER_Provider_H__

#include <binder/IInterface.h>
#include <utils/KeyedVector.h>

namespace android {

class IServer;
class IContact;

// 继承自IInterface，为我们提供asBinder方法，让我们可以从BnCloudServiceProvider 或者 BpCloudServiceProvider 中拿到封装的IBinder对象
class ICloudServiceProvider : public IInterface
{
public:
    // 调用宏，为我们提供asInterface等静态方法和descriptor等静态成员
    DECLARE_META_INTERFACE(CloudServiceProvider);
    // 获取正在售卖的产品
    virtual status_t getProducts(KeyedVector<String8, int32_t> &sales) = 0;
    // 还价
    virtual status_t bargain(const String8 &product, int32_t price, const sp<IContact> &contact) = 0;
    // 订阅
    virtual status_t subscribe(const String8 &orderNo, const String8 &account, const String8 &passwd, sp<IServer> &server) = 0;
    // 取消订阅
    virtual status_t unsubscribe(const String8 &orderNo) = 0;

protected:
    enum {
        TRANSACT_GETPRODUCT = IBinder::FIRST_CALL_TRANSACTION,
        TRANSACT_BARGAIN,
        TRANSACT_SUBSCRIBE,
        TRANSACT_UNSUBSCRIBE,
    };
};

class BnCloudServiceProvider : public BnInterface<ICloudServiceProvider>
{
public:
    virtual status_t onTransact(uint32_t code, 
                                const Parcel& data,
                                Parcel* reply,
                                uint32_t flags = 0);
};

} // android


#endif // !__ICLOUD_SERVER_Provider_H__



