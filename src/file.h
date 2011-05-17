
#define HF_MODE_PIPE        0
#define HF_MODE_CHUNKED     1
#define HF_MODE_KNOW_LENGTH 2
#define HF_MODE_SEEK_START  3
#define HF_MODE_RANGES      4

void file_over_http_init(const char *dir, int mode);
