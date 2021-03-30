#include "statements.h"
#include <freemem.h>
void b(int n)
{
    Serial.println(n);
    while (1)
    {
    }
}
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
    uint8_t ll = strlen(c->buffer) + 2;
    char *rest_buffer = (char *)malloc(ll);
    char *val_buffer;
    memset(rest_buffer, 0, ll);

    rest(c->buffer, strlen("sprint") + 1, rest_buffer);
    int n = c->memory->get_var_size(rest_buffer, c->pid) + 2;
    val_buffer = (char *)malloc(n);
    memset(val_buffer, 0, n);
    int type = c->memory->get_var(rest_buffer, c->pid, val_buffer);
    if (type == TYPE_NUM)
    {
        Serial.print(ctod(val_buffer));
    }
    else
    {
        Serial.print(val_buffer);
    }
    free(rest_buffer);
    free(val_buffer);
    return 0;
}

int m_sprintln(context *c)
{
    uint8_t ll = strlen(c->buffer) + 2;
    char *rest_buffer = (char *)malloc(ll);
    char *val_buffer;
    memset(rest_buffer, 0, ll);

    rest(c->buffer, strlen("sprintln") + 1, rest_buffer);

    int n = c->memory->get_var_size(rest_buffer, c->pid) + 2;
    val_buffer = (char *)malloc(n);
    memset(val_buffer, 0, n);
    int type = c->memory->get_var(rest_buffer, c->pid, val_buffer);
    if (type == TYPE_NUM)
    {
        Serial.println(ctod(val_buffer));
    }
    else
    {
        Serial.println(val_buffer);
    }
    free(rest_buffer);
    free(val_buffer);
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
    int ll = strlen(c->buffer) + 2;
    char *varname = (char *)malloc(ll);
    char *rest_str = (char *)malloc(ll);
    char *val;

    memset(varname, 0, ll);
    memset(rest_str, 0, ll);

    extract(c->buffer, ' ', 1, varname);
    rest(c->buffer, strlen(varname) + 8, rest_str);

    int size = c->memory->get_var_size(rest_str, c->pid) + 2;
    val = (char *)malloc(size);
    memset(val, 0, size);

    int type = c->memory->get_var(rest_str, c->pid, val);
    free(rest_str);
    if (type == TYPE_NUM)
    {
        double d = ctod(val);
        c->memory->write(varname, c->pid, 0, dtoc(d), sizeof(double));
    }
    else
    {
        c->memory->write(varname, c->pid, 0, val, sizeof(val));
    }

    free(varname);
    free(val);

    return 0;
}

int math_command(context *c, double *numbers)
{
    int ll = strlen(c->buffer) + 2;
    char *var_1;
    char *var_2;
    char *var_3;

    if (argc(c->buffer, ' ') < 4)
    {
        return -1;
    }

    var_3 = (char *)malloc(ll);
    memset(var_3, 0, ll);

    extract(c->buffer, ' ', 3, var_3);
    memoryBlockHeader *m = c->memory->findVariable(var_3, c->pid);

    if (m == NULL)
    {
        free(var_3);
        return -1;
    }

    free(m);
    var_1 = (char *)malloc(ll);
    var_2 = (char *)malloc(ll);
    memset(var_1, 0, ll);
    memset(var_2, 0, ll);

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
    free(var_1);
    free(var_2);
    free(var_3);

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

    int ll = strlen(c->buffer) + 2;

    int check = math_command(c, nums);

    if (check != 0)
    {
        return check;
    }

    char *var_3 = (char *)malloc(ll);
    memset(var_3, 0, ll);

    extract(c->buffer, ' ', 3, var_3);
    double resp = 0;

    resp = nums[0] + nums[1];
    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double));

    free(var_3);
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

    int ll = strlen(c->buffer) + 2;

    int check = math_command(c, nums);

    if (check != 0)
    {
        return check;
    }

    char *var_3 = (char *)malloc(ll);
    memset(var_3, 0, ll);

    extract(c->buffer, ' ', 3, var_3);
    double resp = 0;

    resp = nums[0] - nums[1];
    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double));

    free(var_3);
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

    int ll = strlen(c->buffer) + 2;

    int check = math_command(c, nums);

    if (check != 0)
    {
        return check;
    }

    char *var_3 = (char *)malloc(ll);
    memset(var_3, 0, ll);

    extract(c->buffer, ' ', 3, var_3);
    double resp = 0;

    resp = nums[0] * nums[1];
    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double));

    free(var_3);
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

    int ll = strlen(c->buffer) + 2;

    int check = math_command(c, nums);

    if (check != 0)
    {
        return check;
    }

    char *var_3 = (char *)malloc(ll);
    memset(var_3, 0, ll);

    extract(c->buffer, ' ', 3, var_3);
    double resp = 0;

    resp = nums[0] / nums[1];
    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double));

    free(var_3);
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
    int ll = strlen(c->buffer) + 2;
    char *pin_number_str = (char *)malloc(ll);
    char *mode = (char *)malloc(ll);

    char pn[4];
    memset(pn, 0, 4);
    memset(mode, 0, ll);
    memset(pin_number_str, 0, ll);

    extract(c->buffer, ' ', 1, pin_number_str);
    c->memory->get_var(pin_number_str, c->pid, pn);
    free(pin_number_str);

    int pin = int(ctod(pn));

    extract(c->buffer, ' ', 2, mode);

    if (strcmp(mode, "INPUT") == 0)
    {
        pinMode(pin, INPUT);
    }
    if (strcmp(mode, "OUTPUT") == 0)
    {
        pinMode(pin, OUTPUT);
    }
    if (strcmp(mode, "INPUT_PULLUP") == 0)
    {
        pinMode(pin, INPUT_PULLUP);
    }
    free(mode);
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
    int ll = strlen(c->buffer) + 2;

    char *pin_number_str = (char *)malloc(ll);
    char *mode = (char *)malloc(5);
    char pn[4];

    memset(pn, 0, 4);
    memset(mode, 0, 5);

    memset(pin_number_str, 0, ll);
    extract(c->buffer, ' ', 1, pin_number_str);
    c->memory->get_var(pin_number_str, c->pid, pn);
    free(pin_number_str);
    extract(c->buffer, ' ', 2, mode);

    if (strcmp(mode, "HIGH") == 0)
    {
        digitalWrite(int(ctod(pn)), HIGH);
    }
    if (strcmp(mode, "LOW") == 0)
    {
        digitalWrite(int(ctod(pn)), LOW);
    }
    free(mode);
    return 0;
}

