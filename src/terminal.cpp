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

void shutdown()
{
    m.shutdown();
    SD.end();
    Serial.println("Bye");
    Serial.end();
    while(1);
}

int command_ls(File *dir)
{
    while (true)
    {
        File entry = dir->openNextFile();
        if (!entry)
        {
            break;
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
        Serial.println(entry.name());
        entry.close();
    }
    return 0;
}

int command_mem()
{
    println_vga(freeMemory());
    return 0;
}

int readline(char *back)
{
    int pos = 0;
    while (true)
    {
        while (Serial.available())
        {
            char c = Serial.read();
            Serial.print(c);
            if (c == '\n')
            {
                back[pos++] = '\0';
                delay(500);
                clear_input_buffer();
                return 0;
            }
            if (c > 31 && c < 127)
            {
                back[pos++] = c;
                print_vga_input(c);
            }
            if (c == 8)
            {
                pos--;
                back[pos++] = '\0';
            }
        }
    }
}

int init_terminal()
{
    Serial.begin(9600);
    setup_statements();
    start_vga_driver();
    pinMode(SHUTDOWN_PIN, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(SHUTDOWN_PIN), shutdown, CHANGE);
    if (!Serial)
    {
        println_vga(MSG_SERIAL_ERR);
    }

    char command[32];
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

        memset(command, '\0', 32);
        clear_input_buffer();
        readline(command);

        if (strcmp(command, "ls") == 0)
        {
            File f = SD.open("/");
            command_ls(&f);
            f.close();
            continue;
        }
        if (strcmp(command, "free") == 0)
        {
            command_mem();
            continue;
        }
        if (strncmp(command, "pinup", 5) == 0)
        {
            char *rests = (char *)malloc(4);
            memset(rests, '\0', 4);
            extract(command, ' ', 1, rests);
            digitalWrite(atoi(rests), HIGH);
            free(rests);
            continue;
        }
        if (strncmp(command, "pindown", 7) == 0)
        {
            char *rests = (char *)malloc(4);
            memset(rests, '\0', 4);
            extract(command, ' ', 1, rests);
            digitalWrite(atoi(rests), LOW);
            free(rests);
            continue;
        }
        if (strncmp(command, "rm ", 3) == 0)
        {
            char *rests = (char *)malloc(28);
            memset(rests, '\0', 28);
            extract(command, ' ', 1, rests);
            SD.remove(rests);
            free(rests);
            continue;
        }
        if (strncmp(command, "mkdir ", 6) == 0)
        {
            char *rests = (char *)malloc(28);
            memset(rests, '\0', 28);
            extract(command, ' ', 1, rests);
            SD.mkdir(rests);
            free(rests);
            continue;
        }
        if (strncmp(command, "cd ", 3) == 0)
        {
        }
        if (strcmp(command, "cls") == 0)
        {
            clear_buffer();
            continue;
        }
        if (strcmp(command, "exit") == 0)
        {
            shutdown();
            continue;
        }
        command[strlen(command)] = '.';
        command[strlen(command)] = 'm';
        if (SD.exists(command))
        {
            m.add_task(command);
            m.run();
        }
    }
}
