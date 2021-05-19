#include "statements.h"
#include <freemem.h>

jumper jumpers[JUMP_CACHE];
unsigned int jumper_index = 0;

void b(int n)
{
    Serial.println(n);
    while (1)
    {
    }
}

/**
 * @brief Open file for reading and writing
 * 
 *  @NOTE: this method creates the file if not exists
 *  @NOTE: you don't have to close the file
 *  @USAGE:
 * 
 *      .memory
 *      file testing
 * 
 *      .code
 *      open testing "somefile.txt"
 * 
 * @param c 
 * @return int 
 */
int m_fopen(context *c)
{
    int ll = strlen(c->buffer) + 2;
    char *varname = (char *)malloc(ll);
    char *rest_str = (char *)malloc(ll);

    memset(varname, 0, ll);
    memset(rest_str, 0, ll);

    extract(c->buffer, ' ', 1, varname);
    rest(c->buffer, strlen(varname) + 7, rest_str);

    memoryBlockHeader *m = c->memory->findVariable(varname, c->pid);
    if (m->type != TYPE_FILE)
    {
        free(m);
        free(varname);
        free(rest_str);
        return -1;
    }

    char *val = (char *)malloc(MAX_FILE_PATH);
    memset(val, '\0', MAX_FILE_PATH);

    c->memory->get_var(rest_str, c->pid, val);
    free(rest_str);
    c->memory->write(varname, c->pid, 0, val, strlen(val), true);
    free(varname);
    free(val);
    return 0;
}

/**
 * @brief Read file contents into string buffer
 * 
 * @USAGE: Read bytes 0 to 64 into buffer
 * 
 *      .memory
 *      file testing
 *      string buffer 65
 * 
 *      .code
 *      open testing "somefile.txt"
 *      fread testing 0 64 buffer
 * 
 * @param c 
 * @return int 
 */
int m_fread(context *c)
{
    int ll = strlen(c->buffer) + 2;
    char *varname = (char *)malloc(ll);
    char *position = (char *)malloc(ll);
    char *size = (char *)malloc(ll);
    char *destination = (char *)malloc(ll);

    memset(varname, '\0', ll);
    memset(position, '\0', ll);
    memset(size, '\0', ll);
    memset(destination, '\0', ll);

    extract(c->buffer, ' ', 1, varname);
    extract(c->buffer, ' ', 2, position);
    extract(c->buffer, ' ', 3, size);
    extract(c->buffer, ' ', 4, destination);

    char pos[4];
    memset(pos, 0, 4);
    c->memory->get_var(position, c->pid, pos);
    double posi = ctod(pos);
    free(position);

    char sizeb[4];
    memset(sizeb, 0, 4);
    c->memory->get_var(size, c->pid, sizeb);
    double sizei = ctod(sizeb);
    free(size);

    char *dest = (char *)malloc(int(sizei));
    memset(dest, 0, int(sizei));
    int check = c->memory->read(varname, c->pid, int(posi), dest, int(sizei), false);

    if (check <= 0)
    {
        return -1;
    }
    c->memory->write(destination, c->pid, 0, dest, int(sizei), false);
    free(destination);
    free(varname);
    free(dest);
    return 0;
}

/**
 * @brief Get the size of a file into a variable
 * 
 * @USAGE:
 * 
 *      .memory
 *      file testing
 *      number filesize
 * 
 *      .code
 *      open testing "somefile.txt"
 *      fsize testing filesize
 * 
 * @param c 
 * @return int 
 */

int m_fsize(context *c)
{
    int ll = strlen(c->buffer) + 2;
    char *varname = (char *)malloc(ll);
    char *destvarname = (char *)malloc(ll);

    memset(varname, 0, ll);
    memset(destvarname, 0, ll);

    extract(c->buffer, ' ', 1, varname);
    extract(c->buffer, ' ', 2, destvarname);

    memoryBlockHeader *m = c->memory->findVariable(varname, c->pid);
    if (!m)
    {
        free(varname);
        free(destvarname);
        return -1;
    }
    File f = c->memory->get_file(m);
    if (!f)
    {
        free(varname);
        free(destvarname);
        return -1;
    }
    double size = f.size();
    c->memory->write(destvarname, c->pid, 0, dtoc(size), 4, false);

    free(m);
    free(varname);
    free(destvarname);
    f.close();
    return 0;
}

