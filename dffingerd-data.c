/* dffingerd-data by drt@ailis.de
 *
 * creates a data.cdb for dffingerd by gathering informations from data
 *
 * You might find more Info at http://rc23.cx/
 * 
 * I do not belive there is a thing like copyright.
 *
 * $Log: dffingerd-data.c,v $
 * Revision 1.2  2000/04/12 16:02:49  drt
 * fixed typos, compile time warnings
 *
 * Revision 1.1.1.1  2000/04/09 17:24:29  drt
 * initial revision
 *
 * Revision 1.1  2000/04/08 08:54:05  drt
 * Initial revision
 *
 */

#include <unistd.h>
#include <sys/stat.h>
#include <stdio.h>
#include "djb/cdb.h"
#include "djb/stralloc.h"
#include "djb/env.h"
#include "djb/readwrite.h"
#include "djb/open.h"
#include "djb/buffer.h"
#include "djb/str.h"
#include "djb/exit.h"
#include "djb/readwrite.h"
#include "djb/buffer.h"
#include "djb/strerr.h"
#include "djb/getln.h"
#include "djb/cdb_make.h"
#include "djb/direntry.h"
#include "djb/error.h"

static char *rcsid = "$Id: dffingerd-data.c,v 1.2 2000/04/12 16:02:49 drt Exp $";

#define stderr 2
#define stdout 1
#define stdin 0

#define FATAL "dffingerd-data: fatal: "

int fdcdb;
struct cdb_make cdb;

static int readfile(char *filename, stralloc *buf)
{
  buffer b;
  stralloc line = {0};
  char bspace[1024];
  int match = 1;
  int fddata;

  fddata = open_read(filename);
  if (fddata == -1)
    strerr_die4sys(111,FATAL,"unable to open ", filename, " : ");
  
  buffer_init(&b,read,fddata,bspace,sizeof bspace);
  
  while (match) {
    if (getln(&b,&line,&match,'\n') == -1)
      strerr_die2sys(111,FATAL,"unable to read line: ");
    
    if(line.s[line.len - 1] == '\n')
      {
	line.s[line.len - 1] = '\r';
	stralloc_cats(&line, "\n");
      }
    else
      {
	stralloc_cats(&line, "\r\n");
      }
    stralloc_cat(buf, &line);
  }
  return buf->len;
}

static int init2(DIR *dir)
{
  stralloc buf = {0};
  direntry *d;

  errno = 0;
  for (;;) 
    {
      d = readdir(dir);
      if (!d) 
	{
	  if (errno) return 0;
	  return 1;
	}
      
      if (d->d_name[0] != '.') 
	{
	  readfile(d->d_name, &buf);

	  if (cdb_make_add(&cdb, d->d_name,  str_len(d->d_name), buf.s, buf.len) == -1)
	    strerr_die2sys(111,FATAL,"unable to cdb_make_add: ");

	  stralloc_copys(&buf, "");
	}
    }
}


int main(void)
{ 
  DIR *dir;
  int r;

  umask(022);

  fdcdb = open_trunc("data.tmp");
  if (fdcdb == -1) 
    strerr_die2sys(111,FATAL,"unable to open data.tmp: ");
  if (cdb_make_start(&cdb,fdcdb) == -1) 
      strerr_die2sys(111,FATAL,"unable to cdb_make_start: ");

  if (chdir("data") == -1) return 0;

  dir = opendir(".");
  if (!dir) return 0;

  r = init2(dir);

  closedir(dir);

  if (chdir("..") == -1) return 0;

  if (cdb_make_finish(&cdb) == -1)
    strerr_die2sys(111,FATAL,"unable to cdb_make_finish: ");
  if (fsync(fdcdb) == -1)       
    strerr_die2sys(111,FATAL,"unable to fsync: ");
  if (close(fdcdb) == -1)       
    strerr_die2sys(111,FATAL,"unable to close: ");
  if (rename("data.tmp","data.cdb") == -1)
    strerr_die2sys(111,FATAL,"unable to move data.tmp to data.cdb: ");

  return 0;
}
