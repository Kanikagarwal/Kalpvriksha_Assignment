#include <stdio.h>
#include<string.h>

const int STANDARD_PRICE = 150;
const int PREMIUM_PRICE = 250;
const int RECLINER_PRICE = 400;

struct Customer
{
    char name[50];
    int tickets;
    int ticketCategory;
};
int ticketPrice(int);
char *categoryName(int);
int main()
{
    struct Customer c;
    printf("Enter name of buyer:\n");
    fgets(c.name,sizeof(c.name),stdin);
    c.name[strcspn(c.name,"\n")] = '\0';
    printf("Enter number of tickets:\n");
    scanf("%d", &c.tickets);
    if (c.tickets < 1 || c.tickets > 20)
{
    printf("Invalid number of tickets. Enter between 1 and 20.\n");
    return 1;
}
    printf("Enter ticket category (1-Standard, 2-Premium, 3-Recliner):\n");
    scanf("%d", &c.ticketCategory);
    if(c.ticketCategory<1 || c.ticketCategory>3){
        printf("Not a valid category.\n");
        return 1;
    }

    int price = ticketPrice(c.ticketCategory);
    char *category = categoryName(c.ticketCategory);
    float totalPrice = c.tickets * price;
    float discount = 0;
    if (c.tickets >= 5)
    {
        discount = 0.1 * totalPrice;
    }
    float finalPrice = totalPrice - discount;
    // Output
    printf("Customer: %s\n", c.name);
    printf("Ticket Category: %s\n", category);
    printf("Tickets: %d\n", c.tickets);
    printf("Subtotal: %.2f\n", totalPrice);
    printf("Discount: %.2f\n", discount);
    printf("Final Bill: %.2f\n", finalPrice);
    return 0;
}

int ticketPrice(int category)
{
    switch (category)
    {
        case 1:
            return STANDARD_PRICE;
        case 2:
            return PREMIUM_PRICE;
        case 3:
            return RECLINER_PRICE;
        default:
            return 0;
    }
}

char *categoryName(int category)
{
    switch (category)
    {
    case 1:
        return "Standard";
    case 2:
        return "Premium";
    case 3:
        return "Recliner";
    default:
        return "Invalid";
    }
}