/**
 * @brief Write string buffer into file
 * 
 * @USAGE: Write "hello world" to the beginning of the file
 * 
 *      .memory
 *      file testing
 * 
 *      .code
 *      fopen testing "somefile.txt"
 *      fwrite testing "hello world" 0
 * 
 * @param c 
 * @return int 
 */
int m_fwrite(context *c)
{
    int ll = strlen(c->buffer) + 2;
    char *varname = (char *)malloc(ll);
    char *data = (char *)malloc(ll);
    char *location = (char *)malloc(ll);

    memset(varname, 0, ll);
    memset(data, 0, ll);
    memset(location, 0, ll);

    extract(c->buffer, ' ', 1, varname);
    extract(c->buffer, ' ', 2, data);
    extract(c->buffer, ' ', 3, location);

    int size = c->memory->get_var_size(data, c->pid);
    char *val = (char *)malloc(size);
    memset(val, 0, size);

    char pos[4];
    c->memory->get_var(location, c->pid, pos);
    uint32_t posi = *reinterpret_cast<uint32_t *const>(pos);

    free(location);
    int check = c->memory->write(varname, c->pid, posi, val, size, false);

    free(varname);
    free(data);
    free(location);
    free(val);

    if (check <= 0)
    {
        return -1;
    }
    return 0;
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
int m_print(context *c)
{
    uint8_t ll = strlen(c->buffer) + 2;

    char *rest_buffer = (char *)malloc(ll);
    memset(rest_buffer, '\0', ll);

    extract(c->buffer, ' ', 1, rest_buffer);
    int n = c->memory->get_var_size(rest_buffer, c->pid);

    char *val_buffer = (char *)malloc(n);
    memset(val_buffer, '\0', n);

    int type = c->memory->get_var(rest_buffer, c->pid, val_buffer);
    free(rest_buffer);

    if (type == TYPE_NUM)
    {
        if (strncmp(c->buffer, "println", 7) == 0)
        {
            println_vga(ctod(val_buffer));
        }
        else if (strncmp(c->buffer, "sprintln", 8) == 0)
        {
            Serial.println(ctod(val_buffer));
        }
        else if (strncmp(c->buffer, "sprint ", 7) == 0)
        {
            Serial.print(ctod(val_buffer));
        }
        else
        {
            print_vga(ctod(val_buffer));
        }
    }
    else
    {
        if (strncmp(c->buffer, "println", 7) == 0)
        {
            println_vga(val_buffer);
        }
        else if (strncmp(c->buffer, "sprintln", 8) == 0)
        {
            Serial.println(val_buffer);
        }
        else if (strncmp(c->buffer, "usprint ", 7) == 0)
        {
            Serial.print(val_buffer);
        }
        else
        {
            print_vga(val_buffer);
        }
    }
    push_buffer();
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

    memset(varname, 0, ll);
    memset(rest_str, 0, ll);

    extract(c->buffer, ' ', 1, varname);
    rest(c->buffer, strlen(varname) + 8, rest_str);

    int size = c->memory->get_var_size(rest_str, c->pid);
    char *val = (char *)malloc(size);
    memset(val, 0, size);

    int type = c->memory->get_var(rest_str, c->pid, val);
    free(rest_str);

    if (type == TYPE_NUM)
    {
        double d = ctod(val);
        c->memory->write(varname, c->pid, 0, dtoc(d), sizeof(double), false);
    }
    else
    {
        c->memory->write(varname, c->pid, 0, val, size, false);
    }

    free(varname);
    free(val);

    return 0;
}

int math_command(context *c, double *numbers)
{
    int ll = strlen(c->buffer) + 2;

    if (argc(c->buffer, ' ') < 4)
    {
        return -1;
    }

    char *var_3;

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
    free(var_3);

    char *var_1 = (char *)malloc(ll);
    char *var_2 = (char *)malloc(ll);
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

    return 0;
}

/**
 * @brief Math operations
 *
 * @param c
 * @return int
 */
int m_math(context *c)
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

    if (strncmp(c->buffer, "add ", 4) == 0)
    {
        resp = nums[0] + nums[1];
    }
    if (strncmp(c->buffer, "sub ", 4) == 0)
    {
        resp = nums[0] - nums[1];
    }
    if (strncmp(c->buffer, "div ", 4) == 0)
    {
        resp = nums[0] / nums[1];
    }
    if (strncmp(c->buffer, "mul ", 4) == 0)
    {
        resp = nums[0] * nums[1];
    }
    if (strncmp(c->buffer, "mod ", 4) == 0)
    {
        resp = int(nums[0]) % int(nums[1]);
    }
    if (strncmp(c->buffer, "xor ", 4) == 0)
    {
        resp = int(nums[0]) ^ int(nums[1]);
    }
    if (strncmp(c->buffer, "and ", 4) == 0)
    {
        resp = nums[0] && nums[1];
    }
    if (strncmp(c->buffer, "or ", 3) == 0)
    {
        resp = int(nums[0]) || int(nums[1]);
    }

    c->memory->write(var_3, c->pid, 0, dtoc(resp), sizeof(double), false);
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
    memoryBlockHeader *m = c->memory->findVariable(var_name, c->pid);
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
    c->memory->write(var_name, c->pid, 0, dtoc(double(val)), sizeof(double), false);
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
    memoryBlockHeader *m = c->memory->findVariable(var_name, c->pid);
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
    c->memory->write(var_name, c->pid, 0, dtoc(double(val)), sizeof(double), false);
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
    int ll = strlen(c->buffer);
    char *varname = (char *)malloc(ll);
    memset(varname, 0, ll);

    extract(c->buffer, ' ', 1, varname);

    memoryBlockHeader *h = c->memory->findVariable(varname, c->pid);
    if (h == NULL)
    {
        free(varname);
        return -1;
    }
    if (h->type != TYPE_NUM)
    {
        free(varname);
        free(h);
        return 0;
    }

    char value[4];
    memset(value, 0, 4);
    c->memory->get_var(varname, c->pid, value);
    double dvalue = ctod(value);

    dvalue += 1;

    free(h);
    c->memory->write(varname, c->pid, 0, dtoc(dvalue), sizeof(double), false);
    free(varname);
    return 0;
}

