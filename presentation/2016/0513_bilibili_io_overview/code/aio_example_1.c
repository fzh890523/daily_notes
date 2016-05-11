#include <aio.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include "term_utils.h"

#ifndef BUFSIZE
#define BUFSIZE 10
#endif

/*
gcc -lcurses -lrt aio_example_1.c term_utils.c

aio + while(aio_error-aio_return)
*/


struct aiocb my_aiocb;

void setup_aio_buffer();
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
  setup_aio_buffer(0); // 0 means stdin

  int read_end;
  while(1) {
    ret = aio_read(&my_aiocb);  // commit a read request
    if(ret < 0){
      perror("aio_read");
      exit(1);
    }
    read_end = 0;
    while(!read_end){
      ret = aio_error(&my_aiocb);  // check read status
      switch (ret) {
        case 0:
          if((ret = aio_return(&my_aiocb)) > 0){  // get read length
            char *s = (char *)my_aiocb.aio_buf;
            s[ret] = '\0';
            if((strcmp(s, "Q") == 0) || (strcmp(s, "Q\n") == 0)) {
              exit(0);
            }
            printf("%s", s);
            fflush(stdout);
            // notice
            read_end = 1;
          } else {
            perror("aio_return");
            exit(1);
          }
          break;
        case EINPROGRESS:
          continue;  // notice this, continue only do something with LOOP
        case ECANCELED:
          break;
        default:
          printf("aio_error, ret is %d\n", ret);
          exit(1);
          break;
      }
    }
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
}

void exit_reset_mode()
{
  tty_mode(1);
}
