#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char getGrade(double);
char *getPerformance(char);
void getRollList(int);
int getTotal(int, int, int);
double getAverage(int);

struct Student
{
    int roll;
    char name[50];
    int total;
    int m1;
    int m2;
    int m3;
    double average;
    char grade;
    char performance[6];
};

int main()
{
    int count = 0;
    struct Student st;
    int n;
    printf("Enter number of students:\n");
    scanf("%d", &n);
    if (n < 1 || n > 100)
    {
        printf("Number of students can be 1<= N <= 100\n");
        return 0;
    }

    struct Student *arr = malloc(n * sizeof(struct Student));
    for (int i = 0; i < n; i++)
    {
        printf("Enter data of student %d\n", i + 1);
        scanf("%d", &st.roll);
        if (st.roll < 1 || st.roll > 100)
        {
            printf("Roll numbers can be 1<= Roll Number <=100\n");
            return 0;
        }
        if (st.roll != count + 1)
        {
            printf("Enter roll numbers in systematic way\n");
            return 0;
        }
        else
            count++;
        scanf("%49s", st.name);
        scanf("%d", &st.m1);
        scanf("%d", &st.m2);
        scanf("%d", &st.m3);
        if (st.m1 < 0 || st.m1 > 100 || st.m2 < 0 || st.m2 > 100 || st.m3 < 0 || st.m3 > 100)
        {
            printf("Marks range can be 0<= Marks <=100\n");
            return 0;
        }
        arr[i] = st;
    }
    for (int i = 0; i < n; i++)
    {
        arr[i].total = getTotal(arr[i].m1, arr[i].m2, arr[i].m3);
        arr[i].average = getAverage(arr[i].total);
        arr[i].grade = getGrade(arr[i].average);
        if (arr[i].grade != 'F')
        {
            strcpy(arr[i].performance, getPerformance(arr[i].grade));
        }
    }

    for (int i = 0; i < n; i++)
    {
        printf("Roll: %d\n", arr[i].roll);
        printf("Name: %s\n", arr[i].name);
        printf("Total: %d\n", arr[i].total);
        printf("Average: %.2lf\n", arr[i].average);
        printf("Grade: %c\n", arr[i].grade);
        if (arr[i].grade == 'F')
            continue;
        printf("Performance: %s\n", arr[i].performance);
        printf("\n");
    }
    free(arr);

    printf("List of roll numbers (via recursion): ");
    getRollList(n);
    printf("\n");
    return 0;
}

int getTotal(int m1, int m2, int m3)
{
    return m1 + m2 + m3;
}

double getAverage(int total)
{
    return total / 3.0;
}

char getGrade(double avg)
{
    if (avg >= 85)
        return 'A';
    else if (avg >= 70)
        return 'B';
    else if (avg >= 50)
        return 'C';
    else if (avg >= 35)
        return 'D';
    else
        return 'F';
}

char *getPerformance(char grade)
{
    switch (grade)
    {
    case 'A':
        return "*****";
    case 'B':
        return "****";
    case 'C':
        return "***";
    case 'D':
        return "**";
    }
    return "";
}

void getRollList(int n)
{
    if (n == 0)
        return;
    getRollList(n - 1);
    printf("%d ", n);
}