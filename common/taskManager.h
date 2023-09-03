#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#ifdef __cplusplus
extern "C" {
#endif
#include "fsm.h"
#include "timer.h"
#ifdef __cplusplus
}
#endif

class taskManager
{
private:
    void taskScheduler() {
        
    }
public:
    taskManager(/* args */);
    ~taskManager();
};

taskManager::taskManager(/* args */)
{
}

taskManager::~taskManager()
{
}



#endif // TASK_MANAGER_H