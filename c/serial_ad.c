#include <stdio.h>
#include <time.h>
#include <termios.h>

int main(int argc, char *argv[])
{
  const int BufSize=100;
  int Ret=0;
  FILE *InFile = NULL;
  FILE *OutFile = NULL;
  int UseStdOut = 0;
  time_t ShutdwnTime = 0;
  time_t CurTime;
  struct tm TheTime;
  char LineBuf[BufSize+1];
  int NumChar;
  char c;


  if (argc > 1) {
    OutFile = fopen(argv[1], "a");
    if (!OutFile) {
      fprintf(stderr, "***Cannot open output file '%s'\n", argv[1]);
      Ret = -2;
      goto Cleanup;
    }
    fprintf(stderr, "Using '%s'.\n", argv[1]);
  } else {
    fprintf(stderr, "Using stdout.\n");
    UseStdOut = 1;
    OutFile = stdout;
  }

  InFile = fopen("/dev/ttyAMA0", "r");
  if (!InFile) {
    fprintf(stderr, "***Cannot open input file '%s'\n", "/dev/ttyAMA0");
    Ret = -3;
    goto Cleanup;
  }
  {
    struct termios TtyParms;

    if (tcgetattr(fileno(InFile), &TtyParms) != 0) {
      fprintf(stderr, "***Cannot get TTY parameters");
      Ret = -4;
      goto Cleanup;
    }
    cfsetspeed(&TtyParms, B9600);
    if (tcsetattr(fileno(InFile), TCSADRAIN, &TtyParms) != 0) {
      fprintf(stderr, "***Cannot set TTY parameters");
      Ret = -4;
      goto Cleanup;
    }
  }

  NumChar = 0;
  LineBuf[NumChar] = '\0';
  for(;;) {
    CurTime = time(NULL);
    c = fgetc(InFile);
    if (c < 0) {
      fprintf(stderr, "***Error while reading input.\n");
      Ret = -4;
      goto Cleanup;
    }
    if (c == '\r' || c == '\n') {  
      if (NumChar > 0) {
        TheTime = *localtime(&CurTime);
        fprintf(OutFile, "%04d-%02d-%02d %02d:%02d:%02d\t%s\n", TheTime.tm_year+1900, TheTime.tm_mon+1, TheTime.tm_mday, 
                 TheTime.tm_hour, TheTime.tm_min, TheTime.tm_sec, LineBuf);
        fflush(OutFile);

        if (strncmp(LineBuf, "Shutdown:", 9) == 0) {
          ShutdwnTime = CurTime;
	}
      }
      NumChar = 0;
    } else {
      LineBuf[NumChar] = c;
      NumChar++;
    }
    if (NumChar >= BufSize) {
      fprintf(stderr, "***Buffer overflow!\n");
      NumChar = 0;
    }
    LineBuf[NumChar] = '\0';

    if (ShutdwnTime > 0 && CurTime - ShutdwnTime > 15) {
      system("/sbin/shutdown -h now");
      break;
    }
  }

Cleanup:
  if (OutFile && !UseStdOut) fclose(OutFile);
  if (InFile) fclose(InFile);
  return Ret;
}
