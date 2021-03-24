#include "statements.h"

/**
 * @brief Serial.println API
 * 
 * Usage:
 * 
 * sprint "hello world"
 * sprint text_variable
 * 
 * @param c 
 * @return int 
 */
int m_sprint(context *c)
{
    char rest_buffer[MaxLineLength];
    char val_buffer[MaxLineLength];
    memset(rest_buffer, 0, MaxLineLength);
    memset(val_buffer, 0, MaxLineLength);
    rest(c->buffer, strlen("sprint") + 1, rest_buffer);
    int type = c->memory->get_var(rest_buffer, c->pid, val_buffer);
    if (type == TYPE_NUM)
    {
        Serial.println("Print number:");
        Serial.print(ctod(val_buffer));
    }
    else
    {
        Serial.print(val_buffer);
    }
    return 0;
}

int m_sprintln(context *c)
{
    char rest_buffer[MaxLineLength];
    char val_buffer[MaxLineLength];
    memset(rest_buffer, 0, MaxLineLength);
    memset(val_buffer, 0, MaxLineLength);
    rest(c->buffer, strlen("sprintln") + 1, rest_buffer);
    int type = c->memory->get_var(rest_buffer, c->pid, val_buffer);
    if (type == TYPE_NUM)
    {
        Serial.println(ctod(val_buffer));
    }
    else
    {
        Serial.println(val_buffer);
    }
    return 0;
}

/**
 * @brief Set variable value
 * 
 * equals text_variable "hello world"
 * equals number_variable 123.33
 * 
 * @param c 
 * @return int 
 */
int m_equals(context *c)
{
    char varname[MaxNameLength];
    char buffer_rest[MaxLineLength];
    char buffer_val[MaxLineLength];
    memset(varname, 0, MaxNameLength);
    memset(buffer_rest, 0, MaxLineLength);
    memset(buffer_val, 0, MaxLineLength);
    extract(c->buffer, ' ', 1, varname);
    rest(c->buffer, strlen(varname) + 8, buffer_rest);
    int type = c->memory->get_var(buffer_rest, c->pid, buffer_val);
    if (type == TYPE_NUM) {
        double d = ctod(buffer_val);
        c->memory->write(varname, c->pid, 0, dtoc(d), sizeof(double));
    }else{
        c->memory->write(varname, c->pid, 0, buffer_val, sizeof(buffer_val));
    }
    return 0;
}

int math_command(context *c, double *numbers)
{
    char var_1[MaxLineLength];
    char var_2[MaxLineLength];
    char var_3[MaxLineLength];

    memset(var_1, 0, MaxLineLength);
    memset(var_2, 0, MaxLineLength);
    memset(var_3, 0, MaxLineLength);

    if (argc(c->buffer, ' ') < 4)
    {
        return -1;
    }
    extract(c->buffer, ' ', 3, var_3);
    memoryBlockHeader m = c->memory->findVariable(var_3, c->pid);

    if (!m.exists)
    {
        return -1;
    }
    extract(c->buffer, ' ', 1, var_1);
    extract(c->buffer, ' ', 2, var_2);
    char val_1_d[4];
    char val_2_d[4];
    memset(val_1_d, 0, 4);
    memset(val_2_d, 0, 4);

    c->memory->get_var(var_1, c->pid, val_1_d);
    c->memory->get_var(var_2, c->pid, val_2_d);
    numbers[0] = ctod(val_1_d);
    numbers[1] = ctod(val_2_d);
    return 0;
}

/**
 * @brief Add two numbers
 * 
 * add var_1 20 out_variable
 * 
 * @param c 
 * @return int 
 */
int m_add(context *c)
{
    double nums[2];

    int check = math_command(c, nums);
    if (check != 0)
    {
        return check;
    }

    char var_3[MaxLineLength];

    memset(var_3, 0, MaxLineLength);
    extract(c->buffer, ' ', 3, var_3);
    double resp = 0;

    resp = nums[0] + nums[1];
    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double));
    return 0;
}

/**
 * @brief Substract two numbers
 * 
 * sub var_1 20 out_variable
 * 
 * @param c 
 * @return int 
 */
