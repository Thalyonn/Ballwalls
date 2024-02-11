#ifndef MYRUNNABLE_H
#define MYRUNNABLE_H
#include "QRunnable"

class MyRunnable : public QRunnable
{
public:
    MyRunnable();
    void run() override;
};

#endif // MYRUNNABLE_H
