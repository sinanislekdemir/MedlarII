#include "sram.h"

#include <Arduino.h>
#include <freemem.h>
#include <mdisplay.h>
#include <stdint.h>

SRam::SRam()
{
}

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

int extract_size(char *text, char delimiter, uint8_t part)
{
    int result = 0;
    int n = strlen(text);
    int counter = 0;
    bool ignore = false;
    for (uint8_t i = 0; i < n; i++)
    {
        if (text[i] == '"')
        {
            ignore = !ignore;
        }
        if (text[i] != delimiter)
        {
            if (counter == part)
            {
                result++;
            }
        }
        else
        {
            if (ignore && counter == part)
            {
                result++;
            }
        }
        if (text[i] == delimiter && !ignore)
        {
            counter++;
        }
    }

    return result;
}

// strtok should do fine but I need to keep "..." intact
int extract(char *text, char delimiter, uint8_t part, char *back)
{
    int n = strlen(text);
    int j;
    j = 0;
    int counter = 0;
    bool ignore = false;
    for (uint8_t i = 0; i < n; i++)
    {
        if (text[i] == '"')
        {
            ignore = !ignore;
        }
        if (text[i] != delimiter)
        {
            if (counter == part)
            {
                back[j++] = text[i];
            }
        }
        else
        {
            if (ignore && counter == part)
            {
                back[j++] = text[i];
            }
        }
        if (text[i] == delimiter && !ignore)
        {
            counter++;
        }
    }

    back[j] = '\0';

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
    back[j] = '\0';
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
    this->filename = (char *)malloc(strlen(filename) + 1);
    memcpy(this->filename, filename, strlen(filename));
    this->filename[strlen(filename)] = '\0';
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
        free(this->filename);
        this->ram.flush();
        this->ram.close();
    }
}

int SRam::ensureOpen()
{
    if (this->filename == NULL || strlen(this->filename) == 0)
    {
        return -1;
    }
    if (!this->ram)
    {
        this->ram = SD.open(this->filename, O_RDWR);
        if (!this->ram)
        {
            return -1;
        }
    }
    return 0;
}

File SRam::get_file(memoryBlockHeader *m)
{
    File r;
    if (!m || m->type != TYPE_FILE)
    {
        return r;
    }
    char *fname = (char *)malloc(MAX_FILE_PATH);
    memset(fname, '\0', MAX_FILE_PATH);

    this->read_all(m->varname, m->pid, fname, true);

    if (!SD.exists(fname))
    {
        r = SD.open(fname, FILE_WRITE);
        r.close();
    }
    r = SD.open(fname, O_RDWR);
    r.seek(0);
    free(fname);
    return r;
}

