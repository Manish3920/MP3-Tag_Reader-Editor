/* Include all essential header for file for viewing operation */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "type.h"
#include "view.h"

/* 
 * Determine the Operation to be Performed
 *
 * This function analyzes the command-line arguments to identify the operation 
 * requested by the user.
 *
 * Input: Command Line Arguments (argv)
 * Output: Operation Type (r_view, r_edit, r_help, or r_unsupported)
 * Return Value: 
 *   - r_view: If the user requests to view the MP3 tag.
 *   - r_edit: If the user requests to edit the MP3 tag.
 *   - r_help: If the user requests help or information.
 *   - r_unsupported: If an unsupported or invalid option is provided by the user.
 */

/* Function defination for operationType() */
operationType check_operation(char* argv[])
{
    if(strncmp(argv[1],"-v",2) == 0)
    {
	return r_view;
    }
    else if(strncmp(argv[1],"-e",2) == 0)
    {
	return r_edit;
    }
    else if(strncmp(argv[1],"-h",2) == 0)
    {
	return r_help;
    }
    else
    {
	return r_unsupported;
    }
}

/* 
 * Validate Command Line Arguments
 * 
 * Checks the validity of provided command-line arguments.
 * Returns p_success on success, p_failure otherwise.
 */

Status read_validate_mp3_file(char* argv[], tagInfo *tag_Info)
{
    if(argv[2] == NULL)
    {
	printf ("\nINFO: For Viewing the Tags -> ./mp3_tag_reader -v <file_name.mp3>\n");
	return r_failure;
    }
    else
    {
	tag_Info -> fptr_mp3 = fopen(argv[2], "r");    //open file in read mode

    //Success in open file
	if(tag_Info -> fptr_mp3 != NULL)               
	{
	    char tempBuffer[3];
	    fread(tempBuffer, 3, 1, tag_Info -> fptr_mp3);    //Read the signature and copy in buffer
	    if((strncmp(tempBuffer, "ID3", 3)) == 0)          //Compare the signature match with "ID3"
	    {
		printf("\tVersion ID: v2.3\t");
		printf("\n----------------------------------------\n");
	    }
	    else
	    {
		printf("ERROR: The file signature does not match that of an MP3 file.\n");
		printf("INFO: To view tags, use: ./mp3_tag_reader -v <file_name.mp3>\n");
		return r_failure;

	    }
    }

    //Fail to open the file 
    else
    {
        printf("ERROR: Unable to open the file: %s\n", argv[2]);
        printf("INFO: To view tags, use: ./mp3_tag_reader -v <file_name.mp3>\n");
        return r_failure;
    }
    fclose(tag_Info -> fptr_mp3);
    }
    return r_success;
}



/*
 * View all the frame contents of the MP3 file tag.
 *
 * Input: Command Line Arguments and Structure Pointer
 * Output: Display all frame contents
 * Return Value: p_success if successful, p_failure otherwise
 */

Status view_tag_info (char* argv[], tagInfo *tag_info)
{
    tag_info -> fptr_mp3 = fopen(argv[2], "r");    //Open the file in read mode
    fseek(tag_info -> fptr_mp3, 10L, SEEK_SET);    //Skip the first 10 byte ID3 header file

    Status ret_value;

    /* Funnction call to display the Title tag info */
    ret_value = display_mp3_data ("TITLE: ", "TIT2", tag_info -> frame_id, &tag_info -> title_tag_size, tag_info -> title_tag, tag_info -> fptr_mp3);
    if(ret_value == r_failure)
    {
        printf("ERROR: Unable to display the contents of the Title Frame ID.\n");
        return r_failure;

    }

    /* Function call to display the Artist tag info */
    ret_value = display_mp3_data("Artist: ", "TPE1", tag_info -> frame_id, &tag_info -> artist_tag_size, tag_info -> artist_tag, tag_info -> fptr_mp3);
    if(ret_value == r_failure)
    {
        printf("ERROR: Unable to display the contents of the Artist Frame ID.\n");
        return r_failure;

    }

    /*Function call to display the album tag info */
    ret_value = display_mp3_data("Album: ", "TALB", tag_info -> frame_id, &tag_info -> album_tag_size, tag_info -> album_tag, tag_info -> fptr_mp3);
    if(ret_value == r_failure)
    {
        printf("ERROR: Unable to display the contents of the Album Frame ID.\n");
        return r_failure;

    }

    /* Function call to display the year tag info */
    ret_value = display_mp3_data("Year: ", "TYER", tag_info -> frame_id, &tag_info -> year_info_size, tag_info -> year_info, tag_info -> fptr_mp3);
    if(ret_value == r_failure)
    {
        printf("ERROR: Unable to display the contents of the Year Frame ID.\n");
        return r_failure;

    }
    


    /* Function call to display the content tag info */
    ret_value = display_mp3_data("Content: ", "TCON", tag_info -> frame_id, &tag_info -> content_type_size, tag_info -> content_type_info, tag_info -> fptr_mp3);
    if(ret_value == r_failure)
    {
        printf("ERROR: Unable to display the contents of the Content Type Frame ID.\n");
        return r_failure;

    }

    /* Function call to display the comment tag info */
    ret_value = display_mp3_data("Comment: ", "COMM", tag_info -> frame_id, &tag_info -> comment_size, tag_info -> comment_info, tag_info -> fptr_mp3);
    if(ret_value == r_failure)
    {
        printf("ERROR: Unable to display the contents of the Content Type Frame ID.\n");
        return r_failure;

    }

    return r_success; //No failure 

}

