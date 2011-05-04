
#define HF_MODE_PIPE        0
#define HF_MODE_KNOW_LENGTH 1
#define HF_MODE_SEEK_START  2
#define HF_MODE_RANGES      3

void file_over_http_init(const char *dir, int mode);
