/* dffingerd by drt@ailis.de
 *
 * a finger daemon for use with tcpserver
 *
 * You might find more Info at http://rc23.cx/
 * 
 * I do not belive there is a thing like copyright.
 *
 * $Log: dffingerd.c,v $
 * Revision 1.2  2000/04/12 16:02:49  drt
 * fixed typos, compile time warnings
 *
 * Revision 1.1.1.1  2000/04/09 17:24:31  drt
 * initial revision
 *
 * Revision 1.1  2000/04/08 08:54:16  drt
 * Initial revision
 *
 */

#include <unistd.h>              /* for close */
#include "djb/cdb.h"
#include "djb/stralloc.h"
#include "djb/env.h"
#include "djb/readwrite.h"
#include "djb/open.h"
#include "djb/timeoutread.h"
#include "djb/buffer.h"
#include "djb/str.h"
#include "djb/droproot.h"
#include "djb/strerr.h"

static char *rcsid = "$Id: dffingerd.c,v 1.2 2000/04/12 16:02:49 drt Exp $";

#define stderr 2
#define stdout 1
#define stdin 0
#define DEFAULTUSER "default"
#define FATAL "ddfingerd: fatal: "

/* is_meta() returns 1 if c is a meta character, otherwise 0 */
int is_meta(unsigned char c)
{
  /* Mask out meta characters - we better 
     should do whitelisting instead of blacklisting */
  if(c < ' ') return 1;  
  if(c == '&') return 1;  
  if(c == ';') return 1;
  if(c == '`') return 1;
  if(c == '"') return 1;
  if(c == '|') return 1;
  if(c == '*') return 1;
  if(c == '?') return 1;
  if(c == '~') return 1;
  if(c == '<') return 1;
  if(c == '>') return 1;
  if(c == '^') return 1;
  if(c == '(') return 1;
  if(c == ')') return 1;
  if(c == '[') return 1;
  if(c == ']') return 1;
  if(c == '{') return 1;
  if(c == '}') return 1;
  if(c == '$') return 1;
  if(c == '\'') return 1;
  if(c == '\\') return 1;
  if(c == '\n') return 1;
  if(c == '\r') return 1;
  return 0;
}

int main()
{
  unsigned char *remotehost, *remoteinfo, *remoteip, *remoteport;
  unsigned char query[256] = {0};
  unsigned char clean_query[256] = {0};
  unsigned char *qptr = NULL;
  unsigned char *qptr2 = NULL;
  int len, query_len;
  int fd = 0;
  int r = 0;
  stralloc answer = {0};
  static struct cdb c;
  
  /* chroot() to $ROOT and switch to $UID:$GID */
  droproot("dffingerd: ");

  /* since we run under tcpserver, we can get all info 
     about the remote side from the enviroment */
  remotehost = env_get("TCPREMOTEHOST");
  if (!remotehost) remotehost = "unknown";
  remoteinfo = env_get("TCPREMOTEINFO");
  if (!remoteinfo) remoteinfo = "-";
  remoteip = env_get("TCPREMOTEIP");
  if (!remoteip) remoteip = "unknown";
  remoteport = env_get("TCPREMOTEPORT");
  if (!remoteport) remoteport = "unknown";
  
  /* now: 
     remotehost is the remote hostname or "unknown" 
     remoteinfo is some ident string or "-"
     remoteip is the remote ipadress or "unknown" (?)
  */

  /* Read the request from the client and \0-terminate it */
  /* timeout after 60 seconds */
  query_len = timeoutread(60, stdin, query, sizeof(query) - 1);
  query[query_len] = '\0';
     
  /* If there was any data we can go on */
  if (query_len > 0)
    {

      /* Open & init our cdb */
      fd = open_read("data.cdb");
      if (fd == -1)
	{
	  /* If opening failed quit */
	  strerr_die2sys(111, FATAL, "can't open data.cdb");
	}      
      cdb_init(&c,fd);

      /* \0-terminate query at the first \r or \n */
      for (len = 0; query[len]; len++) 
	{
	  if (query[len] == '\r' || query[len] == '\n') 
	    {
	      query[len] = '\0';
	      break;
	    }
	}    
      
      /* Handle RfC 1288 stuff */
      qptr=query;
      if (*qptr==' ') qptr++;
      if (*qptr=='/' && (*(qptr+1)=='W' || *(qptr+1)=='w')) qptr+=2;
      if (*qptr==' ') qptr++;
      
      /* clean up query string a bit by removing chars witch could 
	 clobber logging or so and replace them with _ */
      qptr2 = clean_query;
      for(qptr2 = clean_query; *qptr; qptr++)
	{
	  if(is_meta (*qptr)) 
	    {
	      *qptr2++ = '_';
	    }
     	  else 
	    {
	      *qptr2++ = *qptr;
	    }
	}
      *qptr2 = '\0';
      
      /* Search query for "user" on the database */
      r = cdb_find(&c,clean_query,str_len(clean_query)); 
      if (r == 1)
	{
	  /* read data */
	  stralloc_ready(&answer, cdb_datalen(&c));
	  if (cdb_read(&c, answer.s, cdb_datalen(&c), cdb_datapos(&c)) == -1)
	    {
	      	  strerr_die2sys(111, FATAL, "can't read from data.cdb");
	    }
	  else
	    {
	      answer.len = cdb_datalen(&c);
	    }
	}
      else 
	{
	  /* We didn't find the requested user, try DEFAULTUSER */
	  r = cdb_find(&c,DEFAULTUSER, str_len(DEFAULTUSER)); 
	  if (r == 1)
	    {
	      /* read data */
	      stralloc_ready(&answer, cdb_datalen(&c));
	      if (cdb_read(&c, answer.s ,cdb_datalen(&c) ,cdb_datapos(&c)) == -1)
		{
	      	  strerr_die2sys(111, FATAL, "can't read from data.cdb");
		}
	      else
		{
		  answer.len = cdb_datalen(&c);
		}
	    }
	  else 
	    {
	      /* no data for DEFAULTUSER eighter, so we don't have any data for the client */
	      stralloc_copys(&answer, "nope\n");
	    }
	}      
      stralloc_0(&answer);  
      buffer_puts(buffer_1, answer.s);
      buffer_flush(buffer_1);
      
      /* free database */
      cdb_free(&c);
      close(fd);      
    }
  else
    {
      *query = '\0';
    }
  
  /* Do logging */
  buffer_puts(buffer_2, remotehost);
  buffer_puts(buffer_2, " [");
  buffer_puts(buffer_2, remoteip);  
  buffer_puts(buffer_2, ":");
  buffer_puts(buffer_2, remoteport);
  buffer_puts(buffer_2, "] ");
  buffer_puts(buffer_2, remoteinfo);
  buffer_puts(buffer_2, " ");
  buffer_puts(buffer_2, clean_query);
  buffer_puts(buffer_2, "\n");
  buffer_flush(buffer_2);

  return 0;
}
