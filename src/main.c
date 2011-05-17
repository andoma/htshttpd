#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <getopt.h>

#include "tcp.h"
#include "http.h"
#include "file.h"


static void
handle_sigpipe(int x)
{
  return;
}


int
main(int argc, char **argv)
{
  int c;
  int port = 8000;
  const char *basedir = "./";
  int mode = HF_MODE_RANGES;

  signal(SIGPIPE, handle_sigpipe);

  while((c = getopt(argc, argv, "d:p:m:")) != -1) {
    switch(c) {
    case 'p':
      port = atoi(optarg);
      break;
    case 'd':
      basedir = optarg;
      break;
    case 'm':
      if(!strcmp(optarg, "pipe"))
	mode = HF_MODE_PIPE;
      else if(!strcmp(optarg, "chunked"))
	mode = HF_MODE_CHUNKED;
      else if(!strcmp(optarg, "knowlength"))
	mode = HF_MODE_KNOW_LENGTH;
      else if(!strcmp(optarg, "seekstart"))
	mode = HF_MODE_SEEK_START;
      else if(!strcmp(optarg, "ranges"))
	mode = HF_MODE_RANGES;
      else {
	fprintf(stderr, "Invalid mode %s", optarg);
	exit(1);
      }
      break;
    }
  }

  tcp_server_init();

  http_server_init(port);
  file_over_http_init(basedir, mode);
  while(1) {
    sleep(1000);
  }
  return 0;
}
