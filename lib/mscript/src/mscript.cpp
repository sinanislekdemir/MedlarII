#include "mscript.h"

MScript::MScript()
{
    strcpy(this->statements[0].command, "sprint");
    this->statements[0].function = &m_sprint;

    strcpy(this->statements[1].command, "goto");
    this->statements[1].function = &m_goto;

    strcpy(this->statements[2].command, "jump");
    this->statements[2].function = &m_jump;

    strcpy(this->statements[3].command, "inc");
    this->statements[3].function = &m_inc;

    memset(this->scriptMeta.appname, 0, 32);
    memset(this->scriptMeta.author, 0, 32);
}

MScript::~MScript() {}

int MScript::open(uint16_t pid, char *filename)
{
    char nfilename[255];
    memset(nfilename, 0, 255);
    if (!SD.exists(filename))
    {
        return -1;
    }
    this->file = SD.open(filename, O_READ);
    if (!this->file)
    {
        return -1;
    }
    this->pid = pid;
    this->isOpen = true;
    strcpy(nfilename, filename);
    strcat(nfilename, "tx");
    this->memory.open(nfilename);
    this->prepare();
    return 0;
}

int MScript::close()
{
    if (this->isOpen)
    {
        this->file.close();
        this->finished = true;
        this->memory.close();
    }
    return 0;
}

void MScript::reset_buffer()
{
    memset(this->buffer, 0, MaxLineLength);
}

int MScript::read_meta()
{
    if (!this->isOpen)
    {
        return -1;
    }
    this->file.seek(0);
    char temp[MaxLineLength];
    this->reset_buffer();
    while (this->file.readBytesUntil('\n', this->buffer, MaxLineLength) > 0)
    {
        if (strcmp(buffer, M_MEMORY) == 0 || strcmp(buffer, M_CODE) == 0)
        {
            break;
        }
        if (strncmp(buffer, M_AUTHOR, strlen(M_AUTHOR)) == 0)
        {
            memset(temp, 0, MaxLineLength);
            rest(buffer, strlen(M_AUTHOR) + 1, temp);
            strcpy(this->scriptMeta.author, temp);
        }
        if (strncmp(buffer, M_APPNAME, strlen(M_APPNAME)) == 0)
        {
            memset(temp, 0, MaxLineLength);
            rest(buffer, strlen(M_APPNAME) + 1, temp);
            strcpy(this->scriptMeta.appname, temp);
        }
        this->reset_buffer();
    }
    Serial.print("Author: ");
    Serial.println(this->scriptMeta.author);
    Serial.print("App: ");
    Serial.println(this->scriptMeta.appname);
    return 0;
}

int MScript::read_memory()
{
    char temp[MaxLineLength];
    char name[MaxNameLength];
    uint16_t size;
    uint8_t type;
    this->file.seek(0);
    bool active = false;
    int status = 0;
    this->reset_buffer();
    memset(temp, 0, MaxLineLength);
    memset(name, 0, MaxNameLength);
    while (this->file.readBytesUntil('\n', this->buffer, MaxLineLength) > 0)
    {
        if (active)
        {
            memset(temp, 0, MaxLineLength);
            status = extract(buffer, ' ', 0, temp);
            if (status < 0)
            {
                return -1;
            }
            if (strcmp(temp, "char") == 0 || strcmp(temp, "byte") == 0)
            {
                if (argc(buffer, ' ') < 3)
                {
                    return -1;
                }
                type = TYPE_CHAR;
                if (strcmp(temp, "byte") == 0)
                {
                    type = TYPE_BYTE;
                }
                memset(name, 0, MaxNameLength);
                memset(temp, 0, MaxLineLength);
                extract(buffer, ' ', 1, name);
                extract(buffer, ' ', 2, temp);
                size = atoi(temp);
                this->memory.allocateVariable(name, this->pid, size, type);
            }
            if (strcmp(temp, "number") == 0)
            {
                if (argc(buffer, ' ') < 2)
                {
                    return -1;
                }
                type = TYPE_NUM;
                memset(name, 0, MaxNameLength);
                extract(buffer, ' ', 1, name);
                size = sizeof(double);
                if (argc(buffer, ' ') == 3)
                {
                    memset(temp, 0, MaxLineLength);
                    extract(buffer, ' ', 2, temp);
                    size = atoi(temp) * sizeof(double);
                }
                this->memory.allocateVariable(name, this->pid, size, type);
                this->memory.write(name, this->pid, 0, dtoc(0), sizeof(double));
            }
        }
        if (strncmp(buffer, M_MEMORY, strlen(M_MEMORY)) == 0)
        {
            active = true;
        }
        if (strncmp(buffer, M_CODE, strlen(M_CODE)) == 0)
        {
            active = false;
            break;
        }
        memset(buffer, 0, MaxLineLength);
    }
    return 0;
}

int MScript::prepare()
{
    // jump to the code block.
    if (!this->isOpen)
    {
        return -1;
    }
    this->read_meta();
    this->read_memory();
    this->finished = false;
    return 0;
}

int MScript::exec()
{
    context c;
    char command[MaxLineLength];

    c.buffer = this->buffer;
    c.memory = &this->memory;
    c.script = &this->file;
    c.pid = this->pid;
    memset(command, 0, MaxLineLength);

    extract(this->buffer, ' ', 0, command);
    for (uint8_t i = 0; i < STATEMENT_COUNT; i++)
    {
        if (strcmp(statements[i].command, command) == 0)
        {
            return statements[i].function(&c);
        }
    }
    return -1;
}

int MScript::step()
{
    // assuming we are at the `.code` block
    this->reset_buffer();
    size_t eol = this->file.readBytesUntil('\n', this->buffer, MaxLineLength);

    if (eol == 0)
    {
        this->finished = true;
        return 0;
    }

    this->exec();
    return 0;
}
