/* Header files for view operation */
#ifndef VIEW_H
#define VIEW_H

#include "type.h"

/*
 * -> Structure to store MP3 tag details and related output data.
 *
 *   This structure encapsulates information necessary to store and manage MP3 tag details.
 *   It also holds data related to the output, making it a comprehensive data structure
 *   for handling MP3 file metadata.
 */

typedef struct _tagInfo
{
    /* Source Mp3 file info */
    FILE *fptr_mp3;
    char frame_id[4];

    /* Title tag info */
    char *title_tag;
    uint title_tag_size;

    /* Artist tag info */
    char *artist_tag;
    uint artist_tag_size;

    /* Album tag info */
    char *album_tag;
    uint album_tag_size;

    /* Realease year info */
    char *year_info;
    uint year_info_size;

    /* Content type info */
    char *content_type_info;
    uint content_type_size;

    /* Comment info */
    char *comment_info;
    uint comment_size;

} tagInfo;

/* Function prototype to print Mp3 tag info */

/* Check operation type */
operationType check_operation (char* argv[]);

/* Validate the argument passed through CLA */
Status read_validate_mp3_file (char* argv[], tagInfo *tag_info);

/* View tag details */
Status view_tag_info (char* argv[], tagInfo *tag_info);

/* Display the content of Mp3 file */
Status display_mp3_data (const char* str_frame, const char* str_Id, char* frame_Id, uint* tag_size, char* tag, FILE* fptr);


#endif