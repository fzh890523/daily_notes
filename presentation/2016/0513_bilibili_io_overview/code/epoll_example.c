#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/epoll.h>
#include "term_utils.h"

#ifndef BUFSIZE
#define BUFSIZE 10
#endif

#define EPOLL_EVENTS 10

/*
gcc -lcurses -lrt epoll_example_1.c term_utils.c

while(epoll_wait + read)
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
  int ret, epoll_fd;
  char buf[BUFSIZE+1];
  if((epoll_fd = epoll_create(1024)) < 0){
    perror("epoll_create");
    exit(1);
  }
  {
    struct epoll_event event;
    event.events = EPOLLIN | EPOLLERR;
    if(ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, 0, &event) < 0){
      perror("epoll_ctl");
      exit(1);
    }
  }
  while(1){
    struct epoll_event events[EPOLL_EVENTS];
    if((ret = epoll_wait(epoll_fd, &events[0], EPOLL_EVENTS, 500)) < 0){
      perror("epoll_wait");
      exit(1);
    } else if(ret == 0){
      continue;
    } else {
      int i = 0;
      for(; i<ret; i++){
        if(events[i].events & EPOLLHUP){
          perror("EPOLLHUP");
          exit(1);
        } else if(events[i].events & EPOLLERR) {
          perror("EPOLLERR");
          exit(1);
        } else if(events[i].events & EPOLLIN){
          int event_fd = events[i].data.fd;
          int rret;
          if((rret = read(event_fd, buf, BUFSIZE)) < 0){
            perror("read");
            exit(1);
          } else {
            buf[rret] = '\0';
            if((strcmp(buf, "Q") == 0) || (buf, "Q\n") == 0) {
              exit(0);
            } else {
              printf("%s", buf);
              fflush(stdout);
            }
          }
        }
      }
    }
  }
}

void exit_reset_mode()
{
  tty_mode(1);
}
