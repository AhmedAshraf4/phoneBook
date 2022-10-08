#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* for clearing the console on any system */
#if defined(_POSIX_VERSION)   /* if on POSIX, use ANSI escapes */
    #define clrConsole() printf("\e[1;1H\e[2J")
#elif defined(_WIN32) || defined(_WIN64)  /* if on a windows system, use system("cls") */
    #define clrConsole() system("cls")
#elif defined(__linux__)    /* if on non POSIX linux, use system("clear") */
    #define clrConsole() system("clear")
#else   /* otherwise don't attempt to clear and use a simple line */
    #define clrConsole() printf("\n==================================================\n")
#endif

/*
 * the format in the file is as follows:
 * Steven,Thomas,10-06-1995,26 Elhoreya Street,01234567899,sthomas@gmail.com
 */

typedef struct {
    int day;
    int month;
    int year;
} Date;
typedef struct {
    Date dob;
    char firstname[20];
    char lastname[20];
    char address[60];
    char telephone[16];
    char email[50];
} Contact;

const int sizeContact = sizeof(Contact);
int count;
char fname[70];
FILE *fp = NULL;    /* fp is NULL if no file has been loaded by default to be used in validation checks. */
Contact *contacts;
Contact *alloctest; /* alloctest is used to realloc the contacts array safely in case of insufficient memory. */

/*
 * the scanline function is used to safely take user input while avoiding overflows
 * as well as cleaning the input buffer if extra characters were entered.
 */

void scanline(char * prompt, char * buff, size_t sz){
    char ch;
    if (prompt != NULL){
        printf("%s", prompt);
        fflush(stdout);
    }
    fgets(buff, sz, stdin);
    if (buff[strlen(buff)-1] != '\n'){
        while ((ch = getchar() != '\n') && (ch != EOF)){}
    }
    else{
        buff[strlen(buff)-1] = '\0';
    }
}

FILE *getfile(char fname[]){
    fp = fopen(fname, "r");
    return fp;
}

void load(){
    /*
     * get file name from user, load the file, count the number of lines,
     * malloc with the proper size using the number of lines, reset pointer location to 0,
     * then line by line add the contacts to the array.
     */
    char line[167]; /* the maximum file line length possible plus 1 for NULL at end of string is 167 */
    char tempdate[11];
    int i;
    count = 0;
    scanline("Enter the file name ('.back' to return to main menu): ", fname, 70);
    clrConsole();
    if (strcmp(fname, ".back")){
        fp = getfile(fname);
        if (fp != NULL){
            while(1){
                if (fgetc(fp) == EOF){ /* checks if file is empty */
                    break;
                }
                fseek(fp, -1l, SEEK_CUR); /* reset file pointer location to -1 of current position after fgetc */
                fgets(line, 167, fp); /* reads line */
                count++; /* add 1 per line */
            }
            contacts = malloc(sizeContact * count);
            fseek(fp, 0l, SEEK_SET); /* reset file pointer location to 0 */
            /* for loop to get each line and parse it */
            for(i=0; i<count; i++){
                fgets(line, 167, fp);
                strcpy(contacts[i].lastname, strtok(line, ","));
                strcpy(contacts[i].firstname, strtok(NULL, ","));
                strcpy(tempdate, strtok(NULL, ","));
                strcpy(contacts[i].address, strtok(NULL, ","));
                strcpy(contacts[i].telephone, strtok(NULL, ","));
                strcpy(contacts[i].email, strtok(NULL, ",\n"));
                contacts[i].dob.day = atoi(strtok(tempdate,"-"));
                contacts[i].dob.month = atoi(strtok(NULL,"-"));
                contacts[i].dob.year = atoi(strtok(NULL,"-"));
            }
            if (count==1){
                printf("Successfully loaded the file with 1 contact.\n");
            }
            else{
                printf("Successfully loaded the file with %d contacts.\n", count);
            }
        }
        else{
            printf("Could not read specified file.\n");
        }
    }
}

void printQuery(Contact *contacts, int index){
    printf("%s, %s, %s, %s\n", contacts[index].firstname, contacts[index].address, contacts[index].telephone, contacts[index].email);
}

void query(){
    /*
     * takes query from user,
     * loops through the contacts and prints the contact using printQuery if the query and contacts[i].lastname match
     */
    if (count){
        char query[20];
        int number=1;
        int flag=0, i;
        scanline("Enter last name ('.back' to return to main menu): ", query, 20);
        if (strcmp(query, ".back")){
            clrConsole();
            for(i=0; i<count; i++){
                if(!strcmp(query, contacts[i].lastname)){
                    printf("%d. ", number++);
                    printQuery(contacts, i);
                    flag = 1;
                }
            }
            if (!flag){
                printf("No contacts with the given last name found.\n");
            }
        }
        else{
            clrConsole();
        }
    }
    else{
        printf("There are no contacts in the file.\n");
    }
}

