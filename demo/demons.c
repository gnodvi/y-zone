/****************************************************************************
 *                                                                          *
 *  demons.c  --  demo examples for "Library Yzone";                        *
 *                                                                          *
 *  Copyright (c) 2000, Life Software.                                      *
 *                                                                          *
 *  This library is free software; you can redistribute it and/or           *
 *  modify it under the terms of the GNU Library General Public             *
 *  License as published by the Free Software Foundation; either            *
 *  version 2 of the License, or (at your option) any later version.        *
 *                                                                          *
 *  This library is distributed in the hope that it will be useful,         *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of          *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU       *
 *  Library General Public License for more details.                        *
 *                                                                          *
 *  You should have received a copy of the GNU Library General Public       *
 *  License along with this library; if not, write to the Free              *
 *  Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.      *
 *                                                                          *
 *                Life Software:                                            *
 *                Web:                                                      *
 *                E-mail:  gennady.em@gmail.com                             *
 *                                                                          *
 ****************************************************************************
 */                                                                          
                                                                             


#ifdef  D_DIAL_1  
/*************************************************************************/

#include "y_zone.h" 

int   x0group, y0group;

long  main_proc  (PFUNC_PAR);
long  page1_proc (PFUNC_PAR);
long  page2_proc (PFUNC_PAR);
long  page3_proc (PFUNC_PAR);
long  page4_proc (PFUNC_PAR);
long  push_proc (PFUNC_VAR);   
long  testmsg_proc (PFUNC_VAR);                                                        
long  dial_proc (PFUNC_PAR);

