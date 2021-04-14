#include "manager.h"
#include <freemem.h>

TaskManager::TaskManager()
{
    this->count = 0;
    this->stop = false;
}

void TaskManager::add_task(char *filename)
{
    this->tasks[this->count].open(this->count, filename);
    this->count++;
}

void TaskManager::shutdown()
{
    this->stop = true;
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
    bool oom = false;
    while(this->step() && !this->stop){
        if (freeMemory() < 100)
        {
            oom = true;
            break;
        }
    }
    for (uint8_t i = 0; i < this->count; i++)
    {
        this->tasks[i].close();
    }
    if (oom)
    {
        Serial.println("Ran out of memory to operate");
    }
}
