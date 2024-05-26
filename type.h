#ifndef TYPE_H
#define TYPE_H

/* Typedef for unsigned int */
typedef unsigned int uint;

/* Typedef enum for return type for functions */
typedef enum
{
    r_success,
    r_failure

} Status;

/* Typedef enum for */
typedef enum
{
    r_view,
    r_edit,
    r_help,
    r_unsupported

} operationType;

#endif
