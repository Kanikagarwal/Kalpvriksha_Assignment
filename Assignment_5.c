#include <stdio.h>
#include <stdlib.h>

typedef struct Employee Employee;

typedef struct ReportNode
{
    Employee *employee;
    struct ReportNode *next;

} ReportNode;

struct Employee
{
    int empID;
    char name[50];
    char designation[50];
    Employee *manager;
    ReportNode *reports;
    Employee *next;
};
Employee *head = NULL;

Employee *findEmployee(int id)
{
    Employee *temp = head;
    while (temp != NULL)
    {
        if (temp->empID == id)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

void addReport(Employee *e, Employee *manager)
{
    if (manager == NULL)
        return;
    ReportNode *node = malloc(sizeof(ReportNode));
    node->employee = e;
    node->next = NULL;
    if (manager->reports == NULL)
    {
        manager->reports = node;
    }
    else
    {
        ReportNode *temp = manager->reports;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = node;
    }
}

void addEmployee()
{
    Employee *e = malloc(sizeof(Employee));
    int managerID;
    printf("Enter employee ID:\n");
    scanf("%d", &e->empID);
    if (findEmployee(e->empID) != NULL)
    {
        printf("Employee ID already exists\n");
        return;
    }
    printf("Enter employee name:\n");
    scanf(" %49[^\n]", e->name);
    printf("Enter employee designation:\n");
    scanf(" %49[^\n]", e->designation);
    printf("Enter employee manager ID (0 if no manager):\n");
    scanf("%d", &managerID);
    e->manager = NULL;
    if (managerID != 0)
    {
        e->manager = findEmployee(managerID);
    }
    e->reports = NULL;
    e->next = NULL;

    if (head == NULL)
    {
        head = e;
    }
    else
    {
        Employee *temp = head;
        while (temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = e;
    }

    addReport(e, e->manager);
    printf("Employee added successfully\n");
}

void removeReport(Employee *manager, Employee *e)
{
    if (manager == NULL || manager->reports == NULL)
        return;
    ReportNode *node = manager->reports;
    ReportNode *prev = NULL;
    while (node != NULL && node->employee != e)
    {
        prev = node;
        node = node->next;
    }
    if (node == NULL)
        return;
    if (prev == NULL)
    {
        manager->reports = node->next;
    }
    else
    {
        prev->next = node->next;
    }
}

int countReports(Employee *emp)
{
    int count = 0;
    ReportNode *node = emp->reports;
    while (node != NULL)
    {
        count++;
        node = node->next;
    }
    return count;
}

int countUnder(Employee *e)
{
    if (e == NULL)
        return 0;
    int count = 0;
    ReportNode *node = e->reports;
    while (node != NULL)
    {
        count += 1 + countUnder(node->employee);
        node = node->next;
    }
    return count;
}

void removeEmployee()
{
    int removeID;
    printf("Enter employee ID to be removed\n");
    scanf("%d", &removeID);
    Employee *e = findEmployee(removeID);
    if (e == NULL)
    {
        printf("Employee not found\n");
        return;
    }
    if (e->manager == NULL)
    {
        printf("CEO cannot be removed\n");
        return;
    }
    int minimum = 0;
    Employee *manager = e->manager;
    Employee *target = NULL;
    ReportNode *siblingNode = manager->reports;
    while (siblingNode != NULL)
    {
        Employee *sibling = siblingNode->employee;
        if (sibling != e)
        {
            int count = countReports(sibling);
            if (target == NULL || count < minimum)
            {
                target = sibling;
                minimum = count;
            }
        }
        siblingNode = siblingNode->next;
    }

    if (target == NULL)
    {
        target = manager;
    }

    while (e->reports != NULL)
    {
        ReportNode *node = e->reports;
        Employee *childNode = node->employee;
        e->reports = node->next;
        childNode->manager = target;
        addReport(childNode, target);
    }
    removeReport(manager, e);
    if (head == e)
    {
        head = e->next;
    }
    else
    {
        Employee *temp = head;
        while (temp != NULL && temp->next != e)
        {
            temp = temp->next;
        }
        if (temp != NULL)
        {
            temp->next = temp->next->next;
        }
    }
    printf("EMployee removed successfully\n");
}

void displayTree(Employee *e, int level)
{
    if (e == NULL)
        return;
    for (int i = 0; i < level; i++)
    {
        printf("    ");
    }
    printf("%d - %s - %s\n", e->empID, e->name, e->designation);
    ReportNode *reports = e->reports;
    while (reports != NULL)
    {
        displayTree(reports->employee, level + 1);
        reports = reports->next;
    }
}

void displayEmployeeHierarchy()
{
    int empID;
    printf("Emter ID to display\n");
    scanf("%d", &empID);
    Employee *e = findEmployee(empID);
    if (e == NULL)
    {
        printf("Employee not found\n");
        return;
    }
    printf("\n===== EMPLOYEE HIERARCHY =====\n");
    displayTree(e, 0);
    printf("Total employees under %d: %d\n", empID, countUnder(e));
}

void displayCompleteHierarchy()
{
    printf("\n===== ORGANIZATION HIERARCHY =====\n");
    int total = 0;
    Employee *temp = head;
    while (temp != NULL)
    {
        if (temp->manager == NULL)
        {
            displayTree(temp, 0);
            total += 1 + countUnder(temp);
        }
        temp = temp->next;
    }
    printf("Total Employees: %d\n", total);
}
int main()
{
    int choice;

    do
    {
        printf("\n===== EMPLOYEE HIERARCHY MANAGEMENT =====\n");
        printf("1. Add Employee\n");
        printf("2. Remove Employee\n");
        printf("3. Display Employee Hierarchy\n");
        printf("4. Display Complete Hierarchy\n");
        printf("5. Exit\n");
        printf("Enter your choice: ");
        // scanf("%d", &choice);
        if (scanf("%d", &choice) != 1)
        {
            printf("Invalid input. Enter a number.\n");
            return 1;
        }
        switch (choice)
        {
        case 1:
            addEmployee();
            break;
        case 2:
            removeEmployee();
            break;
        case 3:
            displayEmployeeHierarchy();
            break;
        case 4:
            displayCompleteHierarchy();
            break;
        case 5:
            printf("Exiting program\n");
            break;
        default:
            printf("Invalid input. Choose another input.\n");
        }
    } while (choice != 5);
    return 0;
}
