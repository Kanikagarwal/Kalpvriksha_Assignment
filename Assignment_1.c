#include <stdio.h>
#include <limits.h>

int evaluation(int, int, char);

// Convert a string to an integer with overflow validation
int convertToInt(char *str, int *num)
{
    int i = 0;
    long long value = 0;
    int sign = 1;

    if (str[0] == '-')
    {
        sign = -1;
        i++;
    }

    // There should be at least one digit
    if (str[i] == '\0')
    {
        printf("Error: Invalid expression.");
        return 0;
    }

    while (str[i] != '\0')
    {
        if (str[i] < '0' || str[i] > '9')
        {
            printf("Error: Invalid expression.");
            return 0;
        }

        value = value * 10 + (str[i] - '0');

        if (value > (long long)INT_MAX + 1)
        {
            printf("Error: Integer overflow");
            return 0;
        }

        i++;
    }

    value = value * sign;

    if (value > INT_MAX || value < INT_MIN)
    {
        printf("Error: Integer overflow");
        return 0;
    }

    *num = (int)value;

    return 1;
}

int main()
{
    // Taking expression as user input
    printf("Enter expression: ");

    char inputStr[100];
    fgets(inputStr, sizeof(inputStr), stdin);

    char str[100];
    int idx = 0;

    // Remove whitespace and validate allowed characters
    for (int m = 0; inputStr[m] != '\0'; m++)
    {
        if (inputStr[m] == ' ' || inputStr[m] == '\n')
        {
            continue;
        }
        else if ((inputStr[m] >= '0' && inputStr[m] <= '9') ||
                 inputStr[m] == '+' ||
                 inputStr[m] == '-' ||
                 inputStr[m] == '*' ||
                 inputStr[m] == '/')
        {
            str[idx++] = inputStr[m];
        }
        else
        {
            printf("Error: Invalid expression.");
            return 0;
        }
    }

    str[idx] = '\0';

    int expectedOpr = 0;

    for (int m = 0; str[m] != '\0'; m++)
    {
        if (!expectedOpr)
        {
            // Negative operand
            if (str[m] == '-')
            {
                // '-' must be followed by a digit
                if (str[m + 1] < '0' || str[m + 1] > '9')
                {
                    printf("Error: Invalid expression.");
                    return 0;
                }

                m++;
                expectedOpr = 1;
            }
            else if (str[m] >= '0' && str[m] <= '9')
            {
                expectedOpr = 1;
            }
            else
            {
                printf("Error: Invalid expression.");
                return 0;
            }
        }
        else
        {
            if (str[m] == '+' || str[m] == '-' ||
                str[m] == '*' || str[m] == '/')
            {
                expectedOpr = 0;
            }
            else if (str[m] >= '0' && str[m] <= '9')
            {
                continue;
            }
            else
            {
                printf("Error: Invalid expression.");
                return 0;
            }
        }
    }

    if (expectedOpr == 0)
    {
        printf("Error: Invalid expression.");
        return 0;
    }

    int left = 0;
    int right = 0;
    char opr;
    int i = 0;

    char leftStr[100];
    char rightStr[100];

    int result = 0;

    // Check if the expression contains only a single value
    int hasOpr = 0;

    for (int m = 1; str[m] != '\0'; m++)
    {
        if (str[m] == '+' || str[m] == '-' ||
            str[m] == '*' || str[m] == '/')
        {
            hasOpr = 1;
            break;
        }
    }

    if (hasOpr == 0)
    {
        int num;

        if (!convertToInt(str, &num))
            return 0;

        printf("%d", num);
        return 0;
    }

    
       //Find the first operator.

    i = 0;

    if (str[0] == '-')
    {
        i = 1;
    }

    while (str[i] != '\0')
    {
        if (str[i] == '+' || str[i] == '-' ||
            str[i] == '*' || str[i] == '/')
        {
            opr = str[i];
            break;
        }

        i++;
    }

    // Storing left operand
    for (int j = 0; j < i; j++)
    {
        leftStr[j] = str[j];
    }

    leftStr[i] = '\0';

    if (!convertToInt(leftStr, &left))
        return 0;

    i++;

    while (str[i] != '\0')
    {
        int l = i;
        int k = 0;

        if (str[i] == '-')
        {
            rightStr[k] = str[i];
            k++;
            i++;
            l=i;
        }

        while (str[i] != '\0' &&
               str[i] != '+' &&
               str[i] != '-' &&
               str[i] != '*' &&
               str[i] != '/')
        {
            i++;
        }

        for (int j = l; j < i; j++)
        {
            rightStr[k] = str[j];
            k++;
        }

        rightStr[k] = '\0';

        if (!convertToInt(rightStr, &right))
            return 0;

        if (opr == '*' || opr == '/')
        {
            left = evaluation(left, right, opr);
        }
        else if (opr == '+')
        {
            if ((left > 0 && result > INT_MAX - left) ||
                (left < 0 && result < INT_MIN - left))
            {
                printf("Error: Integer overflow");
                return 0;
            }

            result = result + left;
            left = right;
        }
        else
        {
            if ((left > 0 && result > INT_MAX - left) ||
                (left < 0 && result < INT_MIN - left))
            {
                printf("Error: Integer overflow");
                return 0;
            }

            result = result + left;
            left = -right;
        }

        // Storing next operator
        if (str[i] != '\0')
        {
            opr = str[i];
            i++;
        }
    }

    if ((left > 0 && result > INT_MAX - left) ||
        (left < 0 && result < INT_MIN - left))
    {
        printf("Error: Integer overflow");
        return 0;
    }

    result = result + left;

    printf("%d", result);

    return 0;
}

// Evaluate multiplication and division
int evaluation(int left, int right, char opr)
{
    int ans = 0;

    switch (opr)
    {
        case '*':
            if ((left > 0 && right > 0 && left > INT_MAX / right) ||
                (left < 0 && right < 0 && left < INT_MAX / right) ||
                (left > 0 && right < 0 && right < INT_MIN / left) ||
                (left < 0 && right > 0 && left < INT_MIN / right))
            {
                printf("Error: Integer overflow");
                return 0;
            }

            ans = left * right;
            break;

        case '/':
            if (right == 0)
            {
                printf("Error: Division by zero.\n");
                return 0;
            }

            if (left == INT_MIN && right == -1)
            {
                printf("Error: Integer overflow.\n");
                return 0;
            }

            ans = left / right;
            break;

        default:
            printf("Error: Invalid expression.");
            return 0;
    }

    return ans;
}

