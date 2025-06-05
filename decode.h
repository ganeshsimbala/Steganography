#include "types.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_FILE_SUFFIX 4
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)

typedef struct
{
    char *steged_image_fname;
    FILE *fptr_steged_image;
    int file_extn_size;

    char *decode_fname;
    FILE *fptr_decode_file;
    char extn_decode_file[MAX_FILE_SUFFIX];
    int output_file_size;
    char steg_file_extn[MAX_FILE_SUFFIX];
    char decode_data[MAX_IMAGE_BUF_SIZE];

} DecodeInfo;

Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);
Status open_decode_files(DecodeInfo *decInfo);
Status do_decoding(DecodeInfo *decInfo);
Status decode_magic_string(FILE *fptr_steged_image, DecodeInfo *decInfo);
Status decode_byte_from_lsb(char *data, char *decode_data);
Status decode_data_from_image(char *data, int len, DecodeInfo *decInfo);
Status decode_file_extn_size(DecodeInfo *decInfo);
Status decode_numerical_values(int *value, DecodeInfo *decInfo);
Status decode_int_values_from_lsb(int *value, char *bytes);
Status decode_file_extn(DecodeInfo *decInfo);
Status decode_steged_file_size(DecodeInfo *decInfo);
Status decode_secret_data(DecodeInfo *decInfo);
Status extract_secret_data(uint size, DecodeInfo *decInfo);