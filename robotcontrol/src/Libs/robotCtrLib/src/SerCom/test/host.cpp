/*
 * Copyright (c) 2002-2005, Neuroinformatics research group, 
 * University of Osnabrueck <tribots@informatik.uni-osnabrueck.de>
 *
 * This code is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * It is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */


#include <unistd.h>
#include"sercom.h"
#include <sys/time.h>

int main()
{
  struct timeval tv1,tv2;
  long diff;

  char sbuf[200], rbuf[200];
  

  sprintf(sbuf,"GMODE \n");
  SerCom sercom("host.cfg","SER_COM");

//   for (int i=0;i<2;i++)
//     {
//       gettimeofday(&tv1,0);
//       int res=sercom.talk(sbuf,rbuf,200,::SerComI::T_SECOND);
//       gettimeofday(&tv2,0);
//       diff = (tv2.tv_sec-tv1.tv_sec)*1000 + (tv2.tv_usec -tv1.tv_usec) /1000;
//       printf("Received:[%s] in %ldms\n", rbuf,diff);
    
//     }

  return 0;
}
