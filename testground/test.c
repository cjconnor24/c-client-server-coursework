#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
int main () {


	// ORIGINAL FILE
   FILE *forig;
   forig = fopen("scrnshot.png", "rb");

// OPEN THE NEW FILE
FILE *fnew = fopen("watchawa.png","wb");
fseek(forig, 0L, SEEK_END);

// GET THE SIZE
int sz = ftell(forig);

// CREATE CORRECT SIZE SPACE
unsigned char *buffer = (unsigned char *)malloc(sizeof(char)*sz);

	// GO TO START OF FILE
	fseek(forig, 0, SEEK_SET);

	// READ THE DATA
	fread(buffer, sz, 1, forig);

// WRITE THE DATA TO THE BUFFER
fwrite(buffer,1,sz,fnew);

//CLOSE THE NEW FILE
fclose(fnew);

	// CLOSE THE OLD FILE
   fclose(forig);

// FREE THE HEAP MEME
  free(buffer); 
   return(0);
}
