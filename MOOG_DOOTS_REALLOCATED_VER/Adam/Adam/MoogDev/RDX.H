/*  .title DX.H - External definitions for the DX package
;+
; RDX TTL I/O Support 24Jun03
;   When TTL is implemented, it shouldn't affect RDX Lib users who don't use it.  TTL
;   (Out of Band TTL i/o) implements 2 data bits in both direction which the application
;   can use to communicate information that is not synchronized with the transmission.
;   Essentially, it allows the top two bits of CH and CL to be read/written by
;   the application independently of the RDX protocol.
;
;   The real value here is that VideoSYNC can use the TTL bits to tell the
;   TEMPO server when it is finished processing all commands in its input buffer.
;
;   The RDXTTLSUPPORT preprocessor definition is set to 0 or non-zero.  If 0,
;   all TTL support is omitted and this package is compatible with pre-24Jun03
;   RDX programs.  If set to non-zero, then TTL support is included and the
;   resulting OBJs are not compatible with non-TTL RDX programs.
;
; EDIT HISTORY
;   11May01 sh  Remove definitions for DX_NOTCTSACK and DX_NOTRDSDR1DR2
;   25Jun03 sh  Add DXTTLSUPPORT preprocessor definitions
;   27Jun03 sh  Revert TTL names to TTLin and TTLout
;   30Jun03 sh  move send functions to dxsend.h
;               Add DX_NODRIVER for win32 support of giveio
;-
*/
#ifndef MSDOS                   // win32?
    #define inp     _inp        // win32 prefixes name with underscore
    #define outp    _outp       // win32 prefixes name with underscore
#endif

#define DXTTLSUPPORT         1  // 0=no TTL support, pre 24Jun03 RDX compatible
                                // 1=TTL support, not compatible with previous RDX

#define DX_NHANDLES          4  // Max # of RDX handles that we support at one time

#define DX_NOERR             0
#define DX_NOHANDLES        -1
#define DX_BADHANDLE        -2
#define DX_NOTRECEIVER      -3
#define DX_NOTSENDER        -4
#define DX_FULL             -5
#define DX_NODRIVER         -6

typedef void (*FCNPTR)(char *);

short dx_open_recv(short base, char *buf, short bufsize);
short dx_getchar(short handle, char *c);
short dx_recv(short handle);
short dx_reset(short handle);
short dx_clearbuf(short handle);
short dx_getcount(short handle);
short dx_close(short handle);
char *dx_err(short err);
FCNPTR dx_callback(short, FCNPTR);


short dx_GetTTLOUT(short nHandle);
short dx_SetTTLOUT(short nHandle, short n);
short dx_GetTTLIN(short nHandle);