void addContact(){
    int atCount, periodCount, domainValid, i, j;
    int emailLen, telephoneLen, continueLoop;
    /*
     * the cancelAdd function is used to cancel the addition of a contact
     * by decreasing the count and reallocating the memory for the lower count
     */
    void cancelAdd(){
        clrConsole();
        count--;
        contacts = realloc(contacts, sizeContact * count);
    }
    if (!count){
        alloctest = malloc(sizeContact * (count+1));
    }
    else{
        alloctest = realloc(contacts, sizeContact * (count+1));
    }
    if (alloctest){
        contacts = alloctest;
        count++;
        scanline("Enter last name ('.back' to return to main menu): ", contacts[count-1].lastname, 20);
        if (!strcmp(contacts[count-1].lastname, ".back")){
            cancelAdd();
            return;
        }
        scanline("Enter first name: ", contacts[count-1].firstname, 20);
        while(1){
            continueLoop = 0;
            printf("Enter birth day: "); scanf("%d", &contacts[count-1].dob.day);
            while (getchar()!='\n'){
                continueLoop = 1;
            }
            if (continueLoop){
                printf("Invalid day entered.\n");
                continue;
            }
            if (31<contacts[count-1].dob.day || contacts[count-1].dob.day<1){
                printf("Invalid day entered.\n");
                continue;
            }
            break;
        }
        while(1){
            continueLoop = 0;
            printf("Enter birth month: "); scanf("%d", &contacts[count-1].dob.month);
            while (getchar()!='\n'){
                continueLoop = 1;
            }
            if (continueLoop){
                printf("Invalid month entered.\n");
                continue;
            }
            if (12<contacts[count-1].dob.month || contacts[count-1].dob.month<1){
                printf("Invalid month entered.\n");
                continue;
            }
            break;
        }
        while(1){
            continueLoop = 0;
            printf("Enter birth year: "); scanf("%d", &contacts[count-1].dob.year);
            while (getchar()!='\n'){
                continueLoop = 1;
            }
            if (continueLoop){
                printf("Invalid year entered.\n");
                continue;
            }
            if (contacts[count-1].dob.year < 1900 || contacts[count-1].dob.year > 2020){
                printf("Invalid year entered.\n");
                continue;
            }
            break;
        }
        scanline("Enter address: ", contacts[count-1].address, 60);
        while(1){
            continueLoop = 0;
            scanline("Enter telephone: ", contacts[count-1].telephone, 16);
            telephoneLen = strlen(contacts[count-1].telephone);
            for (i = 0; i< telephoneLen; i++){
                if (contacts[count-1].telephone[i] > '9' || contacts[count-1].telephone[i] < '0'){
                    printf("Invalid telephone number entered.\n");
                    continueLoop = 1;
                    break;
                }
            }
            if (continueLoop){
                continue;
            }
            break;
        }
        while(1){
            atCount = 0; periodCount = 0; domainValid = 0; continueLoop = 0;
            scanline("Enter email: ", contacts[count-1].email, 50);
            emailLen = strlen(contacts[count-1].email);
            for(j=0; atCount<2 && j<emailLen; j++){
                if (contacts[count-1].email[j] == ' '){
                    printf("Invalid email entered.\n");
                    continueLoop = 1;
                    break;
                }
                if (contacts[count-1].email[j] == '@'){
                    if (!j){
                        printf("Invalid email entered.\n");
                        continueLoop = 1;
                        break;
                    }
                    atCount++;
                    if (j<emailLen-3){
                        if (contacts[count-1].email[j+1] != '.'){
                            domainValid = 1;
                        }
                    }
                }
                if (atCount && contacts[count-1].email[j] == '.'){
                    periodCount++;
                }
            }
            if (continueLoop){
                continue;
            }
            if (atCount != 1 || !periodCount || contacts[count-1].email[emailLen-1] == '.' || !domainValid){
                printf("Invalid email entered.\n");
                continue;
            }
            break;
        }
        printf("The contact has been added.\n");
    }
    else{
        printf("Not enough memory for a new contact.\n");
    }
}

