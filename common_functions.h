// HELPER FILE TO SHARE FUNCTIONALITY WITH BOTH CLIENT AND SERVER
// CHRIS CONNOR
#ifndef __COMMON_H__
#define __COMMON_H__

char *read_string(int socket);
void send_string(int socket, char *response);

#endif
