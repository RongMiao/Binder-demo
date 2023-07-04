#ifndef __STUDENT_H__
#define __STUDENT_H__

#include <media/stagefright/foundation/ALooper.h>
#include <media/stagefright/foundation/AHandler.h>

namespace android {

class IServer;
struct ALooper;
struct AMessage;
struct AString;

class Student : public AHandler
{
public:
    class Contact;

    Student();
    status_t getProducts();
    status_t bargain(const AString &product, int32_t price);
    status_t subscribe(const AString &account, const AString &passwd);
    status_t unsubscribe();
    status_t connect();
    status_t doWork(const AString &cmd);

/*
    enum {
        kWhatServerDied         = 'kDie',
    };
*/

protected:
    ~Student();
    virtual void onMessageReceived(const sp<AMessage> &msg) override;

    enum {
        kWhatBargain            = 'kWod',
        kWhatReceiveMsg         = 'kRev',
        kWhatDoWork             = 'kWrk',
    };

private:
    AString mAccount;
    AString mPasswd;
    AString mOrderNo;
    sp<IServer> mServer;
    sp<AReplyToken> mReplyToken;
    sp<Contact> mContact;
};

} // android



#endif // ! __STUDENT_H__