int m_digitalread(context *c) 
{
    int ll = strlen(c->buffer) + 2;

    char *pin_number_str = (char *)malloc(ll);
    char *var_name = (char *)malloc(ll);
    char pn[4];

    memset(pn, 0, 4);
    memset(var_name, 0, ll);

    memset(pin_number_str, 0, ll);
    extract(c->buffer, ' ', 1, pin_number_str);
    extract(c->buffer, ' ', 2, var_name);
    memoryBlockHeader * m = c->memory->findVariable(var_name, c->pid);
    if (m == NULL)
    {
        free(pin_number_str);
        free(var_name);
        return -1;
    }
    if (m->type != TYPE_NUM)
    {
        free(pin_number_str);
        free(var_name);
        return -1;
    }

    int pin = int(ctod(pn));
    c->memory->get_var(pin_number_str, c->pid, pn);
    int val = digitalRead(pin);
    free(pin_number_str);
    free(var_name);
    c->memory->write(var_name, c->pid, 0, dtoc(double(val)), sizeof(double));
    return 0;
}


int m_analogread(context *c) 
{
    int ll = strlen(c->buffer) + 2;

    char *pin_number_str = (char *)malloc(ll);
    char *var_name = (char *)malloc(ll);
    char pn[4];

    memset(pn, 0, 4);
    memset(var_name, 0, ll);

    memset(pin_number_str, 0, ll);
    extract(c->buffer, ' ', 1, pin_number_str);
    extract(c->buffer, ' ', 2, var_name);
    memoryBlockHeader * m = c->memory->findVariable(var_name, c->pid);
    if (m == NULL)
    {
        free(pin_number_str);
        free(var_name);
        return -1;
    }
    if (m->type != TYPE_NUM)
    {
        free(pin_number_str);
        free(var_name);
        return -1;
    }

    int pin = int(ctod(pn));
    c->memory->get_var(pin_number_str, c->pid, pn);
    int val = analogRead(pin);
    free(pin_number_str);
    free(var_name);
    c->memory->write(var_name, c->pid, 0, dtoc(double(val)), sizeof(double));
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
int m_analogwrite(context *c)
{
    int ll = strlen(c->buffer) + 2;

    char *pin_number_str = (char *)malloc(ll);
    char *value = (char *)malloc(ll);
    char pn[4];

    memset(pn, 0, 4);
    memset(value, 0, ll);

    memset(pin_number_str, 0, ll);
    extract(c->buffer, ' ', 1, pin_number_str);
    extract(c->buffer, ' ', 2, value);
    c->memory->get_var(pin_number_str, c->pid, pn);
    int pin = ctod(pn);
    int type = c->memory->get_var(value, c->pid, value);
    if (type != TYPE_NUM)
    {
        free(pin_number_str);
        free(value);
        return -1;
    }
    int val_int = int(ctod(value));
    free(pin_number_str);
    free(value);
    analogWrite(pin, val_int);
    return 0;
}

int m_delay(context *c)
{
    int ll = strlen(c->buffer) + 2;
    char *var_1 = (char *)malloc(ll);
    memset(var_1, 0, ll);
    extract(c->buffer, ' ', 1, var_1);
    char val[4];
    memset(val, 0, 4);
    c->memory->get_var(var_1, c->pid, val);
    free(var_1);
    double x = ctod(val);
    return int(x);
}

int m_inc(context *c)
{
    char varname[MaxNameLength];
    memset(varname, 0, MaxNameLength);
    extract(c->buffer, ' ', 1, varname);

    memoryBlockHeader *h = c->memory->findVariable(varname, c->pid);
    if (h == NULL)
    {
        return -1;
    }
    if (h->type != TYPE_NUM)
    {
        free(h);
        return 0;
    }

    char value[4];
    memset(value, 0, 4);
    c->memory->get_var(varname, c->pid, value);
    double dvalue = ctod(value);

    dvalue += 1;

    free(h);
    c->memory->write(varname, c->pid, 0, dtoc(dvalue), sizeof(double));
    return 0;
}

int m_jump(context *c)
{
    int ll = strlen(c->buffer) + 2;
    int vs = ll - 8;

    char precondition[8];

    char *first = (char *)malloc(vs);
    char *second = (char *)malloc(vs);
    char *label = (char *)malloc(vs);
    char *firstvar;
    char *secondvar;

    memset(first, 0, vs);
    memset(second, 0, vs);
    memset(label, 0, vs);
    memset(precondition, 0, 8);
    if (argc(c->buffer, ' ') == 5)
    {
        extract(c->buffer, ' ', 1, precondition);
        extract(c->buffer, ' ', 2, first);
        extract(c->buffer, ' ', 3, second);
        extract(c->buffer, ' ', 4, label);
        int fv_size = c->memory->get_var_size(first, c->pid);
        int sv_size = c->memory->get_var_size(second, c->pid);

        firstvar = (char *)malloc(fv_size);
        secondvar = (char *)malloc(sv_size);

        int v_type1 = c->memory->get_var(first, c->pid, firstvar);
        int v_type2 = c->memory->get_var(second, c->pid, secondvar);

        free(first);
        free(second);

        if (v_type1 != v_type2)
        {
            free(firstvar);
            free(secondvar);
            free(label);
            return -1;
        }

        if (strcmp(precondition, "equals") == 0)
        {
            if (v_type1 == TYPE_NUM)
            {
                if (abs(ctod(firstvar) - ctod(secondvar)) > 0.0001)
                {
                    free(firstvar);
                    free(secondvar);
                    free(label);
                    return 0;
                }
            }
            if (v_type1 == TYPE_BYTE)
            {
                if (strcmp(firstvar, secondvar) != 0)
                {
                    free(firstvar);
                    free(secondvar);
                    free(label);
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
                    free(firstvar);
                    free(secondvar);
                    free(label);
                    return 0;
                }
            }
            if (v_type1 == TYPE_BYTE)
            {
                if (strcmp(firstvar, secondvar) == 0)
                {
                    free(firstvar);
                    free(secondvar);
                    free(label);
                    return 0;
                }
            }
        }
        if (strcmp(precondition, "bigger") == 0)
        {
            if (v_type1 == TYPE_BYTE)
            {
                free(firstvar);
                free(secondvar);
                free(label);
                return 0;
            }
            if (v_type1 == TYPE_NUM)
            {
                if (ctod(firstvar) <= ctod(secondvar))
                {
                    free(firstvar);
                    free(secondvar);
                    free(label);
                    return 0;
                }
            }
        }
        if (strcmp(precondition, "smaller") == 0)
        {
            if (v_type1 == TYPE_BYTE)
            {
                free(firstvar);
                free(secondvar);
                free(label);
                return 0;
            }
            if (v_type1 == TYPE_NUM)
            {
                if (ctod(firstvar) >= ctod(secondvar))
                {
                    free(firstvar);
                    free(secondvar);
                    free(label);
                    return 0;
                }
            }
        }
        free(firstvar);
        free(secondvar);
    }
    else
    {
        free(first);
        free(second);
        extract(c->buffer, ' ', 1, label);
    }

    strcat(label, ":");

    uint32_t pos = c->script->position();
    c->script->seek(0);

    vs = strlen(label) + 2;
    first = (char *)malloc(vs);

    memset(first, 0, vs);
    while (c->script->available())
    {
        c->script->readBytesUntil('\n', first, vs);
        if (strcmp(first, label) == 0)
        {
            free(first);
            free(label);
            return 0;
        }
        memset(first, 0, vs);
    }
    c->script->seek(pos);
    free(first);
    free(label);
    return -1;
}
