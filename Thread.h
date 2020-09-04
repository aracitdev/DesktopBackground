#ifndef THREAD_H_INCLUDED
#define THREAD_H_INCLUDED

/**************************************************************************************************
* \file Timer.h
* \author Sean Comeau
* \brief Defines the thread class.
* \version 1.0
* Defines a c++ 11 based thread class.
***************************************************************************************************/

#include <thread>


/**************************************************************************************************
* \class Thread
* \brief A thread class designed to work using c++ 11.
* A thread wrapper class designed to work using c++ 11.
* Uses the STL implementation.
***************************************************************************************************/
class Thread
{
public:


/**************************************************************************************************
* \fn Thread(void (*functionPointer)(void*), void* data)
* \param functionPointer A pointer to the entry point of the thread
* \param data A pointer to any data that needs to be passed to the thread.
* \brief A constructor for Thread() that immediately initializes and calls the function in thread.
* A constructor for Thread() that immediately initializes and calls the function in the thread.
* Gives a pass off of a void pointer.
***************************************************************************************************/
    Thread(void (*functionPointer)(void*), void* data);

/**************************************************************************************************
* \fn Thread()
* \brief A constructor for Thread() that leaves the thread pointer empty, allowing for later usage.
* A constructor for Thread() that leaves the thread pointer empty, allowing for later usage by the Create function.
***************************************************************************************************/
    Thread();

/**************************************************************************************************
* \fn void Create(void (*functionPointer)(void*), void* data)
* \param functionPointer The entry point function of the thread.
* \param data A pointer of the data to be passed to the thread.
* \brief Initializes the thread pointer using the values provided.
* Initializes the thread pointer using the values provided and calls the entry of said thread.
***************************************************************************************************/
    void Create(void (*functionPointer)(void*), void* data);


/**************************************************************************************************
* \fn bool IsValid()
* \brief Returns whether or not the current thread is valid.
* Returns whether or not the current thread is valid (pointer non null and running).
* \return Returns true if valid, returns false if otherwise.
***************************************************************************************************/
    bool IsValid(void);

/**************************************************************************************************
* \fn void Join()
* \brief Calls the internalThread's join function (if initialized).
* Calls the internalThread's join function (if initialized). If not, do nothing.
***************************************************************************************************/
    void Join(void);

/**************************************************************************************************
* \fn void Kill()
* \brief Kills the thread.
* Kills the thread by destroying the thread pointer.
* \note This leaves the thread object inoperable, and Create must be called.
***************************************************************************************************/
    void Kill(void);


/**************************************************************************************************
* \fn std::thread::id GetInternalId()
* \brief Returns the thread id of the internalThread.
* Returns the thread id of the internalThread, meant to be used so a thread does not double lock
* a mutex, causing it to hault.
* \return The id of the thread.
***************************************************************************************************/
    std::thread::id GetInternalId(void);

private:
    std::thread* internalThread;
};

#endif // THREAD_H_INCLUDED
