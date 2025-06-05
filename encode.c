#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include <stdlib.h>
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/* Read and validation of command line args for encoding
Validating the CLA's if all are present return e_success and store all the args to the encode structure
*/
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // checking for the src file is .bmp or not
    if (argv[2] != NULL && strstr(argv[2], ".bmp") != NULL)
    {
        encInfo->src_image_fname = argv[2];
        // checking for the Secret file extension is .txt or not
        if (argv[3] != NULL && strstr(argv[3], ".") != NULL)
        {
            encInfo->secret_fname = argv[3];
            strcpy(encInfo->extn_secret_file, strstr(argv[3], "."));

            // if the destination file is provided A destination file is created with that name

            if (argv[4] != NULL && strstr(argv[4], ".bmp") != NULL)
            {
                encInfo->stego_image_fname = argv[4];
            }
            // default destination file name
            else
            {
                encInfo->stego_image_fname = "Encoded_File.bmp";
            }
            return e_success;
        }
        else
        {
            printf("No Secret Message File\n");
            return e_failure;
        }
    }
    else
    {
        printf("NO such image File\n");
        return e_failure;
    }
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}
// Encoding Function in which all the encoding functions are called
Status do_encoding(EncodeInfo *encInfo)
{
    // function call for opening all required files
    if (open_files(encInfo) == e_success)
    {
        printf("Opened Required files successfully\n");
    }
    else
    {
        printf("Failed to open required Files\n");
        return e_failure;
    }
    // checking capacity of the src file
    if (check_capacity(encInfo) == e_success)
    {

        printf("Enough space to store Secret Data\n");
    }
    else
    {
        printf("Not enough sapce to store Secret data\n");
        return e_failure;
    }
    // Calling fuction to copy the header of the src file
    if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("Copied 54 bytes header into ouput image\n");
    }
    else
    {
        printf("Failed copy 54 byte header\n");
    }

    // Function call of encoding Magic String
    if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
    {
        printf("Encoded Magic string successfully\n");
    }
    else
    {
        printf("Failed to Encode magic string\n");
        return e_failure;
    }
    // Function call to encode secret file extension size
    if (encode_secret_file_extn_size(MAX_FILE_SUFFIX, encInfo) == e_success)
    {
        printf("Secret extension size is encoded Successfully\n");
    }
    else
    {
        printf("Failed encode secret file extension size\n");
        return e_failure;
    }

    // Function call to encode the secret file extension
    if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
    {
        printf("Secret file extension is encoded successfully\n");
    }
    else
    {
        printf("Failed to encode secret file extn \n");
        return e_failure;
    }

    // function call to encode the size of secret file
    if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
    {
        printf("Secret file size is encoded successfully\n");
    }
    else
    {
        printf("Failed to encode secret size\n");
        return e_failure;
    }

    // function call to encode the secret file data
    if (encode_secret_file_data(encInfo) == e_success)
    {
        printf("Encoded secret file data\n");
    }
    else
    {
        printf("Failed to encode secret file data\n");
        return e_failure;
    }

    // function call to copy the remaining data
    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
    {
        printf("Copied remaining data\n");
    }
    else
    {
        printf("Failed to copy remaining data\n");
        return e_failure;
    }

    // function to close all the files
    if (file_closing(encInfo) == e_success)
    {
        printf("Closed all the files\n");

        return e_success;
    }
    return e_success;
}

// function to know that enough space to encode all the required data
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if (encInfo->image_capacity > (54 + (strlen(MAGIC_STRING) + 4 + 4 + 4 + encInfo->size_secret_file) * 8))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}
// Copying the first 54 bytes of metadata from .bmp to output stego file
Status copy_bmp_header(FILE *fptr_src_image, FILE *fptr_dest_image)
{
    fseek(fptr_src_image, 0, SEEK_SET);
    char header[54];
    fread(header, sizeof(char), 54, fptr_src_image);
    fwrite(header, sizeof(char), 54, fptr_dest_image);
    return e_success;
}

// functon to return the size of Secret.txt file
uint get_file_size(FILE *fptr)
{
    fseek(fptr, 0, SEEK_END);
    return ftell(fptr);
}

// Encoding the magic string into stego_out file
Status encode_magic_string(char *magic_string, EncodeInfo *encInfo)
{
    if (encode_data_to_image(magic_string, strlen(magic_string), encInfo) == e_success)
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// Function to read the bytes from source file and write the encoded bytes to ouput file
Status encode_data_to_image(char *data, int size, EncodeInfo *encInfo)
{

    for (int i = 0; i < size; i++)
    {
        // for encoding 1byte data we are reading 8 bytes from source file
        fread(encInfo->image_data, sizeof(char), 8, encInfo->fptr_src_image);
        // function which actually encode bits at lsb
        encode_byte_to_lsb(data[i], encInfo->image_data);
        // Writing the encoded bytes to output file
        fwrite(encInfo->image_data, sizeof(char), 8, encInfo->fptr_stego_image);
    }
    return e_success;
}

// Actual encoding at lsb's are performed here
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & ~1) | ((data & (1 << (7 - i))) >> (7 - i));
    }
    return e_success;
}

// encoding secret file extension size
Status encode_secret_file_extn_size(int size, EncodeInfo *encInfo)
{
    // function to encode Numerical values
    encode_numeric_values_to_image(size, encInfo);
    return e_success;
}

// Reading bytess from source file and writing encoded bytes to destinaton file
Status encode_numeric_values_to_image(int size, EncodeInfo *encInfo)

{
    char data_buffer[32];
    fread(data_buffer, sizeof(char), 32, encInfo->fptr_src_image);
    encode_32_bytes_to_lsb(size, data_buffer);
    fwrite(data_buffer, sizeof(char), 32, encInfo->fptr_stego_image);
    return e_success;
}

// Function to encode LSB's of numerical values
Status encode_32_bytes_to_lsb(int value, char *data_buffer)
{
    for (int i = 0; i < 32; i++)
    {
        data_buffer[i] = (data_buffer[i] & ~1) | (value & (1 << (31 - i))) >> (31 - i);
    }
    return e_success;
}

// Function to encode Secret file extension
Status encode_secret_file_extn(char *file_extn, EncodeInfo *encInfo)
{
    encode_data_to_image(file_extn, strlen(file_extn), encInfo);
    return e_success;
}

// Encoding secret file size
Status encode_secret_file_size(long file_size, EncodeInfo *encInfo)
{
    encode_numeric_values_to_image(file_size, encInfo);
    return e_success;
}

// Encoding the Secret file data
Status encode_secret_file_data(EncodeInfo *encInfo)
{
    fseek(encInfo->fptr_secret, 0, SEEK_SET);
    // Allocating the memory to store the secret file data
    char *secret_data = (char *)malloc(encInfo->size_secret_file * sizeof(char));
    if (!secret_data)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return e_failure;
    }
    fread(secret_data, sizeof(char), encInfo->size_secret_file, encInfo->fptr_secret);
    encode_data_to_image(secret_data, encInfo->size_secret_file, encInfo);
    free(secret_data);
    return e_success;
}

// Copying the remaining data from source image
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_dest)
{
    int ch;
    while ((ch = fgetc(fptr_src)) != EOF)
    {
        fputc(ch, fptr_dest);
    }
    return e_success;
}

// closing all the Filepointers
Status file_closing(EncodeInfo *encInfo)
{
    fclose(encInfo->fptr_src_image);
    fclose(encInfo->fptr_secret);
    fclose(encInfo->fptr_stego_image);
    return e_success;
}