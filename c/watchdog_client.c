#include <stdio.h>
#include <wiringPi.h>

int main()
{
  int NumTicks;
  int i;

  wiringPiSetup () ;
  pinMode (0, OUTPUT) ;

  pinMode (1, INPUT) ;
  pullUpDnControl (1, PUD_UP) ;

  NumTicks = 0;
  for (;;)
  {
    digitalWrite (0, HIGH) ;
    for(i = 0; i < 100; i++) {
      if (digitalRead (1) == LOW) {
        NumTicks++;
        if (NumTicks > 100) goto Cleanup;
      } else {
        NumTicks = 0;
      }
      delay(10);
    }	 
    digitalWrite (0,  LOW) ;
    for(i = 0; i < 500; i++) {
      if (digitalRead (1) == LOW) {
        NumTicks++;
        if (NumTicks > 100) goto Cleanup;
      } else {
        NumTicks = 0;
      }
      delay(10);
    }
 
  }
Cleanup:
  printf("Shutting down...\n");
  system("/sbin/shutdown -h now");
  return 0;
}