void deleteContact(){
    int i, j;
    if (count){
        int countdeduct=0;
        char deletelast[20];
        char deletefirst[20];
        scanline("Enter last name ('.back' to return to main menu): ", deletelast, 20);
        if (!strcmp(deletelast, ".back")){
            clrConsole();
            return;
        }
        scanline("Enter first name: ", deletefirst, 20);
        for(i=0; i<count-countdeduct; i++){
            if (!strcmp(deletelast, contacts[i].lastname) && !strcmp(deletefirst, contacts[i].firstname)){
                for(j=i; j<count-countdeduct-1; j++){
                    contacts[j] = contacts[j+1];
                }
                i--;
                countdeduct++;
            }
        }
        contacts = realloc(contacts, sizeContact * (count-countdeduct));
        if (countdeduct==1){
            printf("Deleted 1 contact.\n");
        }
        else{
            printf("Deleted %d contacts.\n", countdeduct);
        }
        count -= countdeduct;
    }
    else{
        printf("There are no contacts in the file.\n");
    }
}

void modify(){
    if (count){
        int atCount, periodCount, domainValid;
        int emailLen, telephoneLen, continueLoop;
        char modifylast[20];
        int number = 1;
        int choice, i, j;
        scanline("Enter last name ('.back' to return to main menu): ", modifylast, 20);
        if (!strcmp(modifylast, ".back")){
            clrConsole();
            return;
        }
        for(i=0; i<count; i++){
            if(!strcmp(modifylast, contacts[i].lastname)){
                printf("%d.", number++);
                printQuery(contacts, i);
            }
        }
        number--;
        if(number>1){
            while(1){
                printf("Enter the number of the contact you want to modify: ");
                scanf("%d", &choice);
                while (getchar()!='\n'){
                    choice = number + 1;
                }
                if (choice <= number && choice > 0){
                    number = 1;
                    for(i=0; i<count; i++){
                        if(!strcmp(modifylast, contacts[i].lastname)){
                            if (number == choice){
                                scanline("Enter last name: ", contacts[i].lastname, 20);
                                scanline("Enter first name: ", contacts[i].firstname, 20);
                                while(1){
                                    continueLoop = 0;
                                    printf("Enter birth day: "); scanf("%d", &contacts[i].dob.day);
                                    while (getchar()!='\n'){
                                        continueLoop = 1;
                                    }
                                    if (continueLoop){
                                        printf("Invalid day entered.\n");
                                        continue;
                                    }
                                    if (31<contacts[i].dob.day || contacts[i].dob.day<1){
                                        printf("Invalid day entered.\n");
                                        continue;
                                    }
                                    break;
                                }
                                while(1){
                                    continueLoop = 0;
                                    printf("Enter birth month: "); scanf("%d", &contacts[i].dob.month);
                                    while (getchar()!='\n'){
                                        continueLoop = 1;
                                    }
                                    if (continueLoop){
                                        printf("Invalid month entered.\n");
                                        continue;
                                    }
                                    if (12<contacts[i].dob.month || contacts[i].dob.month<1){
                                        printf("Invalid month entered.\n");
                                        continue;
                                    }
                                    break;
                                }
                                while(1){
                                    continueLoop = 0;
                                    printf("Enter birth year: "); scanf("%d", &contacts[i].dob.year);
                                    while (getchar()!='\n'){
                                        continueLoop = 1;
                                    }
                                    if (continueLoop){
                                        printf("Invalid year entered.\n");
                                        continue;
                                    }
                                    if (contacts[i].dob.year < 1900 || contacts[i].dob.year > 2020){
                                        printf("Invalid year entered.\n");
                                        continue;
                                    }
                                    break;
                                }

                                scanline("Enter address: ", contacts[i].address, 60);
                                while(1){
                                    continueLoop = 0;
                                    scanline("Enter telephone: ", contacts[i].telephone, 16);
                                    telephoneLen = strlen(contacts[i].telephone);
                                    for (j = 0; j< telephoneLen; j++){
                                        if (contacts[i].telephone[j] > '9' || contacts[i].telephone[j] < '0'){
                                            printf("Invalid telephone number entered.\n");
                                            continueLoop = 1;
                                            break;
                                        }
                                    }
                                    if (continueLoop){
                                        continue;
                                    }
                                    break;
                                }
                                while(1){
                                    atCount = 0; periodCount = 0; domainValid = 0; continueLoop = 0;
                                    scanline("Enter email: ", contacts[i].email, 50);
                                    emailLen = strlen(contacts[i].email);
                                    for(j=0; atCount<2 && j<emailLen; j++){
                                        if (contacts[i].email[j] == ' '){
                                            printf("Invalid email entered.\n");
                                            continueLoop = 1;
                                            break;
                                        }
                                        if (contacts[i].email[j] == '@'){
                                            if (!j){
                                                printf("Invalid email entered.\n");
                                                continueLoop = 1;
                                                break;
                                            }
                                            atCount++;
                                            if (j<emailLen-3){
                                                if (contacts[i].email[j+1] != '.'){
                                                    domainValid = 1;
                                                }
                                            }
                                        }
                                        if (atCount && contacts[i].email[j] == '.'){
                                            periodCount++;
                                        }
                                    }
                                    if (continueLoop){
                                        continue;
                                    }
                                    if (atCount != 1 || !periodCount || contacts[count-1].email[emailLen-1] == '.' || !domainValid){
                                        printf("Invalid email entered.\n");
                                        continue;
                                    }
                                    break;
                                }
                                break;
                            }
                            number++;
                        }
                    }
                    clrConsole();
                    printf("The contact has been modified.\n");
                    break;
                }
                else{
                    printf("Invalid choice entered.\n");
                    continue;
                }
            }
        }
        else if(number){
            for(i=0; i<count; i++){
                if(!strcmp(modifylast, contacts[i].lastname)){
                    scanline("Enter last name: ", contacts[i].lastname, 20);
                    scanline("Enter first name: ", contacts[i].firstname, 20);
                    while(1){
                        continueLoop = 0;
                        printf("Enter birth day: "); scanf("%d", &contacts[i].dob.day);
                        while (getchar()!='\n'){
                            continueLoop = 1;
                        }
                        if (continueLoop){
                            printf("Invalid day entered.\n");
                            continue;
                        }
                        if (31<contacts[i].dob.day || contacts[i].dob.day<1){
                            printf("Invalid day entered.\n");
                            continue;
                        }
                        break;
                    }
                    while(1){
                        continueLoop = 0;
                        printf("Enter birth month: "); scanf("%d", &contacts[i].dob.month);
                        while (getchar()!='\n'){
                            continueLoop = 1;
                        }
                        if (continueLoop){
                            printf("Invalid month entered.\n");
                            continue;
                        }
                        if (12<contacts[i].dob.month || contacts[i].dob.month<1){
                            printf("Invalid month entered.\n");
                            continue;
                        }
                        break;
                    }
                    while(1){
                        continueLoop = 0;
                        printf("Enter birth year: "); scanf("%d", &contacts[i].dob.year);
                        while (getchar()!='\n'){
                            continueLoop = 1;
                        }
                        if (continueLoop){
                            printf("Invalid year entered.\n");
                            continue;
                        }
                        if (contacts[i].dob.year < 1900 || contacts[i].dob.year > 2020){
                            printf("Invalid year entered.\n");
                            continue;
                        }
                        break;
                    }
                    scanline("Enter address: ", contacts[i].address, 60);
                    while(1){
                        continueLoop = 0;
                        scanline("Enter telephone: ", contacts[i].telephone, 16);
                        telephoneLen = strlen(contacts[i].telephone);
                        for (j = 0; j< telephoneLen; j++){
                            if (contacts[i].telephone[j] > '9' || contacts[i].telephone[j] < '0'){
                                printf("Invalid telephone number entered.\n");
                                continueLoop = 1;
                                break;
                            }
                        }
                        if (continueLoop){
                            continue;
                        }
                        break;
                    }
                    while(1){
                        atCount = 0; periodCount = 0; domainValid = 0; continueLoop = 0;
                        scanline("Enter email: ", contacts[i].email, 50);
                        emailLen = strlen(contacts[i].email);
                        for(j=0; atCount<2 && j<emailLen; j++){
                            if (contacts[i].email[j] == ' '){
                                printf("Invalid email entered.\n");
                                continueLoop = 1;
                                break;
                            }
                            if (contacts[i].email[j] == '@'){
                                if (!j){
                                    printf("Invalid email entered.\n");
                                    continueLoop = 1;
                                    break;
                                }
                                atCount++;
                                if (j<emailLen-3){
                                    if (contacts[i].email[j+1] != '.'){
                                        domainValid = 1;
                                    }
                                }
                            }
                            if (atCount && contacts[i].email[j] == '.'){
                                periodCount++;
                            }
                        }
                        if (continueLoop){
                            continue;
                        }
                        if (atCount != 1 || !periodCount || contacts[count-1].email[emailLen-1] == '.' || !domainValid){
                            printf("Invalid email entered.\n");
                            continue;
                        }
                        break;
                    }
                    break;
                }
            }
            clrConsole();
            printf("The contact has been modified.\n");
        }
        else{
            clrConsole();
            printf("No contacts found.\n");
            modify();
        }
    }
    else{
        printf("There are no contacts in the file.\n");
    }
}

 /*
  * the sortDate and sortLastName functions are sorting using
  * the bubble sort algorithm.
  */

