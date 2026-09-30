#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct User
{
    int id;
    char name[50];
    int age;
};
// Functions declarations for CRUD operations
void createUser();
void readUsers();
void updateUser();
void deleteUser();

int main()
{
    int choice;
    char input[100];
    char extra;
    do
    {
        // Display CRUD Menu and handle user choices
        printf("This is CRUD Menu. Select your choice.\n");
        printf("1 -> Create user\n");
        printf("2 -> Read users\n");
        printf("3 -> Update user\n");
        printf("4 -> Delete user\n");
        printf("5 -> Exit the program\n");

        printf("Enter your choice\n");
        fgets(input, sizeof(input), stdin);

        if (sscanf(input, "%d %c", &choice, &extra) != 1)
        {
            printf("Invalid input...\n");
            // while (getchar() != '\n');
            continue;
        }

        switch (choice)
        {
        case 1:
            createUser();
            break;
        case 2:
            readUsers();
            break;
        case 3:
            updateUser();
            break;
        case 4:
            deleteUser();
            break;
        case 5:
            printf("Exiting the program...\n");
            break;
        default:
            printf("Invalid choice!\n");
            break;
        }
    } while (choice != 5);

    return 0;
}

// Function to create user
void createUser()
{
    struct User user;
    struct User existingUser;
    FILE *file;
    FILE *check;
    char input[100];
    char extra;

    printf("Enter id: ");
    fgets(input, sizeof(input), stdin);

    if (sscanf(input, "%d %c", &user.id, &extra) != 1)
    {
        printf("Invalid ID. Please enter valid ID.\n");
        // while (getchar() != '\n');
        return;
    }
    if (user.id < 0)
    {
        printf("ID cannot be negative.\n");
        return;
    }

    check = fopen("users.txt", "r");

    if (check != NULL)
    {
        char line[100];

        while (fgets(line, sizeof(line), check))
        {
            if (sscanf(line, "%d", &existingUser.id) == 1)
            {
                if (user.id == existingUser.id)
                {
                    printf("User already exist. Please enter another ID.\n");
                    fclose(check);
                    return;
                }
            }
        }
        fclose(check);
    }

    printf("Enter name: ");
    fgets(user.name, sizeof(user.name), stdin);

    if (strchr(user.name, '\n') == NULL)
    {
        printf("Name cannot be more than 50 characters.\n");
        while (getchar() != '\n')
            ;
        return;
    }

    user.name[strcspn(user.name, "\n")] = '\0';

    if (user.name[0] == '\0')
    {
        printf("Name cannot be empty.\n");
        return;
    }

    printf("Enter age: ");
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d %c", &user.age, &extra) != 1)
    {
        printf("Invalid age. Please enter valid age.\n");
        // while (getchar() != '\n');
        return;
    }
    if (user.age < 0 || user.age > 150)
    {
        printf("Age cannot be negative or greater than 150.\n");
        return;
    }

    file = fopen("users.txt", "a");

    if (file == NULL)
    {
        printf("Error in opening file.");
        return;
    }

    fprintf(file, "%d %s %d\n", user.id, user.name, user.age);
    printf("User created successfully\n");
    fclose(file);
    return;
}

// Function to read users from file
void readUsers()
{
    struct User user;
    FILE *file;
    char line[100];
    file = fopen("users.txt", "r");
    if (file == NULL)
    {
        printf("Error in opeing file.");
        return;
    }
    while (fgets(line, sizeof(line), file))
    {
        printf("%s", line);
    }
    fclose(file);
    return;
}

// Function to update an user data using ID
void updateUser()
{
    struct User user;
    FILE *file;
    FILE *temp;

    char line[100];

    int searchId;
    int found = 0;
    printf("Enter ID to update: ");
    char input[100];
    char extra;
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d %c", &searchId, &extra) != 1)
    {
        printf("Invalid ID. Please enter valid ID.\n");
        return;
    }
    if(searchId<0){
        printf("ID cannot be negative.\n");
        return;
    }
    file = fopen("users.txt", "r");
    temp = fopen("temp.txt", "w");

    if (file == NULL || temp == NULL)
    {
        printf("Error in opening file");
        if (file != NULL)
            fclose(file);
        if (temp != NULL)
            fclose(temp);
        return;
    }

    while (fgets(line, sizeof(line), file))
    {
        if (sscanf(line, "%d", &user.id) != 1)
        {
            continue;
        }
        if (user.id == searchId)
        {
            found = 1;
            printf("Enter new user name:\n");
            fgets(user.name, sizeof(user.name), stdin);
            if (strchr(user.name, '\n') == NULL)
            {
                printf("Name cannot be more than 50 characters\n");
                while (getchar() != '\n');
                fclose(file);
                fclose(temp);
                remove("temp.txt");
                return;
            }
            user.name[strcspn(user.name, "\n")] = '\0';
            if (user.name[0] == '\0')
            {
                printf("Name cannot be empty.\n");
                fclose(file);
                fclose(temp);
                remove("temp.txt");
                return;
            }
            printf("Enter new age:\n");
            fgets(input, sizeof(input), stdin);
            if (sscanf(input, "%d %c", &user.age, &extra) != 1)
            {
                printf("Invalid age. Please enter valid age.\n");
                fclose(file);
                fclose(temp);
                remove("temp.txt");
                return;
            }
            if (user.age < 0 || user.age > 150)
            {
                printf("Age cannot be negative.\n");
                fclose(file);
                fclose(temp);
                remove("temp.txt");
                return;
            }
            fprintf(temp, "%d %s %d\n", user.id, user.name, user.age);
        }
        else
        {
            fprintf(temp, "%s", line);
        }
    }
    fclose(file);
    fclose(temp);

    if (remove("users.txt") != 0)
    {
        printf("Error deleting in old file.\n");
        return;
    }
    if (rename("temp.txt", "users.txt") != 0)
    {
        printf("Error in renaming file\n");
        return;
    }

    if (found == 1)
    {
        printf("User updated successfully\n");
    }
    if (found == 0)
    {
        printf("UserID not found\n");
    }
    return;
}

// Function to delete an user data using ID
void deleteUser()
{
    struct User user;
    FILE *file;
    FILE *temp;
    char line[100];
    int deleteId;
    int foundDelete = 0;
    printf("Enter ID to delete: ");
    char input[100];
    char extra;
    fgets(input, sizeof(input), stdin);
    if (sscanf(input, "%d %c", &deleteId, &extra) != 1)
    {
        printf("Invalid ID. Please enter valid ID.\n");
        return;
    }

    file = fopen("users.txt", "r");
    temp = fopen("temp.txt", "w");

    if (file == NULL || temp == NULL)
    {
        printf("Error in opening file\n");
        if (file != NULL)
            fclose(file);
        if (temp != NULL)
            fclose(temp);
        return;
    }

    while (fgets(line, sizeof(line), file))
    {

        if (sscanf(line, "%d", &user.id) != 1)
        {
            continue;
        }
        if (user.id == deleteId)
        {
            foundDelete = 1;
        }
        else
        {
            fprintf(temp, "%s", line);
        }
    }

    fclose(file);
    fclose(temp);

    if (remove("users.txt") != 0)
    {
        printf("Error deleting in old file");
        return;
    }
    if (rename("temp.txt", "users.txt") != 0)
    {
        printf("Error in renaming file");
        return;
    }

    if (foundDelete == 1)
    {
        printf("Record deleted successfully\n");
    }
    else
    {
        printf("Record not found\n");
    }
    return;
}