#ifndef _terminal_h
#define _terminal_h

#define MSG_SERIAL_ERR "Serial not available"
#define MSG_DISK_ERR "Disk error"

// Draw main terminal window
int init_terminal();
int command_ls();
int command_run();
int command_help();
int command_edit();
int command_about();
// Connect to the main machine
int command_connect();
int command_copy();
int command_date();
int command_time();
int command_mem(); // show free memory
int command_apps(); // list available apps
int full_refresh(); // full refresh the screen in intervals to avoid burn-in
void kill_tasks();
int s_readline(char *back);
#endif
