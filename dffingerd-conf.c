/* $Id: dffingerd-conf.c,v 1.4 2000/05/04 07:54:08 drt Exp $
 *  --drt@ailis.de
 *
 * create directory structure for using dffingerd with svscan
 *
 * You might find more Info at http://rc23.cx/
 * 
 * I do not belive there is a thing like copyright.
 *
 * $Log: dffingerd-conf.c,v $
 * Revision 1.4  2000/05/04 07:54:08  drt
 * * dffingerd.c: Build fixes, fix at handling \W
 *
 * * dffingerd-data.c: Build fixes
 *
 * * dffingerd-conf.c: Build fixes
 *
 * * Makefile: Build fixes
 *
 * Revision 1.3  2000/04/26 09:37:20  drt
 * cleanups
 *
 * Revision 1.2  2000/04/12 16:02:49  drt
 * fixed typos, compile time warnings
 *
 * Revision 1.1.1.1  2000/04/09 17:24:29  drt
 * initial revision
 *
 * Revision 1.1  2000/04/08 08:53:57  drt
 * Initial revision
 *
 */

#include <pwd.h>
#include "strerr.h"
#include "exit.h"
#include "auto_home.h"
#include "generic-conf.h"

static char rcsid[]="$Id: dffingerd-conf.c,v 1.4 2000/05/04 07:54:08 drt Exp $";

#define FATAL "dffingerd-conf: fatal: "

void usage(void)
{
  strerr_die1x(100,"dffingerd-conf: usage: dffingerd-conf acct logacct /dffingerd myip");
}

char *dir;
char *user;
char *loguser;
struct passwd *pw;
char *myip;

int main(int argc, char **argv)
{
  user = argv[1];
  if (!user) usage();
  loguser = argv[2];
  if (!loguser) usage();
  dir = argv[3];
  if (!dir) usage();
  if (dir[0] != '/') usage();
  myip = argv[4];
  if (!myip) usage();

  pw = getpwnam(loguser);
  if (!pw)
    strerr_die3x(111,FATAL,"unknown account ",loguser);

  init(dir,FATAL);
  makelog(loguser,pw->pw_uid,pw->pw_gid);

  start("run");
  outs("#!/bin/sh\nexec 2>&1\n");
  outs("ROOT="); outs(dir); outs("/root; export ROOT\n");
  outs("IP="); outs(myip); outs("; export IP\n");
  outs("exec envuidgid "); outs(user);
  outs(" \\\nsoftlimit -d250000");
  outs(" \\\ntcpserver -Odhpr -c 10 $IP finger");
  outs(" "); outs(auto_home); outs("/bin/dffingerd");
  outs("\n");
  finish();
  perm(0755);

  makedir("root");
  perm(02755);

  makedir("root/data");
  perm(02755);

  start("root/Makefile");
  outs("data.cdb: data/*\n");
  outs("\t"); outs(auto_home); outs("/bin/dffingerd-data\n");
  finish();
  perm(0644);

  return 0;
}
