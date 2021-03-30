#include "mscript.h"
#include "freemem.h"

MScript::MScript()
{
    memset(this->scriptMeta.appname, 0, 32);
    memset(this->scriptMeta.author, 0, 32);
    this->finished = true;
    this->sleep_start = 0;
    this->sleep_duration = 0;
}

// HERE ARE THE DEFS
statement statements[STATEMENT_COUNT];

MScript::~MScript() {}

void setup_statements()
{
    strcpy(statements[0].command, "sprint");
    statements[0].function = &m_sprint;

    strcpy(statements[1].command, "digitalWrite");
    statements[1].function = &m_digitalwrite;

    strcpy(statements[2].command, "jump");
    statements[2].function = &m_jump;

    strcpy(statements[3].command, "inc");
    statements[3].function = &m_inc;

    strcpy(statements[4].command, "equals");
    statements[4].function = &m_equals;

    strcpy(statements[5].command, "add");
    statements[5].function = &m_add;

    strcpy(statements[6].command, "delay");
    statements[6].function = &m_delay;

    strcpy(statements[7].command, "sprintln");
    statements[7].function = &m_sprintln;

    strcpy(statements[8].command, "sub");
    statements[8].function = &m_sub;

    strcpy(statements[9].command, "div");
    statements[9].function = &m_div;

    strcpy(statements[10].command, "mul");
    statements[10].function = &m_mul;

    strcpy(statements[11].command, "pinMode");
    statements[11].function = &m_pinmode;

    strcpy(statements[12].command, "digitalRead");
    statements[12].function = &m_digitalread;

    strcpy(statements[13].command, "analogRead");
    statements[13].function = &m_analogread;

    strcpy(statements[14].command, "analogWrite");
    statements[14].function = &m_analogwrite;
}

