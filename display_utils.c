#include "display_utils.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// Function to print different headers based on the type
void print_header(int type) {
  // Type 0: Header for per-process display
  if (type == 0) {
    printf("%-4s %-8s \t %-5s \n", " ", "PID", "FD");
    printf("%-4s ==================================\n", " ");
    // Type 1: Header for systemWide display
  } else if (type == 1) {
    printf("%-4s %-8s \t %-5s \t %-12s\n", " ", "PID", "FD", "Filename");
    printf("%-4s ===================================================\n", " ");
    // Type 2: Header for Vnodes display
  } else if (type == 2) {
    printf("%-4s %-8s\n", " ", "Inode");
    printf("%-4s ==========================\n", " ");
    // Type 3: Header for composite display
  } else {
    printf("%-4s %-8s \t %-5s \t %-12s \t %-8s\n", " ", "PID", "FD", "Filename",
           "Inode");
    printf("%-4s ===========================================================\n",
           " ");
  }
}

// Function to print a line of data with a line number
void print_line_with_number(int type, int num, procLL* procNode,
                            fdInfo* fdNode) {
  // Checks the type of data to be printed and formats accordingly
  if (type == 0) {  // per-process
    printf("%-4d %-8d \t %-5d\n", num, procNode->pid, fdNode->fd);
  } else if (type == 1) {  // systemWide
    printf("%-4d %-8d \t %-5d \t %-12s\n", num, procNode->pid, fdNode->fd,
           fdNode->fileName);
  } else if (type == 2) {  // Vnodes
    printf("%-4d %-8ld\n", num, fdNode->iNode);
  } else {  // composite
    printf("%-4d %-8d \t %-5d \t %-12s \t %-8ld\n", num, procNode->pid,
           fdNode->fd, fdNode->fileName, fdNode->iNode);
  }
}

// Function to print a line of data without a line number
void print_line_without_number(int type, procLL* procNode, fdInfo* fdNode) {
  // Formats the line based on the type without the line number
  if (type == 0) {
    printf("%-4s %-8d \t %-5d\n", " ", procNode->pid, fdNode->fd);
  } else if (type == 1) {
    printf("%-4s %-8d \t %-5d \t %-12s\n", " ", procNode->pid, fdNode->fd,
           fdNode->fileName);
  } else if (type == 2) {
    printf("%-4s %-8ld\n", " ", fdNode->iNode);
  } else {
    printf("%-4s %-8d \t %-5d \t %-12s \t %-8ld\n", " ", procNode->pid,
           fdNode->fd, fdNode->fileName, fdNode->iNode);
  }
}

// Function to display a table of type type
void display_table(procLL* procHead, int type, bool givenPid) {
  // Prints the appropriate header based on the type given
  print_header(type);
  procLL* procNode = procHead;
  int lineNumber = 0;
  // Iterates over each process node
  while (procNode != NULL) {
    fdInfo* fdNode = procNode->fdHead;
    // Iterates over each file descriptor node
    while (fdNode != NULL) {
      // Chooses the appropriate printing function based on whether a PID is
      // given
      if (!givenPid) {
        print_line_with_number(type, lineNumber, procNode, fdNode);
        ++lineNumber;
      } else {
        print_line_without_number(type, procNode, fdNode);
      }
      fdNode = fdNode->next;
    }
    procNode = procNode->next;
  }
}

// Function to display processes exceeding a specified threshold of file
// descriptors
void display_offending(int threshold, procLL* procHead) {
  printf("## Offending processes -- #FD threshold=%d\n", threshold);
  procLL* procNode = procHead;
  while (procNode != NULL) {  // Iterate over all process linked list nodes
    int numberOfFD = 0;       // Initialize file descriptor counter
    fdInfo* fdNode = procNode->fdHead;
    while (fdNode !=
           NULL) {  // Count the number of file descriptors for the process
      ++numberOfFD;
      fdNode = fdNode->next;
    }
    if (numberOfFD > threshold) {  // If FD count exceeds threshold, print
                                   // process ID and FD count
      printf(" %d (%d), ", procNode->pid, numberOfFD);
    }
    procNode = procNode->next;  // Move to the next process node
  }
  printf("\n");  // Print newline at the end of the output
}

// Function to save the composite table to the txt file or to the binary file
void save_composite_to_file(procLL* procHead, const char* fileName,
                            bool saveIntoTxt, bool givenPid) {
  // Open file for writing, in text or binary mode based on saveIntoTxt
  FILE* fp = fopen(fileName, saveIntoTxt ? "w" : "wb");
  if (!fp) {  // If file opening fails, print error and exit
    fprintf(stderr, "Error opening file %s\n", fileName);
    exit(1);
  }

  // Print header to the file based on the mode (text or binary)
  if (saveIntoTxt) {
    fprintf(fp, "%-4s %-8s \t %-5s \t %-12s \t %-8s\n", " ", "PID", "FD",
            "Filename", "Inode");
    fprintf(
        fp,
        "%-4s ===========================================================\n",
        " ");
  } else {
    char str1[] = "    PID       FD       Filename      Inode\n";
    char str2[] = "==========================================\n";
    fwrite(str1, sizeof(char), sizeof(str1), fp);
    fwrite(str2, sizeof(char), sizeof(str2), fp);
  }

  procLL* procNode = procHead;
  int lineNumber = 0;  // Initialize line number for text mode with line numbers
  while (procNode != NULL) {  // Iterate over all processes
    fdInfo* fdNode = procNode->fdHead;
    while (fdNode !=
           NULL) {  // Iterate over all file descriptors of the current process
      // Write process and file descriptor info to the file based on the mode
      if (saveIntoTxt) {  // For txt mode, print a formatted string for each
                          // file descriptor to the file
        if (givenPid) {
          fprintf(fp, "%-4s %-8d \t %-5d \t %-12s \t %-8ld\n", " ",
                  procNode->pid, fdNode->fd, fdNode->fileName, fdNode->iNode);
        } else {
          fprintf(fp, "%-4d %-8d \t %-5d \t %-12s \t %-8ld\n", lineNumber,
                  procNode->pid, fdNode->fd, fdNode->fileName, fdNode->iNode);
          ++lineNumber;
        }
      } else {  // For binary mode, write binary data for each file descriptor
        if (givenPid) {
          fwrite(&(procNode->pid), sizeof(procNode->pid), 1, fp);
          fwrite(&(fdNode), sizeof(fdNode), 1, fp);
        } else {
          fwrite(&(lineNumber), sizeof(lineNumber), 1, fp);
          fwrite(&(procNode->pid), sizeof(procNode->pid), 1, fp);
          fwrite(&(fdNode), sizeof(fdNode), 1, fp);
          ++lineNumber;
        }
      }
      fdNode = fdNode->next;  // Move to the next file descriptor node in the
                              // linked list
    }
    procNode =
        procNode->next;  // Move to the next process node in the linked list
  }

  // Close the file and handle potential error
  if (fclose(fp) == EOF) {
    fprintf(stderr, "Error closing file %s\n", fileName);
  }
}