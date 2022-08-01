/**
 */

#include "gpioapi.h"

/**
 * Reads the value of the specified pin.
 *
 * @return Less than 0 on error, otherwise the value of the GPIO pin.
 */
int readGPIO(gpio *node){
   int fd, retval;
   char value;
   char file[MAX_LEN];
   memset(file, '\0', MAX_LEN);

   sprintf(file, GPIO_VALUE, node->pin);
   if ((fd = open(file, O_RDONLY)) < 0){
      retval = errno;
      perror("open");
      if (retval == ENOENT){
         return EGPIONOINIT;
      }
      return ESYSCALLFAIL;
   }
   if (lseek(fd, 0, SEEK_SET) < 0){
      perror("lseek");
      return ESYSCALLFAIL; 
   }

   if ((read(fd, &value, 1)) < 0){
      perror("read");
      return ESYSCALLFAIL;
   }
   close(fd);
   return value;
}

/**
 */
int setGPIO(gpio *node, int direction, int value){
   int fd, retval;
   char file[MAX_LEN];
   memset(file, '\0', MAX_LEN);
   retval = 0;

   sprintf(file, GPIO_DIRECTION, node->pin);
   if ((fd = open(file, O_WRONLY)) < 0){
      retval = errno;
      perror("open");
      if (retval == ENOENT){
         return EGPIONOINIT;
      }
      return ESYSCALLFAIL;
   }
   if (lseek(fd, 0, SEEK_SET) < 0){
      perror("lseek");
      return ESYSCALLFAIL; 
   }
   if (direction == IN){
      if ((write(fd, "in", 2)) < 2){
         perror("write1");
         return ESYSCALLFAIL;
      }
   }
   else if (direction == OUT){
      if (value == 0){
         if ((write(fd, "low", 3)) < 3){
            perror("write2");
            return ESYSCALLFAIL;
         }
      }
      else if (value > 0){
         if ((write(fd, "high", 4)) < 4){
            perror("write3");
            return ESYSCALLFAIL;
         }
      }
      else {
         return EINVALPARAM;
      }
   }
   else{
      return EINVALPARAM;
   }
   close(fd);
   return retval;
}

/**
 * Initializes the GPIO pin specified.
 * If chip doesn't match any of the predefined values then pin is taken as
 * an absolute pin number. This should only be called once. 
 *
 * @param chip The device to reference pin from.
 * @param pin The pin to initialize.
 *
 * @return 0 on success and anything else is an error. 
 */
int initGPIO(int chip, int pin, gpio *node){
   int retval, fd;
   char pins[MAX_LEN];
   memset(pins, '\0', MAX_LEN);
   retval = 0;
   
   switch(chip){
      case PIO_A:
         if (pin >= 32){
            return EINVALPIN;
         }
	      node->pin = PIO_A_BASE + pin;
	      break;
      case PIO_B:
         if (pin >= 32){
            return EINVALPIN;
         }
         node->pin = PIO_B_BASE + pin;
         break;
      case PIO_C:
         if (pin >= 32){
            return EINVALPIN;
         }
         node->pin = PIO_C_BASE + pin;
         break;
      case EXPANDER:
         if (pin >= 16){
            return EINVALPIN;
         }
         node->pin = EXPANDER_BASE + pin;
         break;
      default:
         node->pin = pin;
         break;
   }

   fd = open(GPIO_EXPORT, O_WRONLY);
   if (fd < 0){
      perror("open");
      return ESYSCALLFAIL;
   }
   if (lseek(fd, 0, SEEK_SET) < 0){
      perror("lseek");
      return ESYSCALLFAIL;
   }
   if (sprintf(pins, "%d", node->pin) < 0){
      fprintf(stderr, "sprintf fail\n");
      return ESYSCALLFAIL;
   }
   if(write(fd, pins, strlen(pins)) < strlen(pins)){
      retval = errno;
      perror("write");
      if (retval == EBUSY){
         return EGPIONA;
      }
      return ESYSCALLFAIL;
   }
   close(fd);

   return retval;
}

/*
 */
int freeGPIO(gpio *node){
   int fd;
   char pins[MAX_LEN];
   memset(pins, '\0', MAX_LEN);

   fd = open(GPIO_UNEXPORT, O_WRONLY);
   if (fd < 0){
      perror("open");
      return ESYSCALLFAIL;
   }
   if (lseek(fd, 0, SEEK_SET) < 0){
      perror("lseek");
      return ESYSCALLFAIL;
   }
   if (sprintf(pins, "%d", node->pin) < 0){
      return ESYSCALLFAIL;
   }
   if (write(fd, pins, strlen(pins)) < strlen(pins)){
      perror("write");
      return ESYSCALLFAIL;
   }
   close(fd);
   return 0;
}
