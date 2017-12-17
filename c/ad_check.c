#include <stdio.h>
#include <time.h>

#include <wiringPi.h>
#include <gertboard.h>

int main ()
{
  const int NumProbes = 100;
  const int ProbeDelay = 100;
  time_t CurTime;
  struct tm TheTime;

  int Sum;
  int i;

  fprintf(stderr, "AD converter read\n");
  fprintf(stderr, "=================\n");

  wiringPiSetupSys();
  gertboardAnalogSetup(100);

  for(;;) {
    Sum = 0;
    for(i = 0; i < NumProbes; i++) {
      Sum += analogRead (100);
      delay(ProbeDelay);
    }  
    CurTime = time(NULL);
    TheTime = *localtime(&CurTime);
    printf("%04d-%02d-%02d %02d:%02d:%02d\t%d\n", TheTime.tm_year+1900, TheTime.tm_mon+1, TheTime.tm_mday, 
             TheTime.tm_hour, TheTime.tm_min, TheTime.tm_sec, Sum/NumProbes);
    fflush(stdout);
  }
}
