#include "thread.h"

Thread::Thread()
{
    internalThread=nullptr;
}

Thread::Thread(void (*functionPointer)(void*), void* data)
{
    internalThread=new std::thread(functionPointer, data);
}

void Thread::Join(void)
{
    if(internalThread != nullptr)
        internalThread->join();
    delete internalThread;
}

void Thread::Create(void (*functionPointer)(void*), void* data)
{
    if(internalThread != nullptr)
        Kill();
    internalThread=new std::thread(functionPointer, data);
}

bool Thread::IsValid(void)
{
    return internalThread != nullptr && internalThread->joinable();
}

void Thread::Kill(void)
{
    if(internalThread != nullptr)
        delete internalThread;
    internalThread=nullptr;
}

std::thread::id Thread::GetInternalId(void)
{
    if(internalThread == nullptr)
        return std::thread::id();
    return internalThread->get_id();
}
