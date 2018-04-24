// DX.H - External definitions for the DX package

#define DX_NHANDLES         10          // Max # of handles

#define DX_NOERR             0
#define DX_NOHANDLES        -1
#define DX_BADHANDLE        -2
#define DX_NOTRECEIVER      -3
#define DX_NOTSENDER        -4
#define DX_FULL             -5
#define DX_NOTCTSACK        -6
#define DX_NOTRTSDR1DR2     -7

short dx_open_recv(short base, char *buf, short bufsize);
short dx_getchar(short handle, char *c);
short dx_recv(short handle);
short dx_reset(short handle);
short dx_clearbuf(short handle);
short dx_getcount(short handle);
short dx_close(short handle);
char *dx_err(short err);