int m_pixel(context *c)
{
    int xlen = extract_size(c->buffer, ' ', 1);
    int ylen = extract_size(c->buffer, ' ', 2);
    int clen = extract_size(c->buffer, ' ', 3);

    char *x = (char *)malloc(xlen);
    char *y = (char *)malloc(ylen);
    char *color = (char *)malloc(clen);

    memset(x, 0, xlen);
    memset(y, 0, ylen);
    memset(c, 0, clen);
    extract(c->buffer, ' ', 1, x);
    extract(c->buffer, ' ', 2, y);
    extract(c->buffer, ' ', 3, color);
    char xd[4] = {0, 0, 0, 0};
    char yd[4] = {0, 0, 0, 0};
    char cd[4] = {0, 0, 0, 0};
    c->memory->get_var(x, c->pid, xd);
    c->memory->get_var(y, c->pid, yd);
    c->memory->get_var(color, c->pid, cd);

    byte xi = uint8_t(ctod(xd));
    byte yi = uint8_t(ctod(yd));
    byte ci = uint8_t(ctod(cd));

    pixel(xi, yi, ci);

    free(x);
    free(y);
    free(color);

    return 0;
}

int m_clear(context *c)
{
    clear_display();
    return 0;
}

int m_jump(context *c)
{
    char *label;
    if (argc(c->buffer, ' ') == 2)
    {
        int lsize = extract_size(c->buffer, ' ', 1) + 2;
        label = (char *)malloc(lsize);
        extract(c->buffer, ' ', 1, label);
    }
    else
    {
        int fsize = extract_size(c->buffer, ' ', 2) + 1;
        int ssize = extract_size(c->buffer, ' ', 3) + 1;
        int lsize = extract_size(c->buffer, ' ', 4) + 2;
        char *precondition = (char *)malloc(8);
        char *first = (char *)malloc(fsize);
        char *second = (char *)malloc(ssize);
        label = (char *)malloc(lsize);

        memset(first, 0, fsize);
        memset(second, 0, ssize);
        memset(label, 0, lsize);
        memset(precondition, 0, 8);

        extract(c->buffer, ' ', 1, precondition);
        extract(c->buffer, ' ', 2, first);
        extract(c->buffer, ' ', 3, second);
        extract(c->buffer, ' ', 4, label);

        int fv_size = c->memory->get_var_size(first, c->pid);
        int sv_size = c->memory->get_var_size(second, c->pid);

        char *firstvar = (char *)malloc(fv_size);
        char *secondvar = (char *)malloc(sv_size);

        int v_type1 = c->memory->get_var(first, c->pid, firstvar);
        int v_type2 = c->memory->get_var(second, c->pid, secondvar);

        free(first);
        free(second);

        if (v_type1 != v_type2)
        {
            free(firstvar);
            free(secondvar);
            free(precondition);
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
                    free(precondition);
                    return 0;
                }
            }
            if (v_type1 == TYPE_BYTE || v_type1 == TYPE_CHAR)
            {
                if (strcmp(firstvar, secondvar) != 0)
                {
                    free(firstvar);
                    free(secondvar);
                    free(label);
                    free(precondition);
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
                    free(precondition);
                    return 0;
                }
            }
            if (v_type1 == TYPE_BYTE || v_type1 == TYPE_CHAR)
            {
                if (strcmp(firstvar, secondvar) == 0)
                {
                    free(firstvar);
                    free(secondvar);
                    free(label);
                    free(precondition);
                    return 0;
                }
            }
        }
        if (strcmp(precondition, "bigger") == 0)
        {
            if (v_type1 == TYPE_BYTE || v_type1 == TYPE_CHAR)
            {
                free(firstvar);
                free(secondvar);
                free(precondition);
                free(label);
                return 0;
            }
            if (v_type1 == TYPE_NUM)
            {
                if (ctod(firstvar) <= ctod(secondvar))
                {
                    free(firstvar);
                    free(secondvar);
                    free(precondition);
                    free(label);
                    return 0;
                }
            }
        }
        if (strcmp(precondition, "smaller") == 0)
        {
            if (v_type1 == TYPE_BYTE || v_type1 == TYPE_CHAR)
            {
                free(firstvar);
                free(secondvar);
                free(precondition);
                free(label);
                return 0;
            }
            if (v_type1 == TYPE_NUM)
            {
                if (ctod(firstvar) >= ctod(secondvar))
                {
                    free(firstvar);
                    free(secondvar);
                    free(precondition);
                    free(label);
                    return 0;
                }
            }
        }
        free(firstvar);
        free(secondvar);
        free(precondition);
    }

    get_jumper(c->script, label, c->pid);

    free(label);
    return 0;
}

