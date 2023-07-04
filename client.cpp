
#define LOG_NDEBUG 0
#define LOG_TAG "AIDL-DEMO-client"
#include <binder/ProcessState.h>
#include "Student.h"

int main() {
    using namespace android;

    ALOGV("[%s %d %s]", __FILE__, __LINE__, __FUNCTION__);
    // 必须要开启接收线程，否则将无法收到Contact的回调消息
    // 原先没有调用startThreadPool，IContact的回调消息就无法收到~~~想了一晚上
    sp<ProcessState> proc(ProcessState::self());
    ProcessState::self()->startThreadPool();

    sp<ALooper> looper = new ALooper;
    looper->setName("aidl-client");
    {
        sp<Student> stu = new Student();
        looper->registerHandler(stu);
        looper->start();
        stu->getProducts();
        stu->bargain("TITAN00", 999);
        stu->subscribe("zhang san", "12345678");
        stu->connect();
        stu->doWork("train Fake-GPT 5.0");
        stu->doWork("train Fake-GPT 5.0 again");
        stu->unsubscribe();
        looper->unregisterHandler(stu->id());
    }
    looper->stop();
}