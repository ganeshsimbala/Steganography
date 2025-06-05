#include <stdio.h>
#include "encode.h"
#include "types.h"
#include <string.h>
#include "decode.h"

int main(int argc, char *argv[])
{
    /*
    Structure Variables
    */
    EncodeInfo encInfo;
    DecodeInfo decInfo;

    /*
    Calling the function to check the operation type
    */
    if (argc <= 2)
    {
        printf("#########UNSUPPORTED OPERATION############\nUsage\n");
        printf("For Encoding : ./a.out -e <image_file.bmp> <secret_text> [output_file.bmp]\n");
        printf("For Decoding : ./a.out -d <Encoded_file.bmp> [secret file]\n");
        return 1;
    }
    else if (check_operation_type(argv) == e_encode)
    {
        printf("***********Selected Encoding*************\n");
        if (read_and_validate_encode_args(argv, &encInfo) == e_success)
        {
            //if it encoding validating args for decoding
            printf("Read validation for encoding is done\n");
            //If all the encode args are present calling do encoding function
            if (do_encoding(&encInfo) == e_success)
            {
                printf("Encoded Successfully\n");
                return 0;
            }
            else
            {
                printf("Failed to encode\n");
                return 1;
            }
        }
        else
        {
            printf("Failed to read and encode args \n");
            return 1;
        }
    }
    else if (check_operation_type(argv) == e_decode)
    {
        printf("***********Selected decoding***************\n");
        //If it is encoding we validating the args for encoding
        if (read_and_validate_decode_args(argv, &decInfo) == e_success)
        {
            printf("Read and validation of Decode args is done\n");
            //calling decode function to decode
            if (do_decoding(&decInfo) == e_success)
            {
                printf("Decoded Successfully \n");
                return 0;
            }
            else
            {
                printf("Failed to decode\n");
                return 1;
            }
        }
        else
        {
            printf("Failed to read and validate decode args\n");
            return 1;
        }
    }
    else
    {
        return 1;
    }
    return 0;
}

/*
    Checking wwhich operation to perfor,
    -d -- decoding
    -e -- encoding
*/

OperationType check_operation_type(char *argv[])
{
    if (strcmp(argv[1], "-e") == 0)
    {

        return e_encode;
    }
    else if (strcmp(argv[1], "-d") == 0)
    {

        return e_decode;
    }
    else
    {
        printf("##########Unsupported operation##########\nusage:");
        printf("For encoding: ./a.out -e <image_file.bmp> <text_file> [output.bmp]\n");
        printf("For decoding : ./a.out -d <Encoded_file.bmp> [output] \n");
        return e_unsupported;
    }
}
