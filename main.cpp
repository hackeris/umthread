#include <iostream>
#include <functional>
#include "umthread.h"

using namespace std;

class IRunnable {
public:
    virtual void run() = 0;

    virtual ~IRunnable() { };
};

class CLambdaRunnable
        : public IRunnable {
public:
    CLambdaRunnable(std::function<void(void)> fun) : func(fun) { }

    void run() override {
        func();
    }

    ~CLambdaRunnable() override {

    }

private:
    std::function<void(void)> func;
};

class CUserModeThread {
public:
    CUserModeThread(IRunnable *runnable)
            : mRunnable(runnable) { }

    void start() {
        umthread_create(CUserModeThread::pThreadFunc, this);
    }

    void cancel() {
        delete mRunnable;
    }

private:
    void doRunThread() {
        mRunnable->run();
    }

    static void pThreadFunc(void *param) {
        CUserModeThread *pThis = (CUserModeThread *) param;
        pThis->doRunThread();
        delete pThis;
    }

    ~CUserModeThread() {
        cancel();
    }

private:
    IRunnable *mRunnable;
};

class CHelloTask : public IRunnable {
public:
    void run() override {
        for (long long i = 0; i < 320000000; i++) {
            if (i % 10000000 == 0) {
                printf("%lli\n", i);
            }
        }
    }
};

int main() {

    CUserModeThread *thread = new CUserModeThread(new CHelloTask());
    CUserModeThread *thread2 = new CUserModeThread(new CHelloTask());
    CUserModeThread *thread3 = new CUserModeThread(new CLambdaRunnable([]() {
        printf("Hello Lamba thread.\n");
    }));
    thread->start();
    thread2->start();
    thread3->start();

    return 0;
}