int m_sread(context *c)
{
    char *varname = (char *)malloc(MaxNameLength);
    extract(c->buffer, ' ', 1, varname);
    memoryBlockHeader *m = c->memory->findVariable(varname, c->pid);
    if (!m)
    {
        free(varname);
        return -1;
    }
    c->back = (char *)malloc(MaxNameLength);
    memset(c->back, '\0', MaxNameLength);
    strcpy(c->back, varname);
    clear_input_buffer();
    free(varname);
    return 0;
}

int get_jumper(File *file, char *label, uint16_t pid)
{
    for (unsigned int i = 0; i < JUMP_CACHE; i++)
    {
        if (strcmp(label, jumpers[i].label) == 0 && jumpers[i].pid == pid)
        {
            file->seek(jumpers[i].location);
            return jumpers[i].location;
        }
    }
    int len = strlen(label) + 2;
    char *search = (char *)malloc(len);
    memset(search, 0, len);

    strcpy(search, label);
    strcat(search, ":");

    uint32_t pos = file->position();
    file->seek(0);

    char *buffer = (char *)malloc(len);

    memset(buffer, 0, len);
    while (file->available())
    {
        file->readBytesUntil('\n', buffer, len);
        if (strcmp(buffer, search) == 0)
        {
            free(buffer);
            free(search);

            // allocate space for the label name
            memset(jumpers[jumper_index].label, 0, 32);

            // set label name and location
            strcpy(jumpers[jumper_index].label, label);
            jumpers[jumper_index].location = file->position();
            jumpers[jumper_index].pid = pid;

            jumper_index = (jumper_index + 1) % 10;

            return file->position();
        }
        memset(buffer, 0, len);
    }
    free(buffer);
    free(search);
    file->seek(pos);
    return pos;
}
