/**
 * @file terminal.cpp
 * @author Sinan Islekdemir (sinan@islekdemir.com)
 * @brief Main Medlar II Terminal
 * @version 0.1
 * @date 2021-04-13
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#include "terminal.h"
#include <Arduino.h>
#include <const.h>
#include <manager.h>

TaskManager m;
char command[32];

void kill_tasks()
{
    m.shutdown();
}

int command_ls(File *dir)
{
    dir->seek(0);
    while (dir->available())
    {
        File entry = dir->openNextFile();
        if (!entry)
        {
            continue;
        }
        if (entry.isDirectory())
        {
            print_vga("[");
            print_vga(entry.name());
            println_vga("]");
        }
        else
        {
            println_vga(entry.name());
        }
        entry.close();
    }
    return 0;
}

int command_mem()
{
    println_vga(freeMemory());
    return 0;
}

int s_readline(char *back)
{
    int pos = strlen(back);

    while (Serial.available())
    {
        char c = Serial.read();
        Serial.print(c);
        if (c == '\n')
        {
            back[pos++] = '\0';
            clear_input_buffer();
            return 1;
        }
        if (c > 31 && c < 127)
        {
            back[pos++] = c;
            print_vga_input(c);
        }
        if (c == 8)
        {
            pos--;
            clear_input_buffer();
            back[pos++] = '\0';
            print_vga_input(back);
        }
    }
    return 0;
}

int init_terminal()
{
    Serial.begin(9600);
    setup_statements();
    start_vga_driver();
    pinMode(SHUTDOWN_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SHUTDOWN_PIN), kill_tasks, CHANGE);
    if (!Serial)
    {
        println_vga(MSG_SERIAL_ERR);
    }

    println_vga("Medlar II");
    while (!SD.begin())
    {
        Serial.println("Broken filesystem, waiting 60 seconds for the fix");
        delay(60000);
    }

    Serial.println("Ready");
    clear_buffer();

    while (true)
    {
        int ready = s_readline(command);
        if (ready == 0) {
            f_delay(10);
            continue;
        }
        Serial.print("<");
        Serial.print(command);
        Serial.println(">");

        if (strcmp(command, "ls") == 0)
        {
            File f = SD.open("/");
            command_ls(&f);
            f.close();
            memset(command, '\0', 32);
            clear_input_buffer();
            continue;
        }
        if (strcmp(command, "free") == 0)
        {
            command_mem();
            memset(command, '\0', 32);
            clear_input_buffer();
            continue;
        }
        if (strncmp(command, "pinup", 5) == 0)
        {
            char *rests = (char *)malloc(4);
            memset(rests, '\0', 4);
            extract(command, ' ', 1, rests);
            digitalWrite(atoi(rests), HIGH);
            free(rests);
            memset(command, '\0', 32);
            clear_input_buffer();
            continue;
        }
        if (strncmp(command, "pindown", 7) == 0)
        {
            char *rests = (char *)malloc(4);
            memset(rests, '\0', 4);
            extract(command, ' ', 1, rests);
            digitalWrite(atoi(rests), LOW);
            free(rests);
            memset(command, '\0', 32);
            clear_input_buffer();
            continue;
        }
        if (strncmp(command, "rm ", 3) == 0)
        {
            char *rests = (char *)malloc(28);
            memset(rests, '\0', 28);
            extract(command, ' ', 1, rests);
            SD.remove(rests);
            free(rests);
            memset(command, '\0', 32);
            clear_input_buffer();
            continue;
        }
        if (strncmp(command, "mkdir ", 6) == 0)
        {
            char *rests = (char *)malloc(28);
            memset(rests, '\0', 28);
            extract(command, ' ', 1, rests);
            SD.mkdir(rests);
            free(rests);
            memset(command, '\0', 32);
            clear_input_buffer();
            continue;
        }
        if (strncmp(command, "cd ", 3) == 0)
        {
        }
        if (strcmp(command, "cls") == 0)
        {
            clear_buffer();
            memset(command, '\0', 32);
            clear_input_buffer();
            continue;
        }
        if (strcmp(command, "exit") == 0)
        {
            memset(command, '\0', 32);
            clear_input_buffer();
            clear_buffer();
            println_vga("It's now safe");
            println_vga("   to turn off");
            println_vga("      your computer");
            push_buffer();
            kill_tasks();
            SD.end();
            Serial.println("Bye");
            Serial.end();
            while (1)
                ;
            continue;
        }
        if (strncmp(command, "load ", 5) == 0)
        {
            char *rests = (char *)malloc(14);
            memset(rests, '\0', 14);
            extract(command, ' ', 1, rests);
            if (SD.exists(rests))
            {
                m.add_task(rests);
                println_vga("Loaded script");
            }
            else
            {
                println_vga("File not found");
            }
            free(rests);
        }
        if (strcmp(command, "run") == 0)
        {
            m.run();
        }
        if (strcmp(command, "sizeof") == 0)
        {
            print_vga("Size of mscript:");
            println_vga(sizeof(MScript));
        }
        sprintf(command, "%s%s", command, ".m");
        if (SD.exists(command))
        {
            m.add_task(command);
            m.run();
        }
        push_buffer();
        memset(command, '\0', 32);
        clear_input_buffer();
    }
}
