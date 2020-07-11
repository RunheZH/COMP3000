#include "def.h"

void launch()
{
  int choice;

  printf("Thank you for launching [APPNAME]. Please choose one of the following:\n");
  printf("1 - to view the list of websites currently blocked\n");
  printf("2 - to add website(s) to be blocked\n");
  printf("3 - to remove website(s) from the block list\n");

  promptForInt("\nPlease enter your selection #:", &choice);

  switch(choice) {
    case 1:  printAll(); 
      break;
    case 2:  addWebsite();
      break;
    case 3:  remWebsite();
      break;
    default: printf("Invalid choice.\n");
  }
}

void printAll()
{
  printf("printAll fc\n");

  FILE *eh;
  eh = fopen("/etc/hosts", "r");

  char buffer[MAX_STR];

  while (fgets(buffer, MAX_STR, eh)) {
    if(buffer[0] == '1' && buffer[10] == 'w')
    printf("%s", &buffer[10]);
  }

  fclose(eh);
}

void addWebsite()
{
  printf("addWebsite fc\n");

  FILE *fp;
  char web[MAX_STR];
  char a[MAX_STR];
  //char* a;
  strcpy(a, "127.0.0.1 ");
  fp = fopen("/etc/hosts", "a");
  //fp = fopen("webs.txt", "a");

  promptForString("Please enter the website address (www.website.com):", web);
  while(strcmp(web, "-1\n") != 0) {
    strcat(a, web);
    fputs(a, fp);
    //fputs(web, fp);
    promptForString("Please enter the website address (www.website.com):", web);
  }
  
  fclose(fp);
}

void remWebsite()
{
  printf("remWebsite fc\n");


  /*FILE *eh;
  eh = fopen("/etc/hosts", "w");


  while (fgets(buffer, MAX_STR, eh)) {
    if(buffer[0] == '1' && buffer[10] == 'w')

  }
*/

}

void promptForString(char *prompt, char *str)
{
  printf("%s ", prompt);
  readString(str);
}

void promptForInt(char *prompt, int* num)
{
  printf("%s ", prompt);
  readInt(num);
}

void readString(char *str)
{
  char tmpStr[MAX_STR];

  fgets(tmpStr, sizeof(tmpStr)+1, stdin);
  tmpStr[strlen(tmpStr)-1] = '\0';
  tmpStr[strlen(tmpStr)] = '\n';
  strcpy(str, tmpStr);
}

void readInt(int *x)
{
  char str[MAX_STR];

  readString(str);
  sscanf(str, "%d", x);
}