/*
 * View the contents of a specific Frame ID from an MP3 file.
 *
 * Input: 
 *   - Desired Frame ID Tag 
 *   - Actual Frame ID Tag extracted from '.mp3' file 
 *   - Frame ID size 
 *   - Frame ID contents 
 *   - File pointer to the MP3 file
 */

Status display_mp3_data (const char* str_frame, const char* str_Id, char* frame_Id, uint* tag_size, char* tag_buffer, FILE* fptr)
{
    //printf("Attempting to read Frame ID: %s\n", str_Id); // Debug print
    int counter;                                          //Counter to count number of item successfully scan
    printf("MP3 position = %ld.\n", ftell(fptr));         //Print current position of file pointer
    counter = fread(frame_Id, 1, 4, fptr);                //Read the 4 byte from the source file
    //printf("Read Frame ID from file: %.4s\n", frame_Id);  // Debug print to display the read Frame ID

    if(counter < 4)                                       //Error handling
    {
        printf("ERROR: Failed to read the MP3 file.\n");
        return r_failure;

    }
    else
    {
        if((strncmp(frame_Id, str_Id, 4)) == 0)          //Check if the extracted Frame ID matches the desired Frame ID for displaying its contents.
        {
            counter = fread(tag_size, 4, 1, fptr);


            if(counter < 1)                              //Error handling
            {
                printf("ERROR: Failed to read the MP3 file.\n");
                return r_failure;
            }

            /*
            * Note: The `fread()` function stores the size in reverse byte order compared to the Hex dump of the MP3 file.
            * 
            * Example:
            * Hex dump:    00 00 00 2f (MSB to LSB)
            * `fread()` stores it as: 00 00 00 2f (LSB to MSB)
            * 
            * Therefore, the bytes in 'tag_size' need to be reversed after reading to obtain the correct value.
            */

           size_t size = sizeof(uint);

           char *byte = (char*) tag_size;                //Convert the tag_size in char so we can access single byte
           
           /* Logic for reversing the the byte */
           for(int i = 0; i < (size/2); i++)
           {
             byte[i] = byte[i] ^ byte[size - i - 1];
             byte[size - i - 1] = byte[i] ^ byte[size - i - 1];
             byte[i] = byte[i] ^ byte[size - i - 1];
           }

           printf("%sSize = %u.\n", str_frame, *tag_size);

           tag_buffer = (char*) malloc((*tag_size) * sizeof(char));       // Dynamically allocate memory to store the frame ID content.
           if(tag_buffer == NULL)
           {
                printf("ERROR: Memory allocation failed.\n");
                return r_failure;
           }

           fseek(fptr, 3L, SEEK_CUR);                                     // Skip 1 byte included in the size and 2 additional bytes of Flag, totaling 3 bytes to be skipped.

         /* Skip 3 bytes (1 byte for the Flag and 2 bytes for additional Flag data) 
         and read (tag_size - 1) bytes for the Frame ID content.*/

           counter = fread(tag_buffer, 1, (*tag_size - 1), fptr);

           if(counter < (*tag_size - 1))                                  // Error handling
           {
                printf("ERROR: Failed to read from the MP3 file.\n");
                return r_failure;
           }
           else
           {
            tag_buffer[*tag_size - 1] = '\0';                            //Store '\0' at end of frame id conteent
            printf("%-10s%s.\n",str_frame,tag_buffer);                   //Display frame id content
           }
           
        }
        else
        {
            printf("ERROR: Frame ID '%s' not found.\n", str_Id);
            return r_failure;
        }

    }
    free(tag_buffer);                                                  //Free allocated memory
    tag_buffer = NULL;                                                 //Avoid dagling pointer

    return r_success;
}





