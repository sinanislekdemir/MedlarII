#ifndef _manager_h
#define _manager_h

#include <mscript.h>

class TaskManager {
    private:
        MScript tasks[2];
        int count;
    public:
        TaskManager();
        void add_task(char *filename);
        bool step();
        void run();
};

#endif
