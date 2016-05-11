#include <aio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "term_utils.h"

#ifndef BUFSIZE
#define BUFSIZE 10
#endif

/*
gcc -lcurses async_example.c term_utils.c

aio + while(aio_error-aio_return)
*/


void exit_reset_mode();
void do_io();
void on_input(int);
void enable_kbd_signals();

int done;

int main()
{
  tty_mode(0);
  signal(SIGQUIT, SIG_IGN);
  signal(SIGINT, ctrl_c_handler);
  set_crmode();
  // set_nodelay_mode();
  int ret;
  ret = atexit(exit_reset_mode);
  if(ret != 0) {
    perror("atexit");
    exit(1);
  }

  do_io();
}

void do_io(){
  int ret;
  signal(SIGIO, on_input);
  enable_kbd_signals();
  int read_end;
  while(!done) {
    pause();
  }
}

void on_input(int signum)
{
  static char buf[BUFSIZE + 1];
  int ret;
  if((ret = read(0, buf, BUFSIZE)) < 0){
    if(errno == EAGAIN){
      return;
    } else {
      perror("read");
      exit(1);
    }
  } else {
    buf[ret] = '\0';
    printf("%s", buf);
    fflush(stdout);
  }
}

void exit_reset_mode()
{
  tty_mode(1);
}


void enable_kbd_signals()
{
  int fd_flags;
  fcntl(0, F_SETOWN, getpid());  // 使得打开该文件的进程只有“本进程”才收到信号
  fd_flags = fcntl(0, F_GETFL);
  fcntl(0, F_SETFL, (fd_flags | O_ASYNC));
}
