#include <stdio.h>
#include <sys/types.h>
#include <sys/param.h>
#include <sys/sendfile.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>

#include "http.h"
#include "file.h"

static int hf_mode = HF_MODE_RANGES;
static const char *basedir;

/**
 * Send a file
 */
static int
send_file(http_connection_t *hc, const char *remain, void *opaque)
{
  int fd;
  struct stat st;
  const char *content = NULL, *range = NULL;
  char range_buf[255];
  off_t content_len, file_start, file_end, chunk;
  ssize_t r;
  char filename[512];

  printf("%s\n", remain);

  if(remain == NULL)
    return 404;

  snprintf(filename, sizeof(filename), "%s/%s", basedir, remain);

  fd = open(filename, O_RDONLY, 0);
  if(fd < 0) {
    return 404;
  }    
  if(fstat(fd, &st) < 0) {
    close(fd);
    return 404;
  }

  file_start = 0;
  file_end = st.st_size-1;
  
  if(hf_mode >= HF_MODE_SEEK_START) {

    range = http_arg_get(&hc->hc_args, "Range");
    if(range != NULL) {
      r = sscanf(range, "bytes=%"PRId64"-%"PRId64"", &file_start, &file_end);
      printf("range %s    %zd\n", range, r);
    }
  }

  //Sanity checks
  if(file_start < 0 || file_start >= st.st_size)
    return 200;

  if(file_end < 0 || file_end >= st.st_size)
    return 200;

  if(file_start > file_end)
    return 200;

  content_len = file_end - file_start+1;
  
  sprintf(range_buf, "bytes %"PRId64"-%"PRId64"/%"PRId64"",
	  file_start, file_end, st.st_size);

  if(file_start > 0)
    lseek(fd, file_start, SEEK_SET);
  
  http_send_header(hc, range ? HTTP_STATUS_PARTIAL_CONTENT : HTTP_STATUS_OK,
		   content,
		   hf_mode ==  HF_MODE_PIPE ? 0 : content_len,
		   NULL, NULL, 10, 
		   range ? range_buf : NULL, NULL);

  if(!hc->hc_no_output) {
    while(content_len > 0) {
      chunk = MIN(1024 * 1024 * 1024, content_len);
      r = sendfile(hc->hc_fd, fd, NULL, chunk);
      if(r == -1)
	return -1;
      content_len -= r;
    }
  }
  close(fd);
  return 0;
}


void
file_over_http_init(const char *dir, int mode)
{
  hf_mode = mode;
  basedir = dir;
  http_path_add("/files",  NULL, send_file);
}
