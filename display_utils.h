#ifndef DISPLAY_UTILS_H
#define DISPLAY_UTILS_H

#include <stdbool.h>

#include "types.h"

// Function to display a table of type type
void display_table(procLL* procHead, int type, bool givenPid);
// Function to display processes exceeding a specified threshold of file
// descriptors
void display_offending(int threshold, procLL* procHead);
// Function to save the composite table to the txt file or to the binary file
void save_composite_to_file(procLL* procHead, const char* fileName,
                            bool saveIntoTxt, bool givenPid);

#endif  // DISPLAY_UTILS_H
