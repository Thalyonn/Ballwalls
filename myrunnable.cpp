#include "myrunnable.h"

#include "QDebug"

#include "qthread.h"

MyRunnable::MyRunnable() {

}

void MyRunnable::run() {
    qDebug() << "Hello world from thread" << QThread::currentThread();
}
