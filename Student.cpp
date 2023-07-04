#define LOG_NDEBUG 0
#define LOG_TAG "AIDL-DEMO-CLIENT"

#include <media/stagefright/foundation/AString.h>
#include <media/stagefright/foundation/AMessage.h>
#include <media/stagefright/foundation/ADebug.h>
#include <binder/IServiceManager.h>
#include <binder/IBinder.h>
#include <log/log.h>
#include "ICloudServiceProvider.h"
#include "IServer.h"
#include "IContact.h"
#include "Student.h"

namespace android {

class Student::Contact : public BnContact {
public:
    Contact(const sp<AMessage> &msg);
    virtual status_t sendMessage(const String8 &orderNo) override;
protected:
    ~Contact();
private:
    sp<AMessage> message;
};

Student::Contact::Contact(const sp<AMessage> &msg)
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    message = msg;
}

Student::Contact::~Contact()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
}

status_t Student::Contact::sendMessage(const String8 &orderNo)
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    // 拷贝一份AMessage，并设定新的参数
    sp<AMessage> msg = message->dup();
    msg->setString("orderNo", orderNo.c_str());
    msg->setWhat(kWhatReceiveMsg);
    msg->post();
    return NO_ERROR;
}

/*
class DeathNotifier : public IBinder::DeathRecipient {
public:
    DeathNotifier(sp<AMessage> msg) { message = msg;}
    virtual void binderDied(const wp<IBinder>& who) {
        ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
        sp<AMessage> msg = message->dup();
        msg->setWhat(Student::kWhatServerDied);
        msg->post();
    }
private:
    sp<AMessage> message;
};
*/

static sp<ICloudServiceProvider> gCloudProvider = NULL;

static sp<ICloudServiceProvider> getService() {
    if (gCloudProvider == NULL) {
        sp<IServiceManager> sm = defaultServiceManager();
        sp<IBinder> binder = sm->getService(String16("binder.demo.test"));
        gCloudProvider = interface_cast<ICloudServiceProvider>(binder);
    }
    return gCloudProvider;
}

Student::Student()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    mReplyToken = NULL;
    mServer = NULL;
}

Student::~Student()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
}

status_t Student::getProducts()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    sp<ICloudServiceProvider> cloudProvider = getService();
    if(cloudProvider != NULL) {
        KeyedVector<String8, int32_t> products;
        cloudProvider->getProducts(products);
        for(int i = 0; i < products.size(); i++) {
            ALOGD("[%s %d %s] products[%d]: %s --> %d", __FILE__, __LINE__, __FUNCTION__, i, products.keyAt(i).c_str(), products.valueAt(i));
        }
    } else {
        ALOGE("[%s %d %s] can not get cloudProvider", __FILE__, __LINE__, __FUNCTION__);
    }
    return NO_ERROR;
}

status_t Student::bargain(const AString &product, int32_t price)
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);

    sp<AMessage> msg = new AMessage(kWhatBargain, this);
    msg->setString("product", product);
    msg->setInt32("price", price);
    sp<AMessage> response;
    msg->postAndAwaitResponse(&response);
    CHECK(response->findString("orderNo", &mOrderNo));
    mReplyToken.clear();
    return NO_ERROR;
}

status_t Student::subscribe(const AString &account, const AString &passwd)
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    sp<ICloudServiceProvider> cloudProvider = getService();
    if(cloudProvider != NULL) {
        String8 orde(mOrderNo.c_str());
        String8 acco(account.c_str());
        String8 pass(passwd.c_str());
        cloudProvider->subscribe(orde, acco, pass, mServer);
        CHECK(mServer != NULL);
        mAccount = account;
        mPasswd = passwd;
        /*
        sp<AMessage> msgbk = new AMessage;
        msgbk->setTarget(this);
        sp<DeathNotifier> notify = new DeathNotifier(msgbk);
        IInterface::asBinder(mServer)->linkToDeath(notify);
        */
    } else {
        ALOGE("[%s %d %s] can not get cloudProvider", __FILE__, __LINE__, __FUNCTION__);
    }
    return NO_ERROR;
}

status_t Student::unsubscribe()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    sp<ICloudServiceProvider> cloudProvider = getService();
    if(cloudProvider != NULL) {
        /*
            mServer.clear();
            mServer->unlinkToDeath();
        */
        String8 orde(mOrderNo.c_str());
        cloudProvider->unsubscribe(orde);
    } else {
        ALOGE("[%s %d %s] can not get cloudProvider", __FILE__, __LINE__, __FUNCTION__);
    }
    return NO_ERROR;
}

status_t Student::connect()
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    if (mServer != NULL) {
        String8 acco(mAccount.c_str());
        String8 pass(mPasswd.c_str());
        mServer->connect(acco, pass);
    } else {
        ALOGE("[%s %d %s] can not get Server", __FILE__, __LINE__, __FUNCTION__);
    }
    return NO_ERROR;
}

status_t Student::doWork(const AString &cmd)
{
    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    if (mServer != NULL) {
        String8 command(cmd.c_str());
        String8 result;
        mServer->run(command, result);
        ALOGD("[%s %d %s] work done, precision = %s", __FILE__, __LINE__, __FUNCTION__, result.c_str());
    } else {
        ALOGE("[%s %d %s] can not get Server", __FILE__, __LINE__, __FUNCTION__);
    }
    return NO_ERROR;
}

void Student::onMessageReceived(const sp<AMessage> &msg) {
    switch (msg->what()) {
        case kWhatBargain:
        {
            CHECK(mReplyToken == NULL);
            CHECK(msg->senderAwaitsResponse(&mReplyToken));
            sp<AMessage> response = new AMessage;
            sp<ICloudServiceProvider> cloudProvider = getService();
            if(cloudProvider != NULL) {
                AString product;
                CHECK(msg->findString("product", &product));
                int32_t price;
                CHECK(msg->findInt32("price", &price));
                sp<AMessage> msgbk = new AMessage;
                msgbk->setTarget(this);
                mContact = new Contact(msgbk);
                String8 prod(product.c_str());
                cloudProvider->bargain(prod, price, mContact);
            } else {
                ALOGE("[%s %d %s] can not get cloudProvider", __FILE__, __LINE__, __FUNCTION__);
                response->postReply(mReplyToken);
            }
            break;
        }
        case kWhatReceiveMsg:
        {
            AString orderNum;
            CHECK(msg->findString("orderNo", &orderNum));
            sp<AMessage> response = new AMessage;
            response->setString("orderNo", orderNum);
            response->postReply(mReplyToken);
            break;
        }
        /*
        case kWhatServerDied:
        {
            ALOGE("[%s %d %s] server distroyed !!!", __FILE__, __LINE__, __FUNCTION__);
            //exit(1);
            break;
        }
        */
        default:
            break;
    };
}

}