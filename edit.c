/* Include all essential header for file for editing operation */
#include <stdio.h>
#include <string.h>
#include "type.h"
#include "edit.h"

/* Function for reverse the data 
void reverse_bytes(void* data, size_t size)
{
    char* pos = (char*)data;
    for (size_t i = 0; i < size / 2; i++) {
        char temp = pos[i];
        pos[i] = pos[size - i - 1];
        pos[size - i - 1] = temp;
    }
}
*/


/* Function call to validate the file before edit */
Status read_and_validate_mp3_file_args(char* argv[], tagData* tag_Data)
{
    if(argv[2] == NULL)
    {
        printf("INFO: Edit tags using: ./mp3_tag_reader -e <modifier> \"New_Value\" <file_name.mp3>\n");
        printf("Modifiers: -t Title, -A Artist, -a Album, -y Year, -G Content Type, -c Comments\n");
        return r_failure;
    }
    else
    {
        /* Determine and set the frame ID based on the provided modifier in argv[2]. */

        if(strncmp(argv[2], "-t", 2) == 0)
        {
            strncpy(tag_Data -> frame_Id, "TIT2", 5); //5 because for NULL character.
        }
        else if(strncmp(argv[2], "-A", 2) == 0)
        {
            strncpy(tag_Data -> frame_Id, "TPE1", 5);
        }
        else if(strncmp(argv[2], "-a", 2) == 0)
        {
            strncpy(tag_Data -> frame_Id, "TALB", 5);
        }
        else if(strncmp(argv[2], "-y", 2) == 0)
        {
            strncpy(tag_Data -> frame_Id, "TYER", 5);
        }
        else if(strncmp(argv[2], "-G", 2) == 0)
        {
            strncpy(tag_Data -> frame_Id, "TCON", 5);
        }
        else if(strncmp(argv[2], "-c", 2) == 0)
        {
            strncpy(tag_Data -> frame_Id, "COMM", 5);
        }
        else
        {
            /* Print error message for unsupported modifier and display available modifier functions. */
            printf("ERROR: Unsupported Modifier.\n");
            printf("INFO: Available Modifiers: -t, -A, -a, -y, -G, -c\n");
            return r_failure;

        }
        printf("Changing frame ID to: %s\n", tag_Data ->frame_Id);
    }

    if(argv[3] == NULL)
    {
        printf("ERROR: Empty New_Value for Frame ID %s.\n", tag_Data -> frame_Id);
        printf("INFO: Usage: ./mp3_tag_reader -e <modifier> \"New_Value\" <file_name.mp3>\n");
        return r_failure;
    }

    else
    {
        tag_Data -> frame_Id_size = strlen(argv[3]) + 1;
        if(tag_Data -> frame_Id_size < 100)
        {
            printf("Length of the input data: %d characters.\n", tag_Data -> frame_Id_size);
            strncpy(tag_Data -> frame_Id_value, argv[3], tag_Data -> frame_Id_size);
            printf("Frame ID updated with value: %s.\n", tag_Data -> frame_Id_value);

        }

        else
        {
            printf("ERROR: Input data exceeds maximum allowed length.\n");
            return r_failure;

        }       
    }

    if(argv[4] == NULL)
    {
        printf("INFO: Usage: ./mp3_tag_reader -e <modifier> \"New_Value\" <file_name.mp3>\n");
        return r_failure;
    }
    else
    {
        tag_Data -> fptr_mp3 = fopen(argv[4], "r");
        if(tag_Data -> fptr_mp3 != NULL)
        {
            char temp_buffer[3];
            fread(temp_buffer, 1, 3, tag_Data -> fptr_mp3);
            if(strncmp(temp_buffer, "ID3", 3) == 0)
            {
                printf("\tVersion ID: v2.3\t");
		        printf("\n----------------------------------------\n");
            }
            else
            {
                printf("ERROR: Invalid file signature. This doesn't seem to be an '.mp3' file.\n");
                printf("INFO: Usage: ./mp3_tag_reader -e <modifier> \"New_Value\" <file_name.mp3>\n");
                return r_failure;
            }
        }
        else
        {
            printf("ERROR: Failed to open the file: %s\n", argv[4]);
            printf("INFO: Usage: ./mp3_tag_reader -e <modifier> \"New_Value\" <file_name.mp3>\n");
            return r_failure;
        }

        fclose(tag_Data -> fptr_mp3);
    }
    return r_success;
}


