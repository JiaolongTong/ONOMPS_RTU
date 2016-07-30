#ifndef Communicate_h
#define Communicate_h
#include "otdr.h"
#include <mxml.h>   
#include <sys/stat.h>
#include <fcntl.h>

int saveRecvXML(char *data,int len);
char *getCommendString(int code);
int getCommendCode(mxml_node_t *root,mxml_node_t * tree);
int RespondMessage_TestData(otdr *PXX,int PS,int SNo, int mode);
/********Bsae64编解码********/
char *base64_encode(const char* data, int data_len); 
char *base64_decode(const char* data, int data_len); 
static char find_pos(char ch); 
int  read_file( char *filename, int *len, char **data );
#endif
