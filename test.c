/* Include all neccessary header files */
#include <stdio.h>
#include "type.h"
#include "view.h"
#include "edit.h"


/* All function calling in test.c file */
int main(int argc, char *argv[])
{
    tagInfo tag_info_mp3;
    tagData tag_Data;  

    /* Condition valid argument pass */
    if(argc < 2)
    {
        printf("ERROR: Invalid command line argument format. Please check the provided arguments.\n");
        printf("INFO: For help, run \"./mp3_tag_reader --help\n");
    }
    else
    {
        /* Function call for check operation types */
        operationType ret_value = check_operation (argv);
        
        if(ret_value == r_view)
        {
            printf("----------------------------------------\n");
            printf("\tMP3 TAG READER & EDITOR\n");
            printf("----------------------------------------\n");
        
        /* Function call for validation check */
        Status status_ret_value = read_validate_mp3_file(argv, &tag_info_mp3);
        if(ret_value == r_success )
        {
            status_ret_value = view_tag_info(argv, &tag_info_mp3);
        }

        if(status_ret_value == r_success)
        {
            printf("\nInfo Done");
            printf("\n----------------------------------------\n");
        }

        }

        /* Edit section for MP3 tag reader and editor */
        else if(ret_value == r_edit)
        {
            printf("----------------------------------------\n");
            printf("\tMP3 TAG READER & EDITOR\n");
            printf("----------------------------------------\n");
        
        /* Function call for validation check */
        Status status_ret_value = read_and_validate_mp3_file_args(argv, &tag_Data);

        if(status_ret_value == r_success)
        {
            Status status_ret_value2 = edit_tag(argv, &tag_Data);

            if(status_ret_value2 == r_success)
            {
                 printf ("INFO: Done.\n");
                 printf ("--------------------------\n");
            }

        }


        }

        /* Help section for MP3 tag reader and editor */
        else if(ret_value == r_help)
        {
            printf("Help Menu for MP3 Tag Reader and Editor:\n");
            printf("-----------------------------------------\n");
            printf("Viewing Tags:\n");
            printf("./mp3_tag_reader -v <file_name.mp3>\n");
            printf("-----------------------------------------\n");
            printf("Editing Tags:\n");
            printf("./mp3_tag_reader -e <modifier> \"New_Value\" <file_name.mp3>\n");
            printf("-----------------------------------------\n");
            printf("Available Modifiers:\n");
            printf("-t  Modify Title Tag\n");
            printf("-A  Modify Artist Tag\n");
            printf("-a  Modify Album Tag\n");
            printf("-y  Modify Year Tag\n");
            printf("-G  Modify Content Type Tag\n");
            printf("-c  Modify Comments Tag\n");
            printf("-----------------------------------------\n");

        }
    }
}