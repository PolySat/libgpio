#include "gpioapi.h"
#include <ctype.h>

int main(int argc, char *argv[]){
   gpio node;
   int value;
   int pin;
   int retval;

   if (argc < 3){
      fprintf(stderr, "Usage: %s [gpio number] [direction] <0,1>\n", argv[0]);
      return -1;
   }
   pin = atoi(argv[1]);

   if ((retval = initGPIO(0, pin, &node))){
      fprintf(stderr, "error initing - %d\n", retval);
      return -1;
   }
   if (tolower(argv[2][0]) == 'i'){
      if ((retval = setGPIO(&node, IN, 0))){
	 fprintf(stderr, "error setting - %d\n", retval);
	 return -1;
      }
   }
   else if (tolower(argv[2][0]) == 'o'){
      if (argc != 4){
	 fprintf(stderr, "Error: need a level to set output as\n");
         fprintf(stderr, "Usage: %s [gpio number] [direction] <0,1>\n", argv[0]);
	 return -1;
      }
      value = atoi(argv[3]);
      if ((retval = setGPIO(&node, OUT, value))){
	 fprintf(stderr, "error setting - %d\n", retval);
	 return -1;
      }
   }
   else{
      fprintf(stderr, "invalid direction: %s\n", argv[2]);
      return -1;
   }
   if ((value = readGPIO(&node)) < 0){
      fprintf(stderr, "error reading - %d\n", value);
      return -1;
   }
   printf("%c\n", value);
   if ((freeGPIO(&node))){
      fprintf(stderr, "error letting go\n");
      return -1;
   }
   return 0;
}
