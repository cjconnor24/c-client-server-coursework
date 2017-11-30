// Lab 4: menu.c - simple text-driven menu with switch

#include <stdio.h>
#include <string.h>

#define INPUTSIZ 10

void firstoption()
{
    printf("First option chosen...\n");
}

void secondoption()
{
    printf("Second option chosen...\n");
}

void thirdoption()
{
    printf("Third option chosen...\n");
}

void displaymenu()
{
    
	printf("0. Display menu\n");
	printf("(1.) Get Student Information \n");
    printf("2. Second option\n");
    printf("3. Third option\n");
    printf("4. Exit\n");
}

int getchoice(){

return 0;

}

int main(void)
{
    char input;
    char name[10];

    displaymenu();

    do {
	printf("option> ");
	fgets(name, INPUTSIZ, stdin);	
	name[strcspn(name, "\n")] = 0;
	input = name[0];
	if (strlen(name) > 1)
	    input = 'x';	

	switch (input) {
	case '0':
	    displaymenu();
	    break;
	case '1':
	printf("Get Student Info\n");
	    break;
	case '2':
	printf("Get Server Time\n");
	    break;
	case '3':
	    thirdoption();
	    break;
	case '4':
	    printf("Goodbye!\n");
	    break;
	default:
	    printf("Invalid choice - 0 displays options...!\n");
		displaymenu();
	    break;
	}
    } while (input != '4');

    return 0;

}
