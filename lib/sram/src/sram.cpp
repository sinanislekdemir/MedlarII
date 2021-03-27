#include "sram.h"

#include <Arduino.h>
#include <freemem.h>
#include <stdint.h>

SRam::SRam() {}

SRam::~SRam() { this->close(); }

char *dtoc(double d)
{
    char *value = reinterpret_cast<char *>(&d);
    return value;
}

double ctod(char *data)
{
    double resp = *reinterpret_cast<double *const>(data);
    return resp;
}

uint8_t argc(char *text, char delimiter)
{
    // return argument count given in a text
    bool string_literal = false;
    uint8_t count = 1;
    for (uint8_t i = 0; i < strlen(text); i++)
    {
        if (text[i] == '"')
        {
            string_literal = !string_literal;
        }
        if (!string_literal && text[i] == '#')
        {
            break;
        }
        if (!string_literal && text[i] == delimiter)
        {
            count += 1;
        }
    }
    return count;
}

// strtok should do fine but I need to keep "..." intact
int extract(char *text, char delimiter, uint8_t part, char *back)
{
    int n = strlen(text);
    int j, k;
    j = 0;
    k = 0;
    for (uint8_t i = 0; i < part; i++)
    {
        while (j < n && text[j] != delimiter)
        {
            ++j;
        }
        ++j;
    }
    if (j == n)
    {
        return -1;
    }
    k = 0;
    while (j < n && text[j] != delimiter)
    {
        back[k++] = text[j];
        j++;
    }
    back[k] = '\0';

    return 0;
}

int rest(char *text, uint8_t pos, char *back)
{
    if (pos >= strlen(text))
    {
        return 0;
    }
    uint8_t j = 0;
    for (uint8_t i = pos; i < strlen(text); i++)
    {
        back[j++] = text[i];
    }
    return j;
}

void SRam::open(const char *filename)
{
    if (SD.exists(filename))
    {
        bool d = SD.remove(filename);
        if (!d)
        {
            Serial.println("Failed to delete");
        }
    }
    // FILE_WRITE != O_RDWR
    this->ram = SD.open(filename, FILE_WRITE);
    this->ram.close();
    this->ram = SD.open(filename, O_RDWR);
    if (!this->ram)
    {
        return;
    }
    this->isOpen = true;
}

void SRam::close()
{
    if (this->isOpen)
    {
        this->ram.flush();
        this->ram.close();
    }
}

memoryBlockHeader *SRam::findVariable(char *name, uint16_t pid)
{
    // Idea!
    // PID | VARNAME | USED | POSITION | SIZE | VARIABLE-DATA | PID | VARNAME |
    // USED... Header followed by the value. then the next variable header.
    memoryBlockHeader *variable = (memoryBlockHeader *)malloc(HeaderSize);
    uint32_t pos = 0;
    variable->exists = 0;
    if (!this->isOpen)
    {
        free(variable);
        return NULL;
    }

    this->ram.seek(0);
    // memset(variable, 0, HeaderSize);
    while (this->ram.available())
    {
        this->ram.readBytes((char *)(variable), HeaderSize);
        if (!variable->exists || variable->pid != pid ||
            strcmp(variable->varname, name) != 0)
        {
            pos += HeaderSize +
                   variable->size; // fast forward to the next variable position
            bool seek = this->ram.seek(pos);
            if (!seek)
            {
                break;
            }
            continue;
        }
        variable->position = pos + HeaderSize;
        return variable;
    }
    free(variable);
    return NULL;
}

void SRam::allocateVariable(char *name, uint16_t pid, uint16_t variableSize, uint8_t variable_type)
{
    memoryBlockHeader variable;
    if (!this->isOpen)
    {
        Serial.println("File not open");
        return;
    }
    this->ram.seek(this->ram.size());
    strcpy(variable.varname, name);
    variable.exists = 1;
    variable.size = variableSize;
    variable.pid = pid;
    variable.type = variable_type;
    this->ram.write((char *)(&variable), HeaderSize);
    this->ram.flush();
    // allocate extra space for the variable
    for (uint16_t i = 0; i < variableSize; i++)
    {
        this->ram.write("\0", 1);
        if (i % 128 == 0)
        {
            this->ram.flush();
        }
    }
    this->ram.flush();
}

void SRam::deleteVariable(char *name, uint16_t pid)
{
    memoryBlockHeader *variable = this->findVariable(name, pid);
    if (variable == NULL)
    {
        return;
    }

    uint32_t location = variable->position - HeaderSize + 1;
    variable->exists = 0;

    this->ram.seek(location);
    this->ram.write((char *)(variable), HeaderSize);
    this->ram.flush();
    free(variable);
}

