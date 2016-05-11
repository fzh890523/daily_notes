#include <aio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/eventfd.h>
#include <sys/epoll.h>
#include "term_utils.h"

#ifndef BUFSIZE
#define BUFSIZE 10
#endif

#define EPOLL_EVENTS 10

/*
gcc -lcurses -lrt aio_example_4.c term_utils.c

aio + epoll(eventfd) + callback(only notify count)
// may EPOLLHUP, need improvement
*/


struct aiocb my_aiocb;

void setup_aio_buffer();
void exit_reset_mode();
void do_io();
void on_sigio(int);

int done;
int efd = -1;

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
  uint64_t count = 0;
  int ep_fd = -1;
  struct epoll_event events[EPOLL_EVENTS];

  setup_aio_buffer(0); // 0 means stdin
  ret = aio_read(&my_aiocb);  // commit a read request
  if(ret < 0){
    perror("aio_read");
    exit(1);
  }

  if((ep_fd = epoll_create(1024)) < 0){
    perror("epoll_create");
    exit(1);
  }

  if((efd = eventfd(0, 0)) < 0){
    perror("eventfd");
    exit(1);
  }

  {
    struct epoll_event read_event;
    read_event.events = EPOLLHUP | EPOLLERR | EPOLLIN;

    if((ret = epoll_ctl(ep_fd, EPOLL_CTL_ADD, efd, &read_event)) < 0){
      perror("epoll_ctl_add");
      exit(1);
    }
  }


  while(1) {
    ret = epoll_wait(ep_fd, &events[0], EPOLL_EVENTS, 5000);
    if(ret<0){
      perror("epoll_wait");
      exit(1);
    } else if (ret == 0){
      continue;  // timeout
    } else {
      int i=0;
      for(; i<ret; i++){
        if (events[i].events & EPOLLHUP) {
            printf("epoll eventfd has epoll hup.\n");
            exit(1);
        } else if (events[i].events & EPOLLERR) {
            printf("epoll eventfd has epoll error.\n");
            exit(1);
        } else if (events[i].events & EPOLLIN) {
            int event_fd = events[i].data.fd;
            int rret = read(event_fd, &count, sizeof(count));
            if (rret < 0) {
                perror("read fail:");
                exit(1);
            } else {
              char *s = (char *)my_aiocb.aio_buf;
              s[ret] = '\0';
              if((strcmp(s, "Q") == 0) || (strcmp(s, "Q\n") == 0)) {
                done = 1;
                return;
              }
              printf("%s", s);
              fflush(stdout);
            }
        }
      }
    }
  }
}

void aio_completion_handler(sigval_t sigval)
{
    uint64_t count;
    struct aiocb *req = (struct aiocb *)sigval.sival_ptr;
    int ret = aio_error(req);  // check read status
    switch (ret) {
      case 0:
        if((ret = aio_return(req)) > 0){  // get read length
          count = ret;
          if((ret = write(efd, &count, sizeof(count))) < 0){
            perror("write event fd");
            exit(1);
          }
          // notice
        } else {
          perror("aio_return");
          exit(1);
        }
        break;
      case EINPROGRESS:
        break;  // notice this, continue only do something with LOOP
      case ECANCELED:
        break;
      default:
        printf("aio_error, ret is %d\n", ret);
        exit(1);
        break;
    }
    ret = aio_read(req);  // commit a read request
    if(ret < 0){
      perror("aio_read");
      exit(1);
    }
}

void setup_aio_buffer(int fd)
{
  static char input[BUFSIZE + 1];

  bzero((char *)&my_aiocb, sizeof(struct aiocb));

  my_aiocb.aio_fildes = fd;
  my_aiocb.aio_buf = input;
  my_aiocb.aio_nbytes = BUFSIZE;
  my_aiocb.aio_offset = 0;

  my_aiocb.aio_sigevent.sigev_notify = SIGEV_THREAD;  // notice this
  my_aiocb.aio_sigevent.sigev_notify_function = aio_completion_handler;
  my_aiocb.aio_sigevent.sigev_notify_attributes = NULL;
  my_aiocb.aio_sigevent.sigev_value.sival_ptr = &my_aiocb;
}

void exit_reset_mode()
{
  tty_mode(1);
}
