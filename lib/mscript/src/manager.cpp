#include "manager.h"
#include <freemem.h>

TaskManager::TaskManager()
{
    this->count = 0;
    this->stop = false;
}

void TaskManager::add_task(char *filename)
{
    int c = this->tasks[this->count].open(this->count, filename);
    if (c == -1)
    {
        println_vga("Application crash");
        return;
    }
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
    this->count = 0;
    this->stop = true;
    if (oom)
    {
        Serial.println("Ran out of memory to operate");
    }
}
