#include <stdio.h>
#include <string.h>



int main(int argc, char* argv[])
{
int totallength = 0;
for (int i = 0; i < argc ; ++i)
  {
    int spacebetween = 20 - strlen(argv[i]);
    int numberofchar = strlen(argv[i]);
    totallength += numberofchar;
    printf("%s", argv[i]);
    printf("%*d\n",spacebetween,numberofchar);
  }
  int spacebetween = 20 - strlen("Total length");
  printf("Total length%*d\n",spacebetween,totallength);
  spacebetween = 20 - strlen("Average length");
  printf("Average length%*.2f\n",spacebetween,((float)totallength/(float)argc));
  
return totallength;
}