void sortDate(){
    Contact temp;
    int date1, date2, flag = 1, j = 0, i;
    while(flag){
        flag = 0;
        for (i=0; i<count-j-1; i++){
            date1 = contacts[i].dob.day + contacts[i].dob.month * 100 + contacts[i].dob.year * 10000;
            date2 = contacts[i+1].dob.day + contacts[i+1].dob.month * 100 + contacts[i+1].dob.year * 10000;
            if (date1 > date2){
                temp = contacts[i];
                contacts[i] = contacts[i+1];
                contacts[i+1] = temp;
                flag = 1;
            }
        }
        j++;
    }
    printf("Sorted by date of birth successfully.\n");
}

void sortLastName(){
    Contact temp;
    int flag = 1, j = 0, i;
    while(flag){
        flag=0;
        for (i=0; i<count-j-1; i++){
            if (strcmp(contacts[i].lastname, contacts[i+1].lastname)==1){
                temp = contacts[i];
                contacts[i] = contacts[i+1];
                contacts[i+1] = temp;
                flag = 1;
            }
        }
        j++;
    }
    printf("Sorted by last name successfully.\n");
}

void printAll(){
    int i;
    for (i=0; i<count; i++){
        printf("%d. %s, %s, %02d-%02d-%d, %s, %s, %s\n",i+1, contacts[i].lastname, contacts[i].firstname, contacts[i].dob.day, contacts[i].dob.month, contacts[i].dob.year, contacts[i].address, contacts[i].telephone, contacts[i].email);
    }
}

