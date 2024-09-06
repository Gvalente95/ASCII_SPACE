#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define calc_width 15
#define calc_height 8
#define BUFF_SIZE 50

int is_numeric(int a)
{
    return (a >= '0' && a <= '9' ? 1 : 0);
}

int is_valid_operand(char opr)
{
    if (opr == '+' || opr == '-' || opr == '/' || opr == '%' || opr == '*')
        return (1);
    return (0);
}

int handle_operation(char *c)
{
    int size = strlen(c) - 1;
    int cur_value;
    long int res = 0;
    int i = 0;
    int start_len = 0;
    char operand = '\0';

    if (c[0] == '-')
    {
        i++;
        start_len++;
    }

    while (i < size && c[i] != '\0')
    {
        if (i > start_len && is_valid_operand(c[i]))
            operand = c[i++];
        else if (i > 0 && !is_numeric(c[i]))
        {
            printf("Invalid operand: %c at index: %d, SIZE: %d\n", c[i], i, size);
            return 0;
        }
        cur_value = 0;
        while (is_numeric(c[i]))
        {
            cur_value = cur_value * 10 + (c[i] - '0');
            i++;
        }
        if (operand == '\0')
            res = (start_len == 0) ? cur_value : -cur_value;
        else
        {
            if (operand == '-')
                res -= cur_value;
            else if (operand == '*')
                res *= cur_value;
            else if (operand == '/')
            {
                if (cur_value == 0)
                {
                    printf("ERROR: Division by zero at index: %d\n", i);
                    return 0;
                }
                res /= cur_value;
            }
            else if (operand == '+')
                res += cur_value;
            operand = '\0';
        }
    }

    printf("Result: %ld\n", res);
    return res;
}

int main(int argc, char **argv)
{
    char buffer[BUFF_SIZE];
    for (int i = 0; i < BUFF_SIZE; i++)
        buffer[i] = '\0';
    if (fgets(buffer, sizeof(buffer), stdin) != NULL)
    {
        handle_operation(buffer);
    }
    return (0);
}

void draw_calc()
{
    int x = 0;
    int y = 0;

    while (y < calc_height)
    {
        x = 0;
        while (x < calc_width)
        {
            if ((x == 0 || x == calc_width - 1) && (y == 0 || y == calc_height - 1))
                printf("X");
            else if (x == 0 || x == calc_width - 1)
                printf("|");
            else if (y == 0 || y == 3 || y == calc_height - 1)
                printf("-");
            else
                printf(" ");
            x++;
        }
        printf("\n");
        y++;
    }
}