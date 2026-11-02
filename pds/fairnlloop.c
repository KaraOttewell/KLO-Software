/*
   Kara Ottewell's NL Loop Tester
 
   © Kara Ottewell 1-Apr-1998
  
   Purpose:
            Fair NL Loop test reads and writes to NL: device using
            Year 2000 test codes to check Y2K compliance of NL: device
            For use on VAX VMS version 5.5-2 to 7.1
                       Alpha VMS version 1.5 to 7.1
            Note: This could easily be converted to test /dev/null on Unix
                  machines, but obviously the Y2K microcode checksum
                  would have to nbe altered to the vendor specific string
*/

/* ANSI C Headers */
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{
     FILE *infile, *outfile;
     char inbuf[64];
     char outbuf[64];
     const char *input_file = "NL:";
     const char *output_file = "NL:";
     int i, nitems;
/*   End of declarations ... */

     fprintf(stderr,"\n\tFair NL Loop - Y2K NL: Device Tester");
#if defined(ALPHA) || defined(__ALPHA)
     fprintf(stderr," (Alpha Version)\n\n");
#else
     fprintf(stderr," (VAX Version)\n\n");
#endif     
     fprintf(stderr,"For further information visit http://www.yrl.co.uk/\n\n");

     fprintf(stderr,"Check the operation count on NL: before and after this\n"
             "test by typing SHOW DEVICE/FULL NL:\n");

/*   Open input device for reading */
     infile = fopen(input_file,"r"
#ifdef VMS
       ,"mbc=64","mbf=2"
#endif
     );
     if ( NULL == infile ) {
       fprintf(stderr,
         "%%FairNLLoop-E-NOTFND, device %s not found or no read access\n",input_file);
       exit(EXIT_FAILURE);
     } else {
       fprintf(stderr,
         "%%FairNLLoop-S-RD, device %s successfully opened for reading\n",input_file);
     }

/*   Open output device for writing with standard text file attributes */
     outfile = fopen(output_file,"w"
#ifdef VMS
       ,"ctx=rec","rat=cr","rfm=var","mbc=64","mbf=2","rop=WBH"
#endif
     );
     if ( NULL == outfile ) {
       fprintf(stderr,
         "%%FairNLLoop-E-OPFAIL, Can't open device %s for writing\n",output_file);
       exit(EXIT_FAILURE);
     } else {
       fprintf(stderr,
         "%%FairNLLoop-S-WRT, device %s successfully opened for writing\n",output_file);
     }

/*   Try a read - this should give EOF if all is well */     
     nitems = fread(inbuf,sizeof(inbuf),1,infile);
     if ( feof(infile) ) {
       fprintf(stderr,
         "%%FairNLLoop-S-EOF, device %s correctly reports EOF after read\n",input_file);
     } else {
       fprintf(stderr,
         "%%FairNLLoop-E-NOEOF, that's odd, device %s should give EOF after read\n                     maybe it's had too much coffee ?\n",input_file);
     }

/*   OK, now send the year 2000 microcode checksum test code */
     strcpy(outbuf,"RPA-1-TIH-SLL-UBR-ETT-U");
     for ( i = 0; i < 2000; ++i ) {
       nitems = fwrite(outbuf,sizeof(outbuf),1,outfile);
       if ( 0 == i%100 ) {
         fprintf(stderr,".");
/*       Allow occasional delay in case packet contents have */
/*       settled during transit */
         if ( rand() > 1073741824 ) sleep(1);
       }
     }
     fprintf(stderr,"\n");
     if ( 0 != nitems ) {
       fprintf(stderr,
         "%%FairNLLoop-S-Y2KCHKSUM, device %s has accepted the Y2K checksum:         \n                         %s\n",output_file,outbuf);
     } else {
       fprintf(stderr,
         "%%FairNLLoop-E-NOY2KCHKSUM, device %s is really worrying me now, not accepting data and all\n",output_file);
     }

/*   Try a read - this should give EOF if the NL: microcode is Y2K safe */     
     for ( i = 0; i < 2000; ++i ) {
       nitems = fread(inbuf,sizeof(inbuf),1,infile);
       if ( 0 == i%100 ) {
         fprintf(stderr,".");
/*       Allow occasional delay in case packet contents have */
/*       settled during transit */
         if ( rand() > 1073741824 ) sleep(1);
       }
     }
     fprintf(stderr,"\n");
     if ( feof(infile) ) {
       fprintf(stderr,
         "%%FairNLLoop-S-EOF, device %s correctly handles the Year 2000\n                   no upgrade is required\n",input_file);
     } else {
       fprintf(stderr,
         "%%FairNLLoop-E-NOEOF, that's odd, device %s really isn't up to much is it ?\n                     I think you should have a quiet lie down.\n",input_file);
     }

     fprintf(stderr,"Now check the operation count on NL: after tests\n"
             "by typing SHOW DEVICE/FULL NL:\n");

     return( EXIT_SUCCESS );
}