/* The edit_tag function is used to edit the tags of an MP3 file based on the given command-line arguments */
Status edit_tag(char* argv[], tagData* tag_Data)
{
    /* Opening the require files */

    tag_Data -> fptr_mp3 = fopen(argv[4], "r");             // read mode
    tag_Data -> fptr_temp = fopen("temp.mp3", "w");         // write mode

    /* Resets the file pointers to the beginning of both files. */
    fseek(tag_Data -> fptr_mp3, 0L, SEEK_SET);
    fseek(tag_Data -> fptr_temp, 0L, SEEK_SET);

    /* Initial data copying & writing */
    char tempBuffer[10];
    int counter = 0;

    counter = fread(tempBuffer, 1, 10, tag_Data -> fptr_mp3);                   // Read 10 byte of data form source file

    if(counter < 10)                                                           // error handling
    {
        printf("ERROR: Failed to read from the MP3 file.\n");
        return r_failure;
    }
    else
    {
    counter = fwrite(tempBuffer, 1, 10, tag_Data -> fptr_temp);               // Write copied data in temp file
    if(counter < 10)
    {
        printf("ERROR: Failed to write from the MP3 file.\n");
        return r_failure;
    }
    }

    /* Frame Tag array */
    char* frame_tag[6] = {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"};
    
    /* Loop through each frame ID extracted from the MP3 file */
    for(int i = 0; i < 6; i++)
    {
        counter = fread(tempBuffer, 1, 4, tag_Data -> fptr_mp3);
        if(counter < 4)
        {
            printf("ERROR: Failed to read from the MP3 file.\n");
            return r_failure;
        }
        else
        {
            counter = fwrite(tempBuffer, 1, 4, tag_Data -> fptr_temp);
            if(counter < 4)
            {
                 printf("ERROR: Failed to write from the MP3 file.\n");
                 return r_failure;
            }
            else
            {
                if(strncmp(tag_Data -> frame_Id, frame_tag[i], 4) == 0)
                {
                    /* If tag matches with predefined tag array -> flag = 0*/
                    Status status_ret_value = copy_data(tag_Data, 1);
                    if(status_ret_value == r_failure)
                    {
                        printf("ERROR: Failed to copy updated data to the MP3 file.\n");
                        return r_failure;
                    }
                }
                else
                {
                    /* If tag not matches with predefined tag array -> flag = 0*/
                    Status status_ret_value = copy_data(tag_Data, 0);
                    if(status_ret_value == r_failure)
                    {
                        printf("ERROR: Failed to copy updated data to the MP3 file.\n");
                        return r_failure;
                    }
                }
            }
        }
    }
    
    /* Copying the remaining data */
    Status status_ret_value;
    status_ret_value = copy_remaining_data(tag_Data);
    if(status_ret_value == r_failure)
    {
        printf("ERROR: Unable to copy the remaining data to the destination '.mp3' file.\n");
        return r_failure;
    }

    /* Close the all files */
    fclose(tag_Data -> fptr_mp3);
    fclose(tag_Data -> fptr_temp);

    /* Paste the copied data from temp file -> source file */

    tag_Data -> fptr_mp3 = fopen(argv[4], "w");                // write mode
    tag_Data -> fptr_temp = fopen("temp.mp3","r");             // read mode

    status_ret_value = copy_back_to_src(tag_Data);
    if(status_ret_value == r_failure)
    {
        printf("ERROR: Failed to copy remaining data from destination to source '.mp3' file.\n");
        return r_failure;
    }

    return r_success;
}

/* copy data function copy the data form source file -> temp file */
Status copy_data(tagData* tag_Data, char flag)
{
    /* If flag is 1 & tag matches with the predefined tag array */
    if(flag == 1)
    {
        uint old_Size, new_Size;
        int counter;
        
        //Read the old size for source file
        counter = fread(&old_Size, 4, 1, tag_Data -> fptr_mp3);
        if(counter < 1)
        {
           printf("ERROR: Failed to read from the MP3 file.\n");
           return r_failure;
        }
        else
        {
             char* pos = (char*) &old_Size;
            for (int i = 0; i < ((sizeof (uint)) / 2); i++)     //Reversal of the Bytes after reading to get the correct value of the 'Size'.
            {
                pos [i] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
                pos [(sizeof (uint)) - i - 1] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
                pos [i] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
            }
            
        }
          printf("Old Tag Data Size = %d.\n", old_Size);

        //Read new size form temp file
        new_Size = tag_Data -> frame_Id_size;
        char* pos = (char*) &tag_Data -> frame_Id_size;
        for (int i = 0; i < ((sizeof (uint)) / 2); i++)     //Reversal of the Bytes after reading to get the correct value of the 'Size'.
        {
            pos [i] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
            pos [(sizeof (uint)) - i - 1] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
            pos [i] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
        }
        
        printf("New Tag Data Size = %d.\n",new_Size);

        /* Write new_size to mp3tagData->fptr_temp */
        counter = fwrite(&tag_Data -> frame_Id_size, 4, 1, tag_Data -> fptr_temp);
        if(counter < 1)
        {
            printf("ERROR: Unable to write on the MP3 file.\n");
            return r_failure;
        }

        /* Copy the flag & additional data 3byte */

        char temp_Buffer[3];
        counter = fread(temp_Buffer, 1, 3, tag_Data -> fptr_mp3);
        if(counter < 3)
        {
             printf ("ERROR: Unable to read from the MP3 file.\n");
             return r_failure;
        }
        else
        {
            counter = fwrite(temp_Buffer, 1, 3, tag_Data -> fptr_temp);
            if(counter < 3)
            {
                printf ("ERROR: Unable to write from the MP3 file.\n");
                return r_failure;
            }
        }

        /* write the frame data value from temp -> structure buffer */
        counter = fwrite(tag_Data -> frame_Id_value, 1, (new_Size - 1), tag_Data -> fptr_temp);
        if(counter < (new_Size - 1))
        {
            printf ("ERROR: Unable to write from the MP3 file.\n");
            return r_failure;
        }
        fseek(tag_Data -> fptr_mp3, (long)(old_Size - 1), SEEK_CUR);
        printf("New Data (Frame ID Value): %s\n", tag_Data -> frame_Id_value);
        printf("Current Position in MP3 File: %lu bytes\n", ftell(tag_Data -> fptr_mp3));
        printf("Current Position in Temporary File: %lu bytes\n", ftell(tag_Data -> fptr_temp));
    }

    /* If flag is 0 & tag not matches with the predefined tag array */
    else
    {
        int frameSize;
        int counter;

        /* Write the frame id size in temprory file */
        counter = fread(&frameSize, 4, 1, tag_Data -> fptr_mp3);
        if(counter < 1)
        {
            printf("ERROR: Failed to read from the MP3 file while reading 'size'.\n");
            return r_failure;
        }
        else
        {
            counter =  fwrite(&frameSize, 4, 1, tag_Data -> fptr_temp);
            if(counter < 1)
            {
                printf("ERROR: Failed to write 'size' to the MP3 file.\n");
                return r_failure;
            }
        }

        /* Reverse the byte */
         char* pos = (char*) &frameSize;
        for (int i = 0; i < ((sizeof (uint)) / 2); i++)     //Reversal of the Bytes after reading to get the correct value of the 'Size'.
        {
            pos [i] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
            pos [(sizeof (uint)) - i - 1] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
            pos [i] = pos [i] ^ pos [(sizeof (uint)) - i - 1];
        }

        printf("Frame ID content size: %u bytes.\n", frameSize);
        
        /* Write the frame id in temprory file */
        char frameId[3];
        counter = fread(frameId, 1, 3, tag_Data -> fptr_mp3);
        if(counter < 3)
        {
            printf("ERROR: Failed to read from the MP3 file during 'size' retrieval.\n");
            return r_failure;
        }
        else
        {
            counter = fwrite(frameId, 1, 3, tag_Data -> fptr_temp);
            if(counter < 3)
            {
                printf("ERROR: Failed to write 'size' to the MP3 file.\n");
                return r_failure;
            }
        }

        /* Write the frame data to temporary file */
        char frameData[frameSize - 1];
        counter = fread(frameData, 1, (frameSize - 1), tag_Data -> fptr_mp3);
        if(counter < (frameSize - 1))
        {
            printf("ERROR: Failed to read 'size' from the MP3 file.\n");
            return r_failure;
        }
        else
        {
            frameData[frameSize - 1] = '\0';
            printf("Frame ID content: \"%s\".\n", frameData);

            counter = fwrite(frameData, 1, (frameSize - 1), tag_Data -> fptr_temp);
            if(counter < (frameSize -1))
            {
                printf("ERROR: Failed to write 'size' to the MP3 file.\n");
                return r_failure;
            }


        }

        printf("Current position in MP3 file: %lu bytes.\n", ftell(tag_Data -> fptr_mp3));
        printf("Current position in temporary file: %lu bytes.\n", ftell(tag_Data -> fptr_temp));

    }
    return r_success;

}

Status copy_remaining_data(tagData* tag_Data)
{
    printf("Copy remaining data\n");
    char rData[1024];
    int counter;
    long start = ftell(tag_Data -> fptr_mp3);
    printf("Initial MP3 position: %lu bytes.\n", start);
    printf("Initial temporary file position: %lu bytes.\n", ftell(tag_Data -> fptr_temp));

    fseek(tag_Data -> fptr_mp3, 0L, SEEK_END);
    long end = ftell(tag_Data -> fptr_mp3);
    printf("MP3 position = %lu.\n", end);

    fseek(tag_Data -> fptr_mp3, start, SEEK_SET);
    printf("Current MP3 position: %lu bytes.\n", ftell(tag_Data -> fptr_mp3));

    while((ftell(tag_Data -> fptr_mp3) != end))
    {
        counter = fread(rData, 1, 1024, tag_Data -> fptr_mp3);
        if(counter < 0)
        {
            printf("ERROR: Failed to read data from the MP3 file.\n");
            return r_failure;
        }
        else
        {
            fwrite(rData, 1, counter, tag_Data -> fptr_temp);
        }
    }

    printf("Current MP3 position: %lu bytes.\n", ftell(tag_Data -> fptr_mp3));
    printf("Current temporary file position: %lu bytes.\n", ftell(tag_Data -> fptr_temp));

    return r_success;
}

Status copy_back_to_src(tagData* tag_Data)
{
    printf("Copy data back to source\n");
    fseek(tag_Data -> fptr_mp3, 0L, SEEK_SET);
    fseek(tag_Data -> fptr_temp, 0L, SEEK_SET);

    int counter;
    char sData[1024];

    long start = ftell(tag_Data -> fptr_temp);
    printf("Initial temporary file position: %lu bytes.\n", start);
    printf("Initial MP3 position: %lu bytes.\n", ftell(tag_Data -> fptr_mp3));

    fseek(tag_Data -> fptr_temp, 0L, SEEK_END);
    long end = ftell(tag_Data -> fptr_temp);
    printf("End MP3 position: %lu bytes.\n", end);

    fseek(tag_Data -> fptr_temp, 0L, SEEK_SET);
    printf("Current position in temporary file: %lu.\n", ftell(tag_Data -> fptr_temp));

     while ((ftell (tag_Data -> fptr_temp)) != end)
    {
        counter = fread (sData, 1, 1024, tag_Data -> fptr_temp);
        if (counter < 0)
        {
            printf ("ERROR: Unable to read from the MP3 file.\n");
            return r_failure;
        }
        else
        {
            fwrite (sData, 1, counter, tag_Data -> fptr_mp3);
        }
    }

    printf("Current MP3 position: %lu bytes.\n", ftell(tag_Data -> fptr_mp3));
    printf("Current temporary file position: %lu bytes.\n", ftell(tag_Data -> fptr_temp));

    return r_success;
 



}