int m_sub(context *c)
{
    double nums[2];

    int check = math_command(c, nums);
    if (check != 0)
    {
        return check;
    }

    char var_3[MaxLineLength];

    memset(var_3, 0, MaxLineLength);
    extract(c->buffer, ' ', 3, var_3);
    double resp = 0;

    resp = nums[0] - nums[1];
    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double));
    return 0;
}

/**
 * @brief Multiply two numbers
 * 
 * mul var_1 20 out_variable
 * 
 * @param c 
 * @return int 
 */
int m_mul(context *c)
{
    double nums[2];

    int check = math_command(c, nums);
    if (check != 0)
    {
        return check;
    }

    char var_3[MaxLineLength];

    memset(var_3, 0, MaxLineLength);
    extract(c->buffer, ' ', 3, var_3);
    double resp = 0;

    resp = nums[0] * nums[1];
    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double));
    return 0;
}

/**
 * @brief Divide two numbers
 * 
 * div var_1 20 out_variable
 * 
 * @param c 
 * @return int 
 */
int m_div(context *c)
{
    double nums[2];

    int check = math_command(c, nums);
    if (check != 0)
    {
        return check;
    }

    char var_3[MaxLineLength];

    memset(var_3, 0, MaxLineLength);
    extract(c->buffer, ' ', 3, var_3);
    double resp = 0;

    resp = nums[0] / nums[1];
    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double));
    return 0;
}

/**
 * @brief pinMode of Arduino
 * 
 * INPUT
 * OUTPUT
 * INPUT_PULLUP
 * 
 * @param c 
 * @return int 
 */
int m_pinmode(context *c)
{
    char pin_number_str[MaxLineLength];
    char mode[MaxLineLength];

    char pn[4];

    memset(pn, 0, 4);
    memset(mode, 0, MaxLineLength);

    memset(pin_number_str, 0, MaxLineLength);
    extract(c->buffer, ' ', 1, pin_number_str);
    c->memory->get_var(pin_number_str, c->pid, pn);

    int pin = int(ctod(pn));

    extract(c->buffer, ' ', 2, mode);

    if(strcmp(mode, "INPUT") == 0) {
        pinMode(pin, INPUT);
    }
    if (strcmp(mode, "OUTPUT") == 0) {
        pinMode(pin, OUTPUT);
    }
    if (strcmp(mode, "INPUT_PULLUP") == 0)
    {
        pinMode(pin, INPUT_PULLUP);
    }

    return 0;
}

/**
 * @brief digitalWrite of Arduino
 * 
 * HIGH
 * LOW
 * 
 * @param c 
 * @return int 
 */
int m_digitalwrite(context *c)
{
    char pin_number_str[MaxLineLength];
    char mode[MaxLineLength];
    char pn[4];

    memset(pn, 0, 4);
    memset(mode, 0, MaxLineLength);

    memset(pin_number_str, 0, MaxLineLength);
    extract(c->buffer, ' ', 1, pin_number_str);
    c->memory->get_var(pin_number_str, c->pid, pn);
    extract(c->buffer, ' ', 2, mode);

    if(strcmp(mode, "HIGH") == 0) {
        digitalWrite(int(ctod(pn)), HIGH);
    }
    if (strcmp(mode, "LOW") == 0) {
        digitalWrite(int(ctod(pn)), LOW);
    }

    return 0;
}

int m_delay(context *c)
{
    char var_1[MaxLineLength];
    memset(var_1, 0, MaxLineLength);
    extract(c->buffer, ' ', 1, var_1);
    char val[4];
    memset(val, 0, 4);
    c->memory->get_var(var_1, c->pid, val);
    delay(ctod(val));
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

    while (c->script->available() > 0)
    {
        c->script->readBytesUntil('\n', temp, MaxLineLength);
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
    while (c->script->available() > 0)
    {
        c->script->readBytesUntil('\n', first, MaxLineLength);
        if (strcmp(first, label) == 0)
        {
            return 0;
        }
        memset(first, 0, MaxLineLength);
    }
    c->script->seek(pos);
    return -1;
}
