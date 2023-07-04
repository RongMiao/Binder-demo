#define LOG_NDEBUG 0
#define LOG_TAG "AIDL-DEMO-service"

#include <binder/IServiceManager.h>
#include <binder/IPCThreadState.h>
#include "CloudServiceProvider.h"

int main() {
    using namespace android;

    ALOGV("[%s %s %d]", __FILE__, __FUNCTION__, __LINE__);
    /*
        // 这里没有调用这两个函数是因为defaultServiceManager中会自动调用这两个函数
        // 默认打开 /dev/binder，线程数也为默认
        // main函数最后调用joinThreadPool即可阻塞，线程监听并执行消息
        sp<ProcessState> proc(ProcessState::self());
        ProcessState::self()->startThreadPool();
    */
    
    
    sp<IServiceManager> sm(defaultServiceManager());
    /*
        error: cannot initialize a member subobject of type 'android::IBinder *' with an lvalue of type 'android::ICloudServiceProvider *const'
        sp<ICloudServiceProvider> provider = CloudServiceProvider::initiate();
        addService(const String16& name, const sp<IBinder>& service
        addService要传递的是一个IBinder，并不能从IInterface(ICloudServiceProvider)那边强转，
        sp<IBinder> provider = CloudServiceProvider::initiate();  这样声明是可以的
    */
    sp<CloudServiceProvider> provider = CloudServiceProvider::initiate();
    sm->addService(String16("binder.demo.test"), provider);
    IPCThreadState::self()->joinThreadPool();
}