void printSort(){
    if (count){
        int choice;
        printf("1. Sort by last name.\n2. Sort by date of birth.\n3. Return to main menu.\nEnter a choice (Default 3): ");
        scanf("%d", &choice);
        while (getchar()!='\n'){
            choice = 0;
        }
        switch (choice){
            case 1:
                clrConsole();
                sortLastName();
                printAll();
                break;
            case 2:
                clrConsole();
                sortDate();
                printAll();
                break;
            default:
                clrConsole();
        }
    }
    else{
        printf("There are no contacts in the file.\n");
    }
}

void save(){
    int i;
    if (fp!=1){ /* do not attempt to close the file if fp=1, this happens when the file has been saved once already */
        fclose(fp);
    }
    fp = fopen(fname, "w");
    for(i=0; i<count; i++){
        if (i==count-1){
            fprintf(fp, "%s,%s,%02d-%02d-%d,%s,%s,%s", contacts[i].lastname, contacts[i].firstname, contacts[i].dob.day, contacts[i].dob.month, contacts[i].dob.year, contacts[i].address, contacts[i].telephone, contacts[i].email);
        }
        else{
            fprintf(fp, "%s,%s,%02d-%02d-%d,%s,%s,%s\n", contacts[i].lastname, contacts[i].firstname, contacts[i].dob.day, contacts[i].dob.month, contacts[i].dob.year, contacts[i].address, contacts[i].telephone, contacts[i].email);
        }
    }
    fclose(fp); fp = 1; /* close the file to make sure the writes get flushed, and then fp = 1, as to not have the program think no file is loaded */
    printf("The file has been saved.\n");
}

void quit(){
    char confirm[2];
    printf("Are you sure you want to quit?\nAll unsaved changes will be discarded.\nEnter Y to confirm or N to cancel (default N): ");
    scanline(NULL, confirm, 2);
    if (confirm[0] == 'Y'){
        printf("Exiting.\n");
        exit(0);
    }
    clrConsole();
}

int main() {
    short int input;
    while(1){
        printf("Main menu:\n1.LOAD\t\t2.QUERY\t\t3.ADD\t\t4.DELETE\n5.MODIFY\t6.PRINT\t\t7.SAVE\t\t8.QUIT\nPlease pick an option: ");
        scanf("%hd", &input);
        while (getchar()!='\n'){
            input = 0;
        }
        clrConsole();
        if (fp==NULL && (input != 1 && input != 8)){
            printf("Please load a file first.\n");
            continue;
        }
        switch (input){
            case 1:
                load();
                break;
            case 2:
                query();
                break;
            case 3:
                addContact();
                break;
            case 4:
                deleteContact();
                break;
            case 5:
                modify();
                break;
            case 6:
                printSort();
                break;
            case 7:
                save();
                break;
            case 8:
                quit();
                break;
            default:
                printf("Enter a number between 1 and 8 from the menu.\n");
        }
    }
}