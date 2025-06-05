#include <stdio.h>
#include <string.h>
#include "types.h"
#include "common.h"
#include "decode.h"

// Function to read and validate decode arguments
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo)
{
    // Check if the stego image file has .bmp extension
    if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->steged_image_fname = argv[2];
    }
    else
    {
        return e_failure;
    }

    // Check if output decode file has .txt extension or use default
    if (argv[3] != NULL)
    {
        // strstr(argv[3], ".")

        decInfo->decode_fname = argv[3];
        // // strcpy(decInfo->extn_decode_file, ".txt");
    }
    else{
        decInfo->decode_fname="Decoded_File";
    }

    return e_success;
}

// Function to perform the overall decoding process
Status do_decoding(DecodeInfo *decInfo)
{
    // Open the stego and decode files
    if (open_decode_files(decInfo) == e_success)
    {
        printf("Opened all required Files\n");
    }
    else
    {
        printf("Failed to open required files\n");
        return e_failure;
    }

    // Decode the magic string to validate hidden data
    if (decode_magic_string(decInfo->fptr_steged_image, decInfo) == e_success)
    {
        printf("Magic string Decoded successfully\n");
    }
    else
    {
        printf("Failed to decode Magic string\n");
        return e_failure;
    }

    // Decode the size of the file extension
    if (decode_file_extn_size(decInfo) == e_success)
    {
        printf("File extension size is decoded Successfully\n");
    }
    else
    {
        printf("Failed to decode file extension size\n");
        return e_failure;
    }

    // Decode the file extension
    if (decode_file_extn(decInfo) == e_success)
    {
        printf("File extension is decoded Successfully\n");
    }
    else
    {
        printf("Failed to decode File extension\n");
        return e_failure;
    }

    // Decode the size of the hidden secret file
    if (decode_steged_file_size(decInfo) == e_success)
    {
        printf("Decoded output File size\n");
    }
    else
    {
        printf("Failed to decode file size\n");
        return e_failure;
    }

    // Decode the actual secret data and write to output file
    if (decode_secret_data(decInfo) == e_success)
    {
        printf("Secret Data is decoded Successfully\n");
    }
    else
    {
        printf("Failed to decode Secret data\n");
        return e_failure;
    }

    return e_success;
}

// Function to open stego image file and output decode file
Status open_decode_files(DecodeInfo *decInfo)
{
    decInfo->fptr_steged_image = fopen(decInfo->steged_image_fname, "r"); // Open BMP file

    if (decInfo->fptr_steged_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->steged_image_fname);
        return e_failure;
    }
#if 0
    decInfo->fptr_decode_file = fopen(decInfo->decode_fname, "w"); // Open decode file

    if (decInfo->fptr_decode_file == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", decInfo->decode_fname);
        remove(decInfo->decode_fname); // Remove incomplete file
        return e_failure;
    }
#endif

    return e_success;
}

// Function to decode and verify the magic string
Status decode_magic_string(FILE *fptr_steged_image, DecodeInfo *decInfo)
{
    char magic_string[strlen(MAGIC_STRING) + 1];

    fseek(fptr_steged_image, 54, SEEK_SET); // Skip BMP header
    decode_data_from_image(magic_string, strlen(MAGIC_STRING), decInfo);
    magic_string[strlen(MAGIC_STRING)] = '\0';

    if (strcmp(magic_string, MAGIC_STRING) == 0)
    {
        printf("Magic String Matched\n");
        return e_success;
    }
    else
    {
        printf("Magic string Not matched\n");
        return e_failure;
    }
}

// Function to decode character data from LSBs of image
Status decode_data_from_image(char *data, int len, DecodeInfo *decInfo)
{
    for (int i = 0; i < len; i++)
    {
        fread(decInfo->decode_data, sizeof(char), 8, decInfo->fptr_steged_image); // Read 8 bytes
        decode_byte_from_lsb(&data[i], decInfo->decode_data);                     // Decode one byte from 8 LSBs
    }
    return e_success;
}

// Function to decode a single byte from 8 LSBs
Status decode_byte_from_lsb(char *data, char *decode_data)
{
    int value = 0;

    for (int i = 0; i < 8; i++)
    {
        value = (decode_data[i] & 1) << (7 - i) | value; // Extract LSB and shift to form byte
    }

    *data = (char)value;
    return e_success;
}

// Function to decode file extension size
Status decode_file_extn_size(DecodeInfo *decInfo)
{
    decode_numerical_values(&decInfo->file_extn_size, decInfo); // Decode 32-bit int
    return e_success;
}

// Function to decode a 32-bit integer from image
Status decode_numerical_values(int *value, DecodeInfo *decInfo)
{
    char size[32];

    fread(size, sizeof(char), 32, decInfo->fptr_steged_image); // Read 32 bytes
    decode_int_values_from_lsb(value, size);                   // Decode integer from LSBs

    return e_success;
}

// Function to decode integer value from 32 LSBs
Status decode_int_values_from_lsb(int *value, char *bytes)
{
    int x = 0;

    for (int i = 0; i <= 31; i++)
    {
        x = (bytes[i] & 1) << (31 - i) | x; // Combine bits to form integer
    }

    *value = x;
    return e_success;
}

// Function to decode file extension
Status decode_file_extn(DecodeInfo *decInfo)
{
    decode_data_from_image(decInfo->decode_file_extn, decInfo->file_extn_size, decInfo); // Decode extension string

    if (strstr(decInfo->decode_fname, ".") != NULL)
    {
        if (strtok(decInfo->decode_fname, ".") != NULL)
        {
            strcat(decInfo->decode_fname, decInfo->decode_file_extn);
            decInfo->fptr_decode_file = fopen(decInfo->decode_fname, "w");
        }
    }
    else
    {
        // decInfo->decode_fname = "Decoded_File";
        decInfo->fptr_decode_file = fopen(decInfo->decode_fname, "w");
    }

    return e_success;
}

// Function to decode hidden file size
Status decode_steged_file_size(DecodeInfo *decInfo)
{
    decode_numerical_values(&decInfo->output_file_size, decInfo); // Decode file size
    return e_success;
}

// Function to decode the actual secret data
Status decode_secret_data(DecodeInfo *decInfo)
{
    extract_secret_data(decInfo->output_file_size, decInfo); // Extract secret content
    return e_success;
}

// Function to extract secret data byte by byte and write to file
Status extract_secret_data(uint size, DecodeInfo *decInfo)
{
    for (int i = 0; i < size; i++)
    {
        char ch;

        fread(decInfo->decode_data, sizeof(char), 8, decInfo->fptr_steged_image); // Read 8 bytes
        decode_byte_from_lsb(&ch, decInfo->decode_data);                          // Decode one character
        fwrite(&ch, sizeof(char), 1, decInfo->fptr_decode_file);                  // Write character to file
    }
    return e_success;
}
