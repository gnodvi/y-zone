=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  

                          Y  z  o  n  e    0 . 2  

  Contents: 
 
     1) Introduction .................................. 
     2) Contents of a package ......................... 
     3) Installation .................................. 
     4) Version and their changes ..................... 
     5) Authors ....................................... 
     6) COPYRIGHT ..................................... 
 

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  
1) Introduction. 
   
 Yzone -- universal cross-platform library for creation of the graphical 
       applications for any platform UNIX and Microsoft Windows.

 There are three parts of Library Yzone:

 OPEN -- cross-platform GUI and other kernel primitives (portable operating
         system interface). 
 MORE -- useful additions and improvements; 
 WIND -- control elements of window dialogue; 

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  
2) Contents of a package. 
 
  /demo/dial_1      | 
        dial_2      | demonstration programs (needs to be made);   
        demons.c    - source text of all demonstration examples;
  /doc              - documentation in a format HTML;  
  /lib/y_zone.a     - library (needs to be made)
  /include/y_zone.h - and appropriate h-file; 
  /src/kernel.c     - source code of librarys;
       y_zone.h     - source code of declarations;
   LICENSE          - GNU LIBRARY GENERAL PUBLIC LICENSE;                    
   Make_def         - user sets (include file for Makefile); 
   Makefile         - instruction to a command 'make'; 
   README           - this file;
 
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  
3) Installation. 
 
 
  a) to receive necessary components of a package; 
  b) place all received files in one and the same directory;
  c) to unpack them by any suitable utility (unzip, WinZip, ..); 
  d) new directory will be create;

  e) to make necessary changes in a file Make_def;
  f) to execute a command 'make'; 
  g) to check up work of the demonstration program; 
  h) see /doc for more information;
 
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=  
4) Version and their changes. 
 
The chronology is conducted, as it is usual, from below-upwards, that is 
above latest news: 
 
Yzone-0.2  (Autumn 2000)
  - now it's one library;
  - many bug fixed;
  - shot reference;

Yzone-0.1  (May 2000)
  - now it's under GNU License !!
  - many improve changes;
  - Yzone Team --> Life Software;

Yzone-0.0  (June 1999) 
  - one another "probably portable" multi-platform project was started;
  - policy (status) was selected (for first time) as "binary only";
 
Ylib-Yzone (Febrary 1998) 
  -  Yzone Team was born, that has given a new impulse    
  -  to old ideas.  
 
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
5) Authors. 

Life Software Group:
  ................. 
  ................. 

Orther authors:
  Pavel V. Ozerski (pavel@insect.mail.iephb.ru)

Comments:
  Artem Rasskazov (artem@stl.bereg.ru)
  Mark-Andre Hopf (watchman@mark13.de)
  Carl-Michael Kleffner (kleffner@mail.desy.de)
  Harri Porten (porten@tu-harburg.de)

=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

6) COPYRIGHT. 

   (c) 2000-2020, gnodvi meranov, gnodvimeranov@mail.ru
                                                                             
   This library is free software;  you can redistribute it  and/or           
   modify it  under the terms  of  the  GNU Library General Public           
   License  as published by the  Free Software Foundation;  either           
   version 2 of the License, or (at your option) any later version.          
                                                                             
   This library is distributed  in the hope that it will be useful,          
   but  WITHOUT ANY WARRANTY;  without even the implied warranty of          
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU          
   Library General Public License for more details.                          
                                                                             
   You should have received a copy of the GNU Library General Public         
   License along with this library; if not, write to the Free                
   Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.        
                                                                             
=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
