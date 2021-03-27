#include "manager.h"
#include <freemem.h>

TaskManager::TaskManager()
{
    this->count = 0;
}

void TaskManager::add_task(char *filename)
{
    this->tasks[this->count].open(this->count, filename);
    this->count++;
}

bool TaskManager::step()
{
    bool still_running = false;
    for (uint8_t i = 0; i < this->count; i++)
    {
        if(this->tasks[i].finished) {
            continue;
        }
        this->tasks[i].step();
        still_running = true;
    }
    return still_running;
}

void TaskManager::run()
{
    while(this->step()){
    }
}