int MScript::open(uint16_t pid, char *filename)
{
    char *memory_filename;
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
    memory_filename = (char *)malloc(strlen(filename) + 3);
    memset(memory_filename, 0, strlen(filename) + 3);
    strcpy(memory_filename, filename);
    strcat(memory_filename, "tx");
    this->memory.open(memory_filename);
    free(memory_filename);
    this->prepare();

    this->finished = false;
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

uint8_t MScript::get_line_length()
{
    uint32_t pos = this->file.position();
    uint8_t length = 0;
    char c;
    while (this->file.available())
    {
        c = this->file.read();
        if (c == '\n')
        {
            this->file.seek(pos);
            if (length == 0)
            {
                return 0;
            }
            return length + 2;
        }
        length++;
    }
    return length;
}

int MScript::read_meta()
{
    uint8_t ll;
    if (!this->isOpen)
    {
        return -1;
    }
    this->file.seek(0);
    char *temp;
    while (this->file.available())
    {
        ll = this->get_line_length();
        if (ll == 0)
        {
            this->file.read();
            continue;
        }
        temp = (char *)malloc(ll);
        memset(temp, 0, ll);
        this->file.readBytesUntil('\n', temp, ll);

        if (strcmp(temp, M_MEMORY) == 0 || strcmp(temp, M_CODE) == 0)
        {
            free(temp);
            temp = NULL;
            break;
        }
        if (strncmp(temp, M_AUTHOR, strlen(M_AUTHOR)) == 0)
        {
            char *author = (char *)malloc(ll);
            memset(author, 0, ll);
            rest(temp, strlen(M_AUTHOR) + 1, author);
            strcpy(this->scriptMeta.author, author);
            free(author);
        }
        if (strncmp(temp, M_APPNAME, strlen(M_APPNAME)) == 0)
        {
            char *appname = (char *)malloc(ll);
            memset(appname, 0, ll);
            rest(temp, strlen(M_APPNAME) + 1, appname);
            strcpy(this->scriptMeta.appname, appname);
            free(appname);
        }
        free(temp);
    }
    return 0;
}

int MScript::read_memory()
{
    char *temp;
    uint16_t size;
    uint8_t type_int;
    bool active = false;
    int status = 0;

    this->file.seek(0);

    while (this->file.available())
    {
        uint8_t ll = this->get_line_length();
        if (ll == 0)
        {
            this->file.read();
            continue;
        }
        temp = (char *)malloc(ll);
        memset(temp, 0, ll);
        this->file.readBytesUntil('\n', temp, ll);
        if (active)
        {
            char *type = (char *)malloc(8);
            memset(type, 0, 8);
            status = extract(temp, ' ', 0, type);
            if (status < 0)
            {
                free(temp);
                free(type);
                continue;
            }
            if (strcmp(type, "char") == 0 || strcmp(type, "byte") == 0)
            {
                if (argc(temp, ' ') < 3)
                {
                    free(temp);
                    free(type);
                    return -1;
                }
                type_int = TYPE_CHAR;
                if (strcmp(type, "byte") == 0)
                {
                    type_int = TYPE_BYTE;
                }
                char *name = (char *)malloc(ll);
                char *size_str = (char *)malloc(ll);
                memset(name, 0, ll);
                memset(size_str, 0, ll);
                extract(temp, ' ', 1, name);
                extract(temp, ' ', 2, size_str);
                size = atoi(size_str);

                this->memory.allocateVariable(name, this->pid, size, type_int);
                free(name);
                free(size_str);
                free(temp);
                free(type);
                continue;
            }
            if (strcmp(type, "number") == 0)
            {
                if (argc(temp, ' ') < 2)
                {
                    free(temp);
                    free(type);
                    return -1;
                }
                type_int = TYPE_NUM;
                char *name = (char *)malloc(ll);
                char *size_str = (char *)malloc(ll);

                memset(name, 0, ll);
                memset(size_str, 0, ll);
                extract(temp, ' ', 1, name);
                size = sizeof(double);
                if (argc(temp, ' ') == 3)
                {
                    memset(size_str, 0, ll);
                    extract(temp, ' ', 2, size_str);
                    size = atoi(size_str) * sizeof(double);
                }

                this->memory.allocateVariable(name, this->pid, size, type_int);
                this->memory.write(name, this->pid, 0, dtoc(double(0)), sizeof(double));

                free(name);
                free(size_str);
                free(temp);
                free(type);
                continue;
            }
        }
        if (strncmp(temp, M_MEMORY, strlen(M_MEMORY)) == 0)
        {
            active = true;
        }
        if (strncmp(temp, M_CODE, strlen(M_CODE)) == 0)
        {
            active = false;
            free(temp);
            break;
        }
        free(temp);
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

int MScript::exec(char *cmd)
{
    int n = strlen(cmd) + 2;
    char *command = (char *)malloc(n);
    context c;
    c.buffer = cmd;
    c.memory = &this->memory;
    c.script = &this->file;
    c.pid = this->pid;
    memset(command, 0, n);
    extract(cmd, ' ', 0, command);

    for (uint8_t i = 0; i < STATEMENT_COUNT; i++)
    {
        if (strcmp(statements[i].command, command) == 0)
        {
            free(command);
            int res = statements[i].function(&c);
            if (res > 0)
            {
                this->sleep_start = millis();
                this->sleep_duration = (unsigned long)(res);
            }
            return res;
        }
    }
    free(command);
    return -1;
}

int MScript::step()
{
    // assuming we are at the `.code` block
    if (this->sleep_duration > 0)
    {
        unsigned long w = millis() - this->sleep_start;
        if (w >= this->sleep_duration)
        {
            this->sleep_duration = 0;
            this->sleep_start = 0;
        }
        else
        {
            return 0;
        }
    }
    if (!this->file.available())
    {
        this->finished = true;
        return 0;
    }
    uint8_t ll = this->get_line_length();
    if (ll == 0)
    {
        this->file.read();
        return 0;
    }
    char *buffer = (char *)malloc(ll);
    memset(buffer, 0, ll);
    this->file.readBytesUntil('\n', buffer, ll);
    this->exec(buffer);
    free(buffer);
    return 0;
}
