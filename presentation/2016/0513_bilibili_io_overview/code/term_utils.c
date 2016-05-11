#include <stdlib.h>
#include <termios.h>
#include <fcntl.h>

#ifndef MIN
  #define MIN 1
#endif

void set_crmode()
{
  struct termios ttystate;
  tcgetattr(0, &ttystate);
  ttystate.c_lflag &= ~ICANON;
  ttystate.c_lflag &= ~ECHO;
  ttystate.c_cc[VMIN] = MIN;
  // VMIN为flush的buf大小
  // 为1表示每个字符都flush一次，也即逐字符flush/read/write
  // 0会造成read时如果buf无内容则立即返回EOF（有内容还是读内容） - 也即无阻塞
  tcsetattr(0, TCSANOW, &ttystate);
}

void tty_mode(int how)
{
  static struct termios original_mode;
  static int original_falgs;
  if(how == 0) {
    tcgetattr(0, &original_mode);
    original_falgs = fcntl(0, F_GETFL);
  } else {
    tcsetattr(0, TCSANOW, &original_mode);
    fcntl(0, F_SETFL, original_falgs);
  }
}

void set_nodelay_mode()
{
  int termflags;
  termflags = fcntl(0, F_GETFL);
  termflags |= O_NDELAY;
  fcntl(0, F_SETFL, termflags);
}

void ctrl_c_handler(int signum)
{
  tty_mode(1);
  exit(1);
}
