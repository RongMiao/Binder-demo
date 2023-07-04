#define LOG_NDEBUG 0
#define LOG_TAG "AIDL-DEMO-PROVID"

#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/ADebug.h>
#include <log/log.h>
#include "CloudServiceProvider.h"
#include "IServer.h"
#include "Server.h"
#include "IContact.h"


namespace android {

class ServerWrapper : public RefBase
{
public:
    ServerWrapper(const sp<Server> &server) { mServer = server;}
    sp<Server>& getWrapper() { return mServer;}
protected:
    virtual ~ServerWrapper() {}
private:
    sp<Server> mServer;
};

class ContactWrapper : public RefBase
{
public:
    ContactWrapper(const sp<IContact> &contact) { mContact = contact;}
    sp<IContact>& getWrapper() { return mContact;}
protected:
    virtual ~ContactWrapper() {}
private:
    sp<IContact> mContact;
};

int32_t CloudServiceProvider::gOrderNo = 0;
sp<CloudServiceProvider> gProvider = NULL;

sp<CloudServiceProvider>& CloudServiceProvider::initiate()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    if(gProvider == NULL)
        gProvider = new CloudServiceProvider();
    return gProvider;
}

CloudServiceProvider::CloudServiceProvider()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    mLooper = new ALooper;
    mLooper->setName("CloudServiceProvider");
    mLooper->start();
    /*
        // can not put here ，构造函数中使用不完全的this指针可能会出现未知问题，要保证安全
        mHandler = new OrderHandler(this);
        mLooper->registerHandler(mHandler);
        Load();
    */
}

CloudServiceProvider::~CloudServiceProvider()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    mLooper->unregisterHandler(mHandler->id());
    mLooper->stop();
}

status_t CloudServiceProvider::getProducts(KeyedVector<String8, int32_t> &sales)
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    if(mContent.size() == 0) {
        mHandler = new OrderHandler(this);
        mLooper->registerHandler(mHandler);
        Load();
    }
    sp<AMessage> msg = new AMessage(kWhatGetProduct, mHandler);
    sp<AMessage> response;
    msg->postAndAwaitResponse(&response);
    void* p;
    // 找到回传的content指针
    CHECK(response->findPointer("content", &p));
    // 将指针强转为KeyedVector
    sales = *(static_cast<KeyedVector<String8, int32_t>*>(p));
    return NO_ERROR;
}

status_t CloudServiceProvider::bargain(const String8 &product, int32_t price, const sp<IContact> &contact)
{
    ALOGV("[%s %d %s] product = %s, guidePrice = %d", __FILE__, __LINE__, __FUNCTION__, product.c_str(), price);
    int32_t guidePrice = mContent.valueFor(product);
    sp<AMessage> msg = new AMessage(kWhatBargain, mHandler);
    msg->setString("product", product.c_str());
    msg->setInt32("price", price);
    sp<ContactWrapper> wrapper = new ContactWrapper(contact);
    msg->setObject("contact", wrapper);
    // 砍的越狠，回复越慢
    msg->post((guidePrice - price)*100*2);
    return NO_ERROR;
}

status_t CloudServiceProvider::subscribe(const String8 &orderNo, const String8 &account, const String8 &passwd, sp<IServer> &server)
{
    ALOGV("[%s %d %s] orderNo = %s, account = %s, passwd = %s", __FILE__, __LINE__, __FUNCTION__, orderNo.c_str(), account.c_str(), passwd.c_str());
    sp<AMessage> msg = new AMessage(kWhatSubscribe, mHandler);
    msg->setString("orderNo", orderNo.c_str());
    msg->setString("account", account.c_str());
    msg->setString("passwd", passwd.c_str());
    sp<AMessage> response;
    msg->postAndAwaitResponse(&response);
    sp<RefBase> obj;
    CHECK(response->findObject("server", &obj));
    /*
        cannot cast 'android::RefBase *' to 'android::Server *' via virtual base 'android::RefBase'
        sp<Server> serv = static_cast<Server*>(obj.get());
        由于Server虚继承于RefBase，不能直接将指向基类的指针转换为指向派生类的指针
        所以要用一层wrapper来封装
    */
    sp<ServerWrapper> wrapper = static_cast<ServerWrapper*>(obj.get());
    server = wrapper->getWrapper();
    return NO_ERROR;
}

