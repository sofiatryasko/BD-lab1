#include "src/CRUD.h"

CustomerMetaEntity* customerInput() {
    char firstName[64];
    char lastName[64];
    char address[128];
    int pk;

    printf("\bFirst name: ");
    scanf("%[^\n]", firstName);
    getchar();

    printf("\bLast name: ");
    scanf("%[^\n]", lastName);
    getchar();

    printf("\bAddress: ");
    scanf("%[^\n]", address);
    getchar();

    printf("\bPK: ");
    scanf("%d", &pk);
    getchar();

    Customer* customer = newCustomer(firstName, lastName, address);
    CustomerMetaEntity* customerMetaEntity = newCustomerMetaEntity(pk, *customer);

    free(customer);
    return customerMetaEntity;
}

ReviewMetaEntity* reviewInput() {
    char text[512];
    char createdData[11];
    int pk;
    int fk;

    printf("\bReview text: ");
    scanf("%[^\n]", text);
    getchar();

    printf("\bCreated date (yyyy-mm-dd): ");
    scanf("%s", createdData);
    getchar();

    printf("\bPK: ");
    scanf("%d", &pk);
    getchar();

    printf("\bFK: ");
    scanf("%d", &fk);
    getchar();

    Review* review = newReview(text, createdData);
    ReviewMetaEntity* reviewMetaEntity = newReviewMetaEntity(pk, fk, *review);

    free(review);
    return reviewMetaEntity;
}

int readNumber(const char* printText) {
    int pk;
    printf("%s", printText);
    scanf("%d", &pk);
    getchar();
    return pk;
}

void printResult(bool result) {
    if (result)
        printf("\bOperation was successfully done\n");
    else
        printf("\bOperation wasn't done (pk problem)\n");
}

char commands[11][15] = {
    "insert-m", "insert-s", "get-m", "get-s",
    "update-m", "update-s", "delete-m", "delete-s",
    "count-m", "count-s", "count-m-s"
};

void printAllCommands() {
    printf("Commands:\n");
    for (int i = 0; i < 11; ++i)
        printf("\b%d) %s\n", (i + 1), commands[i]);
}

void userInput() {
    char command[20];

    printAllCommands();

    while (true) {
        printf("Enter a command or \"quit\"\n");
        scanf("%s", command);
        getchar();

        if (strcmp(command, "quit") == 0) break;

        else if (strcmp(command, "get-m") == 0) {
            int pk = readNumber("\bInput master pk: ");
            CustomerMetaEntity customerMetaEntity;
            bool result = readM(&customerMetaEntity, pk, NULL, false);
            printResult(result);
            if (result)
                printCustomer(&customerMetaEntity);
        }
        else if (strcmp(command, "get-s") == 0) {
            int pk = readNumber("\bInput slave pk: ");
            ReviewMetaEntity reviewMetaEntity;
            bool result = readS(&reviewMetaEntity, pk, NULL);
            printResult(result);
            if (result)
                printReview(&reviewMetaEntity);
        }

        else if (strcmp(command, "insert-m") == 0) {
            CustomerMetaEntity* customerMetaEntity = customerInput();
            printResult(createM(customerMetaEntity));
            free(customerMetaEntity);
        }

        else if (strcmp(command, "insert-s") == 0) {
            ReviewMetaEntity* reviewMetaEntity = reviewInput();
            printResult(createS(reviewMetaEntity));
            free(reviewMetaEntity);
        }

        else if (strcmp(command, "update-m") == 0) {
            CustomerMetaEntity* customerMetaEntity = customerInput();
            printResult(updateM(customerMetaEntity->pk, customerMetaEntity));
            free(customerMetaEntity);
        }

        else if (strcmp(command, "update-s") == 0) {
            ReviewMetaEntity* reviewMetaEntity = reviewInput();
            printResult(updateS(reviewMetaEntity->pk, reviewMetaEntity));
            free(reviewMetaEntity);
        }

        else if (strcmp(command, "delete-m") == 0) {
            int pk = readNumber("\bInput master pk: ");
            printResult(deleteM(pk));
        }

        else if (strcmp(command, "delete-s") == 0) {
            int pk = readNumber("\bInput slave pk: ");
            printResult(deleteS(pk));
        }

        else if (strcmp(command, "count-m") == 0)
            printf("Number of customers: %d\n", countM());

        else if (strcmp(command, "count-s") == 0)
            printf("Number of reviews: %d\n", countS());

        else if (strcmp(command, "count-m-s") == 0) {
            int pk;
            printf("PK:");
            scanf("%d", &pk);
            printf("Number of reviews of customer with id %d: %d\n", pk, countSWhichDependFromM(pk));
        }

        else
            printf("\bUnknown command!\n");
    }
}

int main() {
    runDataBase();
    userInput();
    stopDataBase();

    return 0;
}