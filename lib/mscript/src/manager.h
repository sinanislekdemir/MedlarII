#ifndef _manager_h
#define _manager_h

#include <mscript.h>

class TaskManager
{
private:
    MScript tasks[8];
    int count;
    bool stop;

public:
    TaskManager();
    void add_task(char *filename);
    bool step();
    void run();
    void shutdown();
};

#endif