status_t CloudServiceProvider::unsubscribe(const String8& orderNo)
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    sp<AMessage> msg = new AMessage(kWhatUnSubscribe, mHandler);
    msg->setString("orderNo", orderNo.c_str());
    sp<AMessage> response;
    msg->postAndAwaitResponse(&response);
    return NO_ERROR;
}


void CloudServiceProvider::Load()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    // String8不允许隐式转换，先创建String8，再构造keyedvector
    String8 content0("GTX1080");
    String8 content1("RTX2080");
    String8 content2("RTX3080");
    String8 content3("RTX4080");
    String8 content4("TITAN00");
    String8 content5("A100000");
    mContent.add(content0, 1999);
    mContent.add(content1, 2999);
    mContent.add(content2, 3999);
    mContent.add(content3, 5999);
    mContent.add(content4, 7999);
    mContent.add(content5, 9999);
}


void CloudServiceProvider::OrderHandler::onMessageReceived(const sp<AMessage> &msg) {
    // 先获取强引用对象
    sp<CloudServiceProvider> provider = mProvider.promote();
    CHECK(provider != NULL);

    switch (msg->what()) {
        case kWhatGetProduct: 
        {
            sp<AReplyToken> reply;
            CHECK(msg->senderAwaitsResponse(&reply));
            // 创建一个response message
            sp<AMessage> response = new AMessage;
            // 将指针进行回传
            response->setPointer("content", &(provider->mContent));
            response->postReply(reply);
            break;
        }
        case kWhatBargain:
        {
            AString product;
            CHECK(msg->findString("product", &product));
            int32_t price;
            CHECK(msg->findInt32("price", &price));
            sp<RefBase> obj;
            // 获取传过来的IContact
            CHECK(msg->findObject("contact", &obj));
            /*
                error: cannot cast 'android::RefBase *' to 'android::IContact *' via virtual base 'android::RefBase'
                sp<IContact> contact = static_cast<IContact*>(obj.get());
            */
            sp<ContactWrapper> wrapper = static_cast<ContactWrapper*>(obj.get());
            sp<IContact> contact = wrapper->getWrapper();
            // 创建一个订单
            Entry entry = {.product = product, .contact = contact};
            // 创建订单号
            char orderNo[6];
            sprintf(orderNo, "%05d", gOrderNo);
            provider->mOrder.add(orderNo, entry);
            // 通知砍价的人，砍价成功，发送订单号
            String8 s(orderNo);
            contact->sendMessage(s);
            ++gOrderNo;
            break;
        }
        case kWhatSubscribe:
        {
            sp<AReplyToken> reply;
            CHECK(msg->senderAwaitsResponse(&reply));
            AString orderNo, account, passwd;
            CHECK(msg->findString("orderNo", &orderNo));
            CHECK(msg->findString("account", &account));
            CHECK(msg->findString("passwd", &passwd));
            // 找到订单号对应的订单详情
            // 这里要加& ！否则出现问题，另外valueFor返回值是不可以编辑的
            Entry& entry = provider->mOrder.editValueFor(orderNo);
            // 用账号密码开一个Server
            sp<Server> server = new Server(orderNo, entry.product, account, passwd);
            entry.server = server;
            // 将Server进行包装返回给订购者
            sp<ServerWrapper> wrapper = new ServerWrapper(server);
            sp<AMessage> response = new AMessage;
            response->setObject("server", wrapper);
            response->postReply(reply);
            break;
        }
        case kWhatUnSubscribe:
        {
            sp<AReplyToken> reply;
            CHECK(msg->senderAwaitsResponse(&reply));
            AString orderNo;
            CHECK(msg->findString("orderNo", &orderNo));
            // 删除server
            provider->mOrder.removeItem(orderNo);
            sp<AMessage> response = new AMessage;
            response->postReply(reply);
            break;
        }
        default:
            break;
    };
}

} // android
