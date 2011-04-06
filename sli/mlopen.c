/* Interface File that deals with the link from Mathematica to SLI.
Six Functions are implemented here:
1. MathLinkInit: Sets up the link with a variable port.
2. MathLinkSendPort: Sets up a link to Mathematica with the fixed port 8000 
   and sends the port-number of the Interface port through the link. 
   After that the link is closed.
3. MathLinkGetCharString: Takes the string from the link. 
4. MathLinkDisownCharString: Disowns a pointer from a String-Object of the link.
5. MathLinkPutCharString: Puts a string on the link.
6. MathLinkClose: Closes the link.
 
27.11.02, Diesmann 
edited by Sirko Straube, 20.02.03 
updated for Mathematica 5.0.1, 18.4.04, Diesmann
*/

#include "mlopen.h"
#include "mathlink.h"

#include <stdio.h>

 MLENV env;
 MLINK link;
 long errno;

 mlapi_packet p;


void MathLinkInit(const char *args)
{
 const char * e;

 env = MLInitialize(0);

 link = MLOpenString(env, args, &errno);


 MLActivate(link);


 if (MLError(link))
 { 
  e=MLErrorMessage(link);
  printf("%s\n",e);
 }  
 else {
	printf("no error\n"); 
   }

}


/*
for Mathematica 5.0.1 all specifications of the MathLink protocol
(option -linkprotocol have been removed.
18.4.04, Diesmann
*/
/*
void MathLinkInit(void)
{
 const char * e;

 env = MLInitialize(0);

 link = MLOpenString(env, "-linkcreate", &errno );

 MathLinkSendPort();

 MLActivate(link);

 if (MLError(link))
 { 
  e=MLErrorMessage(link);
  printf("%s\n",e);
 }  
 else {
	printf("no error\n"); 
   }

}
*/



/* 20.02.03 Straube  */
/*
void MathLinkSendPort(void) {

    MLENV portenv;
    MLINK portlink;
    long porterr;
    const char * e;
    
    portenv = MLInitialize(0);

    portlink = MLOpenString(portenv, "-linkcreate -linkname 8000 ", &porterr );
    
    MLActivate(portlink);

    MLPutString(portlink,MLName(link)); 

    MLFlush(portlink);

    if (MLError(portlink))
    { 
	e=MLErrorMessage(portlink);
	printf("%s\n",e);
    }
   else {
	printf("no error on 8000\n"); 
   }

    MLClose(portlink);
} 
*/


int MathLinkGetCharString(const char** b)
{
 int valid=0;
 const char * e;

 if (p = MLNextPacket(link))
 {
     switch (p) {
	 case TEXTPKT : 
	     valid=1;
	     MLGetString(link, b);
     }
 
     MLNewPacket(link);
     
 
     if (MLError(link))
     { 
	 e=MLErrorMessage(link);
	 printf("%s\n",e);
     }
     
 }
 return valid;
}

void MathLinkPutCharString(const char *s)
{
 MLPutString(link,s); 
}

void MathLinkDisownCharString(const char *b)
{
 MLDisownString(link,b);
}

void MathLinkFlush(void)
{
 MLFlush(link);
}

void MathLinkClose(void)
{
 MLClose(link);
}





















































