enum ykeys_global {                                                             
  MYPUSH_DOWN = YKEY_LEVEL4,                                                    
  MYPUSH_UP                                                                     
}; 

 
/*-------------------------------MAIN-----------------------------------*/
long
MAIN (PFUNC_VAR)
{

  switch (message) {
  case YCREATE:

    YBigWindow (NULL, main_proc, "Dial_1", 0,0, 550,450, 0,0,0,0, CLR_DEF);
    break; 
  }

  id++;
  RETURN_TRUE;
}
/*-----------------------------draw_rect_text--------------------------------*/
void
draw_rect_text (char *text, int xx, int yy, int ww, int hh, YT_COLOR color)
{
  int x0, y0;
  int     x, y, w, h;

  x0 = xx + ww / 2;
  y0 = yy + hh / 2;

  w = YStringW (text);
  h = YStringH (text);
  ANTICALCWH (w, h);

  x = x0 - w / 2;
  y = y0;

  YPaintString (text, x, y, color);
  return;
}
/*-------------------------------draw_group_old---------------------------------*/
void
draw_group_old (char *sText, int x, int y, int w, int h, YT_COLOR color)
{
  int  w_char = YStringW ("W");
  int  left = 10, dx = 5, dy = 4, xmax, ymax, xl, xr;
  int  wtext;
  YT_COLOR clr_white = YColor("white");
  YT_COLOR clr_black = YColor("black");

  xmax = x+w-1;
  ymax = y+h-1;

  if (color != CLR_NULL) {
    YPaintRectF (x+2, y+2+dy, w-4, h-4-dy, color); 
    YPaintRectF (x+2, y+2, left-2, dy, color); 
  }

  xl = x+left;
  YPaintLine (x,   y,   xl, y,   clr_black);
  YPaintLine (x+1, y+1, xl, y+1, clr_white);

  YPaintString (sText, xl+dx, y, clr_black);
  wtext = YStringW (sText);
  ANTIMULTX (wtext);
  xr = xl+dx+wtext+dx;
  if (color != CLR_NULL) 
    YPaintRectF (xr, y+2, xmax-2-xr+1, dy, color); 

  YPaintLine (xr, y,   xmax,   y,   clr_black);
  YPaintLine (xr, y+1, xmax-1, y+1, clr_white);

  YPaintLine (x+1,    y+1,    x+1,    ymax,   clr_white);
  YPaintLine (x,      ymax,   xmax,   ymax,   clr_white);
  YPaintLine (xmax,   y,      xmax,   ymax,   clr_white);

  YPaintLine (x,      y,      x,      ymax-1, clr_black);
  YPaintLine (x,      ymax-1, xmax-1, ymax-1, clr_black);
  YPaintLine (xmax-1, y,      xmax-1, ymax-1, clr_black);

  return;
}
/*-------------------------------draw_group---------------------------------*/
void
draw_group (char *sText, int xx, int yy, int ww, int hh, YT_COLOR color)
{

  YPaintRectFB (xx, yy, ww, hh, color, YColor("black")); 
  YPaintLine   (xx, yy, xx+YStringW(sText), yy, color);
  YPaintString (sText, xx, yy, YColor("black"));

  return;
}
/*-----------------------------begin_group----------------------------------*/
void
begin_group (char *name, int x, int y, int w, int h, YT_COLOR color)
{
      
  if (name != NULL) {
    draw_group (name, x, y, w, h, color);
  }
  YGetOrigin (&x0group, &y0group);
  CALCWH (x, y);
  YSetOrigin (x0group + x, y0group + y);

}
/*-------------------------------end_group----------------------------------*/
void
end_group ()
{

  YSetOrigin (x0group, y0group);

}
/*-------------------------------dialog_proc-------------------------------------*/
long
dialog_proc (PFUNC_VAR)
{

  static int  hcap = 22;
  static int  focus, hWnd ;
  static long image;
  static YT_DEFWND *wnd; 
  YT_COLOR  col_border  = YColor("red");

  switch (message) {
  case YGETSIZE:
    MYSIZE_IS (W_DLG+2, H_DLG+hcap+3);
    break;
  case YOPEN:
    wnd = (YT_DEFWND*) Y_WLONG1;
    image  = YImageGet (0, 0, WND->w, WND->h);
    focus  = YGetFocus ();
    YKillFocus ();
  case YDRAW:
    YWnd (&hWnd, wnd->proc, "", 1, hcap+2, WND->w-2, WND->h-3-hcap,
	  wnd->long1, wnd->long2, wnd->long3, wnd->long4, wnd->color);

    YPaintRectB  (0, 0, WND->w, WND->h, col_border);
    YPaintRectFB (0, 0, WND->w, hcap, WIN->color, col_border);
    /* YPaintString (WND->name, 10, 10, col_border); */
    draw_rect_text (WND->name, 0, 0, WND->w, hcap, col_border);
    break;
  case YCLOSE:
    YWndClean (id);
    YImagePut (0, 0, image);
    YImageFree (image);
    YSetFocus (focus);
    YWndFree (id);  /*??*/
    break;
  default: ;;
  }

  RETURN_TRUE;
#undef  LONGS 
}
/*------------------------------main_proc--------------------------------*/
long
main_proc (PFUNC_VAR)       
{        
  static int hExit, hCurPage=ID_NULL, hPage1, hPage2, hPage3, hPage4, hTest = ID_NULL;
  static YT_COLOR col1, col2, col3, col4;

  enum local_keys {
    DRAW_MESS = YKEY_LOCALS
  };

  switch (message) {       
  case YCREATE: 
    YSetDialog (dialog_proc);
    break;        
  case YOPEN: 
    col1 = YColor("fuchsia");
    col2 = YColor("white");
    col3 = YColor("blue");
    col4 = YColor("red");
  case YDRAW:       
    YPaintRectF (0,0, WND->w,WND->h, YColor("yellow"));        

    begin_group ("MESSAGES",    20,375, 440,55, YColor(""));
      YWnd (&hTest, testmsg_proc, "",  15,15, 385,30, 0,0,0,0, CLR_DEF);
    end_group ();

    YGoto (YDRAWITEM, 0,0,col1,(long)page1_proc);

    YWnd (&hPage1, push_proc, "Page1", 465, 20, 65,40, 0,0,0,0, col1);
    YWnd (&hPage2, push_proc, "Page2", 465, 70, 65,40, 0,0,0,0, col2);
    YWnd (&hPage3, push_proc, "Page3", 465,120, 65,40, 0,0,0,0, col3);
    YWnd (&hPage4, push_proc, "Page4", 465,170, 65,40, 0,0,0,0, col4);

    YWnd (&hExit,  push_proc, "Exit",  465,380, 65,40, 0,0,0,0, YColor("lime"));
    break;        
  case YDRAWITEM:       
   /*  YWndClose (hCurPage);  */
   /*  YUnWnd (hCurPage);  */       
    YWnd (&hCurPage, (YT_PFUNC)mes4, "", 20,20, 440,340, LP(hTest),0,0,0, (YT_COLOR)mes3); 
    break; 
  case YCLOSE:       
    YWndClean (id);        
    break; 
  case MYPUSH_UP:
    if      (mes1 == hExit )  exit(0); 
    else if (mes1 == hPage1)  YGoto (YDRAWITEM, 0,0,col1,(long)page1_proc);
    else if (mes1 == hPage2)  YGoto (YDRAWITEM, 0,0,col2,(long)page2_proc);
    else if (mes1 == hPage3)  YGoto (YDRAWITEM, 0,0,col3,(long)page3_proc);
    else if (mes1 == hPage4)  YGoto (YDRAWITEM, 0,0,col4,(long)page4_proc);
    break; 
  default:
    return (YSend (hTest, message, mes1,mes2,mes3,mes4)); 
  }       
  
  RETURN_TRUE; 
}
/*--------------------------------test_picture---------------------------*/
void
test_picture ()
{

  YPaintRectF (40,20,  80,90, YColor("white"));
  YPaintRectFB(60,80,  80,90, YColor("aqua"), YColor("black"));
  YPaintLine  (10,10, 110,110,YColor("black"));
  YPaintLine  (5,100, 160,10, YColor("white"));
  YPaintRectFB(60,70,  70,40, YColor("silver"), YColor("white"));
  YPaintRectF (110,40, 60,80, YColor("red"));
  YPaintRectF (10,100, 90,50, YColor("fuchsia"));
  YPaintString("This is test", 20,60, YColor("black"));  
  YPaintLine  (25,25, 160,150, YColor("blue"));
  YPaintLine  (25,25, 160,152, YColor("blue"));
  YPaintLine  (25,25, 160,154, YColor("blue"));
  YPaintLine  (25,25, 160,156, YColor("blue"));
  YPaintArcB (10, 10,  70, 70, 0, 360*64, YColor("black"));

  return;
}
/*--------------------------------page1_proc------------------------------*/
long
page1_proc (PFUNC_VAR)       
{        
  YT_COLOR clr_text;
  static int hTest = ID_NULL;
  static int hMouse, hDialog;
  char     *shape[4] = {"arrow", "wait", "ibeam", "cross"};
  static int shape_i = 0;

  switch (message) {       
  case YOPEN: 
    hTest = *((int*)Y_WLONG1);
  case YDRAW:       
    YPaintRectFB (0,0,WND->w,WND->h, WIN->color, YColor("black"));        

    begin_group ("ABOUT", 20,30, 210,110, YColor("teal"));
      clr_text = YColor("white");
      YPaintString ("   'Library Yzone'    ", 20,20, clr_text);  
      YPaintString ("     version 0.2      ", 20,35, clr_text);  
      YPaintString (" Copyright (c) 2000   ", 20,60, clr_text);  
      YPaintString ("    Life Software.    ", 20,75, clr_text);  
      YPaintString ("  Under GNU License.  ", 20,90, clr_text);  
    end_group ();

    begin_group ("SORRY", 20,200, 210,90, YColor("teal"));
      clr_text = YColor("white");
      YPaintString ("     Sorry! Sorry!    ", 20,20, clr_text);  
      YPaintString ("  This demo example   ", 20,35, clr_text);  
      YPaintString ("          is          ", 20,50, clr_text);  
      YPaintString (" under constructions! ", 20,65, clr_text);  
    end_group ();

    begin_group("YDraw..", 240,30, 185,190, YColor("green")); 
    test_picture ();
    end_group ();

    YWnd (&hMouse,  push_proc, "Mouse",   250,240, 70,50, 0,0,0,0, CLR_DEF);
    YWnd (&hDialog, push_proc, "Dialog",  340,240, 70,50, 0,0,0,0, CLR_DEF);

    break;        
  case MYPUSH_DOWN:  
    if (mes1 == hMouse) {
      shape_i++;
      if (shape_i == 4) shape_i = 0;
      YMouseForm (shape[shape_i]);
    }
    if (mes1 == hDialog) {
      YDlg (dial_proc, "Dialog", 0,0,260,250, 0,0,0,0, CLR_DEF, TRUE); 
    }
    break;  
  case YRMOUSEUP:
    YBeep ();
    YBigWindow (NULL, dial_proc, "Big Window", 0,0,240,220, 0,0,0,0, CLR_DEF);
    break;
  case YCLOSE:       
    YWndClean (id);        
    break;  
  default:
   /*  OUTD(message); */
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*------------------------------page2_proc------------------------------*/
long
page2_proc (PFUNC_VAR)       
{        
  static int hTest = ID_NULL;
  /* static YT_COLOR color; */

  switch (message) {       
  case YOPEN: 
    hTest = *((int*)Y_WLONG1);
  case YDRAW:       
    YPaintRectFB (0,0,WND->w,WND->h,  WIN->color, YColor("black"));        

    break;        
  case YLMOUSEUP:
    /* color = YGetColor (mes1, mes2); */
    break;
  case YCLOSE:       
    YWndClean (id);        
    break;  
  default:
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*------------------------------page3_proc------------------------------*/
long
page3_proc (PFUNC_VAR)       
{        
  static int hTest = ID_NULL;

  switch (message) {       
  case YOPEN: 
    hTest = *((int*)Y_WLONG1);
  case YDRAW:       
    YPaintRectFB (0,0,WND->w,WND->h, YColor("blue"), YColor("black"));        
    break;        
  case YCLOSE:       
    YWndClean (id);        
    break;  
  default:
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*------------------------------page4_proc------------------------------*/
long
page4_proc (PFUNC_VAR)       
{        
  static int hTest = ID_NULL;

  switch (message) {       
  case YOPEN: 
    hTest = *((int*)Y_WLONG1);
  case YDRAW:       
    YPaintRectFB (0,0,WND->w,WND->h, YColor("red"), YColor("black"));        
    break;        
  case YCLOSE:       
    YWndClean (id);        
    break;  
  default:
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*--------------------------------push_proc----------------------------------*/
long
push_proc (PFUNC_VAR)
{

  switch (message) {
  case YOPEN:
  case YDRAW:
    YPaintRectFB (0, 0, WND->w, WND->h, WIN->color, YColor("black"));
    draw_rect_text (WND->name, 0, 0, WND->w, WND->h, YColor("black"));
    break;
  case YLMOUSEDOWN:
    YPost (WND->parent, MYPUSH_DOWN, id, 0, 0, 0);
    break;
  case YLMOUSEUP:
    YPost (WND->parent, MYPUSH_UP, id, 0, 0, 0);
    break;
  case YCLOSE:
    YWndClean (id);
    break;
  default: ;;;;
  }

  RETURN_TRUE;
}
/*-----------------------------dial_proc-------------------------------------*/
long
dial_proc (PFUNC_VAR)
{

  switch (message) {
  case YOPEN:
  case YDRAW:
    YPaintRectFB (0, 0, WND->w, WND->h, YColor("lime"), YColor("black"));
    test_picture ();
 
    YWnd (Ph(), push_proc, "O'Key",  90, 160, 80,30, 0,0,0,0, YColor("yellow"));
    break;
  case MYPUSH_UP:
    YDlgEnd (0);
    break;
  case YLMOUSEDOWN:
		printf ("dial_proc_YLMOUSEDOWN: %d %d \n", mes1, mes2);
    break;
  case YCLOSE:
    YWndClean (id);
    break;
  default: ;;;;
  }

  RETURN_TRUE;
}
/*------------------------------testmsg_proc----------------------------------*/
long                                                                            
testmsg_proc (PFUNC_VAR)                                                        
{                                                                               
  static char str[100];                                                         
                                                                                
  /* OUTD(message); */
  switch (message) {                                                            
  case YOPEN:                                                                   
  case YDRAW:                                                                   
    YPaintRectFB (0,0, WND->w,WND->h, YColor("white"), YColor("black"));         
    draw_rect_text (str, 0,0, WND->w,WND->h, YColor("black"));                   
    break;                                                                      
  case YCLOSE:                                                                  
    YWndClean (id);                                                             
    break;                                                                      
  default:                                                                      
    switch (message) {                                                          
    case YLMOUSEDOWN:  sprintf (str, "LMOUSEDOWN:   %d  %d", mes1, mes2);  break;
    case YLMOUSEUP:    sprintf (str, "YLMOUSEUP:    %d  %d", mes1, mes2);  break;
    case YLMOUSEDRAG:  sprintf (str, "YLMOUSEDRAG:  %d  %d", mes1, mes2);  break;
    case YRMOUSEDOWN:  sprintf (str, "YRMOUSEDOWN:  %d  %d", mes1, mes2);  break;
    case YRMOUSEUP:    sprintf (str, "YRMOUSEUP:    %d  %d", mes1, mes2);  break;
    case YRMOUSEDRAG:  sprintf (str, "YRMOUSEDRAG:  %d  %d", mes1, mes2);  break;
    }                                                                           
    YGoto (YDRAW, 0,0,0,0);                                                     
    break;                                                                      
  }                                                                             
                                                                                
  RETURN_TRUE;                                                                  
}                                                                               
/*----------------------------------------------------------------------*/

#endif
#ifdef  D_DIAL_2  
/*************************************************************************/
#ifdef FLTK
#define PUSH     F_PUSH
#define CHECK    F_CHECK
#define SCROLL   F_SCROLL
#define SCROLL_H F_SCROLL
#define EDIT     F_EDIT
#define CLOCK    F_CLOCK
#define LIST     F_LIST
#define TOPMENU  F_TOPMENU

#define CAPTION "Fltk_2"
#else
#define CAPTION "Dial_2"
#endif

#include "y_zone.h" 

long  dial2_proc  (PFUNC_PAR);
long  impr_proc  (PFUNC_PAR);

long  page1_proc (PFUNC_PAR);
long  page2_proc (PFUNC_PAR);
long  page3_proc (PFUNC_PAR);
long  page4_proc (PFUNC_PAR);
long  page_push  (PFUNC_VAR); 
long  page_clock  (PFUNC_VAR); 
      
long  TESTMSG (PFUNC_VAR); 
                                                                                

/*-------------------------------MAIN-----------------------------------*/
long
MAIN (PFUNC_VAR)
{

  switch (message) {
  case YCREATE:
    /* YInitDRAW (); */
    YInitWIND (); 

    YBigWindow (NULL, dial2_proc, CAPTION, 0,0, 550,450, 0,0,0,0, CLR_DEF);     
    break; 
  }

  id++;
  RETURN_TRUE;
}
/*------------------------------dial2_proc--------------------------------*/
long
dial2_proc (PFUNC_VAR)       
{        
  static int hExit, hTest = ID_NULL;
  static YT_BOOK pages[] = {        
    {"MORE",   impr_proc,   LP(hTest)},       
    {"Page_1", page1_proc,  LP(hTest)},       
    {"Page_2", page2_proc,  LP(hTest)},       
    {"Page_3", page3_proc,  LP(hTest)},       
    {"Page_4", page4_proc,  LP(hTest)},       
    {"", NULL, 0}        
  };        

  enum local_keys {
    DRAW_MESS = YKEY_LOCALS
  };

  switch (message) {       
  case YOPEN: 
  case YDRAW:       
    YDrawRectF (0,0, WND->w,WND->h, YColor("yellow"));        

    YBeginGroup ("MESSAGES", 30,375, 420,55, YColor(""));
    YWnd (&hTest, TESTMSG, "",  15,15, 385,30, 0,0,0,0, CLR_DEF);
   /*  YWnd (&hTest, testmsg_proc, "",  15,15, 385,30, 0,0,0,0, CLR_DEF); */
    YEndGroup ();

    YWnd (&hExit, PUSH, "Exit", 455,380, 65,40, 0,0,0,0, YColor("lime"));

    YWnd (Ph(), BOOK, "", 30,20, WND->w-60,340, (long)pages,75,30,YUP, CLR_DEF);      
    break;        
  case YCLOSE:       
    YWndClean (id);        
    break; 
  case YPUSH:
    if (mes1 == hExit)  exit(0); 
  default:
    return (YSend (hTest, message, mes1,mes2,mes3,mes4)); 
  }       
  
  RETURN_TRUE;
}
/*------------------------------impr_proc------------------------------*/
long
impr_proc (PFUNC_VAR)       
{        
  YT_COLOR clr_text;
  static int hTest = ID_NULL;
	
	char *text = "ьФП is test";
	int   w_text, h_text;


  switch (message) {       
  case YOPEN: 
    hTest = *((int*)YGet("wlong1"));
  case YDRAW:       
    YDrawRectFB (0,0,WND->w,WND->h, YColor("fuchsia"), YColor("black"));        

    YBeginGroup ("ABOUT", 20,30, 210,110, YColor("teal"));
      clr_text = YColor("white");
      YDrawString ("   'Library Yzone'    ", 20,20, clr_text);  
      YDrawString ("     version 0.2      ", 20,35, clr_text);  
      YDrawString (" Copyright (c) 2000   ", 20,60, clr_text);  
      YDrawString ("    Life Software.    ", 20,75, clr_text);  
      YPaintString ("  Under GNU License. ", 20,90, clr_text);  
    YEndGroup ();

    YDrawLogo ("Yzone",      40,160, 170,120);

    YBeginGroup("YDraw..", 240,30, 185,190, YColor("green"));   
      YDrawRectF (40,20,  80,90, YColor("white"));
      YDrawRectFB(60,80,  80,90, YColor("aqua"), YColor("black"));
      YDrawLine  (10,10, 110,110,YColor("black"));
      YDrawLine  (5,100, 160,10, YColor("white"));
      YDrawRectFB(60,70,  70,40, YColor("silver"), YColor("white"));
      YDrawRectF (110,40, 60,80, YColor("red"));
      YDrawRectF (10,100, 90,50, YColor("fuchsia"));
      YDrawString("This is test", 20,60, YColor("black"));  
      YDrawLine  (25,25, 160,150, YColor("blue"));
      YDrawLine  (25,25, 160,152, YColor("blue"));
      YDrawLine  (25,25, 160,154, YColor("blue"));
      YDrawLine  (25,25, 160,156, YColor("blue"));

      YDrawTriFB (50,25,  80,90, 10,120, YColor("aqua"), YColor("black"));
      YPaintArcB (95, 100, 70, 30,  -45*64, -90*64, YColor("black"));
      YDrawArcB  (5, 100, 70, 30,  -45*64, -90*64, YColor("black"));
      YDrawCircB (45, 100,  30, YColor("black"));
    YEndGroup ();

		YDrawString (text, 240,240, YColor("black")); 
		w_text = YStringW(text);
		h_text = YStringH(text);
		YDrawRectB  (240,240, w_text, h_text, YColor("black"));
		/* printf ("w_text=%d  h_text=%d  \n", w_text, h_text); */

    break;        
  case YCLOSE:       
    YWndClean (id);        
    break;  
  default:
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*------------------------------page2_proc----------------------------------*/
long
page2_proc (PFUNC_VAR)       
{        

  static int hEdit1, hEdit2, hCheck1, hCheck2;
  static int hList1, hList2, hDec1, hDec2;
  static int hTest = ID_NULL;

  static char *list1[]= {
    "string_01", "string_02", "string_03",
    "string_04", "string_05", "string_06", 
    "string_07", "string_08", "string_09", 
    "string_10", "string_11", "string_12", 
    NULL}; 
  static char *list2[]={
    "listitem1", "listitem2", "listitem3",
    "listitem4", "listitem5", "listitem6", 
    "listitem7", "listitem8", "listitem9", 
    NULL}; 


  switch (message) {       
  case YOPEN:       
    hTest = *((int*)YGet("wlong1"));
  case YDRAW:       
    YDrawRectF (0,0,WND->w,WND->h, YColor("teal"));        

    YBeginGroup ("LIST", 30,40, 240,110, YColor("navy"));    
    YWnd (&hList1, LIST, NULL,  15,20, 100,70, (long)list1, 3,0,0, YColor("yellow")); 
    YWnd (&hList2, LIST, NULL, 125,20, 100,70, (long)list2, 2,0,0, YColor("lime")); 
    YEndGroup (); 

    YBeginGroup ("CHECK", 300,40, 130,110, YColor("fuchsia"));   
    YWndGroupBegin ();
    YWnd (&hCheck1, CHECK, "Check1", 25,10, 20,20, TRUE,0,0,0, YColor("white"));
    YWnd (&hCheck2, CHECK, "Check2", 25,40, 20,20, FALSE,0,0,0, YColor("white"));
    YWndGroupEnd ();
    YWnd (Ph(), CHECK, "Check3", 25,75, 20,20, FALSE,0,0,0, CLR_DEF);
    YEndGroup (); 

    YBeginGroup ("EDIT", 60,195, 170,70, YColor("fuchsia"));  
    YWnd (&hEdit1, EDIT, "Edit1 ",  65,10, 90,20, (long)"string1",0,0,0, YColor("white"));   
    YWnd (&hEdit2, EDIT, "Edit2 ",  65,40, 90,20, (long)"string2",0,0,0, YColor("white"));   
    YEndGroup (); 

    YBeginGroup ("DECIMAL", 300,195, 130,70, YColor("fuchsia"));  
    YWnd (&hDec1, DECIMAL, "Dec1 ",  60,10, 40,20, 4,0,10,0, YColor("white"));   
    YWnd (&hDec2, DECIMAL, "Dec2 ",  60,40, 40,20, 2,0,15,0, YColor("white"));   
    YEndGroup (); 

    break;        
  case YCLOSE:       
    YWndClean (id);        
    break;   
  default:
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*------------------------------page1_proc----------------------------------*/
long
page1_proc (PFUNC_VAR)       
{        

  enum ywords {
    MY_TESTWORK = YKEY_LOCALS + 1
  };

  static int hTest = ID_NULL;
  static YT_BOOK pages[] = {        
    {"CLOCK", page_clock, LP(hTest)},       
    {"PUSH",  page_push,  LP(hTest)},       
    {"DITTO", DITTO,      0},       
    {"", NULL, 0}        
  };        
  static int hMess, hOpen, hWait, hAbout, hHelp;
  static YT_COLOR colors;
  long  ret_file;

  switch (message) {       
  case YOPEN:       
    hTest = *((int*)YGet("wlong1"));
    colors = YColor("yellow");
  case YDRAW:       
    YDrawRectF (0,0,WND->w,WND->h, YColor("blue"));        

    YBeginGroup ("BOOK", 30,40, 275,210, YColor(""));   
    YWnd (Ph(), BOOK, "", 15,15, 250,180, (long)pages,70,25,YRIGHT, CLR_DEF);      
    YEndGroup();

    YBeginGroup ("Dialogs", 325,20, 130,200, YColor(""));   
    YWnd (&hWait,   PUSH, "Wait",    30, 15,  65,25, 0,0,0,0, CLR_DEF);
    YWnd (&hMess,   PUSH, "Message", 30, 50,  65,25, 0,0,0,0, CLR_DEF);
    YWnd (&hOpen,   PUSH, "F-Files", 30, 85,  65,25, 0,0,0,0, CLR_DEF);
    YWnd (&hAbout,  PUSH, "About",   30,120,  65,25, 0,0,0,0, CLR_DEF);
    YWnd (&hHelp,   PUSH, "Help",    30,155,  65,25, 0,0,0,0, CLR_DEF);
    YEndGroup();

    YBeginGroup ("Colors", 325,235, 130,50, YColor(""));   
    YWnd (Ph(), COLORS, "", 30,15, 65,25, LP(colors),0,0,0, CLR_DEF);
    YEndGroup();

    break;        
  case MY_TESTWORK:
    if (YWaitSend (TRUE))
      break;
  case YTIMER:
    YWaitEnd (); 
    YKillTimer ();
    YKillProcess ();
    break;
  case YPUSH:
    if      (FALSE) YBeep();
    else if (mes1 == hWait) {
      YWaitBegin (WAIT, 20000, "Test work !");
      YSetTimer (id, 1000);
      YSetProcess (id, MY_TESTWORK);
    } 
    else if (mes1 == hOpen) {   
      if (ret_file = YFindFile ())  
				/* 	OUTS ((char*)ret_file); */
				YSend (hTest, YSTRING, 0,0,ret_file,0);
    }
    else if (mes1 == hMess)   YMessageBox ("Do you like beer?", "Yes_No");
    else if (mes1 == hAbout)  YDlg (ABOUT, "About", 0,0,0,0, 0,0,0,0, CLR_DEF, TRUE); 
    else if (mes1 == hHelp)   YHelpPages(); 
    break;
  case YCLOSE:       
    YWndClean (id);        
    break;   
  default:
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*------------------------------page_push----------------------------------*/
long
page_push (PFUNC_VAR)       
{        
  static int hTest = ID_NULL;

  switch (message) {       
  case YOPEN:       
    hTest = *((int*)YGet("wlong1"));
  case YDRAW:       
    YDrawRectF (0,0,WND->w,WND->h, YColor("lime") );        

    YBeginGroup ("PUSH", 30,40, 120,90, YColor("white"));   
    YWnd (Ph(), PUSH, "Push1", 30,20, 65,20, 0,0,0,0, YColor("red"));
    YWnd (Ph(), PUSH, "Push2", 30,50, 65,20, 0,0,0,0, YColor("gray"));
    YEndGroup ();

    break;        
  case YCLOSE:       
    YWndClean (id);        
    break;   
  default:
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*------------------------------page_clock----------------------------------*/
long
page_clock (PFUNC_VAR)       
{        
  static int hTest = ID_NULL;

  switch (message) {       
  case YOPEN:       
    hTest = *((int*)YGet("wlong1"));
  case YDRAW:       
    YDrawRectF (0,0,WND->w,WND->h, YColor("lime") );        

    YWnd (Ph(), CLOCK, "This is Clock", 30,30, WND->w-60,WND->h-60, 0,0,0,0, YColor("green"));

    break;        
  case YCLOSE:       
    YWndClean (id);        
    break;   
  default:
    YSend (hTest, message, mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*-----------------------------page4_proc--------------------------------------*/
long
page4_proc (PFUNC_VAR)
{
  static int hLay;

  switch (message) {
  case YOPEN:
  case YDRAW:
    YDrawRectFB (0, 0, WND->w, WND->h, YColor("green"), YColor("black"));

    YWnd (&hLay, LAYOUT,  "",  40,20, WND->w-80,WND->h-40, 0,0,0,0, YColor("lime"));
    YLayPut (hLay, 1, DITTO, "",  0,0,0,0, CLR_DEF);
    YLayPut (hLay, 2, DITTO, "",  0,0,0,0, CLR_DEF);
    YLayPut (hLay, 3, DITTO, "",  0,0,0,0, CLR_DEF);
    YLayPut (hLay, 4, DITTO, "",  0,0,0,0, CLR_DEF);

    break;
  case YCLOSE:
    YWndClean (id);
    break;
  default: ;;;;
  }

  RETURN_TRUE;
}
/*------------------------------page3_proc----------------------------------*/
long
page3_proc (PFUNC_VAR)       
{        
  char *top_menu[] = {
    "Main",   
    "  This is", 
    "  very good",
    "    item1",
    "    item2",
    "      item0",
    "      item1",
    "      item2",
    "    item3",
    "  program!!",
    "  ---",  
    "  Exit",     
    "Control",     
    "  PUSH",     
    "  LIST",     
    "  SCROLL",    
    "  COMBO",     
    "  TEXT",
    "  CHECK",
    "  and etc.",
    "Colors",
    "  Red",
    "  Green",
    "  Blue",
    "  Yellow",
    "Animals",
    "  Cat",
    "  ----",
    "  Dog",
    "  Wolf",
    "  Elephant",
    "Menu",
    "  Check flag| true",
    "  Make Me Grayed",
    "  Make Prev. Norm.",
    "  (ON/OFF): OFF",
    "  ----",
    "  Short menu",
    "Items",
    "  Item_1",
    "  Item_2",
    "  Item_3",
    "  Item_4",
    "  ----",
    "  Item_5",
    "  Item_6",
    "" 
  };
  
  char *top_menu_short[] = {
    "Main",   
    "  This is", 
    "  very good",
    "  ---",  
    "  Exit",     
    "Menu",
    "  Long menu",
    "Items",
    "  Item_1",
    "  Item_2",
    "  Item_3",
    "" 
  };

  static int hTest = ID_NULL;
  static int hMenu1=ID_NULL, hMenu2=ID_NULL, hMenu;
  char * str;

  static float fvalue=10.9;   
  static int hScroll, hScroll1;
  static int hFloat1, hFloat2;

  static char  **ptr_menu;

  switch (message) {       
  case YOPEN:  
    hTest = *((int*)YGet("wlong1"));
   case YDRAW:       
    YDrawRectF (0,0,WND->w,WND->h, YColor("fuchsia"));        

    YBeginGroup ("FLOATS", 110,170, 140,70, YColor("aqua"));  
    YWnd (&hFloat1, FLOATS, "Float1 ",  65,10, 60,20, LP(fvalue),YFLOAT,0,0, YColor("white"));   
    YWnd (&hFloat2, FLOATS, "Float2 ",  65,40, 60,20, LP(fvalue),YFLOAT,0,0, YColor("white"));   
    YEndGroup (); 

    YBeginGroup ("SCROLL", 280,150, 100,130, YColor("aqua"));  
    YWnd (&hScroll,  SCROLL,   "", 30, 10, 20,90, 100,40,10, YVERT, CLR_DEF);
    YWnd (&hScroll1, SCROLL_H, "", 05,105, 90,20, 100,30,20, YHORZ, CLR_DEF);
    YEndGroup (); 
 
    ptr_menu = top_menu;
  case YREDRAW:  
    YBeginGroup ("TOPMENU", 30,30, 430,100, YColor(""));
    YWnd (&hMenu1, TOPMENU, "", 10, 15, 410, 25, (long)ptr_menu, 0, 0, 0, YColor("aqua")); 
    YWnd (&hMenu2, TOPMENU, "", 10, 60, 410, 25, (long)ptr_menu, 0, 0, 0, YColor("yellow")); 
    YEndGroup();
    break;        
  case YMREGIONOUT:
    OUTD(33);
    break;
  case YMENU:
    str = (char *)mes3;
    hMenu = mes1;
    YSWITCH (str) {
    YCASE ("Main/very good/item2/item1")
      YSend (hMenu, YSET, TRUE, 0, 0, (long)str);
    YCASE ("Main/very good/item2/item2")
      YSend (hMenu, YSET, FALSE, 0, 0, (long)"Main/very good/item2/item1");
    YCASE ("Menu/Make Me Grayed")
      YSend (hMenu, YSET, TRUE, 0, 0, (long)"Menu/Make Me Grayed");
    YCASE ("Menu/Make Prev. Norm.")
      YSend (hMenu, YSET, FALSE, 0, 0, (long)"Menu/Make Me Grayed");
    YCASE ("Menu/Short menu")
      YUnWnd (hMenu);
      ptr_menu = top_menu_short;
      YGoto (YREDRAW, hMenu, 0, 0, 0);
    YCASE ("Menu/Long menu")
      YUnWnd (hMenu);
      /* YWndClose (hMenu); */
      ptr_menu = top_menu;
      YGoto (YREDRAW, 0, 0, 0, 0);
    YDEFAULT
    }
   /* fprintf(YSTDERR,"Choice menuitem = %s \n", mes3); */
    YSend (hTest,message,mes1,mes2,mes3,mes4);
    break;
  case YCLOSE:
    hMenu1=ID_NULL;
/*     hMenu2=ID_NULL; */
   /*  YWndClose (id);   */      
    YWndClean (id);        
    break;   
  default:
    YSend (hTest,message,mes1,mes2,mes3,mes4);
  }       
  
  RETURN_TRUE;
}
/*------------------------------TESTMSG---------------------------------*/
long
TESTMSG (PFUNC_VAR)       
{        
  static int  hMess=ID_NULL;
  static char str[100];

  switch (message) {       
  case YOPEN:       
  case YDRAW:       
    YDrawRectB (0,0, WND->w,WND->h, YColor("black"));        
    YWnd (&hMess, YTEXT, "",  1,1, WND->w-2,WND->h-2, (long)str,0,0,0, CLR_DEF);
    break;        
  case YCLOSE:       
    YWndClean (id);        
    break; 
  default:
    switch (message) {       
    case YLMOUSEDOWN:  sprintf (str, "LMOUSEDOWN:   %d  %d", mes1, mes2);  break;
    case YLMOUSEUP:    sprintf (str, "YLMOUSEUP:    %d  %d", mes1, mes2);  break;
    case YLMOUSEDRAG:  sprintf (str, "YLMOUSEDRAG:  %d  %d", mes1, mes2);  break;
    case YRMOUSEDOWN:  sprintf (str, "YRMOUSEDOWN:  %d  %d", mes1, mes2);  break;
    case YRMOUSEUP:    sprintf (str, "YRMOUSEUP:    %d  %d", mes1, mes2);  break;
    case YRMOUSEDRAG:  sprintf (str, "YRMOUSEDRAG:  %d  %d", mes1, mes2);  break;
    case YPUSH:        
      sprintf (str, "YPUSH:  %s", WNDI(mes1)->name);           
      break;
    case YCHECK:  
      sprintf (str, "YCHECK:  %s  %s", WNDI(mes1)->name, mes2 ? "TRUE" : "FALSE");           
      break;
    case YFLOATS:  
      sprintf (str, "YFLOATS:  ");           
      break;
    case YSCROLL:        
      sprintf (str, "YSCROLL:  %d", mes1);           
      break;
    case YLIST:        
      sprintf (str, "YLIST:  %s", (char*)mes4);           
      break;
    case YMENU:        
      sprintf (str, "YMENU:  %s", (char*)mes3);           
      break;
    case YEDIT:        
      sprintf (str, "YEDIT:  %s  %s", WNDI(mes1)->name, (char*) mes3);           
      break;
    case YDECIMAL:        
      sprintf (str, "YDECIMAL:  %s  %d", WNDI(mes1)->name, mes3);           
      break;
    case YSTRING:        
      sprintf (str, "YSTRING:  %s ", (char*)mes3);           
      break;
    }
    YWndUpdate (hMess);
    break;
  }
  
  return(TRUE); 
}
/*----------------------------------------------------------------------*/


#endif
#ifdef  D_FLTK_TEST1  
/*************************************************************************
 *                                                                       *
 *        Test1 for library FLTK                                         *  
 *                                                                       *
 *************************************************************************
 */ 

#include "y_zone.h" 

enum ykeys_global {                                                             
  MYPUSH_DOWN = YKEY_LEVEL4,                                                    
  MYPUSH_UP                                                                     
}; 

/*----------------------------------main_proc---------------------------------*/
long
main_proc (PFUNC_VAR)       
{        

  switch (message) {       
  case YOPEN: 
  case YDRAW:       
		printf("YOPEN : %d %d %d %d  \n", 0,0, WND->w,WND->h);

    YPaintRectF (30,30, WND->w-60,WND->h-60, YColor("blue"));
		YPaintLine  (30,30, WND->w-60,WND->h-60, YColor("white"));

    YWnd (Ph(), PUSH,   "PUSH",    70,10, 70,30, 0,0,0,0, YColor("yellow"));
    YWnd (Ph(), F_PUSH, "лопрлб", 150,10, 70,30, 0,0,0,0, YColor("yellow"));

    YWnd (Ph(), CHECK,   "CHECK",    70, 60, 30,30, 0,0,0,0, YColor("yellow"));
    YWnd (Ph(), F_CHECK, "F_CHECK",  70,100, 30,30, 0,0,0,0, YColor("yellow"));

    YWnd (Ph(), SCROLL,    "",  70,140, 30,140, 100,40,10, 0, YColor("yellow"));
    YWnd (Ph(), F_SCROLL,  "", 120,140, 30,140, 100,40,10, YVERT, YColor("yellow"));

    YWnd (Ph(), EDIT,    "Edit:",  220,140, 100,30, (long)"string1",0,0,0, YColor("yellow"));
    YWnd (Ph(), F_EDIT,  "Edit:",  220,180, 100,30, (long)"string2",0,0,0, YColor("yellow"));
    YWnd (Ph(), F_EDIT,  "Edit:",  220,220, 100,30, (long)"string3",0,0,0, YColor("yellow"));

    YWnd (Ph(), F_CLOCK,  "Edit:",  220,260, 100,100, 0,0,0,0, YColor("yellow"));
    break;        
  case YPUSH:
		printf("YPUSH \n");
    break;        
  case YCHECK:
		printf("YCHECK \n");
    break;        
  case YSCROLL:
		printf("YSCROLL %d \n", mes1);
    break;        
  case YLMOUSEDOWN:
		printf("YLMOUSEDOWN (%d %d) \n", mes1, mes2);
		YFindFile (); 
    break;        
  case YLMOUSEUP:
		printf("YLMOUSEUP (%d %d) \n", mes1, mes2);
    YWnd (Ph(), PUSH, "PUSH", mes1,mes2, 70,30, 0,0,0,0, YColor("yellow"));
    break;        
  case YRMOUSEDOWN:
		printf("YRMOUSEDOWN (%d %d) \n", mes1, mes2);
    break;        
  case YRMOUSEUP:
		printf("YRMOUSEUP (%d %d) \n", mes1, mes2);
    YWnd (Ph(), F_PUSH, "F_PUSH", mes1,mes2, 70,30, 0,0,0,0, YColor("yellow"));
    break;        
  case YKEYBOARD:
		printf("YKEYBOARD \n");
    break;        
  case YCLOSE:       
    /* YWndClean (id);  */       
    break; 
  default: ;;;;
  }       
  
  RETURN_TRUE; 
}
/*-------------------------------MAIN-----------------------------------*/
long
MAIN (PFUNC_VAR)
{

  switch (message) {
  case YCREATE:
    YInitWIND (); 
		printf("MAIN-YCREATE \n");
    YBigWindow (NULL, main_proc, "Dial FLTK", 0,0, 550,450, 0,0,0,0, CLR_DEF); 
    break; 
  case YFINAL:
		printf("MAIN-YFINAL \n");
    break; 
  }

  id++;
  RETURN_TRUE;
}
/*----------------------------------------------------------------------*/
#endif
#ifdef  D_FLTK_TEST2  
/*************************************************************************
 *                                                                       *
 *        Test2 for library FLTK                                         *  
 *                                                                       *
 *************************************************************************
 */ 

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>                                                        
#include <FL/Fl_Input.H>                                                      
#include <FL/Fl_Button.H>                                                     
#include <FL/Fl_Return_Button.H>                                              
                                                                              

Fl_Menu_Item big_menu[] = {
  {"Buffers", 0,0,0, FL_SUBMENU},
    {"New Window", 0,0,0, FL_MENU_DIVIDER}, 
    {"Open ..."},
    {"Save"},     
    {"Save All"},     
    {"Save As ...", 0,0,0, FL_MENU_DIVIDER},     
    {"Kill"},     
    {"Split"},     
    {"Unsplit"},     
    {"List All", 0,0,0, FL_MENU_DIVIDER},     
    {"Exit"},     
    {0},
  {"Edit", 0,0,0, FL_SUBMENU},
    {"Undo"},
    {"Redo", 0,0,0, FL_MENU_DIVIDER},
    {"Cut"},
    {"Copy"},
    {"Paste"},
    {0},
  {"Search", 0,0,0, FL_SUBMENU},
    {"Find ..."},
    {"Find Again ...", 0,0,0, FL_MENU_DIVIDER},
    {"Replace ..."},
    {"Replace Again", 0,0,0, FL_MENU_DIVIDER},
    {"Goto Line"},
    {0},
  {"Project", 0,0,0, FL_SUBMENU},
    {"Open ..."},
    {"Close ..."},
    {"New", 0,0,0, FL_MENU_DIVIDER},
    {"Options ...", 0,0,0, FL_MENU_DIVIDER},
    {"Run ..."},
    {0},
  {"Compile", 0,0,0, FL_SUBMENU},
    {"Make All"},
    {"Make ...", 0,0,0, FL_MENU_DIVIDER},
    {"Next Error"},
    {"Prev Error", 0,0,0, FL_MENU_DIVIDER},
    {"Help Error"},
    {0},
  {"Debug", 0,0,0, FL_SUBMENU},
    {"Run/Continue"},
    {"Stack Next"},
    {"Stack Prev", 0,0,0, FL_MENU_DIVIDER},
    {"Goto"},
    {"Step"},
    {"Next", 0,0,0, FL_MENU_DIVIDER},
    {"Set B-point"},
    {"Del B-point", 0,0,0, FL_MENU_DIVIDER},
    {"Stop"},
    {0},
  {"Options", 0,0,0, FL_SUBMENU},
    {"Big Menu"},
    {"Min Menu", 0,0,0, FL_MENU_DIVIDER},
    {"Text Font"},
    {0},
  {"Help", 0,0,0, FL_SUBMENU},
    {"About"},
    {"Index"}, 
    {0},
  {0}
};


Fl_Menu_Item min_menu[] = {
  {"Buffers", 0,0,0, FL_SUBMENU},
    {"New Window", 0,0,0, FL_MENU_DIVIDER}, 
    {"Open ..."},
    {"Save"},     
    {"Save All"},     
    {"Save As ...", 0,0,0, FL_MENU_DIVIDER},     
    {"Kill"},     
    {"Split"},     
    {"Unsplit"},     
    {"List All", 0,0,0, FL_MENU_DIVIDER},     
    {"Exit"},     
    {0},
  {"Edit", 0,0,0, FL_SUBMENU},
    {"Undo"},
    {"Redo", 0,0,0, FL_MENU_DIVIDER},
    {"Cut"},
    {"Copy"},
    {"Paste"},
    {0},
  {"Search", 0,0,0, FL_SUBMENU},
    {"Find ..."},
    {"Find Again ...", 0,0,0, FL_MENU_DIVIDER},
    {"Replace ..."},
    {"Replace Again", 0,0,0, FL_MENU_DIVIDER},
    {"Goto Line"},
    {0},
  {"Options", 0,0,0, FL_SUBMENU},
    {"Big Menu"},
    {"Min Menu", 0,0,0, FL_MENU_DIVIDER},
    {"Text Font"},
    {0},
  {"Help", 0,0,0, FL_SUBMENU},
    {"About"},
    {"Index"}, 
    {0},
  {0}
};

  Fl_Window *window;

/*--------------------------------------------------------------------*/
int 
MessageBox (char* message) 
{   
	int w=320, h=200;
                                              
  Fl_Window window (w,h);                                                   

  Fl_Box box(FL_BORDER_BOX, 30,20, w-60,h-10-60, message);  
	box.color(FL_CYAN);

/*   Fl_Button    cancel( 60, h-40, 80, 25, "cancel");                                  */
  Fl_Return_Button ok (w/2-40, h-40, 80, 25, "OK");                                 
	ok.color (FL_CYAN);

	window.color (FL_BLUE);
  window.hotspot(&/* cancel */ok); // you must position modal windows                 
  window.end();                                                               
  window.set_modal();                                                         
  window.label("MessageBox");                                                              
  window.show();                                                              

  for (;;) {                                                                  
    Fl::wait();                                                               
    Fl_Widget *o;                                                             

    while ((o = Fl::readqueue())) {                                           
      if (o == &ok)                                                          
        return 1;                                                             
      else                             
        return 0;                                                             
    }                                                                         
  }                                                                           

}                                                                             
/*----------------------------------------------------*/
void 
test_cb (Fl_Widget* w, void*) 
{
  Fl_Menu_* mw = (Fl_Menu_*)w;
  const Fl_Menu_Item* m = mw->mvalue();

  if (!m) {
    printf("NULL\n");
		return;
	}

	if      (strcmp(m->label(), "Exit")==0) 
		exit(0);
	else if (strcmp(m->label(), "Min Menu")==0) {
		printf("-- Min Menu -- \n");
		window->make_current();
		fl_color(FL_RED);  
		fl_rectf(50, 50, 20, 20);

		static Fl_Button b1(60, 100, 80, 25, "&Beep");                       
		window->add(b1);

		window->redraw();
		

		/* mw->menu(big_menu); */
		/* (w->parent())->redraw(); */
		/* redraw(); */
		/* w->show(); */ /* ??? */
	} else {
		MessageBox (m->label());
	}

  if (m->shortcut())
    printf("%s - %s\n", m->label(), fl_shortcut_label(m->shortcut()));
  else
    printf("%s\n", m->label());

	/* char buffer[80]; */

}
/*-----------------------------------main------------------------------*/
int 
main (int argc, char **argv) {

	int h_menu;
	int x, y, w, h;
	int width  = 600;
	int height = 400;

  window = new Fl_Window(width,height);

	h_menu = 30;

  Fl_Menu_Bar menubar (0,0,width,h_menu); 

	menubar.menu(big_menu);
  menubar.callback(test_cb);
	menubar.color(FL_WHITE);
  menubar.selection_color(FL_BLUE);

  window->size_range(300,20);
  window->color(FL_GREEN);

	x = 170;
	y = 20 + h_menu;
	w = width - 190;
	h = height - h_menu - 40;
  Fl_Box *box = new Fl_Box (FL_BORDER_BOX, x,y,w,h, "Hello, World!");  
	box->color(FL_YELLOW);

  window->end();
  window->show(argc, argv);
  window->make_current();

  return Fl::run();
}
/*******************************************************************************/

#endif
/*******************************************************************************/

