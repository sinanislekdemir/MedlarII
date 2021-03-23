#include "sram.h"

#include <Arduino.h>
#include <stdint.h>

SRam::SRam() {}

SRam::~SRam() { this->close(); }


char *dtoc(double d) {
    char *value = reinterpret_cast<char *>(&d);
    return value;
}


double ctod(char *data) {
    double resp = *reinterpret_cast<double * const>(data);
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
    uint8_t part_count = argc(text, delimiter);
    if (part >= part_count)
    {
        return -1;
    }
    uint8_t active = 0;
    uint8_t pos = 0;
    bool string_literal = false;

    for (uint8_t i = 0; i < strlen(text); i++)
    {
        if (text[i] == '"')
        {
            string_literal = !string_literal;
        }
        if (text[i] == '#' && !string_literal)
        {
            return -1;
        }
        if (text[i] != delimiter && active == part)
        {
            back[pos++] = text[i];
        }
        if (text[i] == '\0') {
            back[pos++] = '\0';
            return 0;
        }
        if (!string_literal && text[i] == delimiter)
        {
            if (active == part)
            {
                back[pos++] = '\0';
                return 0;
            }
            active++;
        }
    }
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
        if (!d) {
            Serial.println("Failed to delete");
        }
    }
    // FILE_WRITE != O_RDWR
    this->ram = SD.open(filename, FILE_WRITE);
    Serial.println(this->ram.availableForWrite());
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

memoryBlockHeader SRam::findVariable(char *name, uint16_t pid)
{
    // Idea!
    // PID | VARNAME | USED | POSITION | SIZE | VARIABLE-DATA | PID | VARNAME |
    // USED... Header followed by the value. then the next variable header.
    memoryBlockHeader variable;
    uint32_t pos = 0;
    variable.exists = 0;
    if (!this->isOpen)
    {
        return variable;
    }
    this->ram.seek(0);
    while (this->ram.readBytes((char *)(&variable), HeaderSize) > 0)
    {
        if (!variable.exists || variable.pid != pid ||
            strcmp(variable.varname, name) != 0)
        {
            pos += HeaderSize +
                   variable.size; // fast forward to the next variable position
            bool seek = this->ram.seek(pos);
            if (!seek)
            {
                break;
            }
            continue;
        }
        variable.position = pos + HeaderSize;
        return variable;
    }
    return variable;
}

void SRam::allocateVariable(char *name, uint16_t pid, uint16_t variableSize, uint8_t variable_type)
{
    memoryBlockHeader variable;
    if (!this->isOpen)
    {
        return;
    }
    this->ram.seek(this->ram.size());
    strcpy(variable.varname, name);
    variable.exists = 1;
    variable.size = variableSize;
    variable.pid = pid;
    variable.type = variable_type;
    this->ram.write((char *)(&variable), HeaderSize);
    // allocate extra space for the variable
    for (uint16_t i = 0; i < variableSize; i++)
    {
        this->ram.write("\0", 1);
        if (i % 512 == 0)
        {
            this->ram.flush();
        }
    }
}

void SRam::deleteVariable(char *name, uint16_t pid)
{
    memoryBlockHeader variable = this->findVariable(name, pid);
    if (variable.exists == 0)
    {
        return;
    }

    uint32_t location = variable.position - HeaderSize + 1;
    variable.exists = 0;

    this->ram.seek(location);
    this->ram.write((char *)(&variable), HeaderSize);
    this->ram.flush();
}

uint16_t SRam::read(char *name, uint16_t pid, uint32_t pos, char *buffer,
                    uint16_t size)
{
    memoryBlockHeader variable = this->findVariable(name, pid);
    if (variable.exists == 0)
    {
        return 0;
    }
    if (pos > variable.size)
    {
        return 0;
    }
    if (pos + size > variable.size)
    {
        size = variable.size - pos;
    }
    bool seek_check = this->ram.seek(variable.position);
    if (!seek_check)
    {
        return 0;
    }
    return this->ram.readBytes(buffer, size);
}

uint16_t SRam::readAll(char *name, uint16_t pid, char *buffer)
{
    memoryBlockHeader variable = this->findVariable(name, pid);
    return this->read(name, pid, 0, buffer, variable.size);
}

uint16_t SRam::write(char *name, uint16_t pid, uint32_t pos, char *data,
                     uint16_t size)
{
    memoryBlockHeader variable = this->findVariable(name, pid);
    if (variable.exists == 0)
    {
        return 0;
    }
    if (size + pos > variable.size)
    {
        return 0;
    }
    bool seek_check = this->ram.seek(variable.position + pos);
    if (!seek_check)
    {
        return 0;
    }
    uint16_t size_w = this->ram.write(data, size);
    this->ram.flush();
    return size_w;
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
    char temp[16];
    if (text[0] == '"' && text[strlen(text)-1] == '"') {
        memset(back, 0, strlen(text) - 2);
        uint8_t p = 0;
        for (int i=1; i < strlen(text)-1; i++){
            back[p++] = text[i];
        }
        return TYPE_BYTE;
    }

    if (strlen(text) > 0 && isdigit(text[0])) {
        double x = atof(text);
        memset(back, 0, 4);
        memcpy(back, dtoc(x), sizeof(double));
        // back = dtoc(atof(text));
        return TYPE_NUM;
    }

    memoryBlockHeader m = this->findVariable(text, pid);

    if (!m.exists) {
        return -1;
    }

    if (m.type == TYPE_CHAR || m.type == TYPE_BYTE || m.type == TYPE_NUM) {
        uint16_t from = 0;
        uint16_t read_size = m.size;

        if (strstr(text, "[") != NULL && strstr(text, "]") != NULL) {
            char *target = NULL;
            char *start, *end;
            start = strstr(text, "[");
            if (start) {
                start += 1;
                end = strstr(start, "]");
                if (end) {
                    target = (char *)malloc(end - start + 1);
                    memcpy(target, start, end-start);
                    target[end-start] = '\0';
                }
            }
            if (argc(target, ':') == 1) {
                from = atol(target);
            }
            if (argc(target, ':') == 2) {
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

        memset(back, 0, read_size);
        this->read(text, pid, from, back, read_size);
        if (m.type == TYPE_NUM) {
            return TYPE_NUM;
        }
        return TYPE_BYTE;
    }
    return -1;
}

void SRam::dump()
{
    // Idea!
    // PID | VARNAME | USED | POSITION | SIZE | VARIABLE-DATA | PID | VARNAME |
    // USED... Header followed by the value. then the next variable header.
    memoryBlockHeader variable;
    this->ram.seek(0);
    if (!this->ram) {
        Serial.println("Memory is not accessable");
    }
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
    }
}
