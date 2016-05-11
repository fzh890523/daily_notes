#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include "term_utils.h"

#ifndef BUFSIZE
#define BUFSIZE 10
#endif

/*
gcc -lcurses -lrt bio_example_1.c term_utils.c

bio + while(read)
*/


void exit_reset_mode();
void do_io();

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
  char buf[BUFSIZE+1];
  fd_set fds;
  struct timeval tv;

  while(1){
    FD_ZERO(&fds);
    FD_SET(0, &fds);  // stdin
    tv.tv_sec = 5;
    tv.tv_usec = 0;

    if((ret = select(1, &fds, NULL, NULL, &tv)) < 0){  // notice the first argument: 1
      perror("select");
      exit(1);
    } else {
      if(FD_ISSET(0, &fds)) {
        if((ret = read(0, buf, BUFSIZE)) < 0){
          perror("read");
          exit(1);
        } else {
          buf[ret] = '\0';
          if((strcmp(buf, "Q") == 0) || (buf, "Q\n") == 0) {
            exit(0);
          } else {
            printf("%s", buf);
            fflush(stdout);
          }
        }
      } else {
        // should not happen
      }
    }
  }
}

void exit_reset_mode()
{
  tty_mode(1);
}
