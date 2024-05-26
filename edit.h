#ifndef EDIT_H
#define EDIT_H

#include "type.h" 

/*
 * Structure to store information required for updating the frame content details.
 * This structure holds information about the source and temporary MP3 files.
 */

typedef struct _tagData
{
    /* Source MP3 file pointer. */
    FILE *fptr_mp3;

    /* Temporary MP3 file pointer. */
    FILE *fptr_temp;

    /* Frame ID extracted from MP3 (4 bytes + null terminator). */
    char frame_Id[5];

    /* Updated frame content value (up to 100 characters). */
    char frame_Id_value[100];

    /* Size of the frame content. */
    uint frame_Id_size;

} tagData;


/* Function prototype to edit Mp3 tag info */

/* Validate the command-line arguments and store them in the structure. */
Status read_and_validate_mp3_file_args(char* argv[], tagData* tag_Data);    //Done

/* Edit the tag details in the MP3 file. */
Status edit_tag(char* argv[], tagData* tag_Data);

/* Copy frame contents from the source to the temporary MP3 file. */
Status copy_data(tagData* tag_Data, char flag);

/* Copy the remaining data after the frames from source to temporary MP3 file. */
Status copy_remaining_data(tagData* tag_Data);

/* Copy the complete data from the temporary to the source MP3 file. */
Status copy_back_to_src(tagData* tag_Data);

void reverse_bytes(void* data, size_t size);

#endif
