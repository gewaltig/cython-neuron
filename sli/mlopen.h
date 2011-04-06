/* 27.11.02, Diesmann
   edited by Sirko Straube, 26.02.03 */

void MathLinkInit(const char *);
int MathLinkGetCharString(const char**);
void MathLinkPutCharString(const char *);
/* void MathLinkSendPort(void); */
void MathLinkDisownCharString(const char *);
void MathLinkFlush(void);
void MathLinkClose(void);
