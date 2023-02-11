
#include <stdio.h>

#include "led0_LA.c"
#include "bp_LA.c"

int main()
{
   char led, bp;
   int fdled0 = open("/dev/led0_LA", O_WR);
   int fdbp = open("/dev/bp_LA", O_RD);

   if (fdled0 < 0) {
      fprintf(stderr, "Erreur: Can not open the driver for LED0!\n");
      exit(1);
   }
   if (fdbp < 0) {
      fprintf(stderr, "Erreur: Can not open the driver for buttom!\n");
      exit(1);
   }


   while(1) { 
      led = (led == '0') ? '1' : '0';

      write( fd, &led, 1);
      sleep( 1);
      //read( fd, &bp, 1);
   } 
   
   return 0;
}