uint16_t SRam::read(char *name, uint16_t pid, uint32_t pos, char *buffer,
                    uint16_t size)
{
    memoryBlockHeader *variable = this->findVariable(name, pid);
    if (variable == NULL)
    {
        return 0;
    }
    if (pos > variable->size)
    {
        free(variable);
        return 0;
    }
    if (pos + size > variable->size)
    {
        size = variable->size - pos;
    }
    bool seek_check = this->ram.seek(variable->position);
    if (!seek_check)
    {
        free(variable);
        return 0;
    }
    free(variable);
    return this->ram.readBytes(buffer, size);
}

uint16_t SRam::readAll(char *name, uint16_t pid, char *buffer)
{
    memoryBlockHeader *variable = this->findVariable(name, pid);
    if (variable == NULL)
    {
        return 0;
    }
    uint16_t r = this->read(name, pid, 0, buffer, variable->size);
    free(variable);
    return r;
}

uint16_t SRam::write(char *name, uint16_t pid, uint32_t pos, char *data,
                     uint16_t size)
{
    memoryBlockHeader *variable = this->findVariable(name, pid);
    if (variable == NULL)
    {
        return -1;
    }
    if (size + pos > variable->size)
    {
        free(variable);
        return -2;
    }
    bool seek_check = this->ram.seek(variable->position + pos);
    if (!seek_check)
    {
        free(variable);
        return -3;
    }
    free(variable);

    uint16_t size_w = this->ram.write(data, size);
    this->ram.flush();
    return size_w;
}

int SRam::get_var_size(char *text, uint16_t pid)
{
    if (text[0] == '"' && text[strlen(text) - 1] == '"')
    {
        return strlen(text);
    }

    if (strlen(text) > 0 && isdigit(text[0]))
    {
        return sizeof(double);
    }

    memoryBlockHeader *m = this->findVariable(text, pid);

    if (m == NULL)
    {
        return 0;
    }

    if (m->type == TYPE_NUM)
    {
        free(m);
        return sizeof(double);
    }

    int s = m->size;
    free(m);

    return s;
}

/**
 * @brief Get variable from memory file
 * 
 * @param text - variable name
 * @param pid  - process id
 * @param back - back buffer
 * @return int - return 0 for text, 1 for number, -1 for no match
 */
int SRam::get_var(char *text, uint16_t pid, char *back)
{
    if (back == NULL)
    {
        return 0;
    }

    if (text[0] == '"' && text[strlen(text) - 1] == '"')
    {
        memset(back, 0, strlen(text));
        uint8_t p = 0;
        for (unsigned int i = 1; i < strlen(text) - 1; i++)
        {
            back[p++] = text[i];
        }
        return TYPE_BYTE;
    }

    if (strlen(text) > 0 && isdigit(text[0]))
    {
        double x = atof(text);
        memset(back, 0, 4);
        memcpy(back, dtoc(x), sizeof(double));
        return TYPE_NUM;
    }

    memoryBlockHeader *m = this->findVariable(text, pid);

    if (m == NULL)
    {
        return -1;
    }

    uint16_t from = 0;
    uint16_t read_size = m->size;

    if (strstr(text, "[") != NULL && strstr(text, "]") != NULL)
    {
        char *target = NULL;
        char *start, *end;
        char temp[16];
        start = strstr(text, "[");
        if (start)
        {
            start += 1;
            end = strstr(start, "]");
            if (end)
            {
                target = (char *)malloc(end - start + 1);
                memcpy(target, start, end - start);
                target[end - start] = '\0';
            }
        }
        if (argc(target, ':') == 1)
        {
            from = atol(target);
        }
        if (argc(target, ':') == 2)
        {
            memset(temp, 0, 16);
            extract(target, ':', 0, temp);
            from = atol(temp);
            memset(temp, 0, 16);
            extract(target, ':', 1, temp);
            read_size = atol(temp);
        }
        free(target);
        free(start);
        free(end);
    }

    this->read(text, pid, from, back, read_size);
    if (m->type == TYPE_NUM)
    {
        free(m);
        return TYPE_NUM;
    }
    free(m);
    return TYPE_BYTE;
}

void SRam::dump()
{
    // Idea!
    // PID | VARNAME | USED | POSITION | SIZE | VARIABLE-DATA | PID | VARNAME |
    // USED... Header followed by the value. then the next variable header.
    memoryBlockHeader variable;
    this->ram.seek(0);
    if (!this->ram)
    {
        Serial.println("Memory is not accessable");
    }
    Serial.print("Memory size:");
    Serial.print(this->ram.size());
    Serial.println(" bytes");
    while (this->ram.readBytes((char *)(&variable), HeaderSize) > 0)
    {
        Serial.print("Variable name: ");
        Serial.println(variable.varname);
        Serial.print("Variable exists: ");
        Serial.println(variable.exists);
        Serial.print("Variable size: ");
        Serial.println(variable.size);
        Serial.print("Variable type: ");
        Serial.println(variable.type);
        this->ram.seek(this->ram.position() + variable.size);
    }
}