memoryBlockHeader *SRam::find_variable(char *name, uint16_t pid)
{
    // Idea!
    // PID | VARNAME | USED | POSITION | SIZE | VARIABLE-DATA | PID | VARNAME |
    // USED... Header followed by the value. then the next variable header.
    memoryBlockHeader *variable = (memoryBlockHeader *)malloc(HeaderSize);
    if (name[0] == '$')
    {
        variable->exists = true;
        variable->pid = pid;
        variable->position = 0;
        variable->size = sizeof(double);
        variable->type = TYPE_DOUBLE;
        return variable;
    }
    uint32_t pos = 0;
    variable->exists = 0;
    this->ensureOpen();

    this->ram.seek(0);
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

void SRam::allocate_variable(char *name, uint16_t pid, uint16_t variableSize, uint8_t variable_type)
{
    memoryBlockHeader variable;
    if (this->ensureOpen() == -1)
    {
        Serial.println("File not open");
        return;
    }
    this->ram.seek(this->ram.size());
    strcpy(variable.varname, name);
    variable.exists = 1;
    if (variable_type == TYPE_FILE)
    {
        variable.size = MAX_FILE_PATH;
    }
    else
    {
        variable.size = variableSize;
    }
    variable.pid = pid;
    variable.type = variable_type;
    this->ram.write((char *)(&variable), HeaderSize);
    this->ram.flush();
    // allocate extra space for the variable
    for (uint16_t i = 0; i < variable.size; i++)
    {
        this->ram.write("\0", 1);
        if (i % 128 == 0)
        {
            this->ram.flush();
        }
    }
    this->ram.flush();
}

void SRam::delete_variable(char *name, uint16_t pid)
{
    memoryBlockHeader *variable = this->find_variable(name, pid);
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
                    uint16_t size, bool raw)
{
    memoryBlockHeader *variable = this->find_variable(name, pid);
    if (variable == NULL)
    {
        return 0;
    }
    if (variable->type == TYPE_FILE && !raw)
    {
        File f = this->get_file(variable);
        if (!f)
        {
            free(variable);
            return 0;
        }
        if (pos > f.size())
        {
            free(variable);
            return 0;
        }
        f.seek(pos);
        if (f.available() == 0)
        {
            free(variable);
            return 0;
        }
        uint16_t size_read = f.readBytes(buffer, size);
        f.close();
        free(variable);
        return size_read;
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
    bool seek_check = this->ram.seek(variable->position + pos);
    if (!seek_check)
    {
        free(variable);
        return 0;
    }
    free(variable);
    return this->ram.readBytes(buffer, size);
}

uint16_t SRam::read_all(char *name, uint16_t pid, char *buffer, bool raw)
{
    memoryBlockHeader *variable = this->find_variable(name, pid);
    if (variable == NULL)
    {
        return 0;
    }
    uint16_t r = this->read(name, pid, 0, buffer, variable->size, raw);
    free(variable);
    return r;
}

uint16_t SRam::write(char *name, uint16_t pid, uint32_t pos, char *data,
                     uint16_t size, bool raw)
{
    if (name[0] == '$')
    {
        char rest[2] = "\0";
        strcpy(rest, name + 1);
        int index = atoi(rest);
        this->registers[index] = ctod(data);
        return sizeof(double);
    }
    memoryBlockHeader *variable = this->find_variable(name, pid);
    if (variable == NULL)
    {
        return -1;
    }
    if (variable->type == TYPE_FILE && !raw)
    {
        File f = this->get_file(variable);
        if (!f)
        {
            free(variable);
            return 0;
        }
        if (pos > f.size())
        {
            pos = f.size();
        }
        f.seek(pos);
        uint16_t size_write = f.write(data);
        f.close();
        free(variable);
        return size_write;
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

// sinan
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
    if (text[0] == '$')
    {
        return sizeof(double);
    }
    memoryBlockHeader *m;
    char *shortened = (char *)malloc(strlen(text));
    if (strstr(text, "[") != NULL && strstr(text, "]") != NULL)
    {
        memset(shortened, '\0', strlen(text));
        extract(text, '[', 0, shortened);
        m = this->find_variable(shortened, pid);
    }
    else
    {
        m = this->find_variable(text, pid);
    }

    if (m == NULL)
    {
        free(shortened);
        return -1;
    }

    if (m->type == TYPE_NUM)
    {
        free(m);
        free(shortened);
        return sizeof(double);
    }

    uint16_t read_size = m->size;

    if (strstr(text, "[") != NULL && strstr(text, "]") != NULL)
    {
        uint32_t end = this->get_end(text, pid);
        free(shortened);
        free(m);
        return end;
    }

    if (m->type == TYPE_FILE)
    {
        File f = this->get_file(m);
        if (!f)
        {
            return -1;
        }
        uint32_t s = f.size();
        f.close();
        free(shortened);
        free(m);
        return s;
    }
    free(shortened);
    free(m);

    return read_size;
}

uint32_t SRam::get_start(char *text, uint16_t pid)
{
    char temp[16];
    char back[4];
    int state = 0;
    int p = 0;

    for (uint8_t i = 0; i < strlen(text); i++)
    {
        if (text[i] == ':' || text[i] == ']')
        {
            break;
        }
        if (state == 1)
        {
            temp[p++] = text[i];
            temp[p] = '\0';
        }
        if (text[i] == '[')
        {
            state = 1;
        }
    }
    memset(back, 0, 4);
    this->get_var(temp, pid, back);

    return ctod(back);
}

uint32_t SRam::get_end(char *text, uint16_t pid)
{
    char temp[16];
    char back[4];
    int state = 0;
    int p = 0;

    for (uint8_t i = 0; i < strlen(text); i++)
    {
        if (text[i] == ']')
        {
            break;
        }
        if (state == 1)
        {
            temp[p++] = text[i];
            temp[p] = '\0';
        }
        if (text[i] == ':')
        {
            state = 1;
        }
    }
    memset(back, 0, 4);
    this->get_var(temp, pid, back);
    return ctod(back);
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
    if (text[0] == '$')
    {
        char rest[2] = "\0";
        strcpy(rest, text + 1);
        int index = atoi(rest);
        memcpy(back, dtoc(this->registers[index]), sizeof(double));
        return TYPE_NUM;
    }

    if (text[0] == '"' && text[strlen(text) - 1] == '"')
    {
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
        memcpy(back, dtoc(x), sizeof(double));
        return TYPE_NUM;
    }

    if (strcmp(text, "millis") == 0)
    {
        double x = millis();
        memset(back, 0, 4);
        memcpy(back, dtoc(x), sizeof(double));
        return TYPE_NUM;
    }

    char *shortened = (char *)malloc(strlen(text));
    bool partial = false;
    memset(shortened, 0, strlen(text));
    memoryBlockHeader *m;

    if (strstr(text, "[") != NULL && strstr(text, "]") != NULL)
    {
        extract(text, '[', 0, shortened);
        partial = true;
        m = this->find_variable(shortened, pid);
    }
    else
    {
        m = this->find_variable(text, pid);
    }

    if (m == NULL)
    {
        free(shortened);
        return -1;
    }

    uint16_t from = 0;
    uint16_t read_size = m->size;

    if (partial)
    {
        from = uint16_t(this->get_start(text, pid));
        read_size = uint16_t(this->get_end(text, pid));

        this->read(shortened, pid, from, back, read_size, false);
    }
    else
    {
        this->read(text, pid, from, back, read_size, false);
    }
    free(shortened);

    int ret = m->type;
    free(m);
    return ret;
}

void SRam::dump()
{
    // Idea!
    // PID | VARNAME | USED | POSITION | SIZE | VARIABLE-DATA | PID | VARNAME |
    // USED... Header followed by the value. then the next variable header.
    memoryBlockHeader variable;
    this->ram.seek(0);
    if (this->ensureOpen() == -1)
    {
        Serial.println("Memory is not accessable");
    }
    Serial.print("Filename: ");
    Serial.println(this->filename);
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
