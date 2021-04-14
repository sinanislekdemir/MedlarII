#ifndef _terminal_h
#define _terminal_h

char *active_directory;
char input_buffer[25];

// Draw main terminal window
int init_terminal();
int command_ls();
int command_dir();
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
#endif
