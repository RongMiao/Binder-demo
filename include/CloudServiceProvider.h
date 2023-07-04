#ifndef __CLOUD_SERVICE_PROVIDER_H__
#define __CLOUD_SERVICE_PROVIDER_H__

#include <media/stagefright/foundation/AHandler.h>
#include <utils/KeyedVector.h>
#include <ICloudServiceProvider.h>

namespace android {

struct AString;
class IContact;
class IServer;
struct ALooper;
struct AMessage;

/*
    原先这里是想使用 class CloudServiceProvider : public BnCloudServiceProvider, public AHandler
    原因是编译时会出现问题，BnCloudServiceProvider虚继承于RefBase，AHandler却没有虚继承RefBase，形成了菱形继承，所以这里Handler需要单独写一个类
 */
class CloudServiceProvider : public BnCloudServiceProvider
{
public:
    static sp<CloudServiceProvider>& initiate();
    virtual status_t getProducts(KeyedVector<String8, int32_t> &sales) override;
    virtual status_t bargain(const String8 &product, int32_t price, const sp<IContact> &contact) override;
    virtual status_t subscribe(const String8 &orderNo, const String8 &account, const String8 &passwd, sp<IServer> &server) override;
    virtual status_t unsubscribe(const String8 &orderNo) override;

protected:
    CloudServiceProvider();
    ~CloudServiceProvider();

private:
    void Load();    // load content
    enum {
        kWhatGetProduct     = 'kGet',
        kWhatBargain        = 'kWBa',
        kWhatSubscribe      = 'kSub',
        kWhatUnSubscribe    = 'kUnS',
    };
    
    /*
        内部类可以访问外部类的私有成员与方法
        反之，外部类是不能访问内部类的私有成员的
    */
    class OrderHandler : public AHandler {
    public:
        OrderHandler(const sp<CloudServiceProvider> &serviceProvider)
            : mProvider(serviceProvider) {
            }
    protected:
        virtual void onMessageReceived(const sp<AMessage> &msg) override;
    private:
        wp<CloudServiceProvider> mProvider;
    };

private:
    struct Entry {
        sp<IContact> contact;
        sp<IServer> server;
        AString product;
    };
    static int32_t gOrderNo;
    sp<ALooper> mLooper;
    sp<OrderHandler> mHandler;
    /*
    reference to type 'const android::String8' could not bind to an lvalue of type 'const char [8]'
    */
    KeyedVector<String8, int32_t> mContent;
    KeyedVector<AString, Entry> mOrder;
};

} // android

#endif // ! __CLOUD_SERVICE_PROVIDER_H__