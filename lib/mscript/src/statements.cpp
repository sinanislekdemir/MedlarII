#include "statements.h"

int m_sprint(context *c)
{
    char rest_buffer[MaxLineLength];
    char val_buffer[MaxLineLength];
    memset(rest_buffer, 0, MaxLineLength);
    memset(val_buffer, 0, MaxLineLength);
    rest(c->buffer, strlen("sprint")+1, rest_buffer);
    c->memory->get_var(rest_buffer, c->pid, val_buffer);
    Serial.println(val_buffer);
    return 0;
}

int m_goto(context *c)
{
    char temp[MaxLineLength];
    char label[MaxLineLength];
    memset(temp, 0, MaxLineLength);
    memset(label, 0, MaxLineLength);
    uint32_t pos = c->script->position();
    c->script->seek(0);
    rest(c->buffer, 5, label);
    strcat(label, ":");

    while (c->script->readBytesUntil('\n', temp, MaxLineLength) > 0)
    {
        if (strncmp(temp, label, strlen(label)) == 0)
        {
            return 0;
        }
    }

    c->script->seek(pos);
    return -1;
}

int m_inc(context *c)
{
    char varname[MaxNameLength];
    memset(varname, 0, MaxNameLength);
    extract(c->buffer, ' ', 1, varname);

    memoryBlockHeader h = c->memory->findVariable(varname, c->pid);
    if (!h.exists || h.type != TYPE_NUM)
    {
        return -1;
    }

    char value[4];
    c->memory->get_var(varname, c->pid, value);
    double dvalue = ctod(value);

    dvalue += 1;

    c->memory->write(varname, c->pid, 0, dtoc(dvalue), sizeof(double));
    return 0;
}

int m_jump(context *c)
{
    char precondition[8];
    char first[MaxLineLength];
    char second[MaxLineLength];
    char label[MaxLineLength];
    char firstvar[MaxLineLength];
    char secondvar[MaxLineLength];
    memset(first, 0, MaxLineLength);
    memset(second, 0, MaxLineLength);
    memset(label, 0, MaxLineLength);
    memset(precondition, 0, 8);
    memset(firstvar, 0, MaxLineLength);
    memset(secondvar, 0, MaxLineLength);

    if (argc(c->buffer, ' ') == 5)
    {
        extract(c->buffer, ' ', 1, precondition);
        extract(c->buffer, ' ', 2, first);
        extract(c->buffer, ' ', 3, second);
        extract(c->buffer, ' ', 4, label);
        int v_type1 = c->memory->get_var(first, c->pid, firstvar);
        int v_type2 = c->memory->get_var(second, c->pid, secondvar);
        if (v_type1 != v_type2)
        {
            return -1;
        }

        if (strcmp(precondition, "equals") == 0)
        {
            if (v_type1 == TYPE_NUM)
            {
                if (abs(ctod(firstvar) - ctod(secondvar)) > 0.0001)
                {
                    return 0;
                }
            }
            if (v_type1 == TYPE_BYTE)
            {
                if (strcmp(firstvar, secondvar) != 0)
                {
                    return 0;
                }
            }
        }
        if (strcmp(precondition, "not") == 0)
        {
            if (v_type1 == TYPE_NUM)
            {
                if (abs(ctod(firstvar) - ctod(secondvar)) < 0.0001)
                {
                    return 0;
                }
            }
            if (v_type1 == TYPE_BYTE)
            {
                if (strcmp(firstvar, secondvar) == 0)
                {
                    return 0;
                }
            }
        }
        if (strcmp(precondition, "bigger") == 0)
        {
            if (v_type1 == TYPE_BYTE)
            {
                return 0;
            }
            if (v_type1 == TYPE_NUM)
            {
                if (ctod(firstvar) <= ctod(secondvar))
                {
                    return 0;
                }
            }
        }
        if (strcmp(precondition, "smaller") == 0)
        {
            if (v_type1 == TYPE_BYTE)
            {
                return 0;
            }
            if (v_type1 == TYPE_NUM)
            {
                if (ctod(firstvar) >= ctod(secondvar))
                {
                    return 0;
                }
            }
        }
    }
    else
    {
        extract(c->buffer, ' ', 1, label);
    }
    strcat(label, ":");

    uint32_t pos = c->script->position();
    c->script->seek(0);
    memset(first, 0, MaxLineLength);
    while (c->script->readBytesUntil('\n', first, MaxLineLength) > 0)
    {
        if (strcmp(first, label) == 0)
        {
            return 0;
        }
        memset(first, 0, MaxLineLength);
    }
    c->script->seek(pos);
    return -1;
}
