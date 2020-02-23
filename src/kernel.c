/**************************************************************************** 
 *                                                                          * 
 *  kernel.c  --  "Library Yzone" (main file);                              * 
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
                                                                              
 
 
 
#if defined(D_OPEN) 
/**************************************************************************** 
 *                                                                          * 
 *                       OPEN OPEN OPEN OPEN OPEN OPEN                      * 
 *                                                                          * 
 **************************************************************************** 
 */  
 
/* #include "y_open.h"   */
#include "y_zone.h" 
 
 
#if defined(BORL_W) || defined(MICR_W) || defined(GCC__W) || defined(LCC__W) // Changes made by Ozerski 
#define API_W 
#endif 
 
                                                                              

#if defined(BORL_W) 
#include  <alloc.h>           
#include  <dir.h>           
#include  <dos.h>           
#include  <windows.h>  
// Changes made by Ozerski 
#elif defined(MICR_W) || defined(GCC__W) || defined(LCC__W) 
#include  <sys/timeb.h>           
#include  <direct.h>   
#include  <time.h>           
#include  <io.h>           
//#define __YERROR__(str) YERROR(str) 
#include  <windows.h> /* ?? */ 
//#define YERROR(str) __YERROR__(str) 
//End Changes 
#else  
#include  <unistd.h> 
#include  <X11/Xlib.h> 
#include  <X11/X.h> 
#include  <X11/Xatom.h> 
#include  <X11/Xutil.h> 
#include  <X11/keysym.h> 
#include  <X11/keysymdef.h> 
#include  <X11/cursorfont.h> 
#include  <dirent.h> 
#include  <sys/time.h> 
#endif  
 
/***************************************************************************/ 
 
 
#define PH   (WND->ch_i<=MAX_CHILD ? &WND->ch[WND->ch_i++] : (int*)NULL) 
 
YT_KERN *kern; 
 
typedef struct { 
  int     id; 
  long    start; 
  long    delta; 
} YT_TIMER; 
 
typedef struct { 
  int     id; 
  int     message; 
} YT_PROCESS; 
 
typedef struct { 
  int     id; 
  int     from; 
  int     message; 
  int     mes1; 
  int     mes2; 
  long    mes3; 
  long    mes4; 
} YT_SVMSG; 
 
#define  MQ_LENGHT   50 
typedef struct { 
  int     head; 
  int     tail; 
  int     num; 
  YT_SVMSG buf[MQ_LENGHT]; 
} YT_QUEUE; 
 
#if defined(API_W) 
#define YTCOL(color) ((COLORREF)color) 
#else /* --------------------- */ 
#define YTCOL(color) ((unsigned long)color) 
#endif 
 
#define CLIP_SIZE  20 
typedef struct { 
#if defined(API_W) 
#else  
  GC      gc; 
  Window  win; 
  Region  region; 
  Region  regs[CLIP_SIZE]; 
#endif 
  int     regs_i; 
  int     layout; 
  int     id_main; 
  YT_PFUNC main_proc; 
	/*   int     id_dialog; */ 
  int     screen_w, screen_h; 
} YT_BIGWND; 
 
#if defined(API_W) 
static struct { 
  HINSTANCE hInstance; 
  HINSTANCE hPrevInstance; 
  LPSTR   lpCmdLine; 
  int     nCmdShow; 
  HWND    hWnd; 
  HDC     hdc; 
  HDC     hdcMemory; 
  HPEN    hpen, hpenOld; 
  HBRUSH  hbr, hbrOld; 
  YT_BOOL is_readsystem; 
  YT_SVMSG *pMsg; 
  RECT    rect; 
  HPALETTE hpal; 
} svw; 
 
static struct { 
  WORD    palVersion; 
  WORD    palNumEntries; 
  PALETTEENTRY palPalEntry[CLR_MAX]; 
} lgpl; 
 
LRESULT CALLBACK  
MainWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);  
#else  
 int     KisaSetColor (Display * dpy, int red, int green, int blue);  
#endif 
 
 
#define  ZIRAND(imin,imax) (int)(YRAND((float)(imin),(float)(imax))) 
#define  NONULL ((void*)!NULL) 
#define  LNULL (long)NULL 
 
#define  NUM_KP 4 
typedef struct { 
  int     type, size; 
  int     kp[NUM_KP]; 
  int     rr[NUM_KP]; 
  int     gg[NUM_KP]; 
  int     bb[NUM_KP]; 
} YT_DEFPALETE; 
 
#define  BIGWND_NUM 20 
 
#define  SV  (KERN_S->bigwnds[KERN_S->bigw_cur]) 
 
#define  WID_OPENED   KERN_S->wid_opened 
#define  MID_OPENED   KERN_S->mid_opened 
 
/* typedef struct { */ 
/*   char     name[30]; */ 
/*   YT_COLOR color; */ 
/* } YT_RCOLOR; */ 
 
#define  W_SCREEN   SV->screen_w 
#define  H_SCREEN   SV->screen_h 
#define  MUDS      (KERN_S->modules) 
#define  DPY        KERN_S->dpy 
#define  SCR        KERN_S->screen 
#define  ID_MAIN   (SV->id_main) 
#define  FONTH     (KERN_S->font_h) 
 
#define  NONE	    0 
#define  MOTION	    1 
#define  LEFTDOWN   2 
#define  RIGHTDOWN  3 
#define  LEFTUP     4 
#define  RIGHTUP    5 
 
void     YWndUpdatePost (int); 
 
void     KerSetFont (char *name); 
void     KerFreeFont (void); 
/* void     YRedrawAll (void);  */
void     YRedrawAllNew (void); 
 
void     KerCreateWindow (char *wname, int w, int h); 
void     KerClipInit (int, int, int, int); 
void     KerClipDel (void); 
 
YT_BOOL  read_timer (YT_SVMSG *); 
YT_BOOL  read_queue (YT_SVMSG *); 
YT_BOOL  KerReadSystem (YT_SVMSG *); 
void     YCheckProcess (void); 
void     YSetColors (int type_scale); 
YT_COLOR YRgbPalette (int r, int g, int b, YT_BOOL first); 
YT_COLOR YSetRgbPalette (int, int, int); 
/* int   KisaSetColor (Display * dpy, int red, int green, int blue); */ 
YT_COLOR YGetRgbPalette (int, int, int); 
 
YT_BOOL  tr_mouse (YT_SVMSG *, int, int, int); 
void     YCloseGraph (void); 
 
#define  COLER(n)  (KERN_S->globcoller[n]) 
YT_COLOR YColer (int n); 
void     YSetColler (YT_BOOL first); 
void     YSetScale (int size_scale, int type_scale, YT_BOOL first); 
void     YColerScale (int type_scale, int size_colors, YT_BOOL first); 
void     YDefColor (YT_BOOL, int r, int g, int b, char *name); 
  
void     YGetDEFPALETE (int i, int *r, int *g, int *b); 
void     YSetDEFPALETE (int type, int size); 
 
typedef struct { 
  char   *name; 
  int     r, g, b; 
} YT_DEFCOLOR; 
 
void     YDefColors (YT_DEFCOLOR*); 
YT_COLOR YSetWndColorI(int id, YT_COLOR def_color); 
 
void     FSetColor (YT_COLOR color);

long     YLIB_PROC (PFUNC_VAR); 
YT_BOOL  KerReadSystem (YT_SVMSG *); 
YT_BOOL  YCheckEvents (void); 
long     YMain_sys (int argc, char *argv[]); 
void     YInitDisplay (void); 
void     YLoop (YT_BOOL *); 
YT_BOOL  YCheckMin (YT_SVMSG *pmsg); 
void     YDispatchMsg (YT_SVMSG *pmsg); 
 
#define  LOOP_WND(i)  for(i=FLAI_BEG;i<=FLAI_END;i++) 
#define  IN_WND(i)    (i>=FLAI_BEG && i<=FLAI_END ? TRUE : FALSE) 
#define  WND_CREATED(i)  (WNDI(i)!=NULL) 
#define  WND_OPENED(i)  (WNDI(i)->opened) 
 
typedef struct { 
  YT_BOOL exist; 
  char    name[30];  
  int     r, g, b; 
  YT_COLOR color; 
} YT_COLER; 
 
typedef struct { 
#if defined(API_W) 
#else  
  Display *dpy; 
  int      screen; 
  Colormap cmap; 
  XFontStruct *font_struct; 
  Font         font; 
#endif 
  YT_COLER   globcoller[SIZE_COLER]; 
  YT_BIGWND *bigwnds[BIGWND_NUM]; 
  int        bigw_cur; 
  char      *parse_geometry, *parse_name; 
  FILE      *par_file; 
  YT_DEFPALETE ds; 
  char     *text1; 
  char     *text2; 
  YT_BOOL   wndgroup_flag; 
  int       wndgroup_first; 
  int       wndgroup_last; 
  char      buffer[300]; 
  int       buffer_l; 
  int       wnd_data; 
  int       font_h; 
  YT_BOOL   dialog_loop; 
  long      dialog_ret; 
  YT_PFUNC  dialog_proc;  
  int       wnd_focus; 
  int       mregion_x, mregion_y, mregion_w, mregion_h; 
  int       mregion_id; 
  /*--------------------*/ 
  long         main_ret; 
  YT_BOOL      main_loop; 
  YT_TIMER     timer; 
  YT_PROCESS   process; 
  YT_QUEUE     q; 
  /*--------------------*/ 
	long   ywin;
} YT_KERN_S; 
 
void    *kern_s; 
#define  KERN_S ((YT_KERN_S *)(kern_s))  
 

#if defined(FLTK) /*---------------------------FLTK FLTK--------------------*/  

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>                                                        
#include <FL/Fl_Input.H>                                                      
#include <FL/Fl_Button.H>                                                     
#include <FL/Fl_Return_Button.H>                                              
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/x.H>

Fl_Window *fltkwin;

/* #define GDI_FLTK  */
#define FLTK_CONTROLS 

/*-------------------------------FL_to_Y------------------------------------*/ 
YT_BOOL
FL_to_Y (int e, YT_SVMSG *pMsg) 
{ 
	YT_BOOL b = FALSE;
  /* pMsg->from = ID_NULL;  */
	int key;
 
  switch (e) { 
  case FL_PUSH:
		if (Fl::event_button() == 1)
			b = tr_mouse (pMsg, LEFTDOWN, Fl::event_x(), Fl::event_y()); 
		else 
			b = tr_mouse (pMsg, RIGHTDOWN, Fl::event_x(), Fl::event_y()); 
    break; 
  case FL_RELEASE: 
		if (Fl::event_button() == 1)
			b = tr_mouse (pMsg, LEFTUP, Fl::event_x(), Fl::event_y()); 
		else 
			b = tr_mouse (pMsg, RIGHTUP, Fl::event_x(), Fl::event_y()); 
    break; 
  case FL_DRAG: 
    b = tr_mouse (pMsg, MOTION, Fl::event_x(), Fl::event_y()); 
    break; 
  case FL_FOCUS: 
		/* YKillFocus(); */
		/* printf("FL_FOCUS \n"); */
    break; 
  case FL_UNFOCUS: 
		/* YKillFocus(); */
		/* printf("FL_UNFOCUS \n"); */
    break; 
  case FL_KEYBOARD: 
    pMsg->message = YKEYBOARD; 
    pMsg->id = YGetFocus ();

    key = Fl::event_key(); 

    if      (key == FL_Up)         pMsg->mes1 = YK_UP; 
    else if (key == FL_Down)       pMsg->mes1 = YK_DOWN; 
    else if (key == FL_Page_Up)    pMsg->mes1 = YK_PAGEUP; 
    else if (key == FL_Page_Down)  pMsg->mes1 = YK_PAGEDOWN; 
    else if (key == FL_Delete)     pMsg->mes1 = YK_DELETE; 
    else if (key == FL_Left)       pMsg->mes1 = YK_LEFT; 
    else if (key == FL_Right)      pMsg->mes1 = YK_RIGHT; 
    else if (key == FL_Home)       pMsg->mes1 = YK_HOME; 
    else if (key == FL_End)        pMsg->mes1 = YK_END; 
    else if (key == FL_BackSpace)  pMsg->mes1 = YK_BACK; 
		else                           pMsg->mes1 = key; 

		b = TRUE;
    break; 
  default:  ;;;;
  } 
 
  return (b); 
} 
/*--------------------------------------------------------------------------*/
#define MY_FL_WINDOW Fl_Window

class My_Window : public MY_FL_WINDOW {
  int handle(int);
  void draw();
public:
  My_Window(int x,int y,const char *l)
    : MY_FL_WINDOW(x,y,l) {;;}
  My_Window(int x,int y,int w,int h,const char *l)
    : MY_FL_WINDOW(x,y,w,h,l) {;;}
};
/* 	Fl::set_font((Fl_Font)1, "-*-fixed-*-r-*-*-*-*-*-*-*-*-*-*"); */
/* 	fl_font((Fl_Font)1, 16); */

void My_Window::draw() {
  YSetFont(Y_FONT); 
  /* YSetColors (YCOLORED);  */ 

	Fl_Widget *const*a = array();
	if (damage() == FL_DAMAGE_CHILD) { // only redraw some children
		for (int i = children(); i --; a ++) update_child(**a);
	} else { // total redraw
		/* ... draw background graphics ... */
		YRedrawAll();
		// now draw all the children atop the background:
		for (int i = children(); i --; a ++) {
			draw_child(**a);
			draw_outside_label(**a); // you may not want to do this
		}
	}
	YSetCliping (FALSE, 0, 0, 0, 0);	/*??*/
}

int My_Window::handle(int e) {
	static YT_SVMSG Msg;

	if (FL_to_Y (e, &Msg)) 
		YDispatchMsg(&Msg); 

  if (MY_FL_WINDOW::handle(e)) return 1;
  return 0;
}

#undef MY_FL_WINDOW

/* #include "y_more.h"  */
#include "y_zone.h" 

#endif /* FLTK */ 

/***************************************************************************/ 
/*                                                                         */ 
/*                                                                         */ 
/*                                                                         */ 
/***************************************************************************/ 
 
 
/*--------------------------------YInt--------------------------------------*/ 
int 
YInt (float f) 
{ 
  int     i; 
  float   o; 
 
  i = (int) f; 
  o = f - i; 
  if      (o > 0.5)   i++; 
  else if (o < -0.5)  i--; 
 
  return (i); 
} 
/*--------------------------------YRandF------------------------------------*/ 
float 
YRandF (float fmin, float fmax) 
{ 
#if defined(BORL_D) || defined(API_W) 
  int     irand, imax = 10000; 
  irand = YRAND (0, imax); 
  return (fmin + irand * ((fmax - fmin) / imax)); 
#else  
  float   choise; 
  choise = drand48 (); 
  return (fmin + choise * (fmax - fmin)); 
#endif 
} 
/*------------------------------ZSortStrings--------------------------------*/ 
int 
zsortstrings_func (const void *a, const void *b) 
{ 
  char   *string1, *string2; 
 
  string1 = *((char **) a); 
  string2 = *((char **) b); 
  return (strcmp (string1, string2)); 
} 
/*--------------------*/ 
void 
ZSortStrings (char **ptr) 
{ 
  int     nelem = 0; 
 
  while ((*(ptr + nelem)) != NULL) 
    nelem++; 
  qsort ((void *) ptr, nelem, sizeof (*ptr), zsortstrings_func); 
 
  return; 
} 
/*-----------------------------ZSortDirStrings------------------------------*/ 
int 
zsortdirstrings_func (const void *a, const void *b) 
{ 
  char   *string1, *string2; 
 
  string1 = *((char **) a); 
  string2 = *((char **) b); 
  if (string1[0] == '\\' && string2[0] != '\\')  return (-1); 
  if (string1[0] != '\\' && string2[0] == '\\')  return (1); 
 
  return (strcmp (string1, string2)); 
} 
/*--------------------------------------*/ 
void 
ZSortDirStrings (char **ptr) 
{ 
  int     nelem = 0; 
 
  while ((*(ptr + nelem)) != NULL) 
    nelem++; 
  qsort ((void *) ptr, nelem, sizeof (*ptr), zsortdirstrings_func); 
 
  return; 
} 
/*------------------------------YDirRead------------------------------------*/ 
char  ** 
YDirRead (void) 
{ 
#define YMAXNUMBER 200 
#define YMAXLENGHT 20 
  int     i; 
  static char *buffer[YMAXNUMBER]; 
  static char fnames[YMAXNUMBER][YMAXLENGHT]; 
 
#if defined(BORL_D) || defined(BORL_W) /*---------------------*/           
           
  struct ffblk ffblk;           
  int done;           
           
  i=0;           
  done = findfirst("*.*", &ffblk ,FA_DIREC);           
  while (!done) {           
    if (strcmp(ffblk.ff_name,".")==0)     goto NEXT;           
    if (ffblk.ff_attrib==FA_DIREC) fnames[i][0]='\\';           
    else                           fnames[i][0]=' ';           
    if (strlen(ffblk.ff_name)>YMAXLENGHT-2 || i>YMAXNUMBER-1)           
      YError("ZScanDir");           
    strcpy(&(fnames[i][1]),ffblk.ff_name);           
    buffer[i]=fnames[i];           
    i++;           
  NEXT:           
    done=findnext(&ffblk);           
}           
//emgena #elif defined(MICR_W) || defined(GCC__W) || defined(LCC__W) // Changes made by Ozerski 
#elif defined(GCC__W) 
#elif defined(MICR_W) || defined(LCC__W) // Changes made by Ozerski 
           
  struct _finddata_t ffblk; 
  long    hFile; 
  YT_BOOL done = FALSE; 
 
  i = 0; 
  hFile = _findfirst ("*.*", &ffblk); 
 
  if (hFile == -1L) 
    done = TRUE; 
  while (!done) { 
    if (strcmp (ffblk.name, ".") == 0) 
      goto NEXT; 
    if (ffblk.attrib & _A_SUBDIR)  fnames[i][0] = '\\'; 
    else                           fnames[i][0] = ' '; 
 
    if (strlen (ffblk.name) > YMAXLENGHT - 2 || i > YMAXNUMBER - 1) 
      YError ("YDirRead"); 
    strcpy (&(fnames[i][1]), ffblk.name); 
    buffer[i] = fnames[i]; 
    i++; 
  NEXT: 
    if (_findnext (hFile, &ffblk) == -1L) 
      done = TRUE; 
  } 
  _findclose (hFile); 
#else /*--------------------*/ 
 
  DIR    *dirp, *dirtemp; 
  struct dirent *direntp; 
  dirp = opendir ("."); 
  i = 0; 
  while ((direntp = readdir (dirp)) != NULL) { 
    if (!strcmp (direntp->d_name, ".")) 
      continue; 
    if (dirtemp = opendir (direntp->d_name)) { 
      fnames[i][0] = '\\'; 
      closedir (dirtemp); 
    } else 
      fnames[i][0] = ' '; 
 
    if (strlen (direntp->d_name) > YMAXLENGHT - 2 || i > YMAXNUMBER - 1) 
      /* YError */YERROR ("YDirRead"); 
    strcpy (&(fnames[i][1]), direntp->d_name); 
    buffer[i] = &(fnames[i][0]); 
    i++; 
  } 
  closedir (dirp); 
 
  buffer[i] = NULL; 
  ZSortDirStrings (buffer); 
 
  return (buffer); 
#endif 
} 
/*-------------------------------YTimeStringMin--------------------------------*/ 
char* 
YTimeStringMin () 
{			 
#if defined(BORL_D) || defined(BORL_W)           
#elif defined(MICR_W) || defined(GCC__W) || defined(LCC__W) // Changes made by Ozerski 
#else /*-------------------*/ 
  struct tm *tim; 
  time_t timv = time (0); 
  static char string[80]; 
 
  tim = localtime (&timv); 
  sprintf (string, "%2d:%2d:%2d", tim->tm_hour, tim->tm_min, tim->tm_sec); 
  return (string); 
#endif 
} 
/*----------------------------------YTimeString--------------------------------*/ 
char* 
YTimeString () 
{			 
#if defined(BORL_D) || defined(BORL_W)           
#elif defined(MICR_W) || defined(GCC__W) || defined(LCC__W) // Changes made by Ozerski 
#else /*-------------------*/ 
  time_t timv = time (0); 
  return (ctime(&timv)); 
#endif 
} 
// Changes made by Ozerski 
#if defined(GCC__W) 
#define _timeb timeb 
#endif 
// End changes 
 
/*----------------------------------YTimeGet--------------------------------*/ 
long 
YTimeGet () 
{			 
#if defined(BORL_D) || defined(BORL_W)           
  struct time tm;           
  gettime(&tm);           
  return(tm.ti_hour*60l*60l*100l+tm.ti_min*60L*100L+tm.ti_sec*100L+tm.ti_hund);           
#elif defined(MICR_W) || defined(GCC__W) || defined(LCC__W) // Changes made by Ozerski 
  struct _timeb tb; 
  _ftime (&tb); 
  return (tb.time * 100 + tb.millitm / 10); 
#else /*-------------------*/ 
  struct timeval tp; 
  struct timezone tz; 
  gettimeofday (&tp, &tz); 
  return (tp.tv_sec * 100 + tp.tv_usec / 10000); 
#endif 
} 
/*-----------------------------------YBeep----------------------------------*/ 
void 
YBeep () 
{ 
 
  fprintf (stderr, "\a"); 
 
} 
/*------------------------------YDirChange----------------------------------*/ 
YT_BOOL 
YDirChange (char *new_dir) 
{ 
  YT_BOOL done; 
  if (chdir (new_dir) == 0)  done = TRUE; 
  else                       done = FALSE; 
  return (done); 
} 
/******************************************************************************/ 
/*-------------------------------main--------------------------------------*/ 
int 
main (int argc, char *argv[]) 
{ 
 
  YMain_sys (argc, argv); 
  return (0); 
 
} 
/*--------------------------------WinMain----------------------------------*/ 
#if defined(API_W) 
// Changes made by Ozerski 
#if defined(GCC__W) || defined(LCC__W) 
#define Win32 
#endif 
#if defined Win32 
#define WINAPI STDCALL 
#else 
#define WINAPI PASCAL 
#endif 
int     WINAPI 
//End changes 
WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine,  
	 int nCmdShow) 
{ 
  int     argc, i;  
  static char *argv[10] = {"svisor"}; 
  static char cmdline[250]; 
 
  svw.hInstance = hInstance; 
  svw.hPrevInstance = hPrevInstance; 
  svw.lpCmdLine = lpCmdLine; 
  svw.nCmdShow = nCmdShow; 
 
  strcpy (cmdline, lpCmdLine); 
  argv[1] = strtok (cmdline, " "); 
  for (i = 2; i < 10; i++) 
    if (!(argv[i] = strtok (NULL, " "))) 
      break; 
  argc = i; 
 
  YMain_sys (argc, argv); 
  return (0); 
 
} 
#endif 
/*-------------------------------YInitKERN------------------------------*/ 
void   
YInitKERN () 
{ 
  int  i, id; 
 
  YT_DEFCOLOR defcolors[] = { 
    { "black",       0,   0,   0},     { "green",     0, 128,   0}, 
  /*{ "silver",    192, 192, 192},     { "lime",      0, 255,   0},*/ 
    { "silver",   0xC0, 0xC0, 0xC0},   { "lime",   0x00, 0xFF, 0x00}, 
    { "gray",      128, 128, 128},     { "olive",   128, 128,   0}, 
    { "white",     255, 255, 255},     { "yellow",  255, 255,   0}, 
    { "maroon",    128,   0,   0},     { "navy",      0,   0, 128}, 
    { "red",       255,   0,   0},     { "blue",      0,   0, 255}, 
    { "purple",    128,   0, 128},     { "teal",      0 ,128, 128}, 
    { "fuchsia",   255,   0, 255},     { "aqua",      0, 255, 255}, 
    {NULL} 
  }; 
 
  if (!(KERN   = (YT_KERN*)  malloc (sizeof(YT_KERN))))    YERROR("YT_KERN");  
  if (!(kern_s = (YT_KERN_S*)malloc (sizeof(YT_KERN_S))))  YERROR("YT_KERN_S");  
  /* YMALLOC (kern_s, YT_KERN_S, 1);  */  
 
  KERN->sys_proc = NULL; 
  
  KERN_S->q.num = 0; 
  KERN_S->q.head = 0; 
  KERN_S->q.tail = 0; 
 
  KERN_S->process.id = ID_NULL; 
  KERN_S->timer.id = ID_NULL; 
  KERN_S->main_loop = TRUE; 
 
  KERN->numer = 0; 
 
  LOOP_WND (id) {    
    PWINDOW(id) = NULL; 
  } 
 
  Y_IDLAST = ID_NULL;   
  Y_WMODE  = FALSE; 
 
/*------------------------------- */ 
 
  KERN->sys_proc = YLIB_PROC; 
  KERN_S->dialog_ret = 0; 
  YSetDialog (NULL); 
  KERN_S->dialog_loop = TRUE; 
 
  KERN_S->wndgroup_flag = FALSE; 
  KERN_S->wndgroup_last = ID_NULL; 
 
  Y_WLPROC = ID_NULL; 
 
  KERN_S->wnd_focus = ID_NULL; 
 
  SECOND_DRAW = FALSE;   
 
  for (i = 0; i < BIGWND_NUM; i++)  KERN_S->bigwnds[i] = NULL; 
 
  KERN_S->par_file = NULL; 
   
  for (i = 0; i<SIZE_COLER; i++) { 
    COLER(i).exist = FALSE; 
    strcpy(COLER(i).name, ""); 
  } 
  YDefColors (defcolors); 
 
	/*   Y_FONT = "-*-fixed-*-r-*-*-*-*-*-*-*-*-*-*";  */
	Y_FONT = "-*-fixed-*-*-*-*-*-*-*-*-*-*-koi8-*";
	Y_FONT_SIZE = 16;
 
  YInitDisplay(); 
  YSetColors (YCOLORED); 
 
  return; 
} 
/*------------------------------------YSetDialog--------------------------------*/ 
void   
YSetDialog (YT_PFUNC dial) 
{ 
 
  KERN_S->dialog_proc = dial; 
 
} 
/*------------------------------YMain_sys---------------------------------*/ 
long 
YMain_sys (int argc, char *argv[]) 
{ 
 
  /* YInitCEXT (); */ 
  YInitKERN (); 
 
  CALL (MAIN, 0,0, YCREATE, 0, argc, (long)argv, 0);  
 
  while (1) { 
    if (!YCheckEvents ()) 
      break; 
  } 
 
  CALL (MAIN, 0,0, YFINAL, 0, 0, 0, 0);  
 
  return (KERN_S->main_ret); 
} 
/*------------------------------YSetTimer--------------------------------*/ 
void 
YSetTimer (int id, long delta) 
{ 
  if (KERN_S->timer.id != ID_NULL) 
    YERROR ("YSetTimer"); 
 
  KERN_S->timer.id = id; 
  KERN_S->timer.delta = delta; 
  KERN_S->timer.start = YTimeGet (); 
} 
/*------------------------------YKillTimer-------------------------------*/ 
void 
YKillTimer () 
{ 
 
  KERN_S->timer.id = ID_NULL; 
 
} 
/*-------------------------------read_timer-----------------------------*/ 
YT_BOOL 
read_timer (YT_SVMSG *pMsg) 
{ 
  long    cur_time, d_time; 
 
  if (KERN_S->timer.id == ID_NULL) 
    return (FALSE); 
 
  cur_time = YTimeGet (); 
  d_time = cur_time - KERN_S->timer.start; 
 
  if (d_time < KERN_S->timer.delta) 
    return (FALSE); 
 
  KERN_S->timer.start = cur_time; 
  pMsg->id = KERN_S->timer.id; 
  pMsg->from = ID_NULL; 
  pMsg->message = YTIMER; 
  pMsg->mes3 = 0l; 
  return (TRUE); 
} 
/*--------------------------------read_queue----------------------------*/ 
YT_BOOL 
read_queue (YT_SVMSG *pMsg) 
{ 
  YT_SVMSG *pBuf; 
 
  if (KERN_S->q.num == 0) 
    return (FALSE); 
  pBuf = &KERN_S->q.buf[KERN_S->q.head]; 
 
  pMsg->id   = pBuf->id; 
  pMsg->from = pBuf->from; 
  pMsg->message = pBuf->message; 
  pMsg->mes1 = pBuf->mes1; 
  pMsg->mes2 = pBuf->mes2; 
  pMsg->mes3 = pBuf->mes3; 
  pMsg->mes4 = pBuf->mes4; 
 
  KERN_S->q.num--; 
  KERN_S->q.head++; 
  if (KERN_S->q.head == MQ_LENGHT) 
    KERN_S->q.head = 0; 
 
  return (TRUE); 
} 
/*-------------------------------YPostFrom----------------------------------*/ 
YT_BOOL 
YPostFrom (int id, int from, int message, int mes1, int mes2, long mes3, long mes4) 
{ 
  YT_SVMSG *pBuf; 
 
  if (KERN_S->q.num == MQ_LENGHT - 2) { 
    YBeep (); 
    return (FALSE); 
  } 
  pBuf = &KERN_S->q.buf[KERN_S->q.tail]; 
 
  pBuf->id = id; 
  pBuf->from = from; 
  pBuf->message = message; 
  pBuf->mes1 = mes1; 
  pBuf->mes2 = mes2; 
  pBuf->mes3 = mes3; 
  pBuf->mes4 = mes4; 
 
  KERN_S->q.num++; 
  KERN_S->q.tail++; 
  if (KERN_S->q.tail == MQ_LENGHT) 
    KERN_S->q.tail = 0; 
 
  return (TRUE); 
} 
/*------------------------------------YPost----------------------------------*/ 
YT_BOOL 
YPost (int id, int message, int mes1, int mes2, long mes3, long mes4) 
{ 
  int from = Y_IDLAST; 
 
  return (YPostFrom (id, from, message, mes1, mes2, mes3, mes4)); 
 
} 
/*---------------------------------YExit--------------------------------------*/ 
void 
YExit (long main_ret) 
{ 
 
  KERN_S->main_ret = main_ret; 
  KERN_S->main_loop = FALSE; 
 
} 
/*---------------------------------YCheckProcess------------------------------*/ 
void 
YCheckProcess () 
{ 
  if (KERN_S->process.id == ID_NULL) 
    return; 
  YSend (KERN_S->process.id, KERN_S->process.message, 0, 0, 0, 0); 
} 
/*--------------------------------YDispatchMsg--------------------------------*/ 
void 
YDispatchMsg (YT_SVMSG *pmsg) 
{ 
 
  YSendFrom (pmsg->id, pmsg->from, pmsg->message, pmsg->mes1, pmsg->mes2,  
	     pmsg->mes3, pmsg->mes4); 
 
} 
/*--------------------------------YCheckEvents--------------------------------*/ 
YT_BOOL 
YCheckEvents () 
{ 
  YT_SVMSG msg; 
  YT_BOOL ret; 
  static int num = 1; 
   
 
  if (!KERN_S->main_loop) 
    return (FALSE); 
 
  /* YCheckProcess (); */ 
  ret = FALSE;  /*!!*/ 
 
  switch (num) { 
  case 1: 
    ret=read_queue(&msg); 
    /* if (ret) OUTS("read_queue"); */ 
    break; 
  case 2: 
/* #if defined(FLTK) */
/* 		Fl::wait(); */
/*     break;  */
/* #endif */
    /* ret=KerReadSystem(&msg); */ 
    if (KERN->sys_proc != NULL) 
      ret = KerReadSystem (&msg); 
      /* ret = CALL (KERN->sys_proc, 0, YKEYWORD1, 0, 0, LP(msg), 0); */   
    break; 
  case 3: 
    ret=read_timer(&msg); 
    break; 
  case 4: 
    YCheckProcess (); 
    break; 
  } 
  if (++num > 4) num=1; 
 
  if (ret)  YDispatchMsg (&msg); 
 
  return (TRUE); 
} 
/*-----------------------------YCheckSystem---------------------------------*/ 
void 
YCheckSystem () 
{ 
  YT_SVMSG msg; 
 
  if (YCheckMin(&msg))  YDispatchMsg (&msg); 
 
  return; 
} 
/*--------------------------------YCheckMin---------------------------------*/ 
YT_BOOL 
YCheckMin (YT_SVMSG *pmsg) 
{ 
 
  if      (read_queue (pmsg))    return (TRUE); 
  /* else if (KerReadSystem (&msg)) break; */ 
  else if ((KERN->sys_proc != NULL) &&  
	   KerReadSystem (pmsg) 
	   /* (CALL(KERN->sys_proc, 0, YKEYWORD1, 0, 0, LP(msg), 0))  */ 
	   )                     return (TRUE); 
  else if (read_timer (pmsg))    return (TRUE); 
 
  return (FALSE); 
} 
/*---------------------------------YLoop------------------------------------*/ 
void 
YLoop (YT_BOOL *looping) 
{ 
  YT_SVMSG msg; 
 
  while (*looping) { 
 
    while (TRUE) { 
      if (YCheckMin(&msg)) break; 
    } 
    YDispatchMsg (&msg); 
 
  } 
  return; 
} 
/*-----------------------------------YGoto-------------------------------------*/ 
long 
YGoto (int message, int mes1, int mes2, long mes3, long mes4) 
{ 
  YT_PFUNC proc; 
  int     id = Y_IDLAST; 
  int  from = ID_NULL; 
 
  if (IN_WND(id))  proc = WND->proc; 
  else  { 
    fprintf (stderr, "YGoto: error id = %d \n", id); 
    return (YNULL); 
  } 
 
  return (CALL (proc, id, from, message, mes1, mes2, mes3, mes4)); 
} 
/*-------------------------------YSendFrom----------------------------------*/ 
long 
YSendFrom (int id, int from, int message, int mes1, int mes2, long mes3, long mes4) 
{ 
  long    ret; 
  int     old_id_last; 
  YT_PFUNC proc; 
  YT_BOOL debug = FALSE; 
 
  if (id == ID_NULL)  { 
    if (debug) fprintf (stderr, "YSend-war: [%d] to ID_NULL \n", Y_IDLAST); 
    return (YNULL);   
  } 
  if (!(IN_WND(id)))  {    
    if (debug) fprintf (stderr, "YSend-err: [%d] to [%d] \n", Y_IDLAST, id); 
    return (YNULL); 
  } 
  if (!(WND_CREATED(id))) { 
    if (debug) fprintf (stderr, "YSend-err: [%d] to [%d] - not created \n", Y_IDLAST, id); 
    return (YNULL); 
  } 
 
  proc = WNDI(id)->proc;   
 
    if (KERN->sys_proc != NULL) YDrawContext (TRUE, id); 
 
    old_id_last = Y_IDLAST; 
    Y_IDLAST = id; 
    ret = CALL (proc, id, from, message, mes1, mes2, mes3, mes4); 
    Y_IDLAST = old_id_last; 
 
    if (KERN->sys_proc != NULL) YDrawContext (FALSE, ID_NULL); 
 
 
  return (ret); 
} 
/*----------------------------------YSend----------------------------------*/ 
long 
YSend (int id, int message, int mes1, int mes2, long mes3, long mes4) 
{ 
  int from = Y_IDLAST; 
 
  return (YSendFrom (id, from, message, mes1, mes2, mes3, mes4)); 
 
 
} 
/*-------------------------------YSetProcess-------------------------------*/ 
void 
YSetProcess (int id, int message) 
{ 
  if (KERN_S->process.id != ID_NULL) 
    YERROR ("YSetProcess"); 
 
  KERN_S->process.id = id; 
  KERN_S->process.message = message; 
} 
/*-----------------------------YKillProcess---------------------------------*/ 
void 
YKillProcess () 
{ 
 
  KERN_S->process.id = ID_NULL; 
 
} 
/*-----------------------------------Ph-------------------------------------*/ 
int  *Ph() 
{ 
 
  return((int*)(PH)); 
 
} 
/*----------------------------------YZoomMe------------------------------------*/ 
void 
YZoomMe (int id) 
{ 
  static int hZom = 0; 
 
  SECOND_DRAW = TRUE;   
  if (hZom == id) { 
    YWnd (&ID_MAIN, SV->main_proc, "", 0,0, W_SCREEN,H_SCREEN, 0,0,0,0, CLR_DEF); 
    hZom = 0; 
  } else if (hZom == 0) { 
    YSend (WND->parent, YZOOMME, id, 0, 0, 0); 
    hZom = id; 
  } 
  SECOND_DRAW = FALSE;  
 
  return; 
} 
/*-------------------------------YDrawContext------------------------------*/ 
void YDrawContext (YT_BOOL is_begin, int id) 
{ 
#define SIZE 300 
  static int    i = 0; 
  static int    old_x0[SIZE],  old_y0[SIZE]; 
  static float  old_x_mult[SIZE], old_y_mult[SIZE]; 
 
  if (is_begin) { 
    YGetOrigin (&old_x0[i], &old_y0[i]); 
    YGetMult   (&old_x_mult[i], &old_y_mult[i]); 
    YSetOrigin (WNDI(id)->x, WNDI(id)->y); 
    YSetMult   (WINI_X_MULT(id), WINI_Y_MULT(id)); 
    i++; 
    if (i == SIZE-1) YERROR ("YDrawContex"); 
  } else { 
    i--; 
    YSetOrigin (old_x0[i], old_y0[i]); 
    YSetMult   (old_x_mult[i], old_y_mult[i]); 
  } 
 
  return; 
#undef SIZE   
} 
/*---------------------------------YLIB_PROC-------------------------------*/ 
long 
YLIB_PROC (PFUNC_VAR) 
{ 
  YT_SVMSG  *p_msg; 
 
  switch (message) { 
  case YGETSIZE: 
    MYSIZE_IS (100,100); 
    break; 
  case YCREATE: 
    break; 
/*   case YKEYWORD1 : */ 
  case YKEYWORD1: 
    p_msg = (YT_SVMSG*)mes3; 
    return (KerReadSystem(p_msg)); 
  case YOPEN: 
  case YDRAW: 
    YPaintRectF (0, 0, WND->w,   WND->h,  YColor("white")/* , YColor("black") */); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: ;;;; 
  } 
 
  RETURN_TRUE; 
} 
/*------------------------------YGet_DPY------------------------------------*/ 
long 
YGet_DPY () 
{ 
 
#if defined(API_W) 
#else  
  return ((long)DPY); 
#endif 
 
} 
/*------------------------------YGet_SCR------------------------------------*/ 
long 
YGet_SCR () 
{ 
 
#if defined(API_W) 
#else  
  return ((long)SCR); 
#endif 
 
} 
/*------------------------------YGet_WIN------------------------------------*/ 
long 
YGet_WIN () 
{ 
 
#if defined(API_W) 
  return ((long)(svw.hWnd)); 
#else  
  return ((long)(SV->win)); 
#endif 
 
} 
/*---------------------------YInitDisplay-----------------------------------*/ 
void 
YInitDisplay () 
{ 
 
#if defined(API_W) 
  static WNDCLASS wc; 
 
  wc.style = 0; 
  wc.lpfnWndProc = MainWndProc; 
  wc.cbClsExtra = 0; 
  wc.cbWndExtra = 0; 
  wc.hInstance = svw.hInstance; 
  if (!(wc.hIcon = LoadIcon (NULL, IDI_APPLICATION)))      YERROR ("LoadIcon"); 
  if (!(wc.hCursor = LoadCursor (NULL, IDC_ARROW)))        YERROR ("LoadCursor"); 
  if (!(wc.hbrBackground = GetStockObject (WHITE_BRUSH)))  YERROR ("Background"); 
  wc.lpszMenuName = NULL; 
  wc.lpszClassName = "sv_class"; 
 
  if (!svw.hPrevInstance) 
    if (!RegisterClass (&wc)) 
      YERROR ("RegisterClass"); 
 
  Y_WDISPLAY = 800; 
  Y_HDISPLAY = 600; 
#else  
  DPY = XOpenDisplay (NULL); 
  SCR = DefaultScreen (DPY); 
  KERN_S->cmap   = DefaultColormap (DPY, KERN_S->screen); 
  Y_WDISPLAY = DisplayWidth  (DPY, KERN_S->screen); 
  Y_HDISPLAY = DisplayHeight (DPY, KERN_S->screen); 
#endif 
 
  return; 
} 
/*--------------------------------KerReadSystem-----------------------------*/ 
YT_BOOL 
KerReadSystem (YT_SVMSG *pMsg) 
{ 

#if defined(FLTK)
		Fl::check();
		/* Fl::wait(); */
		/* ?! message make in My_Window:handle and directly send to windows */
		/* YSetCliping (FALSE, 0, 0, 0, 0); */	/*!!?? htob ne obrezalis' Y-oko[ki ,
																			 a esli nado ?????????? */ 
    return(FALSE); 
#elif defined(API_W) 
  MSG     msg; 
  svw.pMsg = pMsg; 
  if (PeekMessage (&msg, NULL, 0, 0, PM_REMOVE)) { 
    TranslateMessage (&msg); 
    DispatchMessage (&msg); 
    return (svw.is_readsystem); 
  } 
  return (FALSE); 
#else /* ----------------------------- */ 
  char    buffer[5]; 
  int     action; 
  KeySym  keysym; 
  XRectangle rect; 
  XEvent  event; 
  int     i; 
	long    ywin;
 
  if (!XCheckMaskEvent (DPY, 
			ExposureMask | KeyPressMask | ButtonPressMask | 
			ButtonReleaseMask | ButtonMotionMask, 
			&event)) 
    return (FALSE); 
 
  for (i = 0; i < BIGWND_NUM; i++) { 
    KERN_S->bigw_cur = i; 
    if (SV->win == event.xany.window) 
      break; 
  } 
  if (i == BIGWND_NUM)  YERROR ("BIGWND_NUM");

	ywin = (long)event.xany.window;
	/* printf ("event.xany.window=%d \n", ywin); */
	KERN_S->ywin = ywin;
 
 
  switch (event.type) { 
  case KeyPress: 
    pMsg->message = YKEYBOARD; 
    pMsg->id = YGetFocus (); 
    XLookupString (&(event.xkey), buffer, 1, &keysym, NULL); 
    pMsg->mes1 = buffer[0]; 
    if      (keysym == XK_Up)     pMsg->mes1 = YK_UP; 
    else if (keysym == XK_Down)   pMsg->mes1 = YK_DOWN; 
    else if (keysym == XK_Prior)  pMsg->mes1 = YK_PAGEUP; 
    else if (keysym == XK_Next)   pMsg->mes1 = YK_PAGEDOWN; 
    else if (keysym == XK_Delete) pMsg->mes1 = YK_DELETE; 
    else if (keysym == XK_Left)   pMsg->mes1 = YK_LEFT; 
    else if (keysym == XK_Right)  pMsg->mes1 = YK_RIGHT; 
    else if (keysym == XK_Home)   pMsg->mes1 = YK_HOME; 
    else if (keysym == XK_End)    pMsg->mes1 = YK_END; 
    break; 
  case ButtonPress: 
    if (event.xbutton.button == Button1)  action = LEFTDOWN; 
    else                                  action = RIGHTDOWN; 
    tr_mouse (pMsg, action, event.xbutton.x, event.xbutton.y); 
    break; 
  case ButtonRelease: 
    if (event.xbutton.button == Button1)  action = LEFTUP; 
    else                                  action = RIGHTUP; 
    tr_mouse (pMsg, action, event.xbutton.x, event.xbutton.y); 
    break; 
  case MotionNotify: 
    { 
      Window  root, child; 
      int     pos_x, pos_y, root_x, root_y; 
      unsigned int keys_buttons; 
      while (XCheckMaskEvent (DPY, ButtonMotionMask, &event)) ;; 
      XQueryPointer (DPY, event.xmotion.window, &root, &child, &root_x, &root_y, 
		     &pos_x, &pos_y, &keys_buttons); 
      tr_mouse (pMsg, MOTION, pos_x, pos_y); 
    } 
    break; 
  case Expose: 
    { 
      XWindowAttributes w_a; 
 
      rect.x = (short) event.xexpose.x; 
      rect.y = (short) event.xexpose.y; 
      rect.width = (unsigned short) event.xexpose.width; 
      rect.height = (unsigned short) event.xexpose.height; 
      XUnionRectWithRegion (&rect, SV->region, SV->region); 
 
      if (event.xexpose.count == 0) { 
				XSetRegion (DPY, SV->gc, SV->region); 
        /* KerClipAdd(SV->region); */ 
 
				XGetWindowAttributes (DPY, SV->win, &w_a); 
				W_SCREEN = w_a.width; 
				H_SCREEN = w_a.height; 
				
				/* YRedrawAllNew (); */ 
				YRedrawAll (); 
        /* KerClipDel(); */ 
				
				XDestroyRegion (SV->region); 
				SV->region = XCreateRegion (); 
				
				YSetCliping (FALSE, 0, 0, 0, 0);	/*!!?? */ 
      } 
    } 
    return (FALSE); 
  default: break; 
  } 
  return (TRUE); 
#endif 
} 
 
#if defined(API_W) 
/*---------------------------MainWndProc------------------------------------*/ 
LRESULT CALLBACK 
MainWndProc (HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) 
{ 
  YT_SVMSG *pMsg; 
  YT_BOOL b = TRUE; 
	long ywin = (long)hwnd;
 
  pMsg = svw.pMsg; 
 
  switch (message) { 
  case WM_LBUTTONDOWN: 
    b = tr_mouse (pMsg, LEFTDOWN, LOWORD (lParam), HIWORD (lParam)); 
    break; 
  case WM_LBUTTONUP: 
    b = tr_mouse (pMsg, LEFTUP, LOWORD (lParam), HIWORD (lParam)); 
    break; 
  case WM_RBUTTONDOWN: 
    b = tr_mouse (pMsg, RIGHTDOWN, LOWORD (lParam), HIWORD (lParam)); 
    break; 
  case WM_RBUTTONUP: 
    b = tr_mouse (pMsg, RIGHTUP, LOWORD (lParam), HIWORD (lParam)); 
    break; 
  case WM_MOUSEMOVE: //Changes made by Ozerski 
    break; 
    b = tr_mouse (pMsg, MOTION, LOWORD (lParam), HIWORD (lParam)); 
    break; 
  case WM_KEYDOWN: 
    pMsg->mes1 = wParam; 
    if      (wParam == VK_UP)     pMsg->mes1 = YK_UP; 
    else if (wParam == VK_DOWN)   pMsg->mes1 = YK_DOWN; 
    else if (wParam == VK_PRIOR)  pMsg->mes1 = YK_PAGEUP; 
    else if (wParam == VK_NEXT)   pMsg->mes1 = YK_PAGEDOWN; 
    else if (wParam == VK_DELETE) pMsg->mes1 = YK_DELETE; 
    else if (wParam == VK_LEFT)   pMsg->mes1 = YK_LEFT; 
    else if (wParam == VK_RIGHT)  pMsg->mes1 = YK_RIGHT; 
 
    pMsg->message = YKEYBOARD; 
    pMsg->id = YGetFocus (); 
    break; 
  case WM_PAINT: 
    break;   /*?! */ 
    YRedrawAll (); 
    break; 
  default: 
    svw.is_readsystem = FALSE; 
    return (DefWindowProc (hwnd, message, wParam, lParam)); 
  } 
 
  if (b)  svw.is_readsystem = TRUE; 
  else    svw.is_readsystem = FALSE; 
  return (0); 
} 
#endif 
/*-----------------------------YBigWindow--------------------------------------*/ 
void 
YBigWindow (int *pid, YT_PFUNC proc, char *name, int x, int y, int w, int h,  
                  long w_long1, long w_long2, long w_long3, long w_long4,  
                                                          YT_COLOR color) 
{ 

  int  i; 
  static YT_BOOL first = TRUE; 
  int from = ID_NULL; 
 
  if (first)  first = FALSE; 
  else { 
    fprintf (stderr, "Sorry, YBigWindow() is under construction !! \n"); 
    /* return;  */
  } 
 
  for (i = 0; i < BIGWND_NUM; i++) 
    if (KERN_S->bigwnds[i] == NULL) 
      break; 
  if (i == BIGWND_NUM)  YERROR ("XSTRUCT_NUM"); 
 
  Y_WMODE = TRUE; 
 
  KERN_S->bigwnds[i] = (YT_BIGWND *) malloc (sizeof (YT_BIGWND)); 
  KERN_S->bigw_cur = i; 
 
  if (w == 0) { 
    w = CALL (proc, 1, from, YGET, 0, 0, 0, 0); 
    if (w == 0) w = 300; 
  } 
  if (h == 0) { 
    h = CALL (proc, 2, from, YGET, 0, 0, 0, 0); 
    if (h == 0) h = 300; 
  } 
  if (name == NULL) { 
    name = (char*)CALL (proc, 3, from, YGET, 0, 0, 0, 0); 
    if (name == NULL) name = "Unknow Name"; 
  } 
 
  W_SCREEN = w; 
  H_SCREEN = h; 
  YSetMult (1, 1); 
  YSetOrigin (0, 0); 
 
  SV->main_proc = proc; 
  ID_DIALOG = ID_NULL; 
 
  /* YSetFont(Y_FONT);  */
  KerCreateWindow (name, w, h); 
 
  YSetFont(Y_FONT); 
 
  YWnd (&ID_MAIN, SV->main_proc, "", 0,0, W_SCREEN,H_SCREEN,  
	w_long1, w_long2, w_long3, w_long4, color); 
  WNDI(ID_MAIN)->parent = ID_NULL; 

  pid++; x++; y++; 
  return; 
} 
/*--------------------------------KerCreateWindow----------------------------------*/ 
void 
KerCreateWindow (char *wname, int w, int h) 
{ 
 
#if defined(FLTK) /*------------------------*/ 

  fltkwin = new My_Window(w,h, wname);
  fltkwin->color(FL_GREEN);

  fltkwin->end();
  fltkwin->show();
	fltkwin->make_current(); 

  SV->region = XCreateRegion (); 
#if defined(API_W) 
	/* extern HINSTANCE fl_display; */
	svw.hWnd = fl_window;
	svw.hdc  = fl_gc;
/* 	COLORREF fl_RGB(); */
/* 	HPEN fl_pen(); */
/* 	HBRUSH fl_brush(); */
#else
	DPY    = fl_display;
	SV->win= fl_window;
	SV->gc = fl_gc;
	SCR = fl_screen;
  KERN_S->cmap = fl_colormap; 
  /* extern XVisualInfo *fl_visual; */
#endif

#elif defined(API_W) 
  svw.hWnd = CreateWindow ("y_class", wname, WS_OVERLAPPEDWINDOW, 
			   5, 5, W_SCREEN + 20, H_SCREEN + 50, 
			   NULL, NULL, svw.hInstance, NULL); 
 
  if (!svw.hWnd)                      YERROR ("CreateWindow"); 
  if (!(svw.hdc = GetDC (svw.hWnd)))  YERROR ("GetDC"); 
 
  SetBkMode (svw.hdc, TRANSPARENT); 
  if (!(svw.hdcMemory = CreateCompatibleDC (svw.hdc)))  YERROR ("CreateCompDC"); 
 
  ShowWindow (svw.hWnd, svw.nCmdShow); 
 
#else
  XSetWindowAttributes attributes; 
 
  attributes.backing_store = Always; 
  SV->win = XCreateWindow (DPY, RootWindow (DPY, KERN_S->screen), 
			   0, 0, w, h, 
			   1, CopyFromParent, InputOutput, CopyFromParent, 
			   CWBackingStore, &attributes); 
 
  SV->region = XCreateRegion (); 
  XStoreName (DPY, SV->win, wname); 
  XMapWindow (DPY, SV->win); 
  SV->gc = XCreateGC (DPY, SV->win, 0x0, NULL); 
 
  XSelectInput (DPY, SV->win, ExposureMask | KeyPressMask | 
		ButtonPressMask | ButtonReleaseMask | ButtonMotionMask 
		| PointerMotionHintMask); 
 
#endif 
} 
/*--------------------------------YColor----------------------------------------*/ 
YT_COLOR YColor(char *name) 
{ 
  int i; 
 
  if ((name==NULL) || /* YStrCmp(name,"") */!strcmp(name,""))  
    return(CLR_NULL); 
 
  for (i = 0; i<SIZE_COLER; i++) { 
    if (COLER(i).exist == FALSE) continue; 
/*     if (YStrCmp(name, COLER(i).name)) return(COLER(i).color); */ 
    if (!strcmp(name, COLER(i).name)) return(COLER(i).color); 
  } 
 
  fprintf (stderr, "Dont' find color = %s", name); 
  return(CLR_NULL); 
} 
/*--------------------------------YScale-------------------------------------*/ 
YT_COLOR YScale(int n) 
{ 
 
  return(CLR_SCALE(n)); 
 
} 
/*------------------------------YDefColor---------------------------------*/ 
void 
YDefColor (YT_BOOL flag_add, int r, int g, int b, char *name) 
{ 
  YT_BOOL find = FALSE; 
  int i; 
 
  for (i=0; i<SIZE_COLER; i++) { 
    if (COLER(i).exist == FALSE) continue; 
/*     if (YStrCmp(name, COLER(i).name)) {find=TRUE; break;} */ 
    if (!strcmp(name, COLER(i).name)) {find=TRUE; break;} 
  } 
 
  if (!find) { 
    for (i=0; i<SIZE_COLER; i++) { 
      if (COLER(i).exist == FALSE) {find=TRUE; break;} 
    } 
  } 
  if (!find) return; 
 
  COLER(i).exist = TRUE; 
  strcpy(COLER(i).name, name); 
  COLER(i).r = r; 
  COLER(i).g = g; 
  COLER(i).b = b; 
 
  flag_add++; 
  return; 
} 
/*------------------------------YDefColors---------------------------------*/ 
void 
YDefColors (YT_DEFCOLOR *df) 
{ 
 
  for ( ; df->name != NULL; df++)  
    YDefColor (TRUE, df->r, df->g, df->b, df->name); 
   
  return; 
} 
/*------------------------------YSetColors----------------------------------*/ 
void 
YSetColors (int type_scale) 
{ 

#if   defined(FLTK_TMP)
  /* YColerScale (type_scale, 120, FALSE);  */
#elif defined(API_W) 
  lgpl.palNumEntries = 40; 
  lgpl.palVersion = 0x300; 
 
  YColerScale (type_scale, lgpl.palNumEntries, TRUE); 
 
  if (!(svw.hpal = CreatePalette ((LOGPALETTE *) & lgpl)))  YERROR ("CreatePalete"); 
  if (!(SelectPalette (svw.hdc, svw.hpal, FALSE)))          YERROR ("SelectPalette"); 
  if (!(RealizePalette (svw.hdc)))                          YERROR ("RealizePalette"); 
  if (!(SelectPalette (svw.hdcMemory, svw.hpal, FALSE)))    YERROR ("SelectPalette"); 
 
  YColerScale (type_scale, lgpl.palNumEntries, FALSE); 
 
#else  
  YColerScale (type_scale, 120, TRUE); 
#endif 
 
 return; 
} 
/*-----------------------------YColerScale----------------------------------*/ 
void 
YColerScale (int type_scale, int size_colors, YT_BOOL first) 
{ 
 
  YSetColler (first); 
  YSetScale (size_colors - SIZE_COLER, type_scale, first); 
 
  return; 
} 
/*------------------------------YSetColler---------------------------------*/ 
void 
YSetColler (YT_BOOL first) 
{ 
  int i; 
 
  for (i=0; i<SIZE_COLER; i++) { 
    if (COLER(i).exist == FALSE) continue; 
    COLER (i).color = YRgbPalette (COLER (i).r, COLER (i).g, COLER (i).b, first); 
  } 
 
} 
/*------------------------------YSetScale-----------------------------------*/ 
void 
YSetScale (int size, int type, YT_BOOL first) 
{ 
  int     i; 
  int     r, g, b; 
 
  YSetDEFPALETE (type, size); 
 
  SIZE_SCALE = size;   /* YSet ("size_scale", SIZE_SCALE); */ 
  for (i = 0; i < SIZE_SCALE; i++) { 
    YGetDEFPALETE (i, &r, &g, &b); 
    CLR_SCALE (i) = YRgbPalette (r, g, b, first); 
  } 
 
} 
/*------------------------------YRgbPalette---------------------------------*/ 
YT_COLOR 
YRgbPalette (int r, int g, int b, YT_BOOL first) 
{ 
  YT_COLOR color; 
 
  if (first)  color = YSetRgbPalette (r, g, b); 
  else        color = YGetRgbPalette (r, g, b); 
 
  return (color); 
} 
/*-----------------------------KisaSetColor---------------------------------*/ 
#if defined(API_W)  
#else  
int 
KisaSetColor (Display * dpy, int red, int green, int blue) 
{ 
  int     screen = DefaultScreen (dpy); 
  Colormap cmap = DefaultColormap (dpy, screen); 
 
  XColor  cell; 
 
  cell.red = red << 8; 
  cell.green = green << 8; 
  cell.blue = blue << 8; 
  if (!(XAllocColor (dpy, cmap, &cell))) 
    fprintf (stderr, "Error : KisaSetColor \n"); 
 
  return (cell.pixel); 
} 
#endif  
/*----------------------------YSetDEFPALETE---------------------------------*/ 
void 
YSetDEFPALETE (int type, int size) 
{ 
  YT_DEFPALETE *pdef, *p = &(KERN_S->ds); 
 
  YT_DEFPALETE colored = 
  {0, 0, 
   {0, 85, 170, 255}, 
   {0, 0, 255, 255}, 
   {0, 255, 255, 0}, 
   {255, 255, 0, 0}}; 
 
  YT_DEFPALETE grayed = 
  {0, 0, 
   {0, 85, 170, 255}, 
   {70, 150, 200, 255}, 
   {70, 150, 200, 255}, 
   {70, 150, 200, 255}}; 
 
  YT_DEFPALETE inverse = 
  {0, 0, 
   {0, 85, 170, 255}, 
   {255, 255, 0, 0}, 
   {0, 255, 255, 0}, 
   {0, 0, 255, 255}}; 
 
  if      (type == YGRAYED)   pdef = &grayed; 
  else if (type == YCOLORED)  pdef = &colored; 
  else if (type == YINVERSE)  pdef = &inverse; 
  else 
    YERROR ("type_scale"); 
 
  *p = *pdef; 
  p->type = type; 
  p->size = size; 
 
  return; 
} 
/*----------------------------YGetDEFPALETE---------------------------------*/ 
void 
YGetDEFPALETE (int i, int *r, int *g, int *b) 
{ 
  float   step_index; 
  int     index, j; 
  float   d; 
  YT_DEFPALETE *p = &(KERN_S->ds); 
 
  step_index = (float) 255 / (p->size); 
  index = (int) (step_index * i); 
 
  for (j = 0; p->kp[j + 1] <= index; j++) ;; 
 
  d = (float) (index - p->kp[j]) / (p->kp[j + 1] - p->kp[j]); 
  *r = (int) (p->rr[j] + d * (p->rr[j + 1] - p->rr[j])); 
  *g = (int) (p->gg[j] + d * (p->gg[j + 1] - p->gg[j])); 
  *b = (int) (p->bb[j] + d * (p->bb[j + 1] - p->bb[j])); 
 
} 
/*---------------------------YColorToScale----------------------------------*/ 
int 
YColorToScale (YT_COLOR color) 
{ 
  int     i; 
 
  for (i = 0; i < SIZE_SCALE; i++) 
    if (CLR_SCALE (i) == color) 
      return (i); 
 
  return (SIZE_SCALE / 2); 
} 
/*----------------------------YColorToRGB-----------------------------------*/ 
void 
YColorToRGB (YT_COLOR color, int *r, int *g, int *b) 
{ 
  int     i; 
  YT_COLER *clr = &COLER (0); 
 
  for (i = 0; i < SIZE_COLER; i++) { 
    if (COLER(i).exist == FALSE) continue; 
    if ((clr + i)->color != color) 
      continue; 
    *r = (clr + i)->r; 
    *g = (clr + i)->g; 
    *b = (clr + i)->b; 
    return; 
  } 
 
  for (i = 0; i < SIZE_SCALE; i++) { 
    if (CLR_SCALE (i) != color) 
      continue; 
    YGetDEFPALETE (i, r, g, b); 
  } 
 
} 
/*------------------------------YRedrawAll----------------------------------*/ 
void 
YRedrawAll () 
{ 
 
 /*  YSet("second_draw", TRUE); */ 
  SECOND_DRAW = TRUE; 
 
  YWnd (&ID_MAIN, SV->main_proc, "", 0,0, W_SCREEN,H_SCREEN, 0,0,0,0, CLR_DEF) ; 
  if (ID_DIALOG != ID_NULL) { 
    /* OUTD (333); */ 
    YWnd (&(ID_DIALOG), /* DIALOG */WNDI(ID_DIALOG)->proc, "", 0,0, 0,0,  
	                                                      0,0,0,0, CLR_DEF); 
  } 
 
 /*  YSet("second_draw", FALSE); */ 
  SECOND_DRAW = FALSE; 
 
} 
/*------------------------------YRedrawAllNew--------------------------------*/ 
void 
YRedrawAllNew () 
{ 
  int id, last_numer = -5; 
 
  /* YSet("second_draw", TRUE); */ 
  SECOND_DRAW = TRUE; 
 
  LOOP_WND (id) { 
    if (!WND_CREATED (id))  continue; 
    if (!WND_OPENED  (id))  continue; 
    if (last_numer >= WNDI(id)->numer) continue; 
    last_numer = WNDI(id)->numer; 
    YWndUpdate (id); 
    } 
 
 /*  YSet("second_draw", FALSE); */ 
  SECOND_DRAW = FALSE; 
 
} 
/*------------------------------YFindMouseWnd-------------------------------*/ 
int YFindMouseWnd(int x, int y) 
{ 
  int find_id, max_numer, id, wnd_w, wnd_h; 
 
  max_numer = 0; 
  LOOP_WND (id) { 
    if (!WND_CREATED (id)) 
      continue; 
    wnd_w = (WNDI(id)->w) * (WINI_X_MULT(id)/* ->x_mult */); 
    wnd_h = (WNDI(id)->h) * (WINI_Y_MULT(id)/* ->y_mult */); 
    if (!YPtInRect (x, y, WNDI(id)->x, WNDI(id)->y, wnd_w, wnd_h)) 
      continue; 
    if (WNDI(id)->numer > max_numer) { 
      max_numer = WNDI(id)->numer; 
      find_id = id; 
    } 
  } 
 
  return(find_id); 
} 
/*------------------------------tr_mouse------------------------------------*/ 
YT_BOOL 
tr_mouse (YT_SVMSG *pMsg, int action, int x, int y) 
{ 
  int     mes, id; 
  static YT_BOOL ldrag = FALSE, rdrag = FALSE; 
  static int wnd_capture = /* ID_NULL*/  0 ; /*!!!!!!!!*/ 
  static YT_BOOL region_out = FALSE; 
  int     max_numer; 
  static long delta_time = 30, lmouse_time = 0; 
  long    time; 
  int     wnd_w, wnd_h; 
 
  switch (action) { 
  case MOTION: 
    if      (ldrag && rdrag)  mes = YLRMOUSEDRAG; 
    else if (ldrag)           mes = YLMOUSEDRAG; 
    else if (rdrag)           mes = YRMOUSEDRAG; 
    else 
      return (FALSE); 
    break; 
  case LEFTDOWN: 
    mes = YLMOUSEDOWN; 
    ldrag = TRUE; 
    break; 
  case RIGHTDOWN: 
    mes = YRMOUSEDOWN; 
    rdrag = TRUE; 
    break; 
  case LEFTUP: 
    mes = YLMOUSEUP; 
    ldrag = FALSE; 
    break; 
  case RIGHTUP: 
    mes = YRMOUSEUP; 
    rdrag = FALSE; 
    break; 
  default: 
    YERROR ("tr_mouse"); 
  } 
 
/*   pMsg->phrase = ""; */ 
 
  if (region_out) { 
    if (mes == YLMOUSEUP || mes == YRMOUSEUP) 
      region_out = FALSE; 
    pMsg->id = KERN_S->mregion_id; 
    pMsg->message = YMREGIONOUT; 
    return (TRUE); 
  } 
 
  if (mes == YLMOUSEDOWN || mes == YRMOUSEDOWN) { 
    if (KERN_S->mregion_id && !YPtInRect (x, y, KERN_S->mregion_x, KERN_S->mregion_y, 
				      KERN_S->mregion_w, KERN_S->mregion_h)) { 
      pMsg->id = KERN_S->mregion_id; 
      pMsg->message = YMREGIONOUT; 
      region_out = TRUE; 
      return (TRUE); 
    } 
 
    max_numer = 0; 
    LOOP_WND (id) { 
/* 			if (WNDI(id)->ywin != KERN_S->ywin) continue;   */
      if (!WND_CREATED (id))  continue; 
      if (!WND_OPENED (id))   continue; 
      wnd_w = (WNDI(id)->w) * (WINI_X_MULT(id)/* ->x_mult */); 
      wnd_h = (WNDI(id)->h) * (WINI_Y_MULT(id)/* ->y_mult */); 
      if (!YPtInRect (x, y, WNDI(id)->x, WNDI(id)->y, wnd_w, wnd_h)) 
				continue; 
      if (WNDI(id)->numer > max_numer) { 
				max_numer = WNDI(id)->numer; 
				wnd_capture = id; 
      } 
    } 
  } 
 
  id = wnd_capture; /*!!!!*/ 
 
  pMsg->id = id; 
  pMsg->message = mes; 
  pMsg->mes1 = (x - WNDI(id)->x) / WINI_X_MULT(id)/* ->x_mult */; 
  pMsg->mes2 = (y - WNDI(id)->y) / WINI_Y_MULT(id)/* ->y_mult */; 
 
  if (mes == YLMOUSEDOWN) { 
    time = YTimeGet (); 
    if (labs (time - lmouse_time) < delta_time) { 
      pMsg->message = YLMOUSEDOUBLE; 
      lmouse_time = 0; 
    } 
    lmouse_time = time; 
  } 
  return (TRUE); 
} 
/*---------------------------------YWnd--------------------------------------*/ 
void 
YWnd (int *pid, YT_PFUNC proc, char *name, int x, int y, int w, int h, 
               long w_long1, long w_long2, long w_long3, long w_long4, YT_COLOR color) 
{ 
 
  if (pid == NULL) 
    YERROR ("Too many CHILD !"); 
 
/*   if (!YGet("second_draw")) { */ 
  if (!SECOND_DRAW) { 
    YWndCreate (pid, proc, name, w_long1, w_long2, w_long3, w_long4); 
    YWndOpen (*pid, x,y, w,h, color); 
  } else { 
    YWndOpen (*pid, x,y, w,h, color); 
    /* YSend (*pid, YDRAW, 0, 0, 0, 0); */  /*????*/ 
  } 
 
  return; 
} 
/*---------------------------------YWndCreate----------------------------------*/ 
void 
YWndCreate (int *pid, YT_PFUNC proc, char *name, 
                      long w_long1, long w_long2, long w_long3, long w_long4) 
{ 
  int  id, i; 
 
  LOOP_WND (i) { 
    if (!WND_CREATED (i)) 
      break; 
  } 
 
  if (!IN_WND (i)) 
    YERROR ("Too many FLY!"); 
 
 
  PWINDOW(i) = (YT_WINDOW *) malloc (sizeof (YT_WINDOW)); 
 
/*   WNDI(i)->sys = malloc (sizeof (YT_WINDOW_S)); */ 
/*   WNDI(i)->sys = NULL; */ 
 
  id = i; 
 
  *pid = id; 
  WNDI(id)->pid = pid; 
  WNDI(id)->proc = proc; 
 
  WNDI(id)->opened = FALSE; 
 /*  WNDI(id)->parent   = Y_IDLAST; */ 
 
  WNDI(id)->parent_c = Y_IDLAST; 
 
  WNDI(id)->usr    = NULL; 
  Y_WLONG1 = w_long1; 
  Y_WLONG2 = w_long2; 
  Y_WLONG3 = w_long3; 
  Y_WLONG4 = w_long4; 
 
  WNDI(id)->name = NULL; 
  YSend (id, YCREATE, 0, 0, 0, 0); 
  if      (name != NULL)            WNDI(id)->name = name; 
  else if (WNDI(id)->name == NULL)  WNDI(id)->name = ""; 
 
  WNDI(id)->ch_i = 0;   
 
  return; 
} 
/*-------------------------------YWndOpen-------------------------------------*/ 
void 
YWndOpen (int id, int x, int y, int w, int h, YT_COLOR color) 
{ 
  YT_GETSIZE gs = {FALSE, 0, 0}; 
  int  id_wl_proc; 
 
  /*   WNDI(id)->sys = malloc (sizeof (YT_WINDOW_S)); */ 
 
  if (KERN_S->wndgroup_flag) { 
    if (KERN_S->wndgroup_last == ID_NULL)  KERN_S->wndgroup_first = id; 
    else                               WINI(id)->id_goto  = KERN_S->wndgroup_last; 
    KERN_S->wndgroup_last = id; 
  } else 
    WINI(id)->id_goto = ID_NULL; 
 
 
  WNDI(id)->ch_i = 0;  /*??*/ 
 
  WINI(id)->bigw_i = KERN_S->bigw_cur; 
  WNDI(id)->ywin = (long)SV->win; 
 
  WNDI(id)->parent = Y_IDLAST; 
  /* WINI(id)->opened = TRUE; */ 
  WNDI(id)->opened = TRUE; 
  WNDI(id)->numer = ++KERN->numer; /*  OUTD(9999); */ 
  
  if (color == CLR_DEF) { 
    /* id_wl_proc = (int)YGet("wl_proc"); */ 
    id_wl_proc = Y_WLPROC; 
    if  (id_wl_proc == ID_NULL)  color = YColor ("white"); 
    else                         color = (YT_COLOR) YSend (id_wl_proc, YKEYWORD1, id,0,0,0); 
  } 
  WINI(id)->color = color; 
 
  if (x || y || w || h) { 
    CALCXY (x, y); 
    WNDI(id)->x = x; 
    WNDI(id)->y = y; 
    CALCWH (w, h); 
 
    YSend (id, YGETSIZE, 0, 0, (long)&(gs), 0); 
 
    if (gs.flag) { 
      WNDI(id)->w = gs.w; 
      WNDI(id)->h = gs.h; 
    } else { 
      WNDI(id)->w = w; 
      WNDI(id)->h = h; 
    } 
    WINI_X_MULT(id) = (float) w / WNDI(id)->w; 
    WINI_Y_MULT(id) = (float) h / WNDI(id)->h; 
     
    if (WINI_X_MULT(id) == 0) { 
      fprintf (stderr,"%d %d %d \n", gs.flag, gs.w, gs.h); 
      fprintf (stderr,"%d %d \n", w, WNDI(id)->w); 
    } 
  } 
 
  if (SECOND_DRAW)  YSend (id, YDRAW, 0, 0, 0, 0); 
  else              YSend (id, YOPEN, 0, 0, 0, 0); 
 
  return; 
 
} 
/*---------------------------------------YWndClose-----------------------------*/ 
void 
YWndClose (int id) 
{ 
 
  if (id == ID_NULL) return; 
 
  YSend (id, YCLOSE,   0, 0, 0, 0); 
  WNDI(id)->opened = FALSE; 
 
  return; 
} 
/*----------------------------------YWndDelete-------------------------------*/ 
void 
YWndDelete (int id) 
{ 
 
  if (id == ID_NULL) return; 
  if (!PWINDOW(id))  return; 
 
  if (WNDI(id)->opened)  YWndClose (id); 
 
  YSend (id, YDESTROY, 0, 0, 0, 0); 
  free (WNDI(id)); 
  PWINDOW(id) = NULL; 
 
  return; 
  } 
/*--------------------------------------YUnWnd---------------------------------*/ 
/* void */ 
/* YUnWnd (int id) */ 
/* { */ 
 
/*   YWndClose (id); */ 
 
/*   YWndDelete (id); */ 
 
/*   return; */ 
/* } */ 
/*---------------------------------------YWndFree------------------------------*/ 
void 
YWndFree (int id) /*!!?*/ 
{ 
 
  free (WNDI(id)); 
  PWINDOW(id) = NULL; 
 
} 
/*----------------------------------YModClean-------------------------------*/ 
void 
YModClean (int parent_c) 
{ 
  int     id; 
 
  if (parent_c == ID_NULL)   return; 
  
  LOOP_WND (id) { 
 
    if (!WND_CREATED (id))               continue; 
    if (WNDI(id)->parent_c != parent_c)  continue; 
 
    YWndDelete (id); 
  } 
 
  return; 
} 
/*----------------------------------YWndClean----------------------------------*/ 
void 
YWndClean (int parent) 
{ 
  int     id; 
 
  if (parent == ID_NULL)   return; 
  
  LOOP_WND (id) { 
 
    if (!WND_CREATED (id))           continue; 
    if (WNDI(id)->parent != parent)  continue; 
 
    YUnWnd (id); 
  } 
 
  return; 
} 
/*--------------------------------YSetWnd-----------------------------------*/ 
YT_DEFWND * 
YSetWnd (YT_PFUNC proc, char *name, 
	 long long1, long long2, long long3, long long4, YT_COLOR color/* int midata */) 
{ 
  YT_DEFWND *wnd = (YT_DEFWND *) malloc (sizeof (YT_DEFWND)); 
 
/* wnd->pid=pid; */ 
  wnd->proc = proc; 
  wnd->name = name; 
  wnd->long1 = long1; 
  wnd->long2 = long2; 
  wnd->long3 = long3; 
  wnd->long4 = long4; 
 /*  wnd->midata = midata; */ 
  wnd->color = color; 
 
  return (wnd); 
} 
/*----------------------------YGetContext-----------------------------------*/ 
void 
YGetContext (YT_CONTEXT *s) 
{ 
 
  YGetOrigin (&(s->x0_old), &(s->y0_old)); 
  YGetMult (&(s->old_xmult), &(s->old_ymult)); 
  s->id_dialog = ID_DIALOG; 
 
  s->id_last = Y_IDLAST; 
 
} 
/*-------------------------------YSetContext--------------------------------*/ 
void 
YSetContext (YT_CONTEXT *s) 
{ 
 
  Y_IDLAST = s->id_last; 
 
  YSetOrigin (s->x0_old, s->y0_old); 
  YSetMult (s->old_xmult, s->old_ymult); 
  ID_DIALOG = s->id_dialog; 
 
} 
/*------------------------------YSetOrigin--------------------------------*/ 
void 
YSetOrigin (int x0, int y0) 
{ 
  KERN->x0 = x0; 
  KERN->y0 = y0; 
  return; 
} 
/*------------------------------YGetOrigin--------------------------------*/ 
void 
YGetOrigin (int *x0, int *y0) 
{ 
  *x0 = KERN->x0; 
  *y0 = KERN->y0; 
  return; 
} 
/*-------------------------------YSetMult---------------------------------*/ 
void 
YSetMult (float x_mult, float y_mult) 
{ 
  KERN->x_mult = x_mult; 
  KERN->y_mult = y_mult; 
  return; 
} 
/*--------------------------------YGetMult--------------------------------*/ 
void 
YGetMult (float *x_mult, float *y_mult) 
{ 
  *x_mult = KERN->x_mult; 
  *y_mult = KERN->y_mult; 
  return; 
} 
/*--------------------------------YBeginId--------------------------------*/ 
void  YBeginId() 
{ 
 
 
} 
/*---------------------------------YEndId---------------------------------*/ 
void  YEndId() 
{ 
 
 
} 
/*---------------------------------YFinalYlib-----------------------------*/ 
void 
YFinalYlib () 
{ 
 
  if (Y_WMODE) { 
    YUnWnd (ID_MAIN); 
    YCloseGraph (); 
  } 
 
  /* YExit (); */ 
  return; 
} 
/*--------------------------------YWndUpdate------------------------------*/ 
void 
YWndUpdate (int id) 
{ 
 
  YSend (id, YDRAW, 0, 0, 0, 0); 
 
} 
/*-------------------------------YWndUpdatePost---------------------------*/ 
void 
YWndUpdatePost (int id) 
{ 
 
  YPost (id, YDRAW, 0, 0, 0, 0); 
 
} 
/*--------------------------------YGetData--------------------------------*/ 
long 
YGetData (int id) 
{ 
 
  if (!Y_WMODE) return(0); 
 
  return (YSend (id, YGETDATA, 0, 0, 0, 0)); 
 
} 
/*------------------------------YGetFocus---------------------------------*/ 
int 
YGetFocus () 
{ 
 
  return (KERN_S->wnd_focus); 
 
} 
/*-------------------------------YSetFocus--------------------------------*/ 
void 
YSetFocus (int id) 
{ 

#ifdef FLTK
	Fl::focus(fltkwin);
#endif
  if (KERN_S->wnd_focus != ID_NULL)  
    YSend (KERN_S->wnd_focus, YKILLFOCUS, 0, 0, 0, 0); 
   
  KERN_S->wnd_focus = id; 
  YSend (id, YSETFOCUS, 0, 0, 0, 0); 

	return;
} 
/*-------------------------------YKillFocus-------------------------------*/ 
void 
YKillFocus () 
{ 
  if (KERN_S->wnd_focus) 
    YSend (KERN_S->wnd_focus, YKILLFOCUS, 0, 0, 0, 0); 
  KERN_S->wnd_focus = ID_NULL; 
} 
/*---------------------------YSetMouseRegion------------------------------*/ 
void 
YSetMouseRegion (int x, int y, int w, int h, int id) 
{ 
  KERN_S->mregion_x = x; 
  KERN_S->mregion_y = y; 
  KERN_S->mregion_w = w; 
  KERN_S->mregion_h = h; 
  KERN_S->mregion_id = id; 
} 
/*----------------------------YFindModByName-----------------------------*/ 
int 
YFindModByName (char *name) 
{ 
  int   id; 
 
  if (name == NULL)  return (ID_NULL); 
  
  LOOP_WND (id) { 
 
    if (!WND_CREATED (id))               continue; 
    /* if (WNDI(id)->parent_c != parent_c)  continue; */ 
 
/*     if (YStrCmp(name, WNDI(id)->name))  return (id); */ 
    if (!strcmp(name, WNDI(id)->name))  return (id); 
 
  } 
 
  return (ID_NULL); 
} 
/*---------------------------------YPtInRect------------------------------*/ 
YT_BOOL 
YPtInRect (int x0, int y0, int x, int y, int w, int h) 
{ 
 
  return (x0 >= x && x0 <= x + w && y0 >= y && y0 <= y + h); 
 
} 
/*-------------------------------YDlgEnd----------------------------------*/ 
void 
YDlgEnd (long dialog_ret) 
{ 
  KERN_S->dialog_ret = dialog_ret; 
  KERN_S->dialog_loop = FALSE; 
  return; 
} 
/*---------------------------------YDlg-----------------------------------*/ 
long 
YDlg (YT_PFUNC proc, char *name, int x, int y, int w, int h, 
	      long w_long1, long w_long2, long w_long3, long w_long4,  
              YT_COLOR color, long flag) 
{ 
#ifdef FLTK
{   
	int w=320, h=200;
                                              
  Fl_Window window (w,h);                                                   

  Fl_Box box(FL_BORDER_BOX, 30,20, w-60,h-10-60, "");  
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
#else
  typedef struct { 
    YT_CONTEXT save; 
    int     id; 
    /* int     id_dialog; */ 
  } YT_DIALOG; 
 
  YT_DIALOG *dlg = (YT_DIALOG *) malloc (sizeof (YT_DIALOG)); 
 
  YGetContext (&(dlg->save)); 
  YOpenDialog (&(dlg->id), proc, name, x,y,w,h, w_long1,w_long2,w_long3,w_long4, color,flag); 
 
  YLoop (&KERN_S->dialog_loop); 
  KERN_S->dialog_loop = TRUE; 
 
  YCloseDialog (dlg->id); 
  YSetContext (&(dlg->save)); 
 
  free (dlg); 
  return (KERN_S->dialog_ret); 
#endif
} 
/*----------------------------YOpenDialog-----------------------------------*/ 
void 
YOpenDialog (int *pid, YT_PFUNC proc, char *name, int x, int y, int w, int h, 
	     long long1, long long2, long long3, long long4,  
             YT_COLOR color, long flag) 
{ 
 
  int     ma = ID_MAIN; 
  YT_CONTEXT main; 
  YT_COLOR col_caption = YColor("yellow"); 
 
  main.id_last = ma; 
  main.x0_old = WNDI(ma)->x; 
  main.y0_old = WNDI(ma)->y; 
  main.old_xmult = WINI_X_MULT(ma); 
  main.old_ymult = WINI_Y_MULT(ma); 
  YSetContext (&main); 
 
  if (w == 0) { 
    w = W_SCREEN / 2.5; 
    w = YMAX (w, (W_DLG * 1.4)); 
  } 
  if (h == 0) { 
    h = H_SCREEN / 2.5; 
    h = YMAX (h, (H_DLG * 1.4)); 
  } 
  if (x == 0)  x = (W_SCREEN - w) / 2; 
  if (y == 0)  y = (H_SCREEN - h) / 2; 
 
  if (KERN_S->dialog_proc)  
    YWnd (pid, KERN_S->dialog_proc, name, x, y, w, h,   
	  (long)YSetWnd (proc, name, long1, long2, long3, long4, color), 0, 0, 0, col_caption); 
  else  
    YWnd (pid, proc, name, x,y, w,h,  long1, long2, long3, long3, col_caption); 
 
  ID_DIALOG = *pid; 
  YSetMouseRegion (x, y, w, h, *pid); 
 
  return; 
} 
/*--------------------------YCloseDialog------------------------------------*/ 
void 
YCloseDialog (int id) 
{ 
 
  YSetMouseRegion (0, 0, 0, 0, 0); 
  YSend (id, YCLOSE, 0, 0, 0, 0);	/*because not in order opened */ 
 
} 
/*-----------------------------Mysize_is------------------------------------*/ 
void 
Mysize_is (long mes3, int ww, int hh) 
{ 
  YT_GETSIZE *pgs = (YT_GETSIZE *) mes3; 
 
  pgs->flag = TRUE; 
  pgs->w = ww; 
  pgs->h = hh; 
 
} 
/*--------------------------------YColer------------------------------------*/ 
YT_COLOR 
YColer (int n) 
{ 
  if   ((n>=SIZE_COLER) || !(COLER(n).exist))  
    return (CLR_NULL); 
 
  return (COLER(n).color); 
} 
/*------------------------------YMouseHide--------------------------------*/ 
void 
YMouseHide () 
{ 
#if defined(API_W) 
  ShowCursor (FALSE); 
#else  
#endif 
} 
/*------------------------------YMouseShow--------------------------------*/ 
void 
YMouseShow () 
{ 
#if defined(API_W) 
  ShowCursor (TRUE); 
#else  
#endif 
} 
/*----------------------------YSetRgbPalette--------------------------------*/ 
YT_COLOR 
YSetRgbPalette (int red, int green, int blue) 
{ 
#if defined(API_W) 
  static YT_COLOR entry = 0; 
 
  lgpl.palPalEntry[entry].peRed = LOBYTE (red); 
  lgpl.palPalEntry[entry].peGreen = LOBYTE (green); 
  lgpl.palPalEntry[entry].peBlue = LOBYTE (blue); 
  lgpl.palPalEntry[entry].peFlags = PC_RESERVED; 
  entry++; 
 
  return (entry - 1); 
#else /*---------------*/ 
  XColor  cell; 
 
  cell.red = red << 8; 
  cell.green = green << 8; 
  cell.blue = blue << 8; 
  if (!(XAllocColor (DPY, KERN_S->cmap, &cell))) 
    YERROR ("XAllocColor"); 
  return (cell.pixel); 
#endif 
} 
/*----------------------------YGetRgbPalette--------------------------------*/ 
YT_COLOR 
YGetRgbPalette (int r, int g, int b) 
{ 
#if defined(API_W) 
  return (PALETTERGB (r, g, b)); 
#else 
  YERROR ("GetRgbPalette"); 
  r++; 
  g++; 
  b++; 
  /* return (YColorV("vc_black")); */ 
  return (YColor("black")); 
#endif 
} 
/*-----------------------------FSetColor-------------------------------*/
void
FSetColor (YT_COLOR color)
{
#if   defined(FLTK) 

  int     r, g, b; 
  YColorToRGB (color, &r, &g, &b); 
	fl_color(r, g, b); 

#endif
}
/*-----------------------------YPaintRectF---------------------------------*/ 
void 
YPaintRectF (int x, int y, int w, int h, YT_COLOR f_color) 
{ 
  CALCXY (x, y); 
  CALCWH (w, h);

#if defined(FLTK) 
	FSetColor (f_color);
	fl_rectf(x, y, w, h);

#elif defined(API_W) 
  svw.rect.left = x; 
  svw.rect.top = y; 
  svw.rect.right = x + w + 1; 
  svw.rect.bottom = y + h + 1; 
  if (!(svw.hbr = CreateSolidBrush (f_color)))         YERROR ("YPaintRectF-1"); 
  if (!(svw.hbrOld = SelectObject (svw.hdc, svw.hbr))) YERROR ("YPaintRectF-2"); 
  FillRect (svw.hdc, &svw.rect, svw.hbr); 
  if (!SelectObject (svw.hdc, svw.hbrOld))  YERROR ("YPaintRectF-3"); 
  if (!DeleteObject (svw.hbr))              YERROR ("YPaintRectF-4"); 
#else  
  XSetForeground (DPY, SV->gc, f_color); 
  XFillRectangle (DPY, SV->win, SV->gc, x, y, w, h); 
  XFlush (DPY); 
#endif 
} 
/*-----------------------------YPaintString--------------------------------*/ 
void 
YPaintString (char *text, int x, int y, YT_COLOR color) 
{ 
  CALCXY (x, y); 
 
#if defined(FLTK)
  YSetFont(Y_FONT); /*??*/ 
	FSetColor (color);
	/* fl_color(color); */  /*??*/
  y += 4; /*??!!!*/
	fl_draw(text, x, y); /*??*/
#elif defined(API_W) 
  SetTextColor (svw.hdc, color); 
  TextOut (svw.hdc, x, y - FONTH / 2, text, strlen (text)); 
#else  
  y += 4; /*??!!!*/
  XSetForeground (DPY, SV->gc, color); 
  XDrawString (DPY, SV->win, SV->gc, x, y, text, strlen (text)); 
  XFlush (DPY); 
#endif 
} 
/*-------------------------------YStringW-----------------------------------*/ 
int 
YStringW (char *string) 
{ 
#if defined(FLTK)
	return(fl_width(string)); 
/* 	int w=0, h=0; */
/*   fl_measure(string, w, h); */
/* 	return (w); */
#elif defined(BORL_W)           
  return( LOWORD(GetTextExtent(svw.hdc,string,lstrlen(string))) );           
#elif defined(MICR_W) || defined(GCC__W) || defined(LCC__W) // Changes made by Ozerski 
  SIZE    siz; 
  GetTextExtentPoint32 (svw.hdc, string, lstrlen (string), &siz); 
  return (siz.cx); 
#else 
  return (XTextWidth (KERN_S->font_struct, string, strlen (string))); 
#endif 
} 
/*------------------------------YStringH------------------------------------*/ 
int 
YStringH (char *string) 
{ 
#if defined(FLTK)
	return(fl_height()); /*??*/

/* 	int w=0, h=0; */
/*   fl_measure(string, w, h); */
/* 	return (h); */

	/* return (14); */

#elif defined(BORL_W)           
  return( HIWORD(GetTextExtent(svw.hdc,string,lstrlen(string))) );           
#elif defined(MICR_W) || defined(GCC__W) || defined(LCC__W) // Changes made by Ozerski 
  SIZE    siz; 
  GetTextExtentPoint32 (svw.hdc, string, lstrlen (string), &siz); 
  return (siz.cy); 
#else  
  string++; 
  return (FONTH); 
#endif 
} 
/*------------------------------YSetFont------------------------------------*/ 
void 
YSetFont (char *name) 
{ 
  static char fonts[20][160]; 
  static int i = 0; 
 
/*   if (!name) {  */
/*     KerFreeFont ();  */
/*     i--;  */
/*     name = fonts[i - 1];  */
/*   } else {  */
/*     if (i >= 10)  */
/*       YERROR ("YSetFont");  */
/*     strcpy (fonts[i++], name);  */
/*   }  */
 
  KerSetFont (name); 
} 
/*-----------------------------KerSetFont-----------------------------------*/ 
void 
KerSetFont (char *name) 
{ 
#if defined(FLTK) 
/* 	static char font_name[250]; */
/* 	strcpy(font_name, name); */

	Fl::set_font((Fl_Font)3, name);
	fl_font((Fl_Font)3, Y_FONT_SIZE);

  FONTH = YStringH ("Wg"); /*??*/
#elif defined(API_W) 
  FONTH = YStringH ("Wg"); 

#else  
  if ((KERN_S->font_struct = XLoadQueryFont (DPY, name)) == NULL) 
    YERROR ("KerSetFont"); 
 
  XSetFont (DPY, SV->gc, (KERN_S->font_struct)->fid); 
  FONTH = (KERN_S->font_struct)->ascent + (KERN_S->font_struct)->descent; 

#endif 
} 
/*------------------------------KerFreeFont---------------------------------*/ 
void 
KerFreeFont () 
{ 
#if defined(API_W) 
#else  
  XFreeFont (DPY, KERN_S->font_struct); 
#endif 
} 
/*------------------------------YSetXorMode-------------------------------*/ 
void 
YSetXorMode (YT_BOOL xor) 
{ 
#if defined(API_W) 
  if (xor)  SetROP2 (svw.hdc, R2_XORPEN); 
  else      SetROP2 (svw.hdc, R2_COPYPEN); 
#else  
  if (xor)  XSetFunction (DPY, SV->gc, GXxor); 
  else      XSetFunction (DPY, SV->gc, GXcopy); 
#endif 
} 
/*------------------------------YPaintLine---------------------------------*/ 
void  
YPaintLine (int x1, int y1, int x2, int y2, YT_COLOR color) 
{ 
  CALCXY (x1, y1); 
  CALCXY (x2, y2); 

#if defined(BORL_W)           
#define YMoveTo(a,b,c) MoveTo(a,b,c)           
#elif defined(MICR_W) || defined(GCC__W) || defined(LCC__W) // Changes made by Ozerski 
#define YMoveTo(a,b,c) MoveToEx(a,b,c,NULL)           
#endif           

#if   defined(FLTK) 
	FSetColor (color);
	fl_line (x1, y1, x2, y2);
#elif defined(API_W) 
  if (!(svw.hpen = CreatePen (PS_SOLID, 1, color)))       YERROR ("YPaintLine-1"); 
  if (!(svw.hpenOld = SelectObject (svw.hdc, svw.hpen)))  YERROR ("YPaintLine-2"); 
  /* MoveToEx (svw.hdc, x1, y1, NULL); */ 
  YMoveTo(svw.hdc,x1,y1);           
  LineTo (svw.hdc, x2, y2); 
  YPaintPixel (x2 - KERN->x0 , y2 - KERN->y0 , color); //Changes made by Ozerski 
  if (!(SelectObject (svw.hdc, svw.hpenOld)))   YERROR ("YPaintLine-3"); 
  if (!DeleteObject (svw.hpen))                 YERROR ("YPaintLine-4"); 
#else  
  XSetForeground (DPY, SV->gc, color); 
  XDrawLine (DPY, SV->win, SV->gc, x1, y1, x2, y2); 
#endif 

} 
/*------------------------------KerClipInit---------------------------------*/ 
void 
KerClipInit (int x, int y, int w, int h) 
{ 
#if defined(API_W) 
#else  
  static XRectangle rect; 
 
  rect.x = x; 
  rect.y = y; 
  rect.width = w; 
  rect.height = h; 
 
  SV->regs_i = 0; 
  SV->regs[SV->regs_i] = XCreateRegion (); 
  XUnionRectWithRegion (&rect, SV->regs[SV->regs_i], SV->regs[SV->regs_i]); 
  XSetRegion (DPY, SV->gc, SV->regs[SV->regs_i]); 
  SV->regs_i++; 
#endif 
} 
/*-------------------------------KerClipAdd---------------------------------- 
void KerClipAdd(Region r) 
{ 
#if defined(API_W) 
#else         
  int i=SV->regs_i; 
 
  if (i>=CLIP_SIZE) YERROR("KerClipAdd"); 
  OUTD(111); 
  XIntersectRegion(r,SV->regs[i-1],SV->regs[i]);  
  OUTD(222); 
  XSetRegion(DPY,SV->gc,SV->regs[SV->regs_i]); 
  SV->regs_i++; 
#endif 
} */ 
/*------------------------------KerClipDel----------------------------------*/ 
void 
KerClipDel () 
{ 
#if defined(API_W) 
#else  
  if (SV->regs_i == 0) 
    YERROR ("YClipDel"); 
 
  XDestroyRegion (SV->regs[SV->regs_i--]); 
  XSetRegion (DPY, SV->gc, SV->regs[SV->regs_i]); 
#endif 
} 
/*-----------------------------YSetCliping----------------------------------*/ 
void 
YSetCliping (YT_BOOL clip, int x, int y, int w, int h) 
{ 
 
#if defined(FLTK_TMP)
  CALCXY (x, y); 
  CALCWH (w, h); 
	if (clip) {
		/* fl_not_clipped(x, y, w, h); */
		/* fl_clip(x-5, y-5, w+5, h+5); */
		/* fl_push_clip(x, y, w, h); */
		/* fl_push_clip(x, y, w, h); */
		fl_push_clip(x, y, w, h);
	} else {
		fl_pop_clip();
	}

#elif defined(API_W) 
  HRGN    hrgnClip; 
  CALCXY (x, y); 
  CALCWH (w, h); 
  if (clip) { 
    hrgnClip = CreateRectRgn (x, y, x + w, y + h); 
    SelectClipRgn (svw.hdc, hrgnClip); 
    DeleteObject (hrgnClip); 
  } else { 
    hrgnClip = CreateRectRgn (0, 0, W_SCREEN, H_SCREEN); 
    SelectClipRgn (svw.hdc, hrgnClip); 
    DeleteObject (hrgnClip); 
  } 
#else /*---------------------*/ 
  XRectangle rectangles[1]; 
 
/* return; */ 
  CALCXY (x, y); 
  CALCWH (w, h); 
  if (clip) { 
    rectangles[0].x = 0; 
    rectangles[0].y = 0; 
    rectangles[0].width = w; 
    rectangles[0].height = h; 
    XSetClipRectangles (DPY, SV->gc, x, y, rectangles, 1, Unsorted); 
  } else { 
    if (XEmptyRegion (SV->region)) {	/*?! */ 
      rectangles[0].x = 0; 
      rectangles[0].y = 0; 
      rectangles[0].width = W_SCREEN; 
      rectangles[0].height = H_SCREEN; 
      XSetClipRectangles (DPY, SV->gc, 0, 0, rectangles, 1, Unsorted); 
    } else {
      XSetRegion (DPY, SV->gc, SV->region); 
		}
  } 
#endif 
} 
/*------------------------------YCloseGraph---------------------------------*/ 
void 
YCloseGraph () 
{ 
#if defined(API_W) 
  if (!(DestroyWindow (svw.hWnd))) 
    YERROR ("DestroyWindow"); 
 
#else  
  XDestroyWindow (DPY, SV->win); 
  XFlush (DPY); 
#endif 
} 
/*------------------------------YMouseForm----------------------------------*/ 
void 
YMouseForm (char *shape) 
{ 
#if defined(API_W) 
  static HCURSOR hcur; 
 
  if (!Y_WMODE) return; 
 
  if      (!strcmp(shape,"arrow"))  hcur = LoadCursor (NULL, IDC_ARROW);  
  else if (!strcmp(shape,"wait") )  hcur = LoadCursor (NULL, IDC_WAIT);   
  else if (!strcmp(shape,"ibeam"))  hcur = LoadCursor (NULL, IDC_IBEAM);  
  else if (!strcmp(shape,"cross"))  hcur = LoadCursor (NULL, IDC_CROSS);  
  else    YBeep();   
 
/*   YSWITCH (shape) { */ 
/*   YCASE ("arrow")  hcur = LoadCursor (NULL, IDC_ARROW);  */ 
/*   YCASE ("wait")   hcur = LoadCursor (NULL, IDC_WAIT);   */ 
/*   YCASE ("ibeam")  hcur = LoadCursor (NULL, IDC_IBEAM);  */ 
/*   YCASE ("cross")  hcur = LoadCursor (NULL, IDC_CROSS);  */ 
/*   YDEFAULT         YBeep();   */ 
/*   } */ 
  SetCursor (hcur); 
  ShowCursor (TRUE); 
 
#else /*-------------------------*/ 
  Cursor  curs; 
  int  shap; 
 
  if (!Y_WMODE) return; 
 
  if      (!strcmp(shape,"arrow"))  shap = XC_top_left_arrow;  
  else if (!strcmp(shape,"wait") )  shap = XC_watch;   
  else if (!strcmp(shape,"ibeam"))  shap = XC_xterm;  
  else if (!strcmp(shape,"cross"))  shap = XC_crosshair;  
  else    YBeep();   
 
/*   YSWITCH (shape) { */ 
/*   YCASE ("arrow")  shap = XC_top_left_arrow;  */ 
/*   YCASE ("wait")   shap = XC_watch;           */ 
/*   YCASE ("ibeam")  shap = XC_xterm;            */ 
/*   YCASE ("cross")  shap = XC_crosshair;      */ 
/*   YDEFAULT         YBeep();  */ 
/*   } */ 
  curs = XCreateFontCursor (DPY, shap); 
  XDefineCursor (DPY, SV->win, curs); 
  XFlush (DPY); 
 
#endif 
} 
/*----------------------------YWndGroupBegin--------------------------------*/ 
void 
YWndGroupBegin () 
{ 
  /* if (SECOND_DRAW) return;   */             /*!!!!!!*/ 
  KERN_S->wndgroup_flag = TRUE; 
 
} 
/*-----------------------------YWndGroupEnd---------------------------------*/ 
void 
YWndGroupEnd () 
{ 
  /* if (SECOND_DRAW) return;   */             /*!!!!!!*/ 
 
  WINI(KERN_S->wndgroup_first)->id_goto = KERN_S->wndgroup_last; 
  KERN_S->wndgroup_flag = FALSE; 
  KERN_S->wndgroup_last = ID_NULL;  
 
} 
/*---------------------------------YIdGoto----------------------------------*/ 
int 
YIdGoto () 
{ 
 
  return (WIN->id_goto); 
 
} 
/*-----------------------------YImageFree-----------------------------------*/ 
void 
YImageFree (long pImage) 
{ 
#if defined(API_W) 
  if (!DeleteObject ((HBITMAP) pImage)) 
    YERROR ("YImageFree-DeleteObject"); 
#else  
  XDestroyImage ((XImage *) pImage); 
  XFlush (DPY); 
#endif 
} 
/*------------------------------YImageGet-----------------------------------*/ 
long 
YImageGet (int x, int y, int w, int h) 
{ 
#if defined(API_W) 
  HBITMAP hbmp; 
  CALCXY (x, y); 
  CALCWH (w, h); 
  if (!(hbmp = CreateCompatibleBitmap (svw.hdc, w + 1, h + 1))) YERROR ("YImageGet-1"); 
  if (!SelectObject (svw.hdcMemory, hbmp))                      YERROR ("YImageGet-2"); 
  if (!BitBlt (svw.hdcMemory, 0, 0, w + 1, h + 1, svw.hdc,  
	       x, y, SRCCOPY))                                  YERROR ("YImageGet-3"); 
 
  return ((LONG) hbmp); 
#else  
  XImage *xim; 
  CALCXY (x, y); 
  CALCWH (w, h); 
  xim = XGetImage (DPY, SV->win, x, y, w + 1, h + 1, AllPlanes, ZPixmap); 
  XFlush (DPY); 
 
  return ((long)xim); 
#endif 
} 
/*-------------------------------YImagePut----------------------------------*/ 
void 
YImagePut (int x, int y, long pImage) 
{ 
#if defined(API_W) 
  HBITMAP hbmp; 
  BITMAP  bmp; 
  CALCXY (x, y); 
  hbmp = (HBITMAP) pImage; 
  if (!GetObject (hbmp, sizeof (BITMAP), &bmp))           YERROR ("YImagePut-1"); 
  if (!BitBlt (svw.hdc, x, y, bmp.bmWidth, bmp.bmHeight,  
	       svw.hdcMemory, 0, 0, SRCCOPY))             YERROR ("YImagePut-2"); 
#else  
  XImage *xim; 
  CALCXY (x, y); 
  xim = (XImage *) pImage; 
  XPutImage (DPY, SV->win, SV->gc, xim, 0, 0, x, y, xim->width, xim->height); 
  XFlush (DPY); 
#endif 
} 
/*----------------------------YMakeImage----------------------------------*/ 
long 
YMakeImage (int wreal, int hreal) 
{ 
#if defined(API_W) 
#else  
  return ((long)XGetImage (DPY, SV->win, 10, 10, wreal, hreal, AllPlanes, ZPixmap)); 
#endif 
} 
/*-----------------------------YPutPixel----------------------------------*/ 
void 
YPutPixel (long image, int x, int y, YT_COLOR color) 
{ 
#if defined(API_W) 
#else  
  XPutPixel ((XImage *) image, x, y, color); 
#endif 
} 
 
 
/*-----------------------------YPaintPolyF-----------------------------------*/ 
void 
YPaintPolyF (int numpoints, int *points, YT_COLOR fcolor) 
{ 
#define MAX_PNTS 8 
  int    *ptr, i, x, y; 
 
#if defined(API_W) 
  POINT   pnts[MAX_PNTS]; 
 
  if (numpoints > MAX_PNTS) 
    YERROR ("YDrawFLines"); 
  ptr = points; 
  for (i = 1; i <= numpoints; i++) { 
    x = *(ptr + 2 * i - 2); 
    y = *(ptr + 2 * i - 1); 
    CALCXY (x, y); 
    pnts[i - 1].x = x; 
    pnts[i - 1].y = y; 
  } 
  if (!(svw.hbr = CreateSolidBrush (fcolor)))           YERROR ("YPaintPolyF-1"); 
  if (!(svw.hbrOld = SelectObject (svw.hdc, svw.hbr)))  YERROR ("YPaintPolyF-2"); 
  Polygon (svw.hdc, pnts, numpoints); 
  if (!SelectObject (svw.hdc, svw.hbrOld))  YERROR ("YPaintPolyF-3"); 
  if (!DeleteObject (svw.hbr))              YERROR ("YPaintPolyF-4"); 
 
#else /*---------------------*/ 
  XPoint  pnts[MAX_PNTS]; 
 
  if (numpoints > MAX_PNTS) 
    YERROR ("YDrawFLines"); 
 
  ptr = points; 
  for (i = 1; i <= numpoints; i++) { 
    x = *(ptr + 2 * i - 2); 
    y = *(ptr + 2 * i - 1); 
    CALCXY (x, y); 
    pnts[i - 1].x = x; 
    pnts[i - 1].y = y; 
  } 
  XSetForeground (DPY, SV->gc, fcolor); 
  XFillPolygon (DPY, SV->win, SV->gc, pnts, numpoints, Complex, CoordModeOrigin); 
  XFlush (DPY); 
#endif 
} 
/*------------------------------YPaintPixel----------------------------------*/ 
void 
YPaintPixel (int x, int y, YT_COLOR color) 
{ 
 
  CALCXY (x, y); 

#if   defined(FLTK) 
	FSetColor (color);
	fl_point (x, y);
#elif defined(API_W) 
  SetPixel (svw.hdc, x, y, color); 
#else  
  XSetForeground (DPY, SV->gc, color); 
  XDrawPoint (DPY, SV->win, SV->gc, x, y); 
#endif 
 
} 
/*-------------------------------YGetColor----------------------------------*/ 
YT_COLOR 
YGetColor (int x, int y) 
{ 
#if defined(API_W) 
  YT_COLOR color; 
  CALCXY (x, y); 
  color = GetPixel (svw.hdc, x, y); 
#else  
  XImage *xim; 
  YT_COLOR color; 
  CALCXY (x, y); 
  xim = XGetImage (DPY, SV->win, x - 5, y - 5, 10, 10, AllPlanes, ZPixmap); 
  color = XGetPixel (xim, 5, 5); 
  XDestroyImage (xim); 
  XFlush (DPY); 
#endif 
  return (color); 
} 
/*------------------------------YPaintArcF----------------------------------*/ 
void 
YPaintArcF (int x, int y, int w, int h, int ang_begin, int ang_delta, YT_COLOR f_color) 
{
  CALCXY (x, y); 
  CALCWH (w, h); 

#if   defined(FLTK_TMP) 
#elif defined(API_W) 
#else  
  XSetForeground (DPY, SV->gc, f_color); 
  XFillArc (DPY, SV->win, SV->gc, x, y, w, h, ang_begin, ang_delta); 
  XFlush (DPY); 
#endif 
} 
/*------------------------------YPaintArcB----------------------------------*/ 
void 
YPaintArcB (int x, int y, int w, int h, int ang_begin, int ang_delta, YT_COLOR b_color) 
{ 
  CALCXY (x, y); 
  CALCWH (w, h); 

#if   defined(FLTK_TMP) 
#elif defined(API_W) 
#else  
  XSetForeground (DPY, SV->gc, b_color); 
  XDrawArc (DPY, SV->win, SV->gc, x, y, w, h, ang_begin, ang_delta); 
  XFlush (DPY); 
#endif 
} 
/*------------------------------YPaintRectB----------------------------------*/ 
void 
YPaintRectB (int x, int y, int w, int h, YT_COLOR color) 
{ 
  w--; 
  h--; 
  YPaintLine (x, y, x + w, y, color); 
  YPaintLine (x, y + h, x + w, y + h, color); 
  YPaintLine (x, y + h, x, y, color); 
  YPaintLine (x + w, y, x + w, y + h, color); 
} 
/*------------------------------YPaintRectFB---------------------------------*/ 
void 
YPaintRectFB (int x, int y, int width, int height, YT_COLOR fcolor, YT_COLOR lcolor) 
{ 
  YPaintRectF (x, y, width, height, fcolor); 
  YPaintRectB (x, y, width, height, lcolor); 
  return; 
} 
/*****************************************************************************/ 
 
 
#endif 
#if defined(D_MORE) 
/**************************************************************************** 
 *                                                                          * 
 *                          MORE MORE MORE MORE MORE                        * 
 *                                                                          * 
 **************************************************************************** 
 */  
 
/* #include "y_open.h"   */
/* #include "y_more.h"   */
#include "y_zone.h" 
 
 
 
/****************************************************************************/ 
 
void  YInitMORE_S (void); 
 
typedef struct { 
  YT_SIZET ii, jj; 
  YT_COLOR *p; 
  int     x0, y0; 
} YT_CMAP; 
 
typedef struct { 
  char     virt_name[30]; 
  char     real_name[30]; 
} YT_VNAME; 
 
typedef struct { 
  char      proc_name[80]; 
  YT_PFUNC  proc; 
} AT_PNAME2; 
 
#define  SETI_SIZE 100 
typedef struct { 
  char  *text[SETI_SIZE]; 
  long   lpar[SETI_SIZE]; 
} YT_SETI; 
 
#define  NEXTPROC_NUM 100 
typedef struct { 
  YT_PFUNC  prev; 
  YT_PFUNC  next; 
} YT_NEXTPROC; 
 
typedef struct { 
  int     type; 
  void   *ptr; 
  int     i1, i2, i3, i4; 
  YT_COLOR color; 
  int     i5, i6; 
  char   *text; 
  char   *oname; 
  int     status; 
  YT_BOOL xor; 
} YT_SCROBJ; 
 
typedef struct { 
  YT_SCROBJ **ptr; 
  int     size; 
  int     x, y, w, h; 
  char   *save_fname; 
  int     save_xx, save_yy; 
  int     xold, yold, wold, hold; 
  int     old_x0, old_y0; 
  float   old_x_mult, old_y_mult; 
} YT_META;  
 
#define  PNAMES_MAX   100 
#define  NUM_VNAMES   100  
typedef struct { 
  YT_NEXTPROC  nproces[NEXTPROC_NUM]; 
  YT_SETI   *seti; 
  int        x0group, y0group; 
  int        draw_mode; 
  YT_VNAME  *vnames[NUM_VNAMES]; 
  AT_PNAME2  pnames[PNAMES_MAX]; 
  FILE      *post_file; 
  char       draw_oname[30]; 
  int        transf_ymin, transf_ymax; 
  int        hand_savemode; 
  YT_CMAP   *hand_cmap; 
  YT_META   *meta; 
  int        meta_savemode; 
  YT_BOOL    meta_xor; 
  int        WAIT_done, WAIT_doneold; 
  int        WAIT_id; 
  int        WAIT_i; 
  int        WAIT_imax; 
  YT_CONTEXT WAIT_save; 
} YT_MORE_S; 
 
YT_MORE_S impr_s; 
#define  IS (&impr_s)   
 
YT_MORE *impr; 
 
#define DRAW_MODE (IS->draw_mode) 
#define POST_FILE (IS->post_file) 
 
YT_SETI* YSetiCreate (void); 
long     YSetiGet (YT_SETI *seti, char *text); 
void     YSetiPut (YT_SETI *seti, char *text, long lpar); 
 
void    YDrawGroup (char *sText, int,int,int,int, YT_COLOR); 
void    YTransf (int *px, int *py); 
void    paint_winibox (char *text,int, int, int, int, YT_COLOR color_frame, YT_COLOR color_fill); 
int     YVertString (int x, int y, char *string, YT_COLOR color, YT_BOOL); 
 
void    YMetaAdd (int type, void *, int, int, int, int, YT_COLOR, int, int, char *); 
YT_BOOL YMetaIfAll (YT_META *meta, int x, int y, int w, int h); 
void    YMetaDrawAll (YT_META *, YT_BOOL); 
void    YMetaDrawOne (YT_SCROBJ *o); 
void    YMetaDel (YT_META *, int i); 
void    YMetaDelBackups (YT_META *meta); 
 
void    PS_Begin (char *fname, char *fontname); 
void    PS_End (void); 
void    PS_Image (int, int, void *ptr, int, int, YT_COLOR (*get_color) (void *, int, int)); 
void    PS_Utils (void); 
void    PS_Setcolor (YT_COLOR color); 
void    PS_Stroke (void); 
void    PS_Showpage (void); 
 
 
/*********************************************************************************/ 
/*-------------------------------YInitMORE------------------------------*/ 
void   
YInitMORE () 
{ 
 
  YMALLOC (MORE, YT_MORE, 1);  
 
  Y_STDERR = stderr; 
  MORE->print_fout  = Y_STDERR; 
  MORE->print_fname = NULL; 
 
  IS->seti = YSetiCreate (); 
 
  YInitMORE_S (); 
 
  return; 
} 
/*-------------------------------YInitMORE_S------------------------------*/ 
void   
YInitMORE_S () 
{ 
  int i; 
 
  strcpy (IS->draw_oname, ""); 
  DRAW_MODE = YREAL; 
 
  for (i = 0; i < NUM_VNAMES; i++)  /* YSetVirtNames (names); */ 
    IS->vnames[i] = NULL;   
 
  for (i=0; i<PNAMES_MAX; i++)  
    strcpy (IS->pnames[i].proc_name, ""); 
 
  /* FO_EXIST = FALSE; */ 
 
  /* YLoadMyFont (); */ 
  YNextProcInit ();  
 
  return; 
} 
/*-----------------------------YStrCopy-------------------------------------*/ 
YT_BOOL 
YStrCopy (char **t, char **s, int num) 
{ 
  int     i; 
 
  for (i = 0; i < num; i++) { 
    if (s[i] == NULL) { 
      t[i] = NULL; 
      return (TRUE); 
    } 
    t[i] = (char *) malloc (strlen (s[i] + 1)); 
    strcpy (t[i], s[i]); 
  } 
 
  if (s[num - 1] == NULL) 
    return (TRUE); 
 
  s[num - 1] = NULL; 
  return (FALSE); 
} 
/*-------------------------------YStrCmp------------------------------------*/ 
YT_BOOL 
YStrCmp (char *str1, char *str2) 
{ 
  if (!str1 || !str2) return (FALSE);   
   
  return (!(strcmp(str1, str2))); 
/*   if (strcmp (str1, str2) == 0) */ 
/*     return (TRUE); */ 
/*   else */ 
/*     return (FALSE); */ 
} 
/*-----------------------------ZHowManyFree---------------------------------*/ 
YT_SIZET 
ZHowManyFree () 
{ 
  YT_SIZET size, delta; 
  void   *p; 
 
  size = 0; 
  size = ~size; 
  delta = size / 2; 
  while (delta > 1000) { 
    p = malloc (size); 
    if (p == NULL) 
      size -= delta; 
    else { 
      size += delta; 
      free (p); 
    } 
    delta /= 2; 
  } 
 
  free (p); 
  return (size); 
} 
/*-----------------------------YTimePause--------------------------------*/ 
void 
YTimePause (long ms) 
{ 
  long  time1, time2; 
 
  time1 = YTimeGet (); 
  do { 
    time2 = YTimeGet (); 
  } while (labs (time2 - time1) < ms); 
 
  return; 
} 
/*--------------------------------YMpiInit------------------------------------*/ 
void YMpiInit(int argc, char **argv) 
{ 
#ifdef Y_MPI 
  MPI_Init(&argc, &argv); 
#endif 
 
  argc++;  argv++; 
} 
/*------------------------------YMpiFinalize----------------------------------*/ 
void YMpiFinalize() 
{ 
#ifdef Y_MPI 
  MPI_Finalize(); 
#endif 
} 
/*-----------------------------YPrintMode----------------------------------*/ 
void 
YPrintMode (char *fname, YT_BOOL is_add) 
{ 
 
  if (!fname && is_add) fprintf (stderr, "YPrintMode \n"); 
 
  if (MORE->print_fout != Y_STDERR)   
    fclose (MORE->print_fout); 
 
  if (fname) { 
    MORE->print_fout = fopen (fname, "w"); 
    if (is_add) fclose (MORE->print_fout); 
  } else  
    MORE->print_fout = Y_STDERR; 
  
  MORE->print_fname  = fname; 
  MORE->print_is_add = is_add; 
  return; 
} 
/*-------------------------------YPrintDo-----------------------------------*/ 
void 
YPrintDo (char *string) 
{ 
 
  if (MORE->print_is_add)  
    MORE->print_fout = fopen (MORE->print_fname, "a+"); 
 
  fprintf (MORE->print_fout, "%s", string); 
 
  if (MORE->print_is_add)  
    fclose (MORE->print_fout); 
 
  return; 
} 
/*------------------------------YWarningS--------------------------------*/ 
void YWarningS (char *warning, char *string) 
{ 
 
  sprintf  (Y_STR,"%s : %s \n", warning, string);  
  YPrintDo (Y_STR); 
 
} 
/*-------------------------------YWarning--------------------------------*/ 
void 
YWarning (char *string) 
{ 
 
  YWarningS ("YWarning", string); 
 
} 
/*------------------------------YErrorS----------------------------------*/ 
void YErrorS (char *warning, char *string) 
{ 
 
  YBeep (); 
  YWarningS (warning, string); 
  exit (EXIT_FAILURE); 
 
} 
/*-------------------------------YError----------------------------------*/ 
void 
YError (char *string) 
{ 
 
  YErrorS ("YError", string); 
 
} 
/*-------------------------------YNextProcInit----------------------------------*/ 
void 
YNextProcInit () 
{ 
  int i; 
 
  for (i=0; i<NEXTPROC_NUM; i++) { 
    IS->nproces[i].prev = NULL; 
    IS->nproces[i].next = NULL; 
  } 
 
  return; 
} 
/*-------------------------------YNextProcSet----------------------------------*/ 
YT_BOOL 
YNextProcSet (YT_PFUNC prev, YT_PFUNC next) 
{ 
  int i; 
 
  for (i=0; TRUE; i++) { 
    if (i == NEXTPROC_NUM) return (FALSE); 
    if (IS->nproces[i].prev == NULL) break; 
  } 
 
  IS->nproces[i].prev = prev; 
  IS->nproces[i].next = next; 
 
  return (TRUE); 
} 
/*---------------------------------next_proc_null------------------------------*/ 
long 
next_proc_null (PFUNC_VAR) 
{ 
 
  switch (message) { 
  case YCREATE: 
    break; 
  default: ;;;  
  } 
 
  id++; 
  RETURN_TRUE; 
} 
/*---------------------------------YNextProcGet--------------------------------*/ 
YT_PFUNC 
YNextProcGet (YT_PFUNC prev) 
{ 
  int i; 
 
  for (i=0; TRUE; i++) { 
    if (i == NEXTPROC_NUM) return (/* DITTO */next_proc_null); 
    if (IS->nproces[i].prev == NULL) continue; 
    if (IS->nproces[i].prev == prev) break; 
  } 
 
  return (IS->nproces[i].next); 
} 
/*-----------------------------YBeginGroup----------------------------------*/ 
void 
YBeginGroup (char *name, int x, int y, int w, int h, YT_COLOR color) 
{ 
       
  if (name != NULL) { 
    YDrawGroup (name, x, y, w, h, color); 
  } 
  YGetOrigin (&IS->x0group, &IS->y0group); 
  CALCWH (x, y); 
  YSetOrigin (IS->x0group + x, IS->y0group + y); 
 
} 
/*-------------------------------YEndGroup----------------------------------*/ 
void 
YEndGroup () 
{ 
 
  YSetOrigin (IS->x0group, IS->y0group); 
 
} 
/*-------------------------------YDrawGroup---------------------------------*/ 
void 
YDrawGroup (char *sText, int x, int y, int w, int h, YT_COLOR color) 
{ 
  int  w_char = YStringW ("W"); 
  int  left = 10, dx = 5, dy = 4, xmax, ymax, xl, xr; 
  int  wtext; 
  YT_COLOR clr_white = YColor("white"); 
  YT_COLOR clr_black = YColor("black"); 
 
  xmax = x+w-1; 
  ymax = y+h-1; 
 
  if (color != CLR_NULL) { 
    YDrawRectF (x+2, y+2+dy, w-4, h-4-dy, color);  
    YDrawRectF (x+2, y+2, left-2, dy, color);  
  } 
 
  xl = x+left; 
  YDrawLine (x,   y,   xl, y,   clr_black); 
  YDrawLine (x+1, y+1, xl, y+1, clr_white); 
 
  YDrawString (sText, xl+dx, y, clr_black); 
  wtext = YStringW (sText); 
  ANTIMULTX (wtext); 
  xr = xl+dx+wtext+dx; 
  if (color != CLR_NULL)  
    YDrawRectF (xr, y+2, xmax-2-xr+1, dy, color);  
 
  YDrawLine (xr, y,   xmax,   y,   clr_black); 
  YDrawLine (xr, y+1, xmax-1, y+1, clr_white); 
 
  YDrawLine (x+1,    y+1,    x+1,    ymax,   clr_white); 
  YDrawLine (x,      ymax,   xmax,   ymax,   clr_white); 
  YDrawLine (xmax,   y,      xmax,   ymax,   clr_white); 
 
  YDrawLine (x,      y,      x,      ymax-1, clr_black); 
  YDrawLine (x,      ymax-1, xmax-1, ymax-1, clr_black); 
  YDrawLine (xmax-1, y,      xmax-1, ymax-1, clr_black); 
 
  return; 
} 
/*-----------------------------YDrawStringImp------------------------------*/ 
void 
YDrawStringImp (char *text, int x0, int y0, YT_COLOR color, int horz, int vert) 
{ 
  int     x, y, w, h; 
 
  w = YStringW (text); 
  h = YStringH (text); 
  ANTICALCWH (w, h); 
 
  if      (horz == YLEFT)  x = x0 - w; 
  else if (horz == YNONE)  x = x0 - w / 2; 
  else if (horz == YRIGHT) x = x0; 
  else 
    YError ("YDrawStringImp-1"); 
 
  if      (vert == YUP)    y = y0 - h / 2; 
  else if (vert == YNONE)  y = y0; 
  else if (vert == YDOWN)  y = y0 + h / 2; 
  else 
    YError ("YDrawStringImp-2"); 
 
  YDrawString (text, x, y, color); 
} 
/*-----------------------------YDrawRectText--------------------------------*/ 
void 
YDrawRectText (char *text, int x, int y, int w, int h, YT_COLOR color) 
{ 
 
  YDrawStringImp (text, x + w / 2, y + h / 2, color, YNONE, YNONE); 
  return; 
 
} 
/*------------------------------YDrawString--------------------------------*/ 
void 
YDrawString (char *text, int x, int y, YT_COLOR color) 
{ 
  switch (DRAW_MODE) { 
  case YMETA: 
    YMetaAdd (YSTRING, NULL, x, y, 0, 0, color, 0, 0, text); 
    break; 
  case YREAL: 
    YPaintString (text, x, y, color); 
    break; 
  case YPOST: 
    y += 4; 
    YTransf (&x, &y); 
    PS_Setcolor (color); 
    fprintf (POST_FILE, "%d %d %s \n", x, y, "moveto"); 
    fprintf (POST_FILE, "( %s ) show \n", text); 
    PS_Stroke (); 
    break; 
  default: ;; 
  } 
 
} 
/*------------------------------YDrawRectF----------------------------------*/ 
void 
YDrawRectF (int x, int y, int w, int h, YT_COLOR f_color) 
{ 
  switch (DRAW_MODE) { 
  case YMETA: 
    YMetaAdd (YRECTF, NULL, x, y, w, h, f_color, 0, 0, NULL); 
    break; 
  case YREAL: 
    YPaintRectF (x, y, w, h, f_color); 
    break; 
  case YPOST: 
    { 
      int     x1 = x, y1 = y; 
      int     x2 = x + w, y2 = y; 
      int     x3 = x + w, y3 = y + h; 
      int     x4 = x, y4 = y + h; 
      YTransf (&x1, &y1); 
      YTransf (&x2, &y2); 
      YTransf (&x3, &y3); 
      YTransf (&x4, &y4); 
      fprintf (POST_FILE, "newpath \n"); 
      PS_Setcolor (f_color); 
      fprintf (POST_FILE, "%d %d %s \n", x1, y1, "moveto"); 
      fprintf (POST_FILE, "%d %d %s \n", x2, y2, "lineto"); 
      fprintf (POST_FILE, "%d %d %s \n", x3, y3, "lineto"); 
      fprintf (POST_FILE, "%d %d %s \n", x4, y4, "lineto"); 
      fprintf (POST_FILE, "%d %d %s \n", x1, y1, "lineto"); 
      fprintf (POST_FILE, "closepath fill \n"); 
      PS_Stroke (); 
    } 
    break; 
  } 
 
  return; 
} 
/*------------------------------YDrawRectB----------------------------------*/ 
void 
YDrawRectB (int x, int y, int w, int h, YT_COLOR color) 
{ 
  w--; 
  h--; 
  YDrawLine (x, y, x + w, y, color); 
  YDrawLine (x, y + h, x + w, y + h, color); 
  YDrawLine (x, y + h, x, y, color); 
  YDrawLine (x + w, y, x + w, y + h, color); 
} 
/*------------------------------YDrawRectFB---------------------------------*/ 
void 
YDrawRectFB (int x, int y, int width, int height, YT_COLOR fcolor, YT_COLOR lcolor) 
{ 
  YDrawRectF (x, y, width, height, fcolor); 
  YDrawRectB (x, y, width, height, lcolor); 
  return; 
} 
/*-----------------------------YDrawLine------------------------------------*/ 
void 
YDrawLine (int x1, int y1, int x2, int y2, YT_COLOR color) 
{ 
  switch (DRAW_MODE) { 
  case YMETA: 
    YMetaAdd (YLINE, NULL, x1, y1, x2, y2, color, 0, 0, NULL); 
    break; 
  case YREAL: 
    YPaintLine (x1, y1, x2, y2, color); 
    break; 
  case YPOST: 
    YTransf (&x1, &y1); 
    YTransf (&x2, &y2); 
    PS_Setcolor (color); 
    fprintf (POST_FILE, "%d %d %s \n", x1, y1, "moveto"); 
    fprintf (POST_FILE, "%d %d %s \n", x2, y2, "lineto"); 
    PS_Stroke (); 
    break; 
  } 
 
  return; 
} 
/*------------------------------YDrawPixel----------------------------------*/ 
void 
YDrawPixel (int x, int y, YT_COLOR color) 
{ 
 
  switch (DRAW_MODE) { 
  case YMETA: 
    YMetaAdd (YPIXEL, NULL, x, y, 0, 0, color, 0, 0, NULL); 
    break; 
  case YREAL: 
    YPaintPixel (x, y, color); 
    break; 
  case YPOST: 
    YTransf (&x, &y); 
    PS_Setcolor (color); 
    fprintf (POST_FILE, "%d %d %s \n", x,   y, "moveto"); 
    fprintf (POST_FILE, "%d %d %s \n", x+1, y, "lineto"); /*??*/ 
    PS_Stroke (); 
    break; 
  } 
 
  return; 
} 
/*------------------------------YDrawArcB----------------------------------*/ 
void 
YDrawArcB (int x, int y, int w, int h, int angle1, int angle2, YT_COLOR b_color) 
{ 
 
  switch (DRAW_MODE) { 
  case YMETA: 
    YMetaAdd (YARCB, NULL, x, y, w, h, b_color, angle1, angle2, NULL); 
    break; 
  case YREAL: 
    YPaintArcB (x, y, w, h, angle1, angle2, b_color); 
    break; 
  case YPOST: 
    YTransf (&x, &y); 
    YTransf (&w, &h); 
    fprintf (POST_FILE, "newpath \n"); 
    PS_Setcolor (b_color); 
    fprintf (POST_FILE, "%d %d %d %d %d %d %s \n", x, y, w, h, angle1, angle2, "ellipse"); 
    fprintf (POST_FILE, "closepath \n"); 
    PS_Stroke (); 
    break; 
  } 
 
  return; 
 
} 
/*------------------------------YDrawArcF----------------------------------*/ 
void 
YDrawArcF (int x, int y, int w, int h, int angle1, int angle2, YT_COLOR fcolor) 
{ 
 
  switch (DRAW_MODE) { 
  case YMETA: 
    YMetaAdd (YARCF, NULL, x, y, w, h, fcolor, angle1, angle2, NULL); 
    break; 
  case YREAL: 
    YPaintArcF (x, y, w, h, angle1, angle1, fcolor); 
    break; 
  case YPOST: 
    YTransf (&x, &y); 
    YTransf (&w, &h); 
    fprintf (POST_FILE, "newpath \n"); 
    PS_Setcolor (fcolor); 
    fprintf (POST_FILE, "%d %d %d %d %d %d %s \n", x, y, w, h, angle1, angle2, "ellipse"); 
    fprintf (POST_FILE, "closepath fill \n"); 
    PS_Stroke (); 
    break; 
  } 
 
  return; 
 
} 
/*------------------------------YDrawCircB---------------------------------*/ 
void 
YDrawCircB (int cx, int cy, int r, YT_COLOR color) 
{ 
   
  YDrawArcB (cx - r, cy - r, 2*r, 2*r, 0, 360*64, color); 
 
  return; 
} 
/*------------------------------YDrawCircF----------------------------------*/ 
void 
YDrawCircF (int cx, int cy, int r, YT_COLOR color) 
{ 
   
  YDrawArcF (cx - r, cy - r, 2*r, 2*r, 0, 360*64, color); 
 
  return; 
} 
/*-----------------------------YDrawLogo------------------------------------*/ 
void 
YDrawLogo (char *text,int X, int Y, int Width, int Height) 
{ 
  int     left, top, right, bottom; 
/*   char *text="Ylib"; */ 
 
  left = X; 
  top = Y; 
  right = X + Width / 2; 
  bottom = Y + Height / 2; 
  paint_winibox (text,left, top, right, bottom, YColor("yellow"), YColor("red")); 
 
  left = X + Width / 6; 
  top = Y + Height / 6; 
  right = X + 2 * Width / 3; 
  bottom = Y + 4 * Height / 5; 
  paint_winibox (text,left, top, right, bottom, YColor("white"), YColor("fuchsia")); 
 
  left = X + Width / 3; 
  top = Y + Height / 3; 
  right = X + Width; 
  bottom = Y + Height; 
  paint_winibox (text,left, top, right, bottom, YColor("black"), YColor("blue")); 
 
  return; 
} 
/*---------------------------paint_winibox----------------------------------*/ 
void 
paint_winibox (char *text,int left, int top, int right, int bottom, 
	       YT_COLOR color_frame, YT_COLOR color_fill) 
{ 
  int     width, height; 
 
  width = right - left; 
  height = 5 * (bottom - top) / 6; 
 
  YDrawRectFB (left, top, width, bottom - top, color_fill, color_frame); 
  YDrawLine (left + 1, bottom - height, right - 1, bottom - height,  
                    YColor("black") ); 
 
  YDrawStringImp (text, left+width/2, bottom-height/2, YColor("white"), YNONE, YNONE); 
 
  return; 
} 
/*------------------------------YDrawStrings--------------------------------*/ 
void 
YDrawStrings (char **text, int x, int y) 
{ 
  char  **p; 
  int     i, step = 15, w = 0, h, dw = 2, dh = 2; 
  YT_COLOR color_fill = YColorV("vc_draw_strings"); 
  YT_COLOR color_text = YColor("black"); 
 
  y -= UNITY / 2; 
 
  for (i = 0, p = text; *p != NULL; p++, i++) 
    w = YMAX (w, YStringW (*p)); 
  h = step * (i - 1) + UNITY; 
 
  w = w + 2 * dw; 
  h = h + 2 * dh; 
  ANTIMULTX (w); 
  YDrawRectF (x - dw, y - dh, w, h, color_fill); 
 
  for (i = 0, p = text; *p != NULL; p++, i++) 
    YDrawStringImp (*p, x, y + UNITY / 2 + step * i, color_text, YRIGHT, YNONE); 
 
  return; 
} 
/*-----------------------------YDrawSymbol----------------------------------*/ 
void 
YDrawSymbol (char *name, int x, int y, int w, int h, YT_COLOR color) 
{ 
  int  x1, y1, x2, y2, x3, y3; 
 
  YSWITCH (name) { 
  YCASE ("up>") 
    x1 = x; 
    x2 = x + w; 
    x3 = x + w / 2; 
    y1 = y + h; 
    y2 = y1; 
    y3 = y; 
    YDrawTriF (x1, y1, x2, y2, x3, y3, color); 
  YCASE ("down>") 
    x1 = x; 
    x2 = x + w; 
    x3 = x + w / 2; 
    y1 = y; 
    y2 = y1; 
    y3 = y + h; 
    YDrawTriF (x1, y1, x2, y2, x3, y3, color); 
  YCASE ("left>") 
    x1 = x + w; 
    x2 = x1; 
    x3 = x; 
    y1 = y; 
    y2 = y + h; 
    y3 = y + h / 2; 
    YDrawTriF (x1, y1, x2, y2, x3, y3, color); 
  YCASE ("right>") 
    x1 = x; 
    x2 = x1; 
    x3 = x + w; 
    y1 = y; 
    y2 = y + h; 
    y3 = y + h / 2; 
    YDrawTriF (x1, y1, x2, y2, x3, y3, color); 
  YDEFAULT 
  }  
 
  return; 
} 
/*-------------------------------YDrawPolyF----------------------------------*/ 
void 
YDrawPolyF (int num, int *points, YT_COLOR fcolor) 
{ 
  int i, x, y; 
 
  switch (DRAW_MODE) { 
  case YMETA: 
    YMetaAdd (YPOLYF, (void*)points, num, 0, 0, 0, fcolor, 0, 0, NULL); 
    break; 
  case YREAL: 
    YPaintPolyF (num, points, fcolor); 
    break; 
  case YPOST: 
    fprintf (POST_FILE, "newpath \n"); 
    PS_Setcolor (fcolor); 
 
    for (i = 0; i < num; i++) { 
      x = points[2*i]; 
      y = points[2*i+1]; 
      YTransf (&x, &y); 
      if (i == 0) fprintf (POST_FILE, "%d %d %s \n", x, y, "moveto"); 
      else        fprintf (POST_FILE, "%d %d %s \n", x, y, "lineto"); 
    } 
 
    fprintf (POST_FILE, "closepath fill \n"); 
    PS_Stroke (); 
    break; 
  } 
 
  return; 
} 
/*-------------------------------YDrawPolyB----------------------------------*/ 
void 
YDrawPolyB (int num, int *points, YT_COLOR color) 
{ 
  int  i, x, y, x1, y1, x2, y2; 
 
  for (i = 0; i < num; i++) { 
    x = points[2*i]; 
    y = points[2*i+1]; 
    if (i == 0) { 
      x1 = x;  
      y1 = y; 
      continue; 
    } 
    x2 = x;  
    y2 = y; 
    YDrawLine (x1, y1, x2, y2, color); 
    x1 = x2;  
    y1 = y2; 
  } 
 
  return; 
} 
/*-------------------------------YDrawTriF----------------------------------*/ 
void 
YDrawTriF (int x1, int y1, int x2, int y2, int x3, int y3, YT_COLOR fcolor) 
{ 
  /* static int  ls[6]; */ 
  int  *ls = (int*)malloc (6*sizeof(int)); 
 
  ls[0] = x1;  ls[1] = y1; 
  ls[2] = x2;  ls[3] = y2; 
  ls[4] = x3;  ls[5] = y3; 
  YDrawPolyF (3, ls, fcolor); 
 
  return; 
} 
/*--------------------------------YDrawTriB---------------------------------*/ 
void 
YDrawTriB (int x1, int y1, int x2, int y2, int x3, int y3, YT_COLOR bcolor) 
{ 
  YDrawLine (x1, y1, x2, y2, bcolor); 
  YDrawLine (x2, y2, x3, y3, bcolor); 
  YDrawLine (x3, y3, x1, y1, bcolor); 
  return; 
} 
/*--------------------------------YDrawTriFB--------------------------------*/ 
void 
YDrawTriFB (int x1, int y1, int x2, int y2, int x3, int y3, YT_COLOR fcolor,  
	    YT_COLOR bcolor) 
{ 
  YDrawTriF (x1, y1, x2, y2, x3, y3, fcolor); 
  YDrawTriB (x1, y1, x2, y2, x3, y3, bcolor); 
  return; 
} 
/*-------------------------------YDrawFourF---------------------------------*/ 
void 
YDrawFourF (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, 
	    YT_COLOR fcolor) 
{ 
 /*  int     lines[8]; */ 
  int  *lines = (int*)malloc (8*sizeof(int)); 
 
  lines[0] = x1;  lines[1] = y1; 
  lines[2] = x2;  lines[3] = y2; 
  lines[4] = x3;  lines[5] = y3; 
  lines[6] = x4;  lines[7] = y4; 
 
  YDrawPolyF (4, lines, fcolor); 
} 
/*--------------------------------YDrawFourB--------------------------------*/ 
void 
YDrawFourB (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, 
	    YT_COLOR bcolor) 
{ 
  YDrawLine (x1, y1, x2, y2, bcolor); 
  YDrawLine (x2, y2, x3, y3, bcolor); 
  YDrawLine (x3, y3, x4, y4, bcolor); 
  YDrawLine (x4, y4, x1, y1, bcolor); 
} 
/*------------------------------YDrawFourFB---------------------------------*/ 
void 
YDrawFourFB (int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, 
	     YT_COLOR fcolor, YT_COLOR bcolor) 
{ 
  YDrawFourF (x1, y1, x2, y2, x3, y3, x4, y4, fcolor); 
  YDrawFourB (x1, y1, x2, y2, x3, y3, x4, y4, bcolor); 
} 
/*------------------------------YDrawScale----------------------------------*/ 
void 
YDrawScale (int xx, int yy, int ww, int hh, int style) 
{ 
  int     i, beg, end, size, beg0; 
  float   step; 
  int     itemp; 
  int     isize; 
  YT_COLOR color; 
  int     xx_old = xx, yy_old = yy, ww_old = ww, hh_old = hh; 
 
  isize = SIZE_SCALE; 
 
  if (style == YHORZ) { 
    itemp = xx;  xx = yy;  yy = itemp; 
    itemp = ww;  ww = hh;  hh = itemp; 
  } 
 
  step = (float) hh / isize; 
  beg = beg0 = yy; 
  for (i = 1; i <= isize; i++) { 
    end = beg0 + YInt (step * i); 
    size = end - beg + 1; 
    color = CLR_SCALE (isize - i); 
    if (style == YHORZ)  YDrawRectF (beg, xx, size, ww, color); 
    else                 YDrawRectF (xx, beg, ww, size, color); 
    beg = end; 
  } 
 
  YDrawRectB (xx_old, yy_old, ww_old, hh_old, YColor("black")); 
 
  return; 
} 
/*------------------------------YDrawColer----------------------------------*/ 
void 
YDrawColer (int xx, int yy, int ww, int hh) 
{ 
  int     n, i,j,  xbeg0,xbeg,xend,xsize, ybeg0,ybeg,yend,ysize; 
  float   xstep, ystep; 
  int     isize, jsize; 
  YT_COLOR color; 
 
  isize = 10 /*SIZE_COLER*/; 
  jsize = 2; 
 
  xstep = (float) ww / isize; 
  ystep = (float) hh / jsize; 
  n = 0; 
  ybeg = ybeg0 = yy; 
  for (j=0; j<jsize; j++) { 
    yend = ybeg0 + YInt (ystep * (j+1)); 
    ysize = yend - ybeg + 1; 
    xbeg = xbeg0 = xx; 
    for (i=0; i<isize; i++) { 
      xend = xbeg0 + YInt (xstep * (i+1)); 
      xsize = xend - xbeg + 1; 
       
      color = YColer (n); 
      if (color == CLR_NULL)  
	YDrawStringImp ("X", xbeg+xsize/2, ybeg+ysize/2, YColor("black"), YNONE, YNONE); 
      else  
	YDrawRectF (xbeg, ybeg, xsize, ysize, color); 
 
      n++; 
      xbeg = xend; 
    } 
    ybeg = yend; 
  } 
 
  YDrawRectB (xx, yy, ww, hh, YColorV("vc_black")); 
 
  return; 
} 
/*------------------------------YVertString---------------------------------*/ 
int 
YVertString (int x0, int y0, char *string, YT_COLOR color, YT_BOOL drawing) 
{ 
  int   i = 0, x, y; 
  char  bukva[2] = {'\0', '\0'}; 
 
  x = x0 - YStringW ("W") / 2; 
  y = y0 + YStringH ("H"); 
 
  for (i = 0; i < strlen (string); i++) { 
    *&(bukva[0]) = *&(string[i]); 
    if (drawing) 
      YDrawString (bukva, x, y, color); 
    y += YStringH (bukva); 
  } 
 
  return (y - y0); 
} 
/*-----------------------------YDrawVString---------------------------------*/ 
void 
YDrawVString (char *string, int x0, int y0, YT_COLOR color) 
{ 
 
  YVertString (x0, y0, string, color, TRUE); 
 
} 
/*-----------------------------YVertStringH---------------------------------*/ 
int 
YVertStringH (char *string) 
{ 
 
  return (YVertString (0, 0, string, YColor("black"), FALSE)); 
} 
/*-------------------------------YDrawUpRect--------------------------------*/ 
void 
YDrawUpRect (int x, int y, int w, int h) 
{ 
  YDrawLine (x, y, x + w - 1, y, YColorV("vc_white")); 
  YDrawLine (x, y, x, y + h - 1, YColorV("vc_white")); 
  YDrawLine (x + w, y, x + w, y + h, YColorV("vc_black")); 
  YDrawLine (x, y + h, x + w, y + h, YColorV("vc_black")); 
} 
/* ------------------------------YDrawCheck----------------------------------*/ 
void 
YDrawCheck(int x, int y, int w, int h, YT_COLOR color, YT_BOOL flag) 
{ 
  YDrawRectB(x, y, w, h, color); 
  if(flag){ 
    YDrawLine (x+1, y, w+1, y+h-1, color); 
    YDrawLine (x+1, y+h-1, w+1, y, color); 
  } 
} 
/*------------------------------YDrawDownBar--------------------------------*/ 
void 
YDrawDownBar (int x, int y, int w, int h, YT_COLOR color) 
{ 
  YDrawRectF (x, y, w, h, color); 
  YDrawLine (x, y, x + w - 1, y, YColor("black")); 
  YDrawLine (x, y, x, y + h - 1, YColor("black")); 
  YDrawLine (x + w, y, x + w, y + h, YColor("white")); 
  YDrawLine (x, y + h, x + w, y + h, YColor("white")); 
} 
/*-------------------------------YDrawUpBar---------------------------------*/ 
void 
YDrawUpBar (int x, int y, int w, int h, YT_COLOR color) 
{ 
  YDrawRectF (x, y, w, h, color); 
  YDrawLine (x, y, x + w - 1, y, YColor("white")); 
  YDrawLine (x, y, x, y + h - 1, YColor("white")); 
  YDrawLine (x + w, y, x + w, y + h, YColor("black")); 
  YDrawLine (x, y + h, x + w, y + h, YColor("black")); 
} 
/*------------------------------YDrawUpWindow-------------------------------*/ 
void 
YDrawUpWindow (int x, int y, int w, int h, YT_COLOR color, char *caption) 
{ 
  int  h_caption = 20; 
 
  YDrawRectFB (x, y, w, h, color, YColor("black")); 
  YDrawRectB (x + 1, y + 1, w - 2, h - 2, YColor("white")); 
  if (caption != NULL) { 
    YDrawLine (x + 1, y + h_caption, x + w - 1, y + h_caption, YColor("black")); 
    YDrawLine (x + 1, y + h_caption + 1, x + w - 1, y + h_caption + 1, YColor("white")); 
    YDrawRectText (caption, x, y, w, h_caption, YColor("black")); 
  } 
} 
/*------------------------------YDrawDownRect-------------------------------*/ 
void 
YDrawDownRect (int x, int y, int w, int h) 
{ 
  YDrawLine (x, y, x + w - 1, y, YColor("black")); 
  YDrawLine (x, y, x, y + h - 1, YColor("black")); 
  YDrawLine (x + w, y, x + w, y + h, YColorV("white")); 
  YDrawLine (x, y + h, x + w, y + h, YColorV("white")); 
} 
/*-----------------------------YDrawPointer---------------------------------*/ 
void 
YDrawPointer (int direction, int x, int y, YT_COLOR color) 
{ 
  char   *name; 
  if      (direction == YUP)     name = "up>"; 
  else if (direction == YRIGHT)  name = "right>"; 
  else if (direction == YDOWN)   name = "down>"; 
  else if (direction == YLEFT)   name = "left>"; 
  YDrawSymbol (name, x - 4, y - 4, 8, 8, color); 
} 
/*------------------------------YDrawColmap---------------------------------*/ 
void 
YDrawColmap (void *ptr, int xscr, int yscr, int wscr, int hscr) 
{ 
  YT_CMAP *cmap = (YT_CMAP *) ptr; 
 
  switch (DRAW_MODE) { 
  case YMETA: 
    YMetaAdd (YCOLMAP, cmap, xscr, yscr, wscr, hscr, 0, 0, 0, NULL); 
    break; 
  case YREAL: 
    { 
      float   xmult, ymult; 
      float   xscale, yscale; 
      int     i, j, x, y; 
      YT_COLOR color; 
      long    image; 
      int     wreal, hreal; 
 
      if (wscr == 0)  wscr = cmap->ii; 
      if (hscr == 0)  hscr = cmap->jj; 
      wscr--; 
      hscr--; 
 
      YGetMult (&xmult, &ymult); 
      wreal = YInt (wscr * xmult); 
      hreal = YInt (hscr * ymult); 
 
      image = YMakeImage (wreal, hreal); 
      xscale = (float) wreal / (cmap->ii - 1); 
      yscale = (float) hreal / (cmap->jj - 1); 
      for (y = 0; y < hreal; y++) 
	for (x = 0; x < wreal; x++) { 
	  j = YInt (y / yscale); 
	  i = YInt (x / xscale); 
	  color = YColmapGet (cmap, i, j); 
	  YPutPixel (image, x, y, color); 
	} 
      YImagePut (xscr, yscr, image); 
 
    } 
    break; 
  case YPOST: 
    if (wscr == 0)  wscr = cmap->ii; 
    if (hscr == 0)  hscr = cmap->jj; 
    yscr = yscr + hscr; 
    YTransf (&xscr, &yscr); 
    PS_Utils (); 
    PS_Image (xscr, yscr, cmap, wscr, hscr, YColmapGet); 
    break; 
  } 
 
  return; 
} 
/*----------------------------YDrawColmapB----------------------------------*/ 
void 
YDrawColmapB (void *ptr, int x, int y, int w, int h, YT_COLOR b_color) 
{ 
  YT_CMAP *cmap = (YT_CMAP *) ptr; 
 
  YDrawColmap (cmap, x, y, w, h); 
  if (b_color != CLR_NULL) { 
    if (w == 0)  w = cmap->ii; 
    if (h == 0)  h = cmap->jj; 
    YDrawRectB (x, y, w, h, b_color); 
  } 
} 
/*------------------------------YSetXorDraw---------------------------------*/ 
void 
YSetXorDraw (YT_BOOL xor) 
{ 
  switch (DRAW_MODE) { 
  case YMETA: 
    IS->meta_xor = xor; 
    break; 
  case YREAL: 
    YSetXorMode (xor); 
    break; 
  case YPOST: 
    break; 
  } 
 
  return; 
} 
/*------------------------------YDrawXorLine--------------------------------*/ 
void 
YDrawXorLine (int x1, int y1, int x2, int y2, YT_COLOR color) 
{ 
 
  YSetXorDraw (TRUE); 
  YDrawLine (x1, y1, x2, y2, color); 
  YSetXorDraw (FALSE); 
 
} 
/*********************************************************************************/ 
 
/*-------------------------------YPutInCicl----------------------------------*/ 
int 
YPutInCicl (int val, int left, int right) 
{ 
  int  len; 
 
  len = right - left + 1; 
 
  if (val > right) { 
    val = val - right - 1; 
    val = left + val % len; 
  } else if (val < left) { 
    val = left - val - 1; 
    val = right - val % len; 
  } 
 
  return (val); 
} 
/*--------------------------------VN_rab-------------------------------*/ 
char *VN_rab (char *virt_name) 
{ 
  int   i; 
  char *real_name, *real_name_true; 
 
  for (i = 0; TRUE; i++) { 
    if ((i==NUM_VNAMES) || (IS->vnames[i]==NULL))  break; 
    if (YStrCmp(virt_name, (IS->vnames[i])->virt_name)) { 
      real_name = IS->vnames[i]->real_name; 
      real_name_true = VN_rab (real_name); 
      if (real_name_true) return(real_name_true); 
      else                return(real_name); 
    } 
  } 
 
  return(NULL); 
} 
/*--------------------------------VN------------------------------------*/ 
char *VN (char *virt_name) 
{ 
  char *ret; 
 
  ret = VN_rab(virt_name); 
  if (!ret)  YWarningS ("VN", virt_name);  
 
  return(ret); 
} 
/*--------------------------YSetVirtNames-------------------------------*/ 
void 
YSetVirtNames (YT_V_NAME *nm) 
{ 
 
  for ( ; nm->v_name != NULL; nm++)  
     YSetVirtName (nm->v_name, nm->r_name); 
   
  return; 
} 
/*---------------------------YSetiCreate----------------------------*/ 
YT_SETI* 
YSetiCreate () 
{ 
  YT_SETI *seti; 
  int  i; 
 
  YMALLOC (seti, YT_SETI, 1); 
  for (i=0; i<SETI_SIZE; i++) 
    seti->text[i] = NULL; 
 
  return (seti); 
} 
/*---------------------------YSetiPut-------------------------------*/ 
void 
YSetiPut (YT_SETI *seti, char *text, long lpar) 
{ 
  int  i; 
 
  for (i=0; i<SETI_SIZE; i++) { 
    if (seti->text[i] == NULL) continue; 
    if (YStrCmp(seti->text[i], text)) { 
      seti->text[i] = text; 
      seti->lpar[i] = lpar; 
      return; 
    } 
  } 
 
  for (i=0; i<SETI_SIZE; i++) { 
    if (seti->text[i] == NULL) { 
      seti->text[i] = text; 
      seti->lpar[i] = lpar; 
      break; 
    } 
  } 
 
  return; 
} 
/*---------------------------YSetiGet-------------------------------*/ 
long 
YSetiGet (YT_SETI *seti, char *text) 
{ 
  int  i; 
  long lpar/*  = NULL */; 
 
  for (i=0; i<SETI_SIZE; i++) { 
    if (YStrCmp(seti->text[i], text)) { 
      lpar = seti->lpar[i]; 
      return (lpar); 
     /*  break; */ 
    } 
  } 
 
  /* return (lpar); */ 
  YWarningS ("YSetiGet", text); 
  YBeep (); 
  return (YNULL); 
} 
/*--------------------------------YGeti----------------------------------*/ 
long 
YGeti (int id, char *text) 
{ 
  YT_SETI *seti = IS->seti; 
 
 
  if      (FALSE)  {;;;;} 
  else if (YStrCmp(text, "wlong1"))      return(Y_WLONG1); 
  else if (YStrCmp(text, "wlong2"))      return(Y_WLONG2); 
  else if (YStrCmp(text, "wlong3"))      return(Y_WLONG3); 
  else if (YStrCmp(text, "wlong4"))      return(Y_WLONG4); 
/*   else if (YStrCmp(text, "idlast"))      return(Y_IDLAST); */ 
  else if (YStrCmp(text, "yswitch"))     return((long)(MORE->yswitch)); 
  else if (YStrCmp(text, "in_case"))     return(MORE->in_case); 
 
/*   else if (YStrCmp(text, "w_struct"))    return ((long)PWINDOW(id)); */ 
 
/*   else if (YStrCmp(text, "wmode"))       return(Y_WMODE); */ 
 
  else if (YStrCmp(text, "wdisplay"))    return(Y_WDISPLAY); 
  else if (YStrCmp(text, "hdisplay"))    return(Y_HDISPLAY); 
/*   else if (YStrCmp(text, "second_draw")) return(SECOND_DRAW); */ 
  else if (YStrCmp(text, "size_scale"))  return(SIZE_SCALE); 
/*   else if (YStrCmp(text, "ylibstruct"))  return (LP(ylibstruct)); */ 
 
  else  
    return (YSetiGet (seti, text)); 
  
  return (FALSE); 
} 
/*--------------------------------YSeti----------------------------------*/ 
void 
YSeti (int id, char *text, long lpar) 
{ 
 
  YT_SETI *seti = IS->seti; 
 
  if      (FALSE)  {;;;;} 
  else if (YStrCmp(text, "wlong1"))   Y_WLONG1 = lpar; 
  else if (YStrCmp(text, "wlong2"))   Y_WLONG2 = lpar; 
  else if (YStrCmp(text, "wlong3"))   Y_WLONG3 = lpar; 
  else if (YStrCmp(text, "wlong4"))   Y_WLONG4 = lpar; 
/*   else if (YStrCmp(text, "idlast"))   Y_IDLAST = lpar; */ 
  else if (YStrCmp(text, "yswitch"))  MORE->yswitch  = (char*)lpar; 
  else if (YStrCmp(text, "in_case"))  MORE->in_case  = lpar; 
 
  else    { 
    YSetiPut (seti, text, lpar); 
  } 
 
  id++; 
  return; 
} 
/*--------------------------------YGet----------------------------------*/ 
long 
YGet (char *text) 
{ 
 
  return(YGeti(Y_IDLAST, text)); 
 
} 
/*--------------------------------YSet----------------------------------*/ 
void 
YSet (char *text, long lpar) 
{ 
 
  YSeti(Y_IDLAST, text, lpar); 
  return; 
 
} 
/*-----------------------------YColorV-------------------------------------*/ 
YT_COLOR YColorV (char *virt_name) 
{ 
 
  return(YColor(VN(virt_name))); 
 
} 
/*------------------------------------YSetPname----------------------------------*/ 
YT_BOOL YSetPname(char *proc_name, YT_PFUNC proc) 
{ 
  int i; 
 
  for (i=0; TRUE; i++) { 
    if (YStrCmp(IS->pnames[i].proc_name, "")) break; 
    if (YStrCmp(IS->pnames[i].proc_name, proc_name)) break; 
    if (i == PNAMES_MAX) return (FALSE); 
  } 
 
  strcpy (IS->pnames[i].proc_name, proc_name); 
  IS->pnames[i].proc = proc; 
 
  return (TRUE); 
} 
/*----------------------------------YSetPnames--------------------------------*/ 
void YSetPnames(AT_PNAME1 *pnames) 
{ 
  AT_PNAME1 *pn; 
 
  for (pn=pnames; pn->proc_name!=NULL; pn++)  
    YSetPname (pn->proc_name, pn->proc); 
 
  return; 
} 
/*-----------------------------------YProcByName---------------------------------*/ 
YT_PFUNC YProcByName(char *proc_name) 
{ 
  int  i; 
 
  for (i=0; TRUE; i++) { 
    if (i == PNAMES_MAX) { 
      YWarningS ("YProcByName", proc_name); 
      /* return (DITTO); */ 
      return (NULL); 
    } 
    if (YStrCmp(IS->pnames[i].proc_name, proc_name)) return (IS->pnames[i].proc); 
  } 
 
} 
/*------------------------------YSetVirtName-----------------------------*/ 
YT_BOOL 
YSetVirtName (char *virt_name, char *real_name) 
{ 
  int i; 
 
  for (i = 0; TRUE; i++) { 
    if (i == NUM_VNAMES)  return(FALSE); 
    if (IS->vnames[i] == NULL) { 
      IS->vnames[i] = (YT_VNAME*)malloc(sizeof(YT_VNAME)); 
      break; 
    } 
    if (YStrCmp(virt_name, (IS->vnames[i])->virt_name)) break; 
  } 
 
  strcpy((IS->vnames[i])->virt_name, virt_name); 
  strcpy((IS->vnames[i])->real_name, real_name); 
   
  return(TRUE); 
  } 
/*------------------------------YTransf-------------------------------------*/ 
void 
YTransf (int *px, int *py) 
{ 
  int  x = *px; 
  int  y = *py; 
 
  *px = x; 
  y = IS->transf_ymax - (y - IS->transf_ymin); 
  *py = y; 
 
} 
/*-------------------------------YCalcRects---------------------------------*/ 
void 
YCalcRects (int i, int j, YT_RECT *rect, int x0,int y0,int w0,int h0, int ii0,int jj0) 
{ 
  static int x, y, w, h, ii, jj; 
  static int sizew, sizeh, pushw, pushh; 
 
  if (i == 0 && j == 0) { 
    x = x0; 
    y = y0; 
    w = w0; 
    h = h0; 
    ii = ii0; 
    jj = jj0; 
    sizew = w / 10; 
    sizeh = h / 10; 
    pushw = (w - (ii + 1) * sizew) / ii; 
    pushh = (h - (jj + 1) * sizeh) / jj; 
    return; 
  } 
  rect->x = x + i * sizew + (i - 1) * pushw; 
  rect->y = y + j * sizeh + (j - 1) * pushh; 
  rect->w = pushw; 
  rect->h = pushh; 
 
} 
/*--------------------------------YWaitBegin--------------------------------*/ 
YT_BOOL 
YWaitBegin (YT_PFUNC wait_proc, int imax, char *string) 
{ 
/*   YT_PFUNC  wait_proc = WAIT; */ 
  static char str[90], str_imax[90]; 
 
  MORE->WAIT_flag++; 
  if (MORE->WAIT_flag != 1)  return (FALSE); 
 
  strcpy (str, string); 
  sprintf(str_imax, "  (%d)", imax); 
  strcat (str, str_imax); 
 
  MORE->WAIT_string = str; 
  MORE->WAIT_cancel = FALSE; 
  IS->WAIT_imax = imax; 
  IS->WAIT_i = 1; 
  IS->WAIT_done = 0; 
  IS->WAIT_doneold = 0; 
 
  if (Y_WMODE) { 
    YMouseForm ("wait"); 
    YGetContext (&IS->WAIT_save); 
    YOpenDialog (&(IS->WAIT_id), wait_proc, "Please  WAIT !", 0,0,0,0, 0,0,0,0, CLR_DEF, TRUE); 
    YSetContext (&IS->WAIT_save); 
    ID_DIALOG = IS->WAIT_id; 
  } else { 
    /* fprintf (Y_STDERR, "WAIT: %s : ", MORE->WAIT_string); */ 
  } 
    sprintf  (Y_STR, "WAIT: %s : \n", MORE->WAIT_string); 
    YPrintDo (Y_STR); 
 
  return (TRUE); 
} 
/*-------------------------------YWaitEnd-----------------------------------*/ 
void 
YWaitEnd () 
{ 
 
  MORE->WAIT_flag--; 
  if (MORE->WAIT_flag != 0)  return; 
 
/*   if (YGet("wmode")) { */ 
  if (Y_WMODE) { 
    YMouseForm ("arrow"); 
    YCloseDialog (IS->WAIT_id); 
    ID_DIALOG = ID_NULL; 
  } else { 
    /* fprintf (Y_STDERR, "\n"); */ 
  } 
    sprintf  (Y_STR, "\n"); 
    YPrintDo (Y_STR); 
 
  MORE->WAIT_flag = FALSE; 
   
  return; 
} 
/*-------------------------------YWaitSend----------------------------------*/ 
YT_BOOL 
YWaitSend (YT_BOOL flag) 
{ 
  int done; 
  /* YT_SVMSG msg; */ 
  int done_step = 10; 
 
  if (!flag) return (TRUE); 
 
  /* if (YCheckMin(&msg))  YDispatchMsg (&msg); */ 
  YCheckSystem (); 
 
  IS->WAIT_done = (int) ((float) IS->WAIT_i++ / IS->WAIT_imax * 100); 
 
/*   if (YGet("wmode")) { */ 
  if (Y_WMODE) { 
    YSend (IS->WAIT_id, YREDRAW, IS->WAIT_done, 0, (long)(MORE->WAIT_string), 0); 
  } else { 
    done = IS->WAIT_done/done_step; 
    if (IS->WAIT_doneold != done) { 
      sprintf  (Y_STR, "WAIT (%s) : done = %d %% \n", YTimeStringMin(), done*done_step); 
      YPrintDo (Y_STR); 
      IS->WAIT_doneold = done; 
    } 
  } 
 
  if (MORE->WAIT_cancel) return (FALSE);  
  else                  return (TRUE); 
} 
/*-------------------------------YMetaAdd-----------------------------------*/ 
void 
YMetaAdd (int type, void *ptr, int i1, int i2, int i3, int i4, 
	  YT_COLOR color, int i5, int i6, char *text) 
{ 
  YT_SCROBJ *o; 
  YT_META *meta = IS->meta; 
  int     i; 
  char   *name = IS->draw_oname; 
 
  for (i = 0; i < meta->size; i++) 
    if (((meta->ptr)[i]) == NULL) 
      break; 
  if (i == meta->size)  YError ("META_SIZE"); 
 
  o = (YT_SCROBJ *) malloc (sizeof (YT_SCROBJ)); 
 
  if (!o) 
    YError ("malloc YT_SCROBJ"); 
  o->type = type; 
  o->ptr = ptr; 
  o->i1 = i1; 
  o->i2 = i2; 
  o->i3 = i3; 
  o->i4 = i4; 
  o->color = color; 
  o->i5 = i5; 
  o->i6 = i6; 
  o->status = YNEW; 
  o->xor = IS->meta_xor; 
 
  if (text != NULL) { 
    o->text = (char *) malloc (strlen (text) + 1); 
    strcpy (o->text, text); 
  } else 
    o->text = NULL; 
 
  if (!YStrCmp (name, "")) { 
    o->oname = (char *) malloc (strlen (name) + 1); 
    strcpy (o->oname, name); 
  } else 
    o->oname = NULL; 
 
  IS->meta->ptr[i] = o; 
 
  return; 
} 
/*----------------------------PS_Setcolor-----------------------------------*/ 
void 
PS_Setcolor (YT_COLOR color) 
{ 
  int     r, g, b; 
  float   red, green, blue; 
 
  YColorToRGB (color, &r, &g, &b); 
  red = (float) r / 255; 
  green = (float) g / 255; 
  blue = (float) b / 255; 
  fprintf (POST_FILE, "%f %f %f setcolor \n", red, green, blue); 
 
} 
/*-----------------------------PS_Stroke------------------------------------*/ 
void 
PS_Stroke () 
{ 
 
  fprintf (POST_FILE, "%s \n", "stroke"); 
 
} 
/*-----------------------------YCrossRect-----------------------------------*/ 
YT_BOOL 
YCrossRect (int left1, int top1, int right1, int bottom1, 
	    int left2, int top2, int right2, int bottom2) 
{ 
  if ((top1 < top2 && bottom1 < top2) || (top2 < top1 && bottom2 < top1)) 
    return (FALSE); 
  if ((left1 < left2 && right1 < left2) || (left2 < left1 && right2 < left1)) 
    return (FALSE); 
 
  return (TRUE); 
} 
/*------------------------------PS_Utils------------------------------------*/ 
void 
PS_Utils () 
{ 
 
  fprintf (POST_FILE, "%% define 'colorimage' if it isn't defined\n"); 
  fprintf (POST_FILE, "%%   ('colortogray' and 'mergeprocs' come from xwd2ps\n"); 
  fprintf (POST_FILE, "%%     via xgrab)\n"); 
  fprintf (POST_FILE, "/colorimage where   %% do we know about 'colorimage'?\n"); 
  fprintf (POST_FILE, "  { pop }           %% yes: pop off the 'dict' returned\n"); 
  fprintf (POST_FILE, "  {                 %% no:  define one\n"); 
  fprintf (POST_FILE, "    /colortogray {  %% define an RGB->I function\n"); 
  fprintf (POST_FILE, "      /rgbdata exch store    %% call input 'rgbdata'\n"); 
  fprintf (POST_FILE, "      rgbdata length 3 idiv\n"); 
  fprintf (POST_FILE, "      /npixls exch store\n"); 
  fprintf (POST_FILE, "      /rgbindx 0 store\n"); 
  fprintf (POST_FILE, "      /grays npixls string store  %% str to hold the result\n"); 
  fprintf (POST_FILE, "      0 1 npixls 1 sub {\n"); 
  fprintf (POST_FILE, "        grays exch\n"); 
  fprintf (POST_FILE, "        rgbdata rgbindx       get 20 mul    %% Red\n"); 
  fprintf (POST_FILE, "        rgbdata rgbindx 1 add get 32 mul    %% Green\n"); 
  fprintf (POST_FILE, "        rgbdata rgbindx 2 add get 12 mul    %% Blue\n"); 
  fprintf (POST_FILE, "        add add 64 idiv      %% I = .5G + .31R + .18B\n"); 
  fprintf (POST_FILE, "        put\n"); 
  fprintf (POST_FILE, "        /rgbindx rgbindx 3 add store\n"); 
  fprintf (POST_FILE, "      } for\n"); 
  fprintf (POST_FILE, "      grays\n"); 
  fprintf (POST_FILE, "    } bind def\n\n"); 
 
  fprintf (POST_FILE, "    %% Utility procedure for colorimage operator.\n"); 
  fprintf (POST_FILE, "    %% This procedure takes two procedures off the\n"); 
  fprintf (POST_FILE, "    %% stack and merges them into a single procedure.\n\n"); 
 
  fprintf (POST_FILE, "    /mergeprocs { %% def\n"); 
  fprintf (POST_FILE, "      dup length\n"); 
  fprintf (POST_FILE, "      3 -1 roll\n"); 
  fprintf (POST_FILE, "      dup\n"); 
  fprintf (POST_FILE, "      length\n"); 
  fprintf (POST_FILE, "      dup\n"); 
  fprintf (POST_FILE, "      5 1 roll\n"); 
  fprintf (POST_FILE, "      3 -1 roll\n"); 
  fprintf (POST_FILE, "      add\n"); 
  fprintf (POST_FILE, "      array cvx\n"); 
  fprintf (POST_FILE, "      dup\n"); 
  fprintf (POST_FILE, "      3 -1 roll\n"); 
  fprintf (POST_FILE, "      0 exch\n"); 
  fprintf (POST_FILE, "      putinterval\n"); 
  fprintf (POST_FILE, "      dup\n"); 
  fprintf (POST_FILE, "      4 2 roll\n"); 
  fprintf (POST_FILE, "      putinterval\n"); 
  fprintf (POST_FILE, "    } bind def\n\n"); 
 
  fprintf (POST_FILE, "    /colorimage { %% def\n"); 
  fprintf (POST_FILE, "      pop pop     %% remove 'false 3' operands\n"); 
  fprintf (POST_FILE, "      {colortogray} mergeprocs\n"); 
  fprintf (POST_FILE, "      image\n"); 
  fprintf (POST_FILE, "    } bind def\n"); 
  fprintf (POST_FILE, "  } ifelse          %% end of 'false' case\n"); 
  fprintf (POST_FILE, "\n\n\n"); 
 
} 
/*------------------------------PS_Image------------------------------------*/ 
void 
PS_Image (int x, int y, void *ptr, int ii, int jj, 
	  YT_COLOR (*get_color) (void *, int, int)) 
{ 
  int     lwidth; 
  int     i, j, r, g, b; 
 
  PS_Stroke (); 
  fprintf (POST_FILE, "%s \n", "gsave"); 
  fprintf (POST_FILE, "%d %d translate\n", x, y); 
  fprintf (POST_FILE, "%d %d scale\n", ii, jj); 
 
  fprintf (POST_FILE, "/pix %d string def\n\n", ii * 3); 
  fprintf (POST_FILE, "%d %d 8 [%d 0 0 -%d 0 %d] ", ii, jj, ii, jj, jj); 
  fprintf (POST_FILE, "\n"); 
  fprintf (POST_FILE, "{currentfile pix readhexstring pop}\n"); 
  fprintf (POST_FILE, "false 3 colorimage\n\n"); 
 
  for (j = 0; j < jj; j++) { 
    fprintf (POST_FILE, "\n"); 
    lwidth = 0; 
    for (i = 0; i < ii; i++) { 
      YColorToRGB (get_color (ptr, i, j), &r, &g, &b); 
      fprintf (POST_FILE, "%02X%02X%02X", r, g, b); 
      lwidth += 6; 
      if (lwidth > 70) { 
	putc ('\n', POST_FILE); 
	lwidth = 0; 
      } 
    } 
  } 
 
  fprintf (POST_FILE, "\n"); 
  PS_Stroke (); 
  fprintf (POST_FILE, "%s \n", "grestore"); 
 
  return; 
} 
/*----------------------------YColmapGet------------------------------------*/ 
YT_COLOR  
YColmapGet (void *ptr, int i, int j) 
{ 
  YT_COLOR color; 
  YT_CMAP *cmap = (YT_CMAP *) ptr; 
 
  color = (cmap->p)[i * (cmap->jj) + j]; 
 
  return (color); 
} 
/*-------------------------------YMetaDel-----------------------------------*/ 
void 
YMetaDel (YT_META *meta, int i) 
{ 
  YT_SCROBJ *o; 
 
  o = (meta->ptr)[i]; 
  if (o == NULL)  return; 
  if (o->text)    free (o->text); 
 
  ((meta->ptr)[i]) = NULL; 
  return; 
} 
/*------------------------------YMetaCreate---------------------------------*/ 
void * 
YMetaCreate (int size) 
{ 
  YT_META *meta; 
  int     i; 
 
  meta = (YT_META *) malloc (sizeof (YT_META)); 
  if (!meta) 
    YError ("YScrCreate"); 
 
  meta->ptr = (YT_SCROBJ **) malloc (size * sizeof (YT_SCROBJ *)); 
  for (i = 0; i < size; i++) 
    (meta->ptr)[i] = NULL; 
 
  meta->size = size; 
  meta->save_fname = "svisor.eps"; 
  meta->save_xx = 100; 
  meta->save_yy = 80; 
 
  return (meta); 
} 
/*--------------------------------YMetaInit---------------------------------*/ 
void 
YMetaInit (void *ptr, int x, int y, int w, int h) 
{ 
  YT_META *meta = (YT_META *) ptr; 
  int     i; 
 
  for (i = 0; i < meta->size; i++) 
    YMetaDel (meta, i); 
 
  meta->x = x; 
  meta->y = y; 
  meta->w = w; 
  meta->h = h; 
 
  meta->xold = 0; 
  meta->yold = 0; 
  meta->wold = 0; 
  meta->hold = 0; 
 
  return; 
} 
/*---------------------------YMetaDrawTo------------------------------------*/ 
void 
YMetaDrawTo (void *ptr, int id_to, YT_BOOL all, int id_from) 
{ 
  YT_META *meta = (YT_META *) ptr; 
 
  YSend (id_to, YMETADRAW, all, id_from, (long)(meta), 0); 
 
  return;  
} 
/*------------------------------YMetaToPS-----------------------------------*/ 
void 
YMetaToPS (void *ptr) 
{ 
  YT_META *meta = (YT_META *) ptr; 
 
  IS->meta_savemode = DRAW_MODE; 
  DRAW_MODE = YPOST; 
 
  PS_Begin (meta->save_fname, "Times-Roman"); 
  IS->transf_ymin = meta->y; 
  IS->transf_ymax = meta->y + meta->h; 
  YMetaDrawAll (meta, TRUE); 
  PS_End (); 
 
  DRAW_MODE = IS->meta_savemode; 
  return; 
} 
/*------------------------------YMetaW--------------------------------------*/ 
int 
YMetaW (void *ptr) 
{ 
  YT_META *meta = (YT_META *) ptr; 
 
  return (meta->w); 
} 
/*------------------------------YMetaToScr----------------------------------*/ 
void 
YMetaToScr (void *ptr, int x, int y, int w, int h, YT_BOOL all, YT_COLOR color) 
{ 
  YT_META *meta = (YT_META *) ptr; 
  float   x_scale; 
  float   y_scale; 
  float   old_xmult = KERN->x_mult; 
  float   old_ymult = KERN->y_mult; 
  int     x_orig, y_orig; 
 
  all = (all || YMetaIfAll (meta, x, y, w, h)); 
 
  if (all) 
    YDrawRectF (meta->xold, meta->yold, meta->wold, meta->hold, color); 
  meta->xold = x; 
  meta->yold = y; 
  meta->wold = w; 
  meta->hold = h; 
 
  /* YSetCliping(TRUE,x,y,w,h); */ 
 
  x_scale = (float) w / meta->w; 
  y_scale = (float) h / meta->h; 
  YSetMult (x_scale, y_scale); 
  YGetOrigin (&x_orig, &y_orig); 
  YSetOrigin (x_orig + x, y_orig + y); 
 
  YMetaDrawAll (meta, all); 
 
  YSetOrigin (x_orig, y_orig); 
  YSetMult (old_xmult, old_ymult); 
 
  /* YSetCliping(FALSE,0,0,0,0); */ 
  return; 
} 
/*------------------------------YMetaIfAll----------------------------------*/ 
YT_BOOL 
YMetaIfAll (YT_META *meta, int x, int y, int w, int h) 
{ 
  int     i; 
  YT_SCROBJ *o; 
 
  if (meta->xold != x || meta->yold != y || meta->wold != w || meta->hold != h) 
    return (TRUE); 
 
  for (i = 0; i < meta->size; i++) { 
    o = (meta->ptr)[i]; 
    if (o == NULL)  continue; 
    if (o->xor)     continue; 
  } 
 
  return (FALSE); 
} 
/*-----------------------------YMetaDrawAll---------------------------------*/ 
void 
YMetaDrawAll (YT_META *meta, YT_BOOL all) 
{ 
  YT_SCROBJ *o; 
  int     i; 
 
  YMetaDelBackups (meta); 
 
  for (i = 0; i < meta->size; i++) { 
    o = (meta->ptr)[i]; 
    if (o == NULL)  continue; 
    if (all || o->status == YNEW) { 
      YMetaDrawOne (o); 
      o->status = YNORMAL; 
    } 
  } 
 
  YSetXorDraw (FALSE); 
  return; 
} 
/*----------------------------YMetaDelBackups-------------------------------*/ 
void 
YMetaDelBackups (YT_META *meta) 
{ 
  YT_SCROBJ *o; 
  int     i; 
 
  for (i = 0; i < meta->size; i++) { 
    o = (meta->ptr)[i]; 
    if (o == NULL)  continue; 
    if (o->status != YBACKUP) 
      continue; 
    if (o->xor) 
      YMetaDrawOne (o); 
    YMetaDel (meta, i); 
  } 
 
  return; 
} 
/*------------------------------YMetaDrawOne--------------------------------*/ 
void 
YMetaDrawOne (YT_SCROBJ *o) 
{ 
 
  YSetXorDraw (o->xor); 
  switch (o->type) { 
  case YLINE: 
    YDrawLine (o->i1, o->i2, o->i3, o->i4, o->color); 
    break; 
  case YARCF: 
    YDrawArcF (o->i1, o->i2, o->i3, o->i4, o->i5, o->i6, o->color); 
    break; 
  case YARCB: 
    YDrawArcB (o->i1, o->i2, o->i3, o->i4, o->i5, o->i6, o->color); 
    break; 
  case YPOLYF: 
    YDrawPolyF (o->i1, (int*)(o->ptr), o->color); 
    break; 
  case YSTRING: 
    YDrawString (o->text, o->i1, o->i2, o->color); 
    break; 
  case YRECTF: 
    YDrawRectF (o->i1, o->i2, o->i3, o->i4, o->color); 
    break; 
  case YCOLMAP: 
    YDrawColmap ((YT_CMAP *) (o->ptr), o->i1, o->i2, o->i3, o->i4); 
    break; 
  default:;;; 
  } 
 
  return; 
} 
/*----------------------------YMetaMakeBackups------------------------------*/ 
void 
YMetaMakeBackups (YT_META *meta, char *name) 
{ 
  int     i; 
  YT_SCROBJ *o; 
 
  if (YStrCmp (name, "")) 
    return; 
 
  for (i = 0; i < meta->size; i++) { 
    o = (meta->ptr)[i]; 
    if (o == NULL)  continue; 
    if (o->oname == NULL)  continue; 
    if (!YStrCmp (o->oname, name))  continue; 
    o->status = YBACKUP; 
  } 
 
} 
/*------------------------------YMetaBegin----------------------------------*/ 
void 
YMetaBegin (void *ptr, char *oname) 
{ 
  YT_META *meta = (YT_META *) ptr; 
 
  IS->meta = meta; 
  IS->meta_savemode = DRAW_MODE; 
  DRAW_MODE = YMETA; 
 
  strcpy (IS->draw_oname, oname); 
  YMetaMakeBackups (meta, oname); 
 
  YGetOrigin (&(meta->old_x0), &(meta->old_y0)); 
  YGetMult (&(meta->old_x_mult), &(meta->old_y_mult)); 
  YSetOrigin (meta->x, meta->y); 
  YSetMult (1, 1); 
 
  return; 
} 
/*------------------------------YMetaEnd------------------------------------*/ 
void 
YMetaEnd () 
{ 
  YT_META *meta = IS->meta; 
 
  strcpy (IS->draw_oname, ""); 
  IS->meta = NULL; 
  DRAW_MODE = IS->meta_savemode; 
 
  YSetOrigin (meta->old_x0, meta->old_y0); 
  YSetMult (meta->old_x_mult, meta->old_y_mult); 
 
  return; 
} 
/*---------------------------YMetaGetInfo-----------------------------------*/ 
char * 
YMetaGetInfo (void *void_meta, int *xx, int *yy) 
{ 
  YT_META *meta = (YT_META*) void_meta; 
 
  *xx = meta->save_xx; 
  *yy = meta->save_yy; 
 
  return (meta->save_fname); 
} 
/*---------------------------YMetaPutInfo-----------------------------------*/ 
void 
YMetaPutInfo (void *void_meta, char *save_fname, int xx, int yy) 
{ 
  YT_META *meta = (YT_META*) void_meta; 
 
  meta->save_xx = xx; 
  meta->save_yy = yy; 
  meta->save_fname = save_fname; 
  
 return; 
} 
/*-----------------------------PS_Begin-------------------------------------*/ 
void 
PS_Begin (char *fname, char *fontname) 
{ 
  /* char *fontname="Times-Italic"; */ 
  if ((POST_FILE = fopen (fname, "wb")) == NULL) 
    YError ("fopen--SVISOR.PS"); 
 
  fprintf (POST_FILE, "%%!PS-Adobe-2.0 EPSF-2.0\n"); 
  fprintf (POST_FILE, "%%%%Title: %s\n", "pstest.ps"); 
  fprintf (POST_FILE, "%%%%Creator: \n"); 
  fprintf (POST_FILE, "%%%%Pages: 1\n"); 
  fprintf (POST_FILE, "%%%%DocumentFonts:\n"); 
  fprintf (POST_FILE, "%%%%EndComments\n"); 
  fprintf (POST_FILE, "%%%%EndProlog\n\n"); 
  fprintf (POST_FILE, "%%%%Page: 1 1\n\n"); 
 
  fprintf (POST_FILE, "%% remember original state\n"); 
  fprintf (POST_FILE, "/origstate save def\n\n"); 
  fprintf (POST_FILE, "%% build a temporary dictionary\n"); 
  fprintf (POST_FILE, "20 dict begin\n\n"); 
 
  fprintf (POST_FILE, "/%s findfont \n", fontname); 
  fprintf (POST_FILE, "15 scalefont \n"); 
  fprintf (POST_FILE, "setfont \n"); 
 
  fprintf (POST_FILE, "/DeviceRGB \n"); 
  fprintf (POST_FILE, "setcolorspace \n"); 
 
} 
/*-----------------------------PS_Showpage----------------------------------*/ 
void 
PS_Showpage () 
{ 
 
  fprintf (POST_FILE, "%s \n", "showpage"); 
 
} 
/*------------------------------PS_End--------------------------------------*/ 
void 
PS_End () 
{ 
 
  fprintf (POST_FILE, "\n"); 
  PS_Stroke (); 
 
  PS_Showpage (); 
  fprintf (POST_FILE, "end\n"); 
  fprintf (POST_FILE, "origstate restore\n"); 
 
} 
/*----------------------------YColmapCreate---------------------------------*/ 
void   * 
YColmapCreate () 
{ 
  YT_CMAP *cmap; 
 
  if (!(cmap = (YT_CMAP *) malloc (sizeof (YT_CMAP)))) 
            return (NULL); 
  cmap->ii = 0; 
  cmap->jj = 0; 
  cmap->p = NULL; 
 
  return ((void *) cmap); 
} 
/*-----------------------------YColmapInit----------------------------------*/ 
void 
YColmapInit (void *ptr, int ii, int jj) 
{ 
  YT_CMAP *cmap = (YT_CMAP *) ptr; 
 
  cmap->p = (YT_COLOR *) malloc (ii * jj * sizeof (YT_COLOR)); 
  if (!cmap->p) 
    YError ("hhgd"); 
 
  cmap->ii = ii; 
  cmap->jj = jj; 
 
} 
/*----------------------------YColmapPut------------------------------------*/ 
void 
YColmapPut (void *ptr, int i, int j, YT_COLOR color) 
{ 
  YT_CMAP *cmap = (YT_CMAP *) ptr; 
 
  (cmap->p)[i * (cmap->jj) + j] = color; 
 
} 
/*---------------------------YColmapBeginDraw-------------------------------*/ 
void 
YColmapBeginDraw (void *ptr, int x0, int y0, YT_COLOR bgcolor) 
{ 
  YT_CMAP *cmap = (YT_CMAP *) ptr; 
  int     i, j; 
 
  cmap->x0 = x0; 
  cmap->y0 = y0; 
 
  /* if (!cmap) return; */ 
 
  IS->hand_savemode = DRAW_MODE; 
  IS->hand_cmap = cmap; 
  DRAW_MODE = YHAND; 
 
  for (i = 0; i < cmap->ii; i++) 
  for (j = 0; j < cmap->jj; j++) 
    YColmapPut (cmap, i, j, bgcolor); 
 
  return; 
} 
/*---------------------------YColmapDrawPixel-------------------------------*/ 
void 
YColmapDrawPixel (int x, int y, YT_COLOR color) 
{ 
  YT_CMAP *cmap = IS->hand_cmap; 
 
  if (DRAW_MODE != YHAND) { 
    /* YPaintPixel (x, y, color); */  /* !!?? */ 
    return; 
  } 
 
  x += cmap->x0; 
  y += cmap->y0; 
 
  if (!(x >= 0 && y >= 0 && x < cmap->ii && y < cmap->jj)) 
    return; 
 
/* OUTD(3); */ 
  YColmapPut (cmap, x, y, color/* YColor("red" )*/); 
/* OUTD(4); */ 
 
  return; 
} 
/*----------------------------YColmapEndDraw--------------------------------*/ 
void 
YColmapEndDraw () 
{ 
 
  DRAW_MODE = IS->hand_savemode; 
 
} 
/*---------------------------------------------------------------------------*/ 
 
 
#endif 
#if  defined(D_WIND) 
/**************************************************************************** 
 *                                                                          * 
 *                          WIND WIND WIND WIND WIND                        * 
 *                                                                          * 
 **************************************************************************** 
 */  
 
/* #include "y_open.h"   */
/* #include "y_more.h"   */
#include "y_zone.h" 
 
#if defined(FLTK) /*-------------------------FLTK FLTK---------------------*/  

#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Single_Window.H>
#include <FL/Fl_Menu_Bar.H>
#include <FL/fl_draw.H>
#include <FL/Fl_Box.H>                                                        
#include <FL/Fl_Input.H>                                                      
#include <FL/Fl_Button.H>                                                     
#include <FL/Fl_Return_Button.H>                                              
#include <FL/Fl_Toggle_Button.H>
#include <FL/Fl_Menu_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Input.H>
#include <FL/x.H>
#include <FL/Fl_Slider.H>                                                       
#include <FL/Fl_Scrollbar.H>                                                    
#include <FL/Fl_Clock.H>                                                        
#include <FL/fl_file_chooser.H>                                                 
#include <FL/Fl_Hold_Browser.H>

extern Fl_Window *fltkwin;

/* #define GDI_FLTK  */
#define FLTK_CONTROLS 

#endif      /*--------------------------------FLTK FLTK--------------------*/  

long     LIST_RAB (PFUNC_PAR); 
long     choice_colors (PFUNC_PAR); 
 
/***************************************************************************/ 
 
/*--------------------------------YInitWIND--------------------------------*/ 
void   
YInitWIND () 
{ 
  YT_PFUNC   wl_proc = YWL_MAIN; 
 
  YInitMORE (); 
 
  YWndCreate (&(Y_WLPROC), wl_proc, NULL, 0,0,0,0); 
 
  MORE->WAIT_flag = 0; 
 
  YSetDialog (DIALOG); 
 
  return; 
} 
/*-----------------------------YWL_MAIN--------------------------------------*/ 
long 
YWL_MAIN (PFUNC_VAR) 
{ 
  YT_V_NAME names[] = { 
    { "vc_black",        "black"        },    
    { "vc_white",        "white"        },       
    { "vc_lightfill",    "silver"       },   
    { "vc_darkfill",     "aqua"         },    
    { "vc_control",      "vc_darkfill"  },   
    { "vc_draw_strings", "vc_control"   },  
    { "vc_push",         "vc_control"   },    
    { "vc_check",        "vc_control"   } ,    
    { "vc_edit",         "vc_control"   } ,    
    { "vc_list",         "vc_control"   },    
    { "vc_decimal",      "vc_control"   },    
    { "vc_ipush",        "vc_control"   },    
    { "vc_combo",        "vc_control"   },    
    { "vc_defcolor",     "vc_lightfill" },   
    { NULL } 
  }; 
 
 
  switch (message) { 
  case YGETSIZE: 
    MYSIZE_IS (100,100); 
    break; 
  case YCREATE: 
    YSetVirtNames (names);  
    break; 
  case YKEYWORD1: 
    { 
      YT_PFUNC proc = WNDI(mes1)->proc; 
      YT_COLOR def_color; 
       
      if      (proc == PUSH)     def_color = YColorV("vc_push");  
      else if (proc == CHECK)    def_color = YColorV("vc_check"); 
      else if (proc == EDIT)     def_color = YColorV("vc_edit"); 
      else if (proc == LIST)     def_color = YColorV("vc_list"); 
      else if (proc == BOOK)     def_color = YColorV("vc_lightfill"); 
      else if (proc == DECIMAL)  def_color = YColorV("vc_decimal"); 
      else if (proc == RAMKA)    def_color = YColorV("vc_control"); 
      else if (proc == BUTTS)    def_color = YColorV("vc_control"); 
      else if (proc == IPUSH)    def_color = YColorV("vc_ipush"); 
      else if (proc == COMBO)    def_color = YColorV("vc_combo"); 
      else    
	def_color = YColorV("vc_defcolor"); 
       
      return (def_color); 
    } 
  case YOPEN: 
  case YDRAW: 
    YDrawRectFB (0, 0, WND->w,   WND->h,  YColor("white"), YColor("black")); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: ;;;; 
  } 
 
  RETURN_TRUE; 
} 
/*-------------------------------DIALOG-------------------------------------*/ 
long 
DIALOG (PFUNC_VAR) 
{ 
#define  FLAG  (U->flag) 
  static int x0, y0, x, y, w, h; 
  YT_COLOR   col_caption, col_border, col_black, col_window; 
  int  wndx, wndy, wndw, wndh; 
  static int  hcap, wb; 
 
  typedef struct { 
    YT_BOOL    flag; 
    int        focus, hWnd ; 
    long       image; 
    YT_DEFWND  *wnd;  
  } YT_USR; 
 
  switch (message) { 
  case YCREATE: /* !!! */ 
    break; 
  case YGETSIZE: 
    MYSIZE_IS (W_DLG + 2 * DLG_WB, H_DLG + DLG_HCAP + 3 * DLG_WB); 
    break; 
  case YOPEN: 
    U_MALLOC; 
    U->wnd = (YT_DEFWND*) Y_WLONG1; 
    FLAG      = TRUE; 
    U->image  = YImageGet (0, 0, WND->w, WND->h); 
    U->focus  = YGetFocus (); 
    YKillFocus (); 
  case YDRAW: 
    col_border  = YColor("red"); 
    col_caption = YColor("yellow"); 
    col_black   = YColor("black"); 
    col_window  = YColor("lime"); 
    w = WND->w; 
    h = WND->h; 
    if (FLAG) { 
      hcap = DLG_HCAP; 
      wb   = DLG_WB; 
    } else { 
      hcap = 0; 
      wb   = 0; 
    } 
 
    wndx = wb; 
    wndy = 2 * wb + hcap; 
    wndw = w - 2 * wb; 
    wndh = h - 3 * wb - hcap; 
 
    YSetCliping (TRUE, wndx, wndy, wndw, wndh);  
    YWnd (&U->hWnd, (U->wnd)->proc, "", wndx, wndy, wndw, wndh, 
	  (U->wnd)->long1, (U->wnd)->long2, (U->wnd)->long3, (U->wnd)->long4, col_window); 
    YSetCliping (FALSE, 0,0,0,0);  
 
    if (FLAG) { 
      YDrawRectF (1, 1, w - 2, 2 * wb + hcap, col_caption); 
      YDrawRectFB (0, 0, wb, h,         col_border,  col_black); 
      YDrawRectFB (w - wb, 0, wb, h,    col_border,  col_black); 
      YDrawRectFB (0, 0, w, wb,         col_border,  col_black); 
      YDrawRectFB (0, h - wb, w, wb,    col_border,  col_black); 
      YDrawRectFB (0, wb + hcap, w, wb, col_border,  col_black); 
      YDrawRectText (WND->name, 1, 1, w - 2, 2 * wb + hcap, col_black); 
    } 
    break; 
  case YLMOUSEDOWN: 
    x0 = mes1; 
    y0 = mes2; 
    x = 0; 
    y = 0; 
    YGoto (YDRAWITEM, 0, 0, 0, 0); 
    break; 
  case YLMOUSEDRAG: 
    YGoto (YDRAWITEM, 0, 0, 0, 0); 
    x = mes1 - x0; 
    y = mes2 - y0; 
    YGoto (YDRAWITEM, 0, 0, 0, 0); 
    break; 
  case YLMOUSEUP: 
    YGoto (YDRAWITEM, 0, 0, 0, 0); 
    YGoto (YKEYWORD1, 0, 0, 0, 0); 
    break; 
  case YDRAWITEM: 
    YSetXorDraw (TRUE); 
    YDrawRectB (x, y, w, h, YColorV("vc_white")); 
    YSetXorDraw (FALSE); 
    break; 
  case YKEYWORD1: 
    break; 
  case YMREGIONOUT: 
    YSend (U->hWnd, YMREGIONOUT, 0,0,0, 0); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
 
    YImagePut (0, 0, U->image); 
    YImageFree (U->image); 
    YSetFocus (U->focus); 
 
    YWndFree (id);  /*??*/ 
    break; 
  default: 
    YSend (U->hWnd, message, mes1, mes2, mes3, mes4);  /* !!! */ 
  } 
 
  RETURN_TRUE; 
#undef  FLAG  
} 
/*-------------------------------F_PUSH_CB------------------------------------*/
#if defined(FLTK_CONTROLS) 
void 
F_PUSH_CB (Fl_Widget *, void *pid) 
{  

	int id = *(int*)pid;
	YPost (WNDI(id)->parent, YPUSH, id, 0, 0, 0);

}                                                                               
/*-------------------------------F_PUSH---------------------------------------*/
long
F_PUSH (PFUNC_VAR)
{
#define OB ((Fl_Button*)(WND->fltk_o))
  int x, y;
  static int f = 0;
  static Fl_Button *buton;
   

  switch (message) {
  case YOPEN:
	x = 0;
	y = 0;
	CALCXY (x, y); 

	/* OB = new Fl_Button(x, y, WND->w, WND->h, WND->name);   */                     
    WND->fltk_o = (long) new Fl_Button(x, y, WND->w, WND->h, WND->name); 		
	OB->callback(F_PUSH_CB, WND->pid);
	OB->color(WND->color);
	OB->down_color(YColor("white"));
	OB->labelfont(3);
	OB->labelsize(Y_FONT_SIZE);

	fltkwin->add(OB);

  case YDRAW:
		OB->redraw();
    break;
  case YCLOSE:
		fltkwin->remove(OB);
    break;
  default: ;;;;
  }

  RETURN_TRUE;
#undef OB
}
#endif
/*--------------------------------PUSH--------------------------------------*/ 
/* #else */
long 
PUSH (PFUNC_VAR) 
{ 
  static int flag;     
 
  switch (message) { 
  case YOPEN: 
    /* YSetWndColorI(id,YColorV("vc_push"));  */ 
  case YDRAW: 
    YGoto (YREDRAW, TRUE, 0, 0, 0); 
    break; 
  case YREDRAW: 
    if (mes1 == TRUE) { 
      YDrawUpBar (0, 0, WND->w, WND->h, WIN->color); 
      YDrawRectText (WND->name, 0, 0, WND->w, WND->h, YColorV("vc_black")); 
/*       YDrawRectText (WND->name, 0, 0, WND->w, WND->h, YColor("black"));  */
    } else { 
      YDrawDownBar (0, 0, WND->w, WND->h, WIN->color); 
      YDrawRectText (WND->name, 1, 1, WND->w, WND->h, YColorV("vc_black")); 
/*       YDrawRectText (WND->name, 1, 1, WND->w, WND->h, YColor("black"));  */
    } 
    break; 
  case YGETDATA: 
    return ((long) WND->name); 
  case YLMOUSEDOWN: 
    flag = 1; 
    YGoto (YREDRAW, FALSE, 0, 0, 0); 
    YPost (WND->parent, YPUSHDOWN, id, 0, 0, 0); 
    break; 
  case YRMOUSEDOWN: 
    flag = 1; 
    YGoto (YREDRAW, FALSE, 0, 0, 0); 
    YPost (WND->parent, YRPUSHDOWN, id, 0, 0, 0); 
    break; 
  case YLMOUSEDRAG: 
    if (flag) { 
      if (mes1<0 || mes1>WND->w || mes2>WND->h || mes2<0) { 
	flag = 0; 
	YGoto (YREDRAW, TRUE, 0, 0, 0); 
      } 
    }else 
      if (mes1>0 && mes1<WND->w && mes2<WND->h && mes2>0) 
	  YGoto (YLMOUSEDOWN, 0, 0, 0, 0); 
    break;         
  case YRMOUSEDRAG: 
    if(flag){ 
      if(mes1<0 || mes1>WND->w || mes2>WND->h || mes2<0){ 
	flag = 0; 
	YGoto (YREDRAW, TRUE, 0, 0, 0); 
      } 
    }else 
      if(mes1>0 && mes1<WND->w && mes2<WND->h && mes2>0) 
	  YGoto(YRMOUSEDOWN, 0, 0, 0, 0); 
    break;         
  case YLMOUSEUP: 
    YGoto (YREDRAW, TRUE, 0, 0, 0); 
    if (flag) 
      YPost (WND->parent, YPUSH, id, 0, 0l, 0); 
    break; 
  case YRMOUSEUP: 
    YGoto (YREDRAW, TRUE, 0, 0, 0); 
    if (flag) 
      YPost (WND->parent, YRPUSH, id, 0, 0l, 0); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: ;;;; 
  } 
 
  RETURN_TRUE; 
} 
/* #endif */
/*-------------------------------F_CHECK_CB------------------------------------*/
#if defined(FLTK_CONTROLS) 
void 
F_CHECK_CB (Fl_Widget *w, void *pid) 
{  
	Fl_Button *b = (Fl_Button *)w;                       
	int id = *(int*)pid;

	YPost (WNDI(id)->parent, YCHECK, id, b->value(), 0, 0);

}                                                                               
/*-------------------------------F_CHECK---------------------------------------*/
long
F_CHECK (PFUNC_VAR)
{
#define OB ((Fl_Button*)(WND->fltk_o))
	/* static Fl_Button *b1;  */                      
	int x , y;

  switch (message) {
  case YOPEN:
	x = 0;
	y = 0;
	CALCXY (x, y); 

	WND->fltk_o = (long) new Fl_Button(x, y, WND->w, WND->h, ""  /* WND->name */);          
	OB->callback(F_CHECK_CB, WND->pid);
	OB->when(FL_WHEN_RELEASE);

	OB->type(FL_TOGGLE_BUTTON);
	OB->align(FL_ALIGN_RIGHT);
	OB->labelfont(3);
	OB->labelsize(Y_FONT_SIZE);

	OB->color(WND->color);
	OB->selection_color(FL_RED);

	fltkwin->add(OB);
  case YDRAW:
		OB->redraw();
    YDrawString (WND->name, WND->w+WND->w/2, WND->h/2, YColorV("vc_black")); 
		/* fltkwin->draw_outside_label(OB);  */
    break;
  case YCLOSE:
		fltkwin->remove(OB);
    break;
  case YGETDATA: 
		return(OB->value());
  default: ;;;;
  }

  RETURN_TRUE;
#undef OB
}
#endif
/*--------------------------------CHECK-------------------------------------*/ 
long 
CHECK (PFUNC_VAR) 
{ 
#define FLAG (*(U->pflag)) 
 
  typedef struct { 
    YT_BOOL  flag; 
    YT_BOOL *pflag; 
  } YT_USR; 
 
  YT_COLOR c_line; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->pflag = (YT_BOOL*)YGet("wlong2")/* Y_WLONG2 */; 
    if (U->pflag == NULL) { 
      U->flag  = (YT_BOOL)YGet("wlong1")/* Y_WLONG1 */; 
      U->pflag = &(U->flag); 
    } 
  case YDRAW: 
    YDrawDownBar (0, 0, WND->w, WND->h, WIN->color); 
  case YREDRAW: 
    if (FLAG == FALSE)   c_line = WIN->color; 
    else                 c_line = YColorV("vc_black"); 
    YDrawLine (1, 1, WND->w-1, WND->h-1, c_line); 
    YDrawLine (1, WND->h-1, WND->w-1, 1, c_line); 
    YDrawString (WND->name, WND->w+WND->w/2, WND->h/2, YColorV("vc_black")); 
    break; 
  case YLMOUSEDOWN: 
    FLAG = !(FLAG); 
    YPost (WND->parent, YCHECK, id, FLAG, 0, 0); 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    if (!FLAG)   
      break; 
    if (YIdGoto() != ID_NULL)  
      YSend (YIdGoto(), YGOTOFROM, id, 0, 0, 0); 
    break; 
  case YGOTOFROM: 
    if (mes1 == id) 
      break; 
    FLAG = FALSE; 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    YSend (YIdGoto(), YGOTOFROM, mes1, 0, 0, 0); 
    break; 
  case YCLOSE: 
    break; 
  case YGETDATA: 
    return ((long)FLAG); 
  default: 
    break; 
  } 
 
  RETURN_TRUE; 
#undef FLAG 
} 
/*-------------------------------F_SCROLL_CB------------------------------------*/
#if defined(FLTK_CONTROLS) 
void 
F_SCROLL_CB (Fl_Widget *w, void *pid) 
{  
	Fl_Valuator *o = (Fl_Valuator *)o;                       
	int id = *(int*)pid;

	YPost (WNDI(id)->parent, YSCROLL, o->value(), 0, 0, 0);

}                                                                               
/*-------------------------------F_SCROLL---------------------------------------*/
long
F_SCROLL (PFUNC_VAR)
{
#define OB ((Fl_Scrollbar*)(WND->fltk_o))
	/* static Fl_Scrollbar *o;  */                      
	int x , y;

  static   int full_size; 
  static   int item_size; 
  static   int item_top; 

  switch (message) {
  case YOPEN:
    if (YGet("wlong1")!=0 && YGet("wlong2")!=0 && YGet("wlong3")!=0) {
    	full_size = YGet("wlong1"); 
			item_size	= YGet("wlong2"); 
			item_top 	= YGet("wlong3"); 
		}

	x = 0;
	y = 0;
	CALCXY (x, y); 

	WND->fltk_o = (long) new Fl_Scrollbar(x, y, WND->w, WND->h, WND->name);                  
	OB->callback(F_SCROLL_CB, WND->pid);

	if (YGet("wlong4") == YVERT)
	  OB->type(FL_VERTICAL);
	else
	  OB->type(FL_HORIZONTAL);

	OB->minimum(0);
	OB->maximum(full_size);
	/* o->value(item_top); */  /*??*/
	OB->slider_size((float)item_size/full_size);

	OB->color(WND->color);
	OB->selection_color(FL_RED);

	fltkwin->add(OB);
  case YDRAW:
	OB->redraw();
    break;
  case YSC_SET: 
    full_size = mes1; 
    item_size = mes2; 
    item_top  = mes3; 
	/* YGoto (YDRAW, 0, 0, 0, 0);  */
	/* o->redraw(); */
    break; 
  case YCLOSE:
	fltkwin->remove(OB);
    /* YWndClean (id); */
    break;
  case YGETDATA: 
	return(OB->value());
  default: ;;;;
  }

  RETURN_TRUE;
#undef OB
}
#endif
/*-------------------------------SCROLL---------------------------------------*/ 
long 
SCROLL (PFUNC_VAR) 
{ 
#define xitem (U->xi) 
#define yitem (U->yi) 
#define witem (U->wi) 
#define hitem (U->hi) 
 
#define full_size (U->int1) 
#define item_top  (U->int2) 
#define item_size (U->int3) 
 
  static int ypos, yitem_new; 
  YT_COLOR color; 
  int     delta, item_top_new; 
  float prom; 
 
  typedef struct { 
    int  xi, yi, wi, hi;  
    int  int1, int2, int3; 
  } YT_USR; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
/*     if (Y_WLONG1!=0 && Y_WLONG2!=0 && Y_WLONG3!=0) */ 
/*     	YGoto(YSC_SET, Y_WLONG1,Y_WLONG2,Y_WLONG3, 0);  */ 
    if (YGet("wlong1")!=0 && YGet("wlong2")!=0 && YGet("wlong3")!=0) 
    	YGoto(YSC_SET, YGet("wlong1"), YGet("wlong2"), YGet("wlong3"), 0);  
    break; 
  case YSC_SET: 
    full_size = mes1; 
    item_size = mes2; 
    item_top  = mes3; 
    YGoto (YDRAW, 0, 0, 0, 0); 
    break; 
  case YDRAW: 
    xitem = 1; 
    witem = WND->w - 2; 
    yitem = (float) item_top / full_size * WND->h; 
    hitem = (float) item_size / full_size * WND->h; 
    if      (yitem == 0)              yitem++;  /* else don't drawing top and bottom */ 
    else if (yitem == WND->h - hitem) yitem--; 
 
    YDrawRectFB (0, 0, WND->w, WND->h, WIN->color, YColorV("vc_black")); 
    YGoto (YDRAWITEM, TRUE, 0, 0, 0); 
    break; 
  case YDRAWITEM: 
    if (mes1 == TRUE)   
      color = YColorV("vc_black"); 
    else 
      color = WIN->color; 
    YDrawRectF (xitem, yitem, witem, hitem, color); 
    /*   YDrawRectB (xitem+1, yitem+1, witem-2, hitem-2, YColorV("vc_black"));*/ 
    break; 
  case YLMOUSEDOUBLE: 
  case YLMOUSEDOWN: 
    ypos = mes2 - yitem; 
    YSend (WND->parent, YSETFOCUS, 0, 0, 0, 0); 
    break; 
  case YLMOUSEDRAG: 
    yitem_new = mes2 - ypos; 
    yitem_new = YMAX (yitem_new, 1); 
    yitem_new = YMIN (yitem_new, WND->h - 1 - hitem); 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    prom = (float) yitem / WND->h * full_size; 
    /* округление */ 
    item_top_new=YInt(prom);   
    if (item_top_new != item_top) { 
      YSend (WND->parent, YSCROLL, item_top_new, 0, 0, 0); 
      item_top = item_top_new; 
    } 
    break; 
  case YREDRAW: 
    delta = yitem_new - yitem; 
    if (delta > 0) { 
      delta = YMIN (delta, hitem); 
      YDrawRectF (xitem, yitem, witem, delta, WIN->color); 
      YDrawRectF (xitem, yitem_new + hitem - delta, witem, delta, YColorV("vc_black")); 
    } else { 
      delta = -delta; 
      delta = YMIN (delta, hitem); 
      YDrawRectF (xitem, yitem + hitem - delta, witem, delta, WIN->color); 
      YDrawRectF (xitem, yitem_new, witem, delta, YColorV("vc_black")); 
    } 
    yitem = yitem_new; 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  } 
 
  RETURN_TRUE; 
#undef xitem  
#undef yitem  
#undef witem  
#undef hitem  
#undef full_size  
#undef item_top  
#undef item_size  
} 
/*---------------------------SCROLL_H-----------------------------*/           
long  
SCROLL_H (PFUNC_VAR)           
{  
#define xitem (U->int1) 
#define yitem (U->int2) 
#define witem (U->int3) 
#define hitem (U->int4) 
 
#define full_size (U->long1) 
#define item_top  (U->long2) 
#define item_size (U->long3) 
 
  typedef struct { 
    int   int1, int2, int3, int4; 
    long  long1, long2, long3; 
  } YT_USR; 
/*   static int full_size,item_size,item_top; */ 
  static int xpos,xitem_new; 
  int delta,item_top_new; 
  YT_COLOR color;  
  
  switch (message) {           
  case YOPEN:  
    U_MALLOC; 
/*     if (Y_WLONG1!=0 && Y_WLONG2!=0 && Y_WLONG3!=0) */ 
/*     	YGoto(YSC_SET, Y_WLONG1,Y_WLONG2,Y_WLONG3, 0);  */ 
    if (YGet("wlong1")!=0 && YGet("wlong2")!=0 && YGet("wlong3")!=0) 
    	YGoto(YSC_SET, YGet("wlong1"), YGet("wlong2"), YGet("wlong3"), 0);  
    break; 
  case YSC_SET: 
    full_size=mes1; 
    item_size=mes2; 
    item_top=mes3;   
    YGoto (YDRAW, 0,0,0, 0); 
    break;          
  case YDRAW:          
    yitem=1; 
    xitem=(float)item_top/full_size*WND->w;       
    hitem=WND->h-2;  
    witem=(float)item_size/full_size*WND->w;       
    if      (xitem == 0)              xitem++; 
    else if (xitem == WND->w - witem) xitem--; 
 
    YDrawRectFB(0,0,WND->w, WND->h,WIN->color,YColorV("vc_black")); 
    YGoto (YDRAWITEM, TRUE,0,0, 0);  
    break;         
  case YDRAWITEM: 
    if (mes1==TRUE)  
      color=YColorV("vc_black"); 
    else 
      color=WIN->color;  
    YDrawRectF(xitem,yitem,witem,hitem,color);  
    break;  
  case YLMOUSEDOUBLE:          
  case YLMOUSEDOWN:  
    xpos=mes1-xitem; 
    break;        
  case YLMOUSEDRAG: 
    xitem_new=mes1-xpos; 
    xitem_new=YMAX(xitem_new,1); 
    xitem_new=YMIN(xitem_new,WND->w-1-witem); 
    YGoto (YREDRAW, 0,0,0, 0);  
    item_top_new=(float)xitem/WND->w*full_size; 
    if (item_top_new!=item_top) { 
      YSend (WND->parent, YSCROLL, item_top,0,0, 0);  
      item_top=item_top_new; 
    } 
    break;         
  case YREDRAW:  
    delta=xitem_new-xitem; 
    if (delta>0) { 
      delta=YMIN(delta,witem); 
      YDrawRectF(xitem,yitem,delta,hitem,WIN->color);  
      YDrawRectF(xitem_new+witem-delta,yitem,delta,hitem,YColorV("vc_black")); 
    }else{ 
      delta=-delta; 
      delta=YMIN(delta,witem);  
      YDrawRectF(xitem+witem-delta,yitem,delta,hitem,WIN->color); 
      YDrawRectF(xitem_new,yitem,delta,hitem,YColorV("vc_black")); 
    }  
    xitem=xitem_new; 
    break;          
  case YCLOSE:           
    YWndClean(id);           
    break;           
  }           
   
#undef xitem  
#undef yitem  
#undef witem  
#undef hitem  
#undef full_size  
#undef item_top  
#undef item_size  
  RETURN_TRUE; 
} 
/*-------------------------------F_EDIT_CB------------------------------------*/
#if defined(FLTK_CONTROLS) 
void 
F_EDIT_CB (Fl_Widget *w, void *pid) 
{  

	Fl_Input *o = (Fl_Input *)w;                       
	int id = *(int*)pid;

/* 	YPost (WNDI(id)->parent, YEDIT, id, o->value(), 0, 0); */
	printf ("F_EDIT_CB \n");
	YKillFocus();

}                                                                               
/*-------------------------------F_EDIT---------------------------------------*/
long
F_EDIT (PFUNC_VAR)
{
#define OB ((Fl_Input*)(WND->fltk_o))
	/* static Fl_Button *b1;  */                      
	int x , y;

  switch (message) {
  case YOPEN:
	x = 0;
	y = 0;
	CALCXY (x, y); 

	WND->fltk_o = (long) new Fl_Input(x, y, WND->w, WND->h, ""  /* WND->name */);               
	OB->callback(F_EDIT_CB, WND->pid);

	/* OB->type(FL_TOGGLE_BUTTON); */
	/* OB->align(FL_ALIGN_RIGHT); */
	OB->textfont(3);
	OB->textsize(Y_FONT_SIZE);

	OB->labelfont(3);
	OB->labelsize(Y_FONT_SIZE);

	OB->insert((char*)YGet("wlong1"));
	/* OB->when(FL_WHEN_ENTER_KEY|FL_WHEN_NOT_CHANGED); */
	OB->when(FL_WHEN_RELEASE|FL_WHEN_NOT_CHANGED);

	OB->color(WND->color);
	OB->selection_color(FL_RED);

	fltkwin->add(OB);
  case YDRAW:
		OB->redraw();
    YDrawString (WND->name, -YStringW (WND->name), WND->h / 2, YColorV("vc_black")); 
		/* fltkwin->draw_outside_label(OB);  */
    break;
  case YCLOSE:
		fltkwin->remove(OB);
    break;
  case YGETDATA: 
	return((long)(OB->value()));
  default: ;;;;
  }

  RETURN_TRUE;
#undef OB
}
#endif
/*-------------------------------F_CLOCK---------------------------------------*/
#if defined(FLTK_CONTROLS) 
long
F_CLOCK (PFUNC_VAR)
{
#define OB ((Fl_Clock*)(WND->fltk_o))
	int x , y;

  switch (message) {
  case YOPEN:
	x = 0;
	y = 0;
	CALCXY (x, y); 

	WND->fltk_o = (long) new Fl_Clock(x, y, WND->w, WND->h, "This is clock"  /* WND->name */);
	OB->labelfont(3);
	OB->labelsize(Y_FONT_SIZE);
	OB->box(FL_OSHADOW_BOX);
	OB->align(130);

	OB->color(WND->color);
	OB->selection_color(FL_RED);

	OB->hide();
	fltkwin->add(OB);
  case YDRAW:
	OB->show();
	/*     YDrawString (WND->name, -YStringW (WND->name), WND->h / 2, YColorV("vc_black"));  */
	/* fltkwin->draw_outside_label(OB);  */
    break;
  case YCLOSE:
	OB->hide();
    break;
  default: ;;;;
  }

  RETURN_TRUE;
#undef OB
}
#endif
/*-------------------------------CLOCK---------------------------------------*/
long
CLOCK (PFUNC_VAR)
{
	int r;

  switch (message) {
  case YOPEN:
  case YDRAW: 
    YDrawRectFB (0, 0, WND->w,   WND->h,   WND->color, YColor("black")); 
		r = YMIN (WND->w/2, WND->h/2);
		YDrawCircB  (WND->w/2, WND->h/2, r, YColor("black"));
	  YDrawRectText ("It's Clock", 0, 0, WND->w, WND->h, YColor("black")); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: ;;;;
  }

  RETURN_TRUE;
}
/*-------------------------------YStrLen-----------------------------------*/ 
int 
YStrLen (char *string, int w_text) 
{ 
  char buf[256], *p; 
 
  if (string == NULL || *string == '\0') 
    return 0; 
 
  p = buf; 
 
  while (*string != '\0') { 
    *p++ = *string++; 
    *p = '\0'; 
    if (YStringW(buf) > w_text) { 
      *(p-1) = '\0'; 
      break; 
    } 
  } 
 
  return (strlen(buf)); 
} 
/*--------------------------------EDIT--------------------------------------*/ 
long 
EDIT (PFUNC_VAR) 
{ 
#define  VIS_TEXT     (U->vis_text) 
#define  VIS_TEXT_LEN (YStrLen (VIS_TEXT, WND->w-left_marg-right_marg))  
#define  VIS_CUR      (U->vis_cur) 
#define  BUF          (U->text)   
#define  BUF_LEN      (U->text_len)   
#define  STR          (U->string) 
 
  typedef struct { 
    char  *p_usrtext; 
    int    l_usrtext; 
    char  *text, *vis_text; 
    int    text_len, vis_cur; 
    char  *string; 
  } YT_USR; 
 
  enum keys { 
    DRAWCARET = YKEY_LOCALS, 
    CURTOX, XTOCUR,  
    YKEYCHAR, YKEYBACKSPACE, YKEYLEFT, YKEYRIGHT, YKEYDELETE 
  }; 
 
  int   left_marg=8, right_marg=8;   
  int   x_print = left_marg, y_print = WND->h/2+2; 
  int   x, i; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->p_usrtext = (char*)YGet("wlong1"); 
    U->l_usrtext =        YGet("wlong2"); 
 
    if   (U->l_usrtext == 0)  { 
      BUF_LEN = YMAX(2*strlen(U->p_usrtext),200); 
      if ((BUF = (char*)malloc(BUF_LEN+2)) == NULL)  YError ("EDIT-malloc"); 
      strcpy (BUF, U->p_usrtext); 
    } else {                        
      BUF_LEN = U->l_usrtext;  
      BUF = U->p_usrtext;  
    } 
    if ((STR = (char*)malloc(BUF_LEN+5)) == NULL)  YError ("EDIT-malloc"); 
    VIS_TEXT = BUF; 
    VIS_CUR = strlen (VIS_TEXT); 
  case YDRAW: 
    YDrawDownBar (0, 0, WND->w, WND->h, WIN->color); 
    YDrawString (WND->name, -YStringW (WND->name), WND->h / 2, YColorV("vc_black")); 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YREDRAW: 
    YDrawRectF (2, 1, WND->w-2, WND->h-2, WIN->color); 
    strcpy (STR, VIS_TEXT); 
    STR[VIS_TEXT_LEN] = '\0'; 
    YDrawString (STR, x_print, y_print, YColorV("vc_black"));  
    if (id == YGetFocus ()) 
      YGoto (DRAWCARET, TRUE, 0, 0, 0); 
    break; 
  case DRAWCARET: 
    if (VIS_CUR < 0) 
      break; 
    else 
      if (VIS_TEXT_LEN < VIS_CUR) 
	VIS_CUR--; 
    x = (int) YGoto (CURTOX, VIS_CUR, 0, 0, 0); 
    if (mes1) { 
      YDrawLine (x, 1, x, 3, YColorV("vc_black")); 
      YDrawLine (x + 1, 1, x + 1, 3, YColorV("vc_black")); 
    } else { 
      YDrawLine (x, 1, x, 3, WIN->color); 
      YDrawLine (x + 1, 1, x + 1, 3, WIN->color); 
    } 
    break; 
  case CURTOX: 
    strncpy (STR, VIS_TEXT, mes1); 
    STR[mes1] = '\0'; 
    return (x_print + YStringW (STR)); 
  case XTOCUR: 
    for (i = 1; (i <= strlen (VIS_TEXT)) && (YGoto (CURTOX, i, 0, 0, 0) <= mes1); i++)  
      ;;;; 
    return (i - 1); 
  case YSETFOCUS: 
    YGoto (DRAWCARET, TRUE, 0, 0, 0); 
    break; 
  case YKILLFOCUS: 
    YGoto (DRAWCARET, FALSE, 0, 0, 0); 
    VIS_CUR = -1; 
    break; 
  case YLMOUSEDOWN: 
    if (id != YGetFocus ()) { 
      VIS_CUR = (int) YGoto (XTOCUR, mes1, 0, 0, 0); 
      YSetFocus (id); 
    } else { 
      YGoto (DRAWCARET, FALSE, 0, 0, 0); 
      VIS_CUR = (int) YGoto (XTOCUR, mes1, 0, 0, 0); 
      YGoto (DRAWCARET, TRUE, 0, 0, 0); 
    } 
    break; 
  case YKEYBOARD: 
    switch (mes1) { 
    case YK_BACK: 
      YGoto (YKEYBACKSPACE, 0, 0, 0, 0); 
      break; 
    case YK_DELETE: 
      YGoto (YKEYDELETE, 0, 0, 0, 0); 
      break; 
    case YK_TAB: 
      YSend (WND->parent, YKEYTAB, id, 0, 0, 0); 
      break; 
    case YK_ENTER: 
      YGoto (YDATAMADE, 0, 0, 0, 0); 
      /* YSend (WND->parent, YEDIT, id, 0, 0, 0); */ 
      break; 
    case YK_DOWN: 
    case YK_LEFT: 
      YGoto (YKEYLEFT, 0, 0, 0, 0); 
      break; 
    case YK_UP: 
    case YK_RIGHT: 
      YGoto (YKEYRIGHT, 0, 0, 0, 0); 
      break; 
    default: 
      YGoto (YKEYCHAR, mes1, 0, 0, 0); 
      break; 
    } 
    break; 
  case YDATAMADE: 
    YSend (WND->parent, YEDIT, id, 0, (long)BUF, 0); 
    break; 
  case YGETDATA: 
    return ((long)BUF); 
  case YKEYBACKSPACE: 
    if (VIS_CUR == 0) 
      if(VIS_TEXT == BUF) 
	break; 
      else 
	VIS_TEXT--; 
    for (i = VIS_CUR; i <= strlen (VIS_TEXT); i++) 
      *(VIS_TEXT + i - 1) = *(VIS_TEXT + i); 
    if(VIS_CUR) VIS_CUR--; 
    YGoto (YREDRAW, /* VIS_CUR */0, 0, 0, 0); 
    break; 
  case YKEYDELETE: 
    if (VIS_CUR == strlen (VIS_TEXT)) 
      break; 
    for (i = VIS_CUR + 1; i <= strlen (VIS_TEXT); i++) 
      *(VIS_TEXT + i - 1) = *(VIS_TEXT + i); 
    YGoto (YREDRAW, /* VIS_CUR */0, 0, 0, 0); 
    break; 
  case YKEYLEFT: 
    if (VIS_CUR == 0){ 
      if(VIS_TEXT == BUF){ 
	break; 
      } 
      VIS_TEXT --; 
      YGoto (YREDRAW, 0, 0, 0, 0); 
      YGoto (DRAWCARET, TRUE, 0, 0, 0); 
      break; 
    } 
    YGoto (DRAWCARET, FALSE, 0, 0, 0); 
    VIS_CUR--; 
    YGoto (DRAWCARET, TRUE, 0, 0, 0); 
    break; 
  case YKEYRIGHT: 
    if (VIS_CUR == strlen (VIS_TEXT)) 
      break; 
    x = (int) YGoto (XTOCUR, WND->w-x_print, 0, 0, 0); 
    if (VIS_CUR >= x){ 
      VIS_TEXT++; 
      YGoto (YREDRAW, 0, 0, 0, 0); 
      YGoto (DRAWCARET, TRUE, 0, 0, 0); 
      break; 
    } 
    YGoto (DRAWCARET, FALSE, 0, 0, 0); 
    VIS_CUR++; 
    YGoto (DRAWCARET, TRUE, 0, 0, 0); 
    break; 
  case YKEYCHAR: 
    if (strlen (BUF) >= BUF_LEN - 1) 
      break; 
    if (WND->w - YStringW (VIS_TEXT) < 23){ 
      for (i = strlen (VIS_TEXT); i >= VIS_CUR; i--) 
	*(VIS_TEXT + i + 1) = *(VIS_TEXT + i); 
      *(VIS_TEXT + VIS_CUR) = mes1; 
      if (VIS_CUR >= VIS_TEXT_LEN) 
	VIS_TEXT++; 
    } else { 
      for (i = strlen (VIS_TEXT); i >= VIS_CUR; i--) 
	*(VIS_TEXT + i + 1) = *(VIS_TEXT + i); 
      *(VIS_TEXT + VIS_CUR) = mes1; 
/*        VIS_CUR++; */ 
    } 
    VIS_CUR++; 
    YGoto (YREDRAW, /* VIS_CUR-1 */0, 0, 0, 0); 
    break; 
  case YCLOSE: 
    YKillFocus (); 
    free (STR); 
    if (U->l_usrtext == 0)  free (BUF); 
    /* YWndClose (id); */ 
    break; 
  } 
 
  RETURN_TRUE; 
#undef VIS_TEXT 
#undef VIS_CUR 
#undef BUF_LEN 
} 
/*--------------------------------YTEXT-------------------------------------*/ 
long 
YTEXT (PFUNC_VAR) 
{ 
 
  typedef struct { 
    char  *txt; 
  } YT_USR; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->txt = (char*)YGet("wlong1")/* Y_WLONG1 */; 
  case YDRAW: 
    YDrawRectF (0, 0, WND->w, WND->h, WIN->color); 
    YDrawRectText (U->txt, 0, 0, WND->w, WND->h, YColorV("vc_black")); 
    YDrawString (WND->name, -YStringW (WND->name), WND->h / 2, YColorV("vc_black")); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  } 
 
  RETURN_TRUE; 
/* #undef TXT */ 
} 
/*-----------------------------DITTO----------------------------------------*/ 
long 
DITTO (PFUNC_VAR) 
{ 
 
  typedef struct { 
    YT_COLOR  color; 
  } YT_USR; 
 
  switch (message) { 
  case YGETSIZE: 
    MYSIZE_IS (100,100); 
    break; 
  case YOPEN: 
    U_MALLOC; 
    U->color = YColor("white"); 
  case YDRAW: 
    YDrawRectFB (0, 0, WND->w,   WND->h,   U->color, YColor("black")); 
    YDrawLogo ("Y",  25, 25, 50, 50); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: ;;;; 
  } 
 
  RETURN_TRUE; 
} 
/*-------------------------------ABOUT--------------------------------------*/ 
long 
ABOUT (PFUNC_VAR) 
{ 
 
  char *str1 = "     'Library Yzone'      "; 
  char *str2 = "       version 0.2        "; 
 
  char *str3 = "    Copyright (c) 2000    "; 
  char *str4 = "       Life Software.     "; 
  char *str5 = "     Under GNU License.   "; 
  int x, y; 
  YT_COLOR clr_text = YColor("black"); 
  YT_COLOR clr_line = YColor("white"); 
  YT_COLOR clr_fill = YColor("fuchsia"); 
  static int hPush; 
 
  switch (message) {   
  case YOPEN: 
  case YDRAW: 
    YDrawRectF (0, 0, WND->w, WND->h, clr_fill); 
    x = (WND->w - YStringW(str1))/2;   
    y = 30; 
    YDrawString(str1, x, y,     clr_text); 
    YDrawString(str2, x, y+=20, clr_text); 
    YDrawRectF (30, y+=15, WND->w-60, 3, clr_line); 
    YDrawString(str3, x, y+=20, clr_text); 
    YDrawString(str4, x, y+=20, clr_text); 
    YDrawString(str5, x, y+=20, clr_text); 
    YDrawRectF (30, y+=15, WND->w-60, 3, clr_line); 
 
    YWnd (&hPush, PUSH, "OK", WND->w/2-30,WND->h-40, 60,20, 0,0,0,0, CLR_DEF); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  case YPUSH: 
    YDlgEnd (0); 
    break; 
  } 
 
  RETURN_TRUE;  
} 
/*-------------------------------YHelpPages--------------------------------*/ 
void 
YHelpPages () 
{ 

#ifdef FLTK
#else
	YMessageBox ("Don't work yet!", "Ok"); 
#endif

} 
/*-------------------------------YMessageBox--------------------------------*/ 
int 
YMessageBox (char *text, char *buttons) 
{ 
  if (!Y_WMODE) return(0); 
  /* if (!YGet("wmode")) return(0); */ 
 
/*   KERN_S->text1 = text; */ 
/*   KERN_S->text2 = buttons; */ 
 
  return ((int) YDlg (MESSAGE, "Message!", 0,0,0,0, (long)text,(long)buttons,0,0, CLR_DEF, TRUE)); 
} 
/*---------------------------------MESSAGE----------------------------------*/ 
long 
MESSAGE (PFUNC_VAR) 
{ 
#define MAX_PUSH 7 
  static int num; 
  static int hPush[MAX_PUSH]; 
  static char Butts[30]; 
  static char buttons[10][10]; 
  static int i, push_w = 60, push_h = 20, delta_w = 20, butts_w, x0, y0; 
  char   *p; 
 
  typedef struct { 
    char  *text1; 
    char  *text2; 
  } YT_USR; 
 
  switch (message) { 
  case YGETSIZE: 
    MYSIZE_IS (W_DLG, H_DLG); 
    break; 
  case YOPEN: 
    U_MALLOC; 
    U->text1 = (char*)YGet("wlong1")/* KERN_S->text1 */; 
    U->text2 = (char*)YGet("wlong2")/* KERN_S->text2 */; 
    strcpy (Butts, U->text2); 
    p = strtok (Butts, "_"); 
    i = 0; 
    while (p) { 
      strcpy (buttons[i], p); 
      p = strtok (NULL, "_"); 
      i++; 
    } 
    num = i; 
 
    if (num > MAX_PUSH) 
      YError ("Too many PUSH in MESSAGE !"); 
 
    butts_w = num * push_w + (num - 1) * delta_w; 
  case YDRAW: 
    YDrawRectF (0, 0, WND->w, WND->h, WIN->color); 
 
    YDrawDownBar (20, 20, WND->w - 40, 80, YColorV("vc_darkfill")); 
    YDrawRectText (U->text1, 20, 20, WND->w - 40, 80, YColorV("vc_black")); 
 
    x0 = (WND->w - butts_w) / 2; 
    y0 = 130; 
    for (i = 0; i < num; i++) { 
      YWnd (&hPush[i], PUSH, buttons[i], x0,y0, push_w,push_h, 0,0,0,0, YColor("yellow")); 
      x0 += push_w + delta_w; 
    } 
    break; 
  case YPUSH: 
    for (i = 0; hPush[i] != mes1; i++) ;; 
    YDlgEnd (i + 1); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  case YMREGIONOUT: 
    break; 
  } 
 
  RETURN_TRUE; 
} 
/*--------------------------------BOOK--------------------------------------*/ 
long 
BOOK (PFUNC_VAR) 
{ 
#define PB     (U->pb) 
#define WMARK  (U->wmark) 
#define HMARK  (U->hmark) 
#define DIRECT (U->direct) 
 
#define CUR    (U->cur) 
#define HCUR   (U->hcur) 
#define NUM    (U->num) 
 
  int     x1, y1, x2, y2; 
  int     i; 
  YT_BOOK *ptr; 
  int     x, y; 
 
  typedef struct { 
    YT_BOOK  *pb; 
    int       wmark, hmark; 
    int       direct; 
    int       cur, hcur, num; 
  } YT_USR; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->pb     = (YT_BOOK*)YGet("wlong1")/* Y_WLONG1 */; 
    U->wmark  = YGet("wlong2")/* Y_WLONG2 */; 
    U->hmark  = YGet("wlong3")/* Y_WLONG3 */; 
    U->direct = YGet("wlong4")/* Y_WLONG4 */; 
    CUR = 1; 
    for (i = 0; i < 1000; i++) { 
      ptr = PB + i; 
      if (ptr->proc == NULL) 
	break; 
    } 
    NUM = i; 
  case YDRAW: 
    for (i = NUM - 1; i >= 0; i--) { 
      ptr = PB + i; 
      if (DIRECT == YUP) { 
	YDrawUpBar (i * WMARK, 0, WMARK, HMARK, WIN->color); 
	YDrawRectText (ptr->text, i * WMARK, 0, WMARK, HMARK, YColorV("vc_black")); 
      } else if (DIRECT == YRIGHT) { 
	YDrawUpBar (WND->w - WMARK, i * HMARK, WMARK, HMARK, WIN->color); 
	YDrawRectText (ptr->text, WND->w - WMARK, i * HMARK, WMARK, HMARK, YColorV("vc_black")); 
      } 
    } 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YREDRAW: 
    ptr = PB + CUR - 1; 
    if (DIRECT == YUP) { 
      x1 = (CUR - 1) * WMARK + 1; 
      y1 = HMARK; 
      x2 = x1 + WMARK - 2; 
      y2 = y1; 
      YDrawUpRect (0, HMARK, WND->w, WND->h - HMARK); 
      for (i = 1; i < NUM; i++) 
	YDrawLine (i * WMARK, 0, i * WMARK, HMARK - 1, YColorV("vc_black")); 
      YDrawLine (x1, y1, x2, y2, WIN->color); 
      YDrawLine (x1 - 1, y1, x1 - 1, 0, YColorV("vc_white")); 
      YWnd (&HCUR, ptr->proc, "", 1, HMARK+1, WND->w-2, WND->h-HMARK-2,  
	                                    ptr->long1, 0,0,0, CLR_DEF); 
    } else if (DIRECT == YRIGHT) { 
      x1 = WND->w - WMARK; 
      y1 = (CUR - 1) * HMARK + 1; 
      x2 = x1; 
      y2 = y1 + HMARK - 2; 
      YDrawUpRect (0, 0, WND->w - WMARK, WND->h); 
      for (i = 0; i < NUM; i++) 
	YDrawLine (x1, i * HMARK, WND->w, i * HMARK, YColorV("vc_black")); 
      YDrawLine (x1, y1, x2, y2, WIN->color /*WNDI(ptr->id).color */ ); 
      YDrawLine (x1, y1 - 1, WND->w - 1, y1 - 1, YColorV("vc_white")); 
      YWnd (&HCUR, ptr->proc, "", 1, 1, WND->w-2-WMARK, WND->h-2, ptr->long1,  
                                                  0,0,0, CLR_DEF); 
    } 
    break; 
  case YGETDATA: 
    return ((long)CUR); 
  case YLMOUSEDOWN: 
    for (i = 1; i <= NUM; i++) { 
      if (DIRECT == YUP) { 
	x = (i - 1) * WMARK; 
	y = 0; 
      } else if (DIRECT == YRIGHT) { 
	x = WND->w - WMARK; 
	y = (i - 1) * HMARK; 
      } 
      if (YPtInWind (mes1, mes2, x, y, WMARK, HMARK)) 
	break; 
    }; 
    if (i > NUM || i == CUR) 
      break; 
    ptr = PB + CUR - 1; 
    YSend (HCUR, YCLOSE, 0, 0, 0, 0); 
    CUR = i; 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YPUT: 
    YSend (HCUR, YPUT, 0, 0, 0, 0); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: /* YBeep(); */ ;; 
    /* YSend (WND->parent, message, mes1,mes2,mes3,mes4); */  /* ???? */ 
  } 
 
#undef PB     
#undef WMARK  
#undef HMARK  
#undef DIRECT  
 
#undef CUR   
#undef HCUR  
#undef NUM   
 
  RETURN_TRUE;  
} 
/*-------------------------------F_LIST_CB------------------------------------*/
#if defined(FLTK_CONTROLS) 
void 
F_LIST_CB (Fl_Widget *, void *pid) 
{  

	int id = *(int*)pid;
	YPost (WNDI(id)->parent, YLIST, 0, 0, 0, 0);

}                                                                               
/*-------------------------------F_LIST---------------------------------------*/
long
F_LIST (PFUNC_VAR)
{
#define OB ((Fl_Hold_Browser*)(WND->fltk_o))
	int x, y;

	char **list;

  switch (message) {
  case YOPEN:
	x = 0;
	y = 0;
	CALCXY (x, y); 

	WND->fltk_o = (long) new Fl_Hold_Browser (x, y, WND->w, WND->h/* , WND->name */);       
	OB->callback(F_LIST_CB, WND->pid);
	OB->color(WND->color, YColor("white"));  /* FColor() ????*/
	OB->box(FL_FRAME_BOX);

	for (list=(char**)YGet("wlong1"); *list!=NULL; list++) 
	  OB->add(*list);
	OB->topline(YGet("wlong2"));

	fltkwin->add(OB);

  case YDRAW:
	OB->redraw();
    break;
  case YCLOSE:
	fltkwin->remove(OB);
    break;
  default: ;;;;
  }

  RETURN_TRUE;
#undef OB
}
#endif
/*--------------------------------LIST-------------------------------------*/        
long 
LIST(PFUNC_VAR) 
{ 
#define  MENU  (U->menu) 
#define  JLIST (U->jlist) 
 
  typedef struct { 
    int       hList, hScroll; 
    char    **jlist; 
    YT_MENU  *menu; 
    int       sel, *psel; 
    int       full_size, item_size; 
  } YT_USR; 
 
  YT_LIST li; 
  static int scroll_w = 15; 
  int i,imax; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    JLIST  = (char**) YGet("wlong1")/* Y_WLONG1 */; 
    U->psel  = (int*) YGet("wlong3")/* Y_WLONG3 */; 
    if (U->psel == NULL) { 
      U->sel  = YGet("wlong2")/* Y_WLONG2 */; 
      U->psel = &(U->sel); 
    } 
    for (i = 0; *(JLIST + i) != NULL; i++) ;;  
    imax = i; 
 
    MENU = (YT_MENU*) malloc((imax+1)*sizeof(YT_MENU)); 
    for (i = 0; i < imax; i++) { 
      (MENU+i)->is_string = TRUE; 
      (MENU+i)->is_check  = FALSE; 
      (MENU+i)->is_gray  = FALSE; 
      (MENU+i)->is_menu  = FALSE; 
/*       (MENU+i)->string = *(JLIST + i); */ 
      strcpy((MENU+i)->string, *(JLIST + i)); 
      (MENU+i)->long1 = YNULL; 
    } 
/*     (MENU+i)->string = NULL; */ 
    ((MENU+i)->string)[0] = YNULL; 
  case YDRAW: 
    YDrawRectF(0, 0, WND->w, WND->h, WIN->color); 
 
    YWnd (&(U->hList), LIST_RAB, "", 0,0, WND->w-scroll_w,WND->h, (long)MENU,(long)U->psel,0,0,  
                                                                                     WIN->color); 
    YSend (U->hList, YGET, 0, 0, LP(li), 0); 
    YWnd (&(U->hScroll), SCROLL, "", WND->w-scroll_w-1,0, scroll_w,WND->h, 0,0,0,0, WIN->color); 
	 
    if (li.full_size <= li.item_size){ 
      li.full_size = li.item_size;  
      li.item_top=1; 
    } 
    YSend (U->hScroll, YSC_SET, li.full_size, li.item_size, li.item_top - 1, 0); 
 
/*     WIN->int3=li.full_size; */ 
/*     WIN->int4=li.item_size; */ 
    U->full_size = li.full_size; 
    U->item_size = li.item_size; 
    break; 
  case YSCROLL: 
    YSend (U->hList, YKEYWORD2, mes1, 0, 0, 0); 
    break; 
  case YKEYWORD1: 
    YSend (U->hScroll, YSC_SET, /* WIN->int3 */U->full_size, U->item_size, mes1 - 1, 0); 
    break; 
  case YSETFOCUS: 
    YSetFocus(U->hList); 
    break; 
  case YDATAMADE:  
    YSend (WND->parent, YLIST, id, mes2, mes3, mes4); 
    break; 
  case YGETDATA: 
   return (YSend(U->hList, YGETDATA, 0, 0, 0, 0)); 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default:;;; 
  } 
 
  RETURN_TRUE; 
#undef JLIST    
#undef MENU  
} 
/*-----------------------------LIST_RAB-----------------------------*/ 
long 
LIST_RAB (PFUNC_VAR) 
{ 
#define PMENU  (U->menu) 
#define JTOP   (U->jtop) 
#define JSEL   (*(U->pjsel)) 
#define JMAX   (U->jmax) 
#define i_max  (U->max) 
#define top    (U->tp) 
#define bottom (U->bot) 
 
  enum ywords { 
    YMOVESELECT = YKEY_LOCALS + 1, 
    YLINEUP, YLINEDOWN 
  }; 
 
  static YT_BOOL moveup, moved; 
  YT_GETSIZE *pgs; 
  YT_LIST *listik; 
  YT_MENU *j_list, *elem; 
  YT_COLOR text_c, fill_c; 
  static YT_BOOL flag_lmd = FALSE, flag_check = FALSE; 
  char *string; 
  int w_max, pos, left_marg, top_marg, arrow_marg, check_marg; 
  int step, step_sep, sel_new; 
  int i, y_print, x_print; 
  int num_str, num_sep, num_menu; 
 
  typedef struct { 
    YT_BOOL  menu_flag, ch;  
    YT_MENU *menu; 
    int      jtop, jmax; 
    int     *pjsel, jsel; 
    int     max, tp, bot; 
  } YT_USR; 
 
  step = YStringH ("Wg"); 
  left_marg = 6; 
  top_marg = 4; 
  arrow_marg = 20; 
  x_print = left_marg; 
  step_sep = step/2; 
  check_marg = 10; 
  
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    if(WNDI(WND->parent)->proc == LIST)  
      U->menu_flag = FALSE; 
    else 
      U->menu_flag = TRUE;   
    PMENU = (YT_MENU*)YGet("wlong1")/* Y_WLONG1 */; 
    U->pjsel  = (int*)YGet("wlong2")/* Y_WLONG2 */; 
    if (U->pjsel == NULL) { 
      U->jsel = 0; 
      U->pjsel = &(U->jsel); 
    } 
    JTOP = 1; 
/*     for(i = 0; (PMENU + i)->string != NULL; i++) ;; */ 
   for(i = 0; *((PMENU + i)->string) != '\0'; i++) ;; 
    JMAX = i; 
    if(U->menu_flag){ 
      U->max = i; 
      U->tp = top_marg; 
      U->bot = WND->h - top_marg; 
    }else{ 
      U->max =  (WND->h - 2) / step; 
      U->tp = 1 + ((WND->h - 2) % step) / 2; 
      U->bot = top + step * i_max - 1; 
    } 
    U->ch = flag_check; 
    flag_check = FALSE; 
  case YDRAW: 
    moved = FALSE; 
    YDrawRectFB (0, 0, WND->w, WND->h, WIN->color, YColorV("vc_black")); 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    if(id == YGetFocus()) YGoto (YSETFOCUS, 0,0,0, 0); 
    break; 
  case YREDRAW: 
    for(i = 1; i <= i_max; i++)  
      YGoto (YDRAWITEM, i, 0, 0, 0); 
    break; 
  case YDRAWITEM: 
    if(mes1 < 1 || mes1 > i_max) 
      break; 
 
    num_str = 0; 
    num_sep = 0; 
    for(i=1; i<mes1; i++) { 
      if((PMENU+JTOP+i-2)->is_string) 
	num_str ++; 
      else{  
	num_sep ++; 
      } 
    } 
    y_print = top + step*num_str + step_sep*num_sep;  
    if(U->ch) 
      x_print += check_marg; 
    if(mes1 == JSEL) { 
      YSend (WND->parent, YNEW, 0, y_print, JTOP+JSEL-1, 0); 
      text_c = YColorV("vc_white"); 
      fill_c = YColorV("vc_black"); 
    }else{ 
      text_c = YColorV("vc_black"); 
      fill_c = WIN->color; 
    } 
 
    if(JTOP + mes1 - 1 > JMAX) 
      break; 
 
    elem = PMENU+JTOP+mes1-2; 
    string = (char *) elem->string; 
    if(elem->is_string){ 
      if(elem->is_gray) 
	text_c = YColor("gray"); 
      YDrawRectF (left_marg/2, y_print+1, WND->w-left_marg, step-1, fill_c); 
      if(elem->is_check) 
	  YDrawCheck (left_marg/2, y_print+1, left_marg/2+8, step/2+4,  
		     text_c,(YT_BOOL)elem->long1);  
      YDrawString (string, x_print, y_print+step/2, text_c); 
      if(elem->is_menu) 
/*    YDrawSymbol ("right>", WND->w-left_marg-8, y_print+step/2 - 4, 8, 8, text_c); */ 
	YDrawString (">", WND->w-left_marg-8, y_print+step/2, text_c); 
    }else{ 
      y_print += step_sep/2; 
      YDrawLine (0, y_print, WND->w-1, y_print, text_c); 
    } 
    break; 
  case YGET: 
    listik = (YT_LIST*)mes3; 
    listik->full_size = JMAX; 
    listik->item_size = i_max; 
    listik->item_top = JTOP; 
    break; 
  case YKEYWORD2: 
    JTOP = mes1 + 1; 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YSETFOCUS: 
    YDrawRectB (1, 1, WND->w-2, WND->h-2, YColorV("vc_white")); 
    YDrawRectB (2, 2, WND->w-4, WND->h-4, YColorV("vc_white")); 
    break; 
  case YKILLFOCUS: 
    YDrawRectB (1, 1, WND->w-2, WND->h-2,  WIN->color); 
    YDrawRectB (2, 2, WND->w-4, WND->h-4,  WIN->color); 
    break; 
  case YLMOUSEDOWN: 
    if(id != YGetFocus ()) 
      YSetFocus (id); 
  case YLMOUSEDRAG: 
    if(!moved && mes2 < top){ 
      YSetTimer (id, 10); 
      moveup = TRUE; 
      moved = TRUE; 
      break; 
    }else if(!moved && mes2 > bottom){ 
      YSetTimer (id, 10); 
      moveup = FALSE; 
      moved = TRUE; 
      break; 
    } 
    if(moved && mes2 > top && mes2 < bottom){ 
      moved = FALSE; 
      /* OUTD(555); */ 
      YKillTimer (); 
    } 
    /* sel_new = YGoto (YKEYWORD3,"", 0, mes2, 0); */ 
    pos = (mes2 - top) / step; 
    elem = PMENU + JTOP + pos - 1; 
    if(elem -> is_string && !(elem -> is_gray)) 
      sel_new = YMIN (YMAX (pos + 1, 1), i_max);  
    else  
      sel_new = YMIN (YMAX (pos + 2, 1), i_max);  
    YGoto (YMOVESELECT, JSEL, sel_new, 0, 0); 
    break; 
  case YMOVESELECT: 
    if(mes1 == mes2) 
      break; 
    if(mes2 < 1) 
      YGoto (YLINEUP, 0, 0, 0, 0); 
    else if(mes2 > i_max) 
      YGoto (YLINEDOWN, 0, 0, 0, 0); 
    else{ 
      if (mes2 > JMAX - JTOP + 1) 
	break; 
      JSEL = mes2;  
      YGoto (YDRAWITEM, mes1, 0, 0, 0); 
      YGoto (YDRAWITEM, JSEL, 0, 0, 0); 
    } 
    /* YSend(WND->parent, YKEYWORD1,"", JTOP, 0, 0); */ 
    break; 
  case YLMOUSEUP: 
    moved = FALSE; 
    YKillTimer (); 
    if(flag_lmd){ 
      YGoto (YDATAMADE, 0,0,0,0); 
/*       YSend (WND->parent, YDATAMADE, 0, 0, JTOP+JSEL-1, (long)(PMENU+JTOP+JSEL-2)->string);  */ 
      flag_lmd = FALSE; 
    }  
    if(U->menu_flag) 
      YGoto (YDATAMADE, 0,0,0,0);   /*  Send to MENU  */ 
/*       YSend (WND->parent, YDATAMADE, 0, 0, JTOP+JSEL-1, 0);  */  
    break; 
  case YDATAMADE: 
    YSend (WND->parent, YDATAMADE, 0, 0, JTOP+JSEL-1, (long)(PMENU+JTOP+JSEL-2)->string);  
    break; 
  case YLMOUSEDOUBLE: 
    flag_lmd = TRUE; 
    break; 
  case YKEYBOARD: 
    switch (mes1) { 
    case YK_UP: 
      elem = PMENU + JTOP + JSEL - 3; 
      if(U->menu_flag && JSEL==1) sel_new=i_max; 
      else if(elem -> is_string && !(elem -> is_gray)) 
	sel_new = JSEL - 1;  
      else  
	sel_new = JSEL - 2;  
      YGoto (YMOVESELECT, JSEL, sel_new, 0, 0); 
      break; 
    case YK_DOWN: 
      elem = PMENU + JTOP + JSEL - 1; 
      if(JSEL == i_max && U->menu_flag) 
	sel_new = 1; 
      else  if(elem -> is_string && !(elem -> is_gray)) 
	sel_new = JSEL + 1;  
      else  
	sel_new = JSEL + 2;  
      YGoto (YMOVESELECT, JSEL, sel_new, 0, 0); 
      break; 
    case YK_ENTER: 
      YGoto (YDATAMADE, 0,0,0,0); 
      /* YSend (WND->parent, YDATAMADE, 0, 0, JTOP+JSEL-1, 0);  */ 
      break; 
    case YK_RIGHT: 
      if(U->menu_flag) 
	/* YSend (WND->parent, YK_RIGHT,"", 0, 0, 0); */ 
	YSend (WND->parent, YRIGHT, 0, 0, 0, 0); 
      break; 
    case YK_LEFT: 
      if(U->menu_flag) 
	/* YSend (WND->parent, YK_LEFT,"", 0, 0, 0); */ 
	YSend (WND->parent, YLEFT, 0, 0, 0, 0); 
      break; 
    } 
    break; 
  case YTIMER: 
    if(moveup) 
      YGoto (YLINEUP, 0, 0, 0, 0); 
    else 
      YGoto (YLINEDOWN, 0, 0, 0, 0); 
    break; 
  case YLINEUP: 
    if(JTOP == 1) 
      break; 
    JTOP--; 
    YSend (WND->parent, YKEYWORD1, JTOP, 0, 0, 0);  
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YLINEDOWN: 
    if(JMAX - JTOP + 1 <= i_max) 
      break; 
    JTOP++; 
    YSend (WND->parent, YKEYWORD1, JTOP, 0, 0, 0);  
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YGETDATA: 
    return (JTOP + JSEL - 1); 
  case YKEYWORD1: 
    if(mes1==1)  
      j_list = (YT_MENU*)mes3; 
    else{ 
      pgs = (YT_GETSIZE*)mes3; 
      w_max=0; 
      num_str = num_sep = num_menu = 0; 
/*       for(i = 0; j_list->string != NULL; i++, j_list++){ */ 
      for(i = 0; *(j_list->string) != '\0'; i++, j_list++){ 
	w_max=YMAX(w_max,YStringW (j_list->string)); 
	if(j_list->is_string){  
	  num_str++; 
	  if(j_list->is_menu)  num_menu ++; 
	  if(j_list->is_check)  flag_check = TRUE; 
	}else 
	  num_sep ++; 
      } 
      if(num_menu) 
	w_max += arrow_marg;  
      if(flag_check) 
	w_max += check_marg; 
      pgs->w = w_max + left_marg*2; 
      pgs->h = num_sep*step_sep + num_str*step + top_marg*2; 
	 
    } 
    break; 
  case YCLOSE: 
    YKillFocus (); 
    YWndClean (id); 
    break; 
  } 
 
  RETURN_TRUE; 
#undef JLIST  
#undef JTOP   
#undef JSEL   
#undef JMAX   
#undef PMENU  
} 
/*-----------------------------FILES----------------------------------------*/ 
long 
FILES (PFUNC_VAR) 
{ 
  static long ret=0; 
  int push_w=50; 
 
  typedef struct { 
    int   hFname; 
    long  w_long1, w_long2;  
  } YT_USR; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->w_long1 = YGet("wlong1"); 
    U->w_long2 = YGet("wlong2"); 
  case YDRAW: 
    YDrawRectB (0, 0, WND->w, WND->h, YColorV("vc_black")); 
    YWnd (&U->hFname, EDIT, "", 0,0, WND->w-push_w-2,WND->h-2,  
	                   U->w_long1, U->w_long2,0,0, CLR_DEF); 
    YWnd (Ph(), PUSH, "File", WND->w-push_w-1,1, push_w,WND->h-2, 0,0,0,0, CLR_DEF); 
    YDrawString (WND->name, -YStringW (WND->name), WND->h / 2, YColorV("vc_black")); 
    break; 
  case YPUSH: 
    if (!(ret = YFindFile ()))   
      break; 
    strncpy ((char*)U->w_long1, (char *)ret, U->w_long2); 
    YWndUpdate(U->hFname); 
    break; 
  case YGETDATA: 
    return (YGetData(U->hFname)); 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: ;;;; 
  } 
 
  RETURN_TRUE; 
} 
/*----------------------------YFindFile-------------------------------------*/ 
long 
YFindFile () 
{ 
#ifdef FLTK
	return ((long)(fl_file_chooser("fgdfg", "", "")));
#else
  long    ret; 
  static char name[80]; 
 
  ret = YDlg (FINDF, "File navigation", 0,0,0,0, (long)name,0,0,0, CLR_DEF, TRUE); 
  if (ret) 
    return ((long)name); 
  else 
    return (ret); 
#endif
} 
/*-------------------------------FINDF--------------------------------------*/ 
long 
FINDF (PFUNC_VAR) 
{ 
  static char **lst = NULL; 
  static int hList, hOk, hCancel; 
  static int sel; 
  char *new_name; 
  typedef struct { 
    char  *text1; 
  } YT_USR; 
 
  switch (message) { 
  case YGETSIZE: 
    MYSIZE_IS (W_DLG, H_DLG); 
    break; 
  case YOPEN: 
    U_MALLOC; 
    U->text1 = (char *) YGet("wlong1")/* Y_WLONG1 */; 
  case YDRAW: 
    lst = YDirRead (); 
    YDrawRectF (0, 0, WND->w, WND->h, WIN->color); 
    sel = 1; 
    YWnd (&hList, LIST, "", 10,20, 160,140, (long)lst,0,LP(sel),0, CLR_DEF); 
    YSetFocus (hList); 
    YWnd (&hOk,     PUSH, "OK",     175, 90, 55,20, 0,0,0,0, YColor("yellow")); 
    YWnd (&hCancel, PUSH, "Cancel", 175,120, 55,20, 0,0,0,0, YColor("yellow")); 
    break; 
  case YMREGIONOUT: 
    YBeep (); 
    break; 
  /* case YDATAMADE: */ 
  case YLIST: 
    YGoto (YPUSH, hOk, 0, 0, 0); 
    break; 
  case YPUSH: 
    if (mes1 == hCancel) { 
      strcpy (U->text1, ""); 
      YDlgEnd (FALSE); 
    } else if (mes1 == hOk) { 
      new_name=lst[(int)YGetData(hList)-1] + 1; 
      if (!YDirChange (new_name)) { 
	strcpy (U->text1, new_name); 
	YDlgEnd (TRUE); 
	break; 
      } 
      lst = YDirRead (); 
      sel = 1; 
      YUnWnd (hList);      
      YWnd (&hList, LIST, "", 10,20, 160,140, (long)lst,0,LP(sel),0, CLR_DEF); 
      YSetFocus (hList); 
    } 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  } 
 
  RETURN_TRUE; 
} 
/*--------------------------------DECIMAL-----------------------------------*/ 
long 
DECIMAL (PFUNC_VAR) 
{ 
#define DVAL (*(U->pdval)) 
#define DMIN (U->dmin) 
#define DMAX (U->dmax) 
  char    string[20]; 
  static YT_BOOL minus; 
  static int delay, x0, y0, delta; 
 
  typedef struct { 
    int  *pdval, dval, dmin, dmax; 
  } YT_USR; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    if (/* Y_WLONG4 */YGet("wlong4") == 0) { 
      U->dval  = YGet("wlong1")/* Y_WLONG1 */; 
      U->pdval = &(U->dval); 
    } 
    else 
      U->pdval = (int*) YGet("wlong4")/* Y_WLONG4 */; 
    DMIN = YGet("wlong2")/* Y_WLONG2 */; 
    DMAX = YGet("wlong3")/* Y_WLONG3 */; 
  case YDRAW: 
    YDrawRectB (0, 0, WND->w, WND->h, YColorV("vc_black")); 
    YDrawString (WND->name, -YStringW (WND->name), WND->h / 2, YColorV("vc_black")); 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YREDRAW: 
    YDrawRectF (1, 1, WND->w - 2, WND->h - 2, WIN->color); 
    sprintf (string, "%d", DVAL); 
    YDrawRectText (string, 1, 2, WND->w - 2, WND->h - 1, YColorV("vc_black")); 
    break; 
  case YLMOUSEDOUBLE: 
  case YLMOUSEDOWN: 
    DVAL--; 
    if (DVAL < DMIN) { 
      DVAL = DMIN; 
      break; 
    } 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    YSetTimer (id, 10); 
    minus = TRUE; 
    delay = 0; 
    x0 = mes1; 
    y0 = mes2; 
    delta = 1; 
    break; 
  case YRMOUSEDOWN: 
    DVAL++; 
    if (DVAL > DMAX) { 
      DVAL = DMAX; 
      break; 
    } 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    YSetTimer (id, 10); 
    minus = FALSE; 
    delay = 0; 
    x0 = mes1; 
    y0 = mes2; 
    delta = 1; 
    break; 
  case YLMOUSEDRAG: 
  case YRMOUSEDRAG: 
    /* delta=1+sqrt((mes1-x0)*(mes1-x0)+(mes2-y0)*(mes2-y0))/10; */ 
    delta = 1 + (abs (mes1 - x0) + abs (mes2 - y0)) / 5; 
    break; 
  case YTIMER: 
    if (delay++ < 3) 
      break; 
    if (minus) { 
      DVAL -= delta; 
      if (DVAL < DMIN)  DVAL = DMIN; 
    } else { 
      DVAL += delta; 
      if (DVAL > DMAX)  DVAL = DMAX; 
    } 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YLMOUSEUP: 
  case YRMOUSEUP: 
    YKillTimer (); 
    /* YSend (WND->parent, YDECIMAL, id, 0, DVAL, 0); */ 
    YGoto (YDATAMADE, 0,0,0,0); 
    break; 
  case YDATAMADE: 
    YSend (WND->parent, YDECIMAL, id, 0, DVAL, 0); 
    break;  
  case YGETDATA: 
    return(DVAL);  
  case YCLOSE: 
    YWndClean (id); 
    break; 
  } 
 
  RETURN_TRUE; 
} 
/*-------------------------------COLORS-------------------------------------*/ 
long 
COLORS (PFUNC_VAR) 
{ 
/* #define COL (*((YT_COLOR*)WIN->long1)) */ 
#define COL (*((YT_COLOR*)(U->long1))) 
  typedef struct { 
    long  long1; 
  } YT_USR; 
 
  YT_COLOR color; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
   /*  WIN->long1 */U->long1 = YGet("wlong1")/* Y_WLONG1 */; 
  case YDRAW: 
  case YREDRAW: 
    YDrawRectFB (0, 0, WND->w, WND->h, COL, YColorV("vc_black")); 
    YDrawString (WND->name, WND->w + 10, WND->h / 2, YColorV("vc_black")); 
    break; 
  case YLMOUSEDOWN: 
    color = YDlg (choice_colors, "Choice color", 0,0,0,0, 0,0,0,0, CLR_DEF, TRUE); 
    if (color == CLR_NULL) 
      break; 
    COL = color; 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: break; 
  } 
 
  RETURN_TRUE; 
#undef COL 
} 
/*-----------------------------choice_colors--------------------------------*/ 
long 
choice_colors (PFUNC_VAR) 
{ 
  static YT_COLOR color; 
  char   *text[] = 
  { 
    "Simply to click left mouse key", 
    "on choose color:", 
    NULL 
  }; 
 
  switch (message) { 
  case YGETSIZE: 
    MYSIZE_IS (W_DLG, H_DLG); 
    break; 
  case YOPEN: 
  case YDRAW: 
    YDrawRectF (0, 0, WND->w, WND->h, WIN->color); 
    YDrawStrings (text, 20, 20); 
 
    YDrawScale (20, 55, 200, 20, YHORZ); 
    YGoto (YDRAWITEM, 0,0,0, 0); 
 
    YWnd (Ph(), PUSH, "Cancel", 90,140, 70,20, 0,0,0,0, YColor("yellow")); 
    break; 
  case YDRAWITEM: 
    YDrawColer (20, 90, 200, 40); 
    break; 
  case YLMOUSEUP: 
    color = YGetColor (mes1, mes2); 
    YDlgEnd (color); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  case YPUSH: 
    YDlgEnd (CLR_NULL); 
    break; 
  } 
 
  RETURN_TRUE; 
} 
/*-------------------------------IPUSH--------------------------------------*/ 
long 
IPUSH (PFUNC_VAR) 
{ 
#define BRIGHT (*((YT_BOOL*)U->long1)) 
#define BMAIN  (*((YT_BOOL*)U->long2)) 
  typedef struct { 
    long  long1, long2; 
  } YT_USR; 
 
  int     right_w = 2; 
  static YT_BOOL main = TRUE; 
  YT_COLOR color; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->long1 = YGet("wlong1")/* Y_WLONG1 */; 
    U->long2 = LP(main); 
  case YDRAW: 
    if (BMAIN)  color = WIN->color; 
    else        color = YColorV("vc_white"); 
    YDrawRectFB (0, 0, WND->w, WND->h, color, YColorV("vc_black")); 
    YDrawRectText (WND->name, 0, 0, WND->w - right_w, WND->h, YColorV("vc_black")); 
    YGoto (YDRAWITEM, 0, 0, 0, 0); 
    break; 
  case YDRAWITEM: 
    if (BRIGHT)  color = YColorV("vc_black"); 
    else         color = WIN->color; 
    YDrawRectF (WND->w - right_w, 1, right_w - 1, WND->h - 2, color); 
    break; 
  case YGETDATA: 
    return ((long)WND->name); 
  case YLMOUSEDOWN: 
    BMAIN = FALSE; 
    YGoto (YDRAW, 0, 0, 0, 0); 
    break; 
  case YLMOUSEUP: 
    BMAIN = TRUE; 
    YGoto (YDRAW, 0, 0, 0, 0); 
    YPost (WND->parent, YPUSH, id, 0, 0l, 0); 
    break; 
  case YRMOUSEUP: 
    BRIGHT = !BRIGHT; 
    YGoto (YDRAWITEM, 0, 0, 0, 0); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  } 
 
  RETURN_TRUE; 
#undef BRIGHT 
#undef BMAIN 
} 
/*-------------------------------RAMKA--------------------------------------*/ 
long 
RAMKA (PFUNC_VAR) 
{ 
#define BUT   (U->but) 
#define PROC  (U->proc) 
#define HREND (U->hRend) 
#define PTEXT (U->butts) 
 
  int     h_caption, vert1, w_butt, marg_bot; 
  int     vert2, x, y, w, h; 
  static int wscroll = 20, hScroll; 
  static long  add_butts=0; 
 
  typedef struct { 
    char   **but; 
    YT_PFUNC proc; 
    int      hButts; 
    int      num_butts; 
    char    *butts[20]; 
    long     w_long4; 
    int      hRend; 
  } YT_USR; 
  char  **add_bts; 
  int    i; 
 
  h_caption = H_CAPTION; 
  vert1     = 10; 
  w_butt    = 65; 
  marg_bot  =  5; 
 
 
  switch (message) { 
  case YGETSIZE: 
    break; 
  case YCREATE: 
    break; 
  case YOPEN: 
    U_MALLOC; 
    BUT  = (char**) YGet("wlong1"); 
    if (YGet("wlong2"))  wscroll = 20; 
    else           wscroll = 0; 
    PROC = (YT_PFUNC) YGet("wlong3"); 
    U->w_long4      = YGet("wlong4"); 
 
    U->num_butts = 0; 
    CALL  (PROC, 0, from, YGETCOMS, 0,0, LP(add_butts), 0);  
    YGoto (YADD, 0, 0, add_butts, 0); 
    YGoto (YADD, 0, 0, (long)BUT, 0); 
  case YDRAW: 
  case YREDRAW: 
    w = WND->w; 
    h = WND->h; 
    /* color_ramka = YColorFor (id) */ 
    YDrawRectF (0, 0, w, h_caption, WIN->color); 
    YDrawRectF (0, 0, vert1 + 2, h, WIN->color); 
    YDrawRectF (0, h - marg_bot, w, marg_bot, WIN->color); 
    YDrawRectF (w - 5, 0, 5, h, WIN->color); 
 
    YDrawRectB (0, 0, w, h, YColorV("vc_black")); 
    YDrawRectText (WND->name, 0, 0, w, h_caption, YColorV("vc_black")); 
    vert2 = WND->w - w_butt; 
 
    x = vert1 + wscroll; 
    y = h_caption; 
    w = vert2 - vert1 + 1 - wscroll; 
    h = WND->h - h_caption - marg_bot; 
    YDrawRectB (x, y, w, h, YColorV("vc_black")); 
    YDrawRectB (x + 1, y + 1, w - 2, h - 2, YColorV("vc_white")); 
    YDrawRectB (x + 2, y + 2, w - 4, h - 4, YColorV("vc_black")); 
 
    if (wscroll) 
      YWnd (&hScroll, SCROLL, "", vert1+1,y, wscroll,h, 0,0,0,0, CLR_DEF); 
 
    YWnd (&HREND, PROC, "", x+3,y+3, w-6,h-6, 0,0,0,U->w_long4, CLR_DEF); 
 
    YWnd (&(U->hButts), BUTTS, "", vert2,h_caption, WND->w-vert2-5,h, (long)PTEXT,0,0,0, WIN->color); 
 
    break; 
  case YADD: 
    add_bts = (char **) mes3; 
    if (add_bts == NULL) break; 
 
    for (i=0; add_bts[i]!=NULL; i++)  
      (U->butts)[U->num_butts++] = add_bts[i]; 
 
    (U->butts)[U->num_butts] = NULL; 
    break; 
  case YLMOUSEDOWN: 
    YZoomMe (id); 
    break; 
  case YSC_SET: 
    if (!wscroll) 
      break; 
    YSend (hScroll, message, mes1, mes2, mes3, mes4); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  case YGETITEM: 
    return (HREND); 
  default:  
    /* YSend (HREND, message, mes1, mes2, mes3, mes4); */   
    return (YSend (HREND, message, mes1, mes2, mes3, mes4));   
  } 
 
  RETURN_TRUE; 
#undef PTEXT 
#undef BUT 
#undef HREND 
#undef PROC 
} 
/*-----------------------------BUTTS----------------------------------------*/ 
long 
BUTTS (PFUNC_VAR) 
{ 
  int     h_butts; 
 
  int     i, iy; 
  int     butt; 
  char   *butt_name; 
#define PTEXT (U->butts) 
 
  typedef struct { 
    int    num_butts; 
    char  **butts; 
  } YT_USR; 
 
  h_butts = 2 * UNITY; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
 
    U->num_butts = 0; 
    PTEXT = (char**)YGet("wlong1")/* Y_WLONG1 */; 
    if (PTEXT == NULL) break; 
 
    for (i=0; PTEXT[i]!=NULL; i++) ;;;; 
    U->num_butts = i; 
  case YDRAW: 
    YDrawRectFB (0, 0, WND->w, WND->h, WIN->color, YColorV("vc_black")); 
    YDrawRectB (1, 1, WND->w - 2, WND->h - 2, YColorV("vc_white")); 
    if (U->num_butts == 0) break;  
 
    for (i = 0; *(PTEXT + i) != NULL; i++) { 
      iy = h_butts * (i + 1); 
      YDrawLine (1, iy, WND->w - 2, iy, YColorV("vc_white")); 
      YDrawLine (0, iy + 1, WND->w - 1, iy + 1, YColorV("vc_black")); 
      YDrawRectText (*(PTEXT + i), 0, iy - h_butts, WND->w, h_butts, YColorV("vc_black")); 
    } 
    break; 
  case YLMOUSEUP: 
  case YRMOUSEUP: 
    butt = mes2 / h_butts + 1; 
    if   (butt <= U->num_butts)  butt_name = (U->butts)[butt-1]; 
    else                         butt_name = NULL; 
    if       (message==YLMOUSEUP)  YPost (WND->parent, YLBUTT, id, 0, (long)butt_name, 0); 
    else if  (message==YRMOUSEUP)  YPost (WND->parent, YRBUTT, id, 0, (long)butt_name, 0); 
    break; 
  default: ;;;; 
  } 
 
  RETURN_TRUE; 
#undef PTEXT 
} 
/*-------------------------------COMBO--------------------------------------*/ 
long 
COMBO (PFUNC_VAR) 
{ 
#define X (*(int*)(U->long3)) 
 
  typedef struct { 
    int   hList; 
    int   topp, num; 
    int   x_list, y_list, w_list, h_list; 
    int   long1, long3; 
    long  image; 
  } YT_USR; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->long1 = YGet("wlong1"); 
    U->topp = 1;	          	/* top string in list     */ 
    U->long3 = YGet("wlong3");        	/* choise string          */ 
    U->num = (int) YGet("wlong2");	/* number strings in list */ 
  case YDRAW: 
    YDrawString (WND->name, -YStringW (WND->name), WND->h / 2, YColorV("vc_black")); 
    YDrawRectFB (0, 0, WND->w, WND->h, WIN->color, YColorV("vc_black")); 
    YDrawLine (WND->w - WND->h, 0, WND->w - WND->h, WND->h, YColorV("vc_black")); 
    YDrawPointer (YDOWN, WND->w - WND->h / 2, WND->h / 2, YColorV("vc_black")); 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YREDRAW: 
    YDrawRectF (1, 1, WND->w - WND->h - 2, WND->h - 2, WIN->color); 
    YDrawRectText (*((char **) U->long1 + X - 1), 0, 0, WND->w - WND->h, WND->h, YColorV("vc_black")); 
    break; 
  case YLMOUSEDOWN: 
    U->x_list = 0; 
    U->y_list = WND->h + 1; 
    U->w_list = WND->w - WND->h; 
    U->h_list = UNITY * U->num + 3; 
    U->image = YImageGet (U->x_list, U->y_list, U->w_list, U->h_list); 
    YWnd (&U->hList, LIST, "", U->x_list,U->y_list, U->w_list,U->h_list, U->long1,0,0,0, CLR_DEF); 
    YSetMouseRegion (WND->x + U->x_list, WND->y + U->y_list, U->w_list, U->h_list, id); 
    YSetFocus (U->hList); 
    break; 
  case YMREGIONOUT: 
  case YLIST: 
    U->topp = 1;  
    X = (int) YGetData (U->hList); 
    YKillFocus (); 
    YImagePut (U->x_list, U->y_list, U->image); 
   /*  YWndClose (U->hList); */ 
    YUnWnd (U->hList); 
    YImageFree (U->image); 
    YSetMouseRegion (0, 0, 0, 0, 0); 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    YSend (WND->parent, YCOMBO, id, 0, 0, 0); 
    break; 
  case YGETDATA: 
    YError ("COMBO-YGETDATA"); 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  default: ;;; 
  } 
 
  RETURN_TRUE; 
#undef X 
} 
/*-----------------------------FLOATS---------------------------------------*/ 
long 
FLOATS (PFUNC_VAR) 
{ 
#define BUFER   (U->text1) 
#define PF      (U->long1) 
#define YTYPE   (U->long2) 
#define HEDIT   (U->hEdit) 
 
  double  dval; 
 
  typedef struct { 
    char  *text1; 
    int    hEdit; 
    long   long1, long2; 
  } YT_USR; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->long1 = YGet("wlong1")/* Y_WLONG1 */; 
    U->long2 = YGet("wlong2")/* Y_WLONG2 */; 
    if ((BUFER = (char *) malloc (20)) == NULL) 
      YError ("FLOATS-malloc"); 
  case YDRAW: 
    if      (YTYPE == YFLOAT)   sprintf (BUFER, "%.1f", *(float *) PF); 
    else if (YTYPE == YDOUBLE)  sprintf (BUFER, "%.1f", *(double *) PF); 
/*     else if (YTYPE == YSPACE)   sprintf (BUFER, "%.1f", *(YT_SPACE *) PF); */ 
    else if (YTYPE == YSPACE)   sprintf (BUFER, "%.2f", *(YT_SPACE *) PF); 
    else 
      YError ("FLOATS-YTYPE"); 
    YWnd (&HEDIT, EDIT, WND->name, 0,0, WND->w,WND->h, (long)BUFER,0,0,0, WIN->color); 
    break; 
  /* case YKEYENTER: */ 
  case YEDIT: 
    YGoto (YGETDATA, 0, 0, 0, 0); 
    YSend (WND->parent, /* YDATAMADE */YFLOATS, id, 0, 0, 0); 
    break; 
  case YGETDATA: 
    dval = atof ((char *) YGetData (HEDIT)); 
    if      (YTYPE == YFLOAT)   *(float *) PF = dval; 
    else if (YTYPE == YDOUBLE)  *(double *) PF = dval; 
    else if (YTYPE == YSPACE)   *(YT_SPACE *) PF = dval; 
    else 
      YError ("FLOATS-YTYPE"); 
    break; 
  case YCLOSE: 
    YGoto (YGETDATA, 0, 0, 0, 0); 
/*     free (BUFER); */ 
/*     YWndClose (id); */ 
    YWndClean (id); 
    free (BUFER); 
    break; 
  default: 
    break; 
  } 
 
  RETURN_TRUE; 
#undef PF 
#undef HEDIT 
#undef BUFER 
#undef YTYPE 
} 
/*--------------------------------YLayPut-----------------------------------*/ 
int 
YLayPut (int id_lay, int place, YT_PFUNC proc, char *name, 
	 long long1, long long2, long long3, long long4, YT_COLOR color) 
{ 
  int id_put; 
 
  id_put = YSend (id_lay, YPUTWND, place, 0, 
	 (long)YSetWnd (proc, name, long1, long2, long3, long4, color), 0); 
 
  return (id_put); 
} 
/*------------------------------LAYOUT--------------------------------------*/ 
long 
LAYOUT (PFUNC_VAR) 
{ 
#define  W_LAY  (WND->w) 
#define  H_LAY  (WND->h) 
#define  MAX_PUTS  10	     /* 5 */ 
 
  static int horz, vert1, vert2, d = 2, numer = 0, x_old, y_old; 
  int     r = d + 1; 
  static float k_vert1, k_vert2, k_horz; 
/*   static int w_map = W_MOD, h_map = H_MOD + 20; */ 
  int     i, x, y, w, h; 
  static YT_COLOR color_lines; 
  enum words {YDRAWWND=YKEY_LOCALS+1}; 
 
  static YT_PUTWND puts[MAX_PUTS]; 
  YT_PUTWND *p; 
  YT_DEFWND *p1, *p2; 
 
  switch (message) { 
  case YOPEN: 
     k_vert1 = 0.5 ;  
/*      k_vert2 = 0.5 ; */ 
     vert2  = YGet("wlong2"); 
     if  (vert2 == 0) k_vert2  = 0.5 ; 
     else             k_vert2  = (float) vert2 / W_LAY; 
     horz  = YGet("wlong3"); 
     if  (horz == 0) k_horz  = 0.5 ; 
     else            k_horz  = (float) horz / H_LAY; 
 
    /* SV->layout = id; */ 
    color_lines = WIN->color /* YColorV("vc_white") */; 
    for (i = 0; i < MAX_PUTS; i++) 
      puts[i].id = ID_NULL /* 0 */ ; 
    YGoto (YDRAW, 0, 0, 0, 0); 
    break; 
  case YDRAW: 
    vert1 = (int) (k_vert1 * W_LAY); 
    vert2 = (int) (k_vert2 * W_LAY); 
    horz = (int) (k_horz * H_LAY); 
    YGoto (YKEYWORD4, 0, 0, 0, 0); 
    /* YDrawRectF(0,0,WND->w,WND->h,WIN->color);  */ 
 
    for (i = 0; i < MAX_PUTS; i++) 
      if (puts[i].id != ID_NULL) 
	YGoto (YDRAWWND, i + 1, 0, 0, 0); 
 
    YGoto (YKEYWORD1, TRUE, horz,  0, 0); 
    YGoto (YKEYWORD2, TRUE, vert1, 0, 0); 
    YGoto (YKEYWORD3, TRUE, vert2, 0, 0); 
    break; 
  case YREDRAW: 
    YGoto (YKEYWORD4, 0, 0, 0, 0); 
    YSend (WND->parent, YREDRAW, 0, 0, 0, 0); 
    numer = 0; 
    break; 
  case YPUTWND: 
    p2 = (YT_DEFWND *) mes3; 
    p1 = (YT_DEFWND *) (&((puts[mes1 - 1]).wnd)); 
    *p1 = *p2; 
    return (YGoto (YDRAWWND, mes1, 0, 0, 0)); 
    /* break; */ 
  case YDRAWWND: 
    if (mes1 == 1) { 
      x = vert2 + r; 
      y = horz + r; 
      w = WND->w - vert2 - r; 
      h = WND->h - (horz + r); 
    } else if (mes1 == 2) { 
      x = vert1 + r; 
      y = 0; 
      w = WND->w - vert1 - r; 
      h = horz - r + 1; 
    } else if (mes1 == 3) { 
      x = 0; 
      y = 0; 
      w = vert1 - r + 1; 
      h = horz - r + 1; 
    } else if (mes1 == 4) { 
      x = 0; 
      y = horz + r; 
      w = vert2 - r + 1; 
      h = WND->h - horz - r; 
    } else if (mes1 == 5) { 
      x = 0; 
      y = 0; 
      w = W_LAY; 
      h = H_LAY; 
    } else if (mes1 == 6) { 
      x = 0; 
      y = 0; 
      w = W_LAY; 
      h = horz - r + 1; 
    } else 
      YError ("jhjhj"); 
 
    p = (YT_PUTWND *) (&puts[mes1 - 1]); 
    YWnd (&(p->id), (p->wnd).proc, (p->wnd).name, x,y, w,h, 
       (p->wnd).long1,(p->wnd).long2,(p->wnd).long3,(p->wnd).long4, (p->wnd).color); 
    return (p->id); 
    /* break; */ 
  case YZOOMME: 
    for (i = 0; i < MAX_PUTS; i++) 
      if (puts[i].id == mes1) 
	break; 
    p = (YT_PUTWND *) (&puts[i]); 
    YWnd (&(p->id), (p->wnd).proc, (p->wnd).name, 0,0, W_LAY,H_LAY, 0,0,0,0, (p->wnd).color); 
    break; 
  case YDESTITEM: 
   /*  YWndClose (puts[mes1 - 1].id); */ 
    YUnWnd (puts[mes1 - 1].id); 
    break; 
  case YKEYWORD1: 
    for (i = -d; i <= d; i++) 
      if (mes1)  YDrawLine    (0, mes2 + i, WND->w, mes2 + i, color_lines); 
      else       YDrawXorLine (0, mes2 + i, WND->w, mes2 + i, color_lines); 
    break; 
  case YKEYWORD2: 
    for (i = -d; i <= d; i++) 
      if (mes1)  YDrawLine    (mes2 + i, 0, mes2 + i, horz, color_lines); 
      else       YDrawXorLine (mes2 + i, 0, mes2 + i, horz, color_lines); 
    break; 
  case YKEYWORD3: 
    for (i = -d; i <= d; i++) 
      if (mes1)  YDrawLine    (mes2 + i, horz, mes2 + i, WND->h, color_lines); 
      else       YDrawXorLine (mes2 + i, horz, mes2 + i, WND->h, color_lines); 
    break; 
  case YKEYWORD0: 
    if      (numer == 1)  YGoto (YKEYWORD1, FALSE, y_old, 0, 0); 
    else if (numer == 2)  YGoto (YKEYWORD2, FALSE, x_old, 0, 0); 
    else if (numer == 3)  YGoto (YKEYWORD3, FALSE, x_old, 0, 0); 
    break; 
    /*case YLMOUSEDOUBLE: 
    horz = WND->h - 10 - h_map - r; 
    vert1 = WND->w / 2; 
    vert2 = WND->w - 10 - w_map - r; 
    YGoto (YREDRAW,"", 0, 0, 0); 
    break;*/ 
  case YLMOUSEDOWN: 
    if      (mes2 > horz - d && mes2 < horz + d)  numer = 1; 
    else if (mes2 < horz)                         numer = 2; 
    else                                          numer = 3; 
    x_old = mes1; 
    y_old = mes2; 
    YGoto (YKEYWORD0, x_old, y_old, 0, 0); 
    break; 
  case YLMOUSEDRAG: 
    YGoto (YKEYWORD0, x_old, y_old, 0, 0); 
    x_old = mes1; 
    y_old = mes2; 
    YGoto (YKEYWORD0, x_old, y_old, 0, 0); 
    break; 
  case YLMOUSEUP: 
    YGoto (YKEYWORD0, x_old, y_old, 0, 0); 
    numer = 0; /*  !!?? */ 
    if      (numer == 1)  horz = y_old; 
    else if (numer == 2)  vert1 = x_old; 
    else if (numer == 3)  vert2 = x_old; 
    else 
      break; 
    YGoto (YREDRAW, 0, 0, 0, 0); 
    break; 
  case YKEYWORD4: 
    k_horz  = (float) horz  / H_LAY; 
    k_vert1 = (float) vert1 / W_LAY; 
    k_vert2 = (float) vert2 / W_LAY; 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  }   
 
  id++; 
  RETURN_TRUE; 
#undef  W_LAY   
#undef  H_LAY  
} 
/*-----------------------------PROCESS--------------------------------------*/ 
long 
PROCESS (PFUNC_VAR) 
{ 
#define id_proc (U->int1) 
  enum keys { 
    MY_BEGIN = YKEY_LOCALS + 1, MY_STEP, MY_END 
  }; 
 
  typedef struct { 
    int  int1; 
  } YT_USR; 
 
  static int hStart, hStop, hContinue, hStep; 
  static YT_BOOL bBegin = FALSE; 
  int     j, d, pushs_num, w, h, pushw, pushh; 
/*   int     id_proc = U->int1; */ 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
  case YDRAW: 
    /* WIN *//* U->int1 */id_proc = (int) YGet("wlong1")/* Y_WLONG1 */; 
    w = WND->w; 
    h = WND->h; 
    d = 10; 
    pushs_num = 4; 
    pushw = (w - 10 - (pushs_num + 1) * d) / pushs_num; 
    pushh = h - 30; 
 
    YDrawRectF (0, 0, w, h, WIN->color); 
    /* YBeginGroup (WND->name, 5, 5, w - 10, h - 10, YColor("")); */ 
    YBeginGroup (WND->name, 0, 0, w, h, YColor("")); 
 
    YWnd (&hStart,    PUSH, "Start",j=d,         10, pushw,pushh, 0,0,0,0, CLR_DEF); 
    YWnd (&hStop,     PUSH, "Stop", j+=(pushw+d),10, pushw,pushh, 0,0,0,0, CLR_DEF); 
    YWnd (&hContinue, PUSH, "Next", j+=(pushw+d),10, pushw,pushh, 0,0,0,0, CLR_DEF); 
    YWnd (&hStep,     PUSH, "Step", j+=(pushw+d),10, pushw,pushh, 0,0,0,0, CLR_DEF); 
    YEndGroup (); 
    break; 
  case MY_BEGIN: 
    YSend (id_proc, PROCESS_BEGIN, 0, 0, 0, 0); 
    bBegin = TRUE; 
    break; 
  case MY_STEP: 
    if (!bBegin) 
      YGoto (MY_BEGIN, 0, 0, 0, 0); 
    YSend (id_proc, PROCESS_STEP, 0, 0, 0, 0); 
    break; 
  case MY_END: 
    YSend (id_proc, PROCESS_END, 0, 0, 0, 0); 
    YKillProcess (); 
    bBegin = FALSE; 
    break; 
  case YPUSH: 
    if (mes1 == hStart) { 
      YGoto (MY_END, 0, 0, 0, 0); 
      YGoto (MY_BEGIN, 0, 0, 0, 0); 
      YSetProcess (id_proc, PROCESS_STEP); 
    }  
    else if (mes1 == hStop)      YKillProcess (); 
    else if (mes1 == hContinue)  YSetProcess (id_proc, PROCESS_STEP); 
    else if (mes1 == hStep)      YGoto (MY_STEP, 0, 0, 0, 0); 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  } 
 
  RETURN_TRUE; 
#undef id_proc 
} 
/*---------------------------------read_menu---------------------------*/ 
char ** 
read_menu (YT_MENU **men, char **text, int num) 
{ 
  char   **str, *part1, *prom; 
  int    num1, i; 
  YT_MENU *mass, *menu; 
  char rab_str[200]; 
  str = text; 
 
 /*  menu = (YT_MENU*)malloc(sizeof(YT_MENU)*20); */ 
  menu = (YT_MENU*)calloc(20, sizeof(YT_MENU)); 
  *men = menu; 
  for (i = 1; *str!=NULL && i<20; i++){ 
    num1 = 0; 
  /*   prom = *str; */ 
    strcpy (rab_str, *str); 
    prom = &rab_str[0]; 
    while(*prom ++ == ' ') 
      num1++; 
    if(num1<num){ 
      menu++; 
      *(menu->string) = '\0'; 
      return str; 
    }else if(num1>num){ 
	(menu-1)->is_menu = TRUE; 
	str = read_menu(&mass, str, num1);  
	(menu-1)->long1 = (long)mass; 
    }else { 
      menu->is_string = TRUE; 
      menu->is_check = FALSE; 
      menu->is_gray= FALSE; 
      part1 = strtok(prom-1, "|"); 
      strcpy(menu->string, part1); 
      if(strncmp(part1, "--", 2)==0)   
	menu->is_string = FALSE; 
      while((part1 = strtok(NULL, " "))){ 
	YSWITCH (part1) { 
	  YCASE("gray") 
	    menu->is_gray = TRUE; 
	  YCASE("true") 
	    menu->is_check = TRUE; 
	  YDEFAULT  
	    } 
      } 
      if(num1 == num){ 
	menu->is_menu = FALSE; 
	menu++; 
	str++; 
      }else 
	printf("Error in read_menu()\n");  
    } 
  } 
  return 0; 
} 
/*--------------------------------YTextToMenu-----------------------------*/ 
YT_DMENU * 
YTextToMenu (char **text) 
{ 
#define DMENU_MAX  20 
  char   **str, *prom, *part1;  
  YT_DMENU *dm; 
  YT_MENU *m; 
  static YT_DMENU dmenu[DMENU_MAX]; 
  int num_next=0, i;  
  char rab_str[200]; 
 
  str= text; 
  for (i = 0; ** str!= '\0' && i<DMENU_MAX; i++){ 
    num_next = 0; 
 /*    prom = *str; */ 
    strcpy (rab_str, *str); 
    prom = &rab_str[0]; 
    while(*prom ++ == ' ') 
      num_next++; 
    if(num_next == 0){ /* Dmenu */ 
      dm = (YT_DMENU*)malloc(sizeof(YT_DMENU)); 
      part1 = strtok(prom-1, "|"); 
      strcpy(dm->name, part1); 
      str++; 
      prom = *str; 
      num_next = 0; 
      while(*prom ++ == ' ') 
	num_next++; 
      if(num_next != 0){ 
	str = read_menu(&m, str, num_next); 
	dm->items = m; 
      }else 
	dm->items = NULL; 
      dmenu[i] = *dm; 
    }else{ 
       printf("Can't find first element\n"); 
       * dmenu[i].name = '\0';  
       return NULL; 
    } 
  } 
  * dmenu[i].name = '\0';  
 
  return (&dmenu[0]); 
} 
/*---------------------------------FindItem-----------------------------------*/ 
YT_MENU * 
FindItem(YT_DMENU *dmenu, char * string) 
{ 
  char buf[100], *part; 
  YT_DMENU * dm; 
  YT_MENU * mm; 
 
  strcpy(buf, string); 
  part = strtok(buf, "/"); 
  for (dm=dmenu; *(dm->name) != '\0'; dm++){ 
    if(strcmp(dm->name, part) == 0){ 
      mm = dm->items; 
      while((part = strtok(NULL, "/"))){ 
	while(*(mm->string) != '\0'){ 
	  if(strcmp(mm->string, part) == 0){ 
/* 	    if(mm->long1 != NULL) */ 
	    if(mm->long1 != YNULL) 
	      mm = (YT_MENU*)mm->long1; 
	    break; 
	  }else 
	    mm ++; 
	} 
      } 
      return mm; 
    } 
  } 
  return NULL; 
} 
/*-------------------------------F_TOPMENU_CB------------------------------------*/
#if defined(FLTK_CONTROLS) 
void 
F_TOPMENU_CB (Fl_Widget *w, void *pid) 
{  
  Fl_Menu_* mw = (Fl_Menu_*)w;
  const Fl_Menu_Item* m = mw->mvalue();

	int id = *(int*)pid;
	printf ("F_TOPMENU_CB: %s \n", m->label());
	YSend (WNDI(id)->parent, YMENU, id, 0, (long)(m->label()), 0);
	

} 
/*------------------------------NataToFltk--------------------------------------*/
Fl_Menu_Item *
NataToFltk (YT_DMENU *dmenu)
{
	Fl_Menu_Item *fltk_menu = NULL;


	return (fltk_menu);
}                                                                              
/*-------------------------------F_TOPMENU---------------------------------------*/
long
F_TOPMENU (PFUNC_VAR)
{
#define OB ((Fl_Menu_Bar*)(WND->fltk_o))

static Fl_Menu_Item min_menu[] = {
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

	int x, y;
	static int f = 0;
	YT_DMENU *dmenu; 
	Fl_Menu_Item *fltk_menu;


  switch (message) {
  case YOPEN:
	x = 0;
	y = 0;
	CALCXY (x, y); 

	WND->fltk_o = (long) new Fl_Menu_Bar (x, y, WND->w, WND->h);                       
	OB->callback(F_TOPMENU_CB, WND->pid);
	OB->color(WND->color);
	OB->selection_color(FL_BLUE);
	OB->textfont(3);
	OB->textsize(Y_FONT_SIZE);

    dmenu = YTextToMenu((char **)YGet("wlong1")); 
	fltk_menu = NataToFltk (dmenu);

	OB->menu(min_menu);

	fltkwin->add(OB);

  case YDRAW:
	OB->redraw();
    break;
  case YCLOSE:
	fltkwin->remove(OB);
    break;
  default: ;;;;
  }

  RETURN_TRUE;
#undef OB
}
#endif
/*-----------------------------TOPMENU----------------------------------------*/ 
long 
TOPMENU (PFUNC_VAR) 
{ 
#define ITEMS  (U->dmenu) 
#define hMenu  (U->hMenu_usr) 
#define marg   (U->marg_usr)  
#define x      (U->x_usr) 
#define x0     (U->x0_usr) 
#define x_last (U->x_last_usr) 
#define item   (U->item_usr) 
 
  typedef struct { 
    int marg_usr, x_usr, y_usr, x0_usr, hMenu_usr, x_last_usr; 
    char item_usr[20]; 
    YT_DMENU *dmenu; 
  } YT_USR; 
 
  char *str; 
  YT_DMENU *it; 
  YT_MENU * m; 
  int i=0, y; 
  static char big_string[100]; 
 
  switch (message) { 
  case YOPEN: 
    U_MALLOC; 
    U->dmenu = YTextToMenu((char **)YGet("wlong1"));    
    hMenu=ID_NULL; 
    marg = 7; 
  case YDRAW: 
  case YREDRAW: 
    YDrawRectFB (0, 0, WND->w, WND->h, WIN->color, YColor("black")); 
    y = WND->h/2; 
    x = 0; 
    for (it=ITEMS; *(it->name) != '\0'; it++) {  
      str = it->name; 
      YDrawString(str, x+marg, y, YColor("black")); 
      x = x + YStringW(str) + 2*marg; 
      YDrawLine(x, 0, x, WND->h-1, YColor("black"));  
    } 
    x_last = x; 
    break; 
  case YMENU: 
    strcpy (big_string, item); 
    strcat (big_string, "/"); 
    strcat (big_string,(char*) mes3); 
/*     YSend (WND->parent, message, mes1, mes2, (long)big_string, 0);   */ 
    /* YSend (WND->parent, message, id, mes2, (long)big_string, 0);   */ 
    YPost (WND->parent, message, id, mes2, (long)big_string, 0);   
    YSend (hMenu, YCLOSE, 0, 0, 0, 0);  
    hMenu=ID_NULL; 
    break; 
  case YJUMP: 
/*   case YK_RIGHT: */ 
    if(x >= x_last) 
      x = 1; 
    YGoto (YLMOUSEDOWN, x, 0, 0, 0); 
    break; 
/*   case YK_LEFT: */ 
  case YLEFT: 
    if(x0 == 0)  
      x0 = x_last; 
    YGoto (YLMOUSEDOWN, x0-1, 0, 0, 0); 
    break; 
  case YSET:  
    m = FindItem(U->dmenu, (char *)mes4); 
    if(m == NULL) 
      printf("Don't find. (TOPMENU, YSET)\n"); 
    else 
      m->is_gray = (YT_BOOL)mes1; 
    break; 
  case YLMOUSEDRAG: 
    if(x0 < mes1 && mes1 < x){ 
      break; 
    }else 
      if(mes1 <= 0) 
	break; 
    if(WND->h < mes2 || mes2 < 0) 
      break; 
  case YLMOUSEDOWN: 
    x = 0; 
    for (i=0,it=ITEMS; it->name  != NULL; i++,it++) {  
      x0 = x; 
      str = it->name; 
      x = x + YStringW(str) + 2*marg; 
      if(mes1 < x) { 
	if (hMenu!=ID_NULL)  YSend (hMenu, YCLOSE, 0, 0, 0, 0);  
	if(it->items != NULL){ 
	  /* YSetMult (1, 1); */ 
	  YWnd (&hMenu, MENU, "", x0, WND->h, x-x0+3, 0, (long)(it->items), 0,0,0, WIN->color); 
	  YSetFocus(hMenu); 
	}else 
	  hMenu = ID_NULL; 
	strcpy(item, it->name); 
	break; 
      } 
    } 
    break; 
  case YCLOSE: 
    YWndClean (id); 
   /*  YWndClose (id); */ 
    break; 
  default: ;;;; 
  } 
 
  RETURN_TRUE; 
#undef ITEMS 
#undef hMenu  
#undef marg  
#undef x    
#undef x0   
#undef x_last  
#undef item  
} 
/*-------------------------------MENU------------------------------------*/ 
long 
MENU (PFUNC_VAR) 
{ 
#define PMENU ((YT_MENU *)U->w_long1) 
 
  static YT_GETSIZE gs = {FALSE, 0, 0}; 
  YT_MENU * item; 
 
  typedef struct { 
    int   sel, hSub, hListRab; 
    char  str [20]; 
    long  w_long1; 
    long  image; 
  } YT_USR; 
 
  static char big_string[100]; 
  char prom_str[100]; 
 
  switch (message){ 
  case YOPEN: 
    U_MALLOC; 
    U->sel = 1; 
    U->hSub = ID_NULL; 
    /* WIN */U->w_long1 = YGet("wlong1")/* Y_WLONG1 */; 
 
/*     WIN->x_mult = 1; */ 
/*     WIN->y_mult = 1; */ 
    /* YSetMult (WIN->x_mult, WIN->y_mult); */ 
    YSetMult (1, 1); 
 
    CALL (LIST_RAB , 0, from, YKEYWORD1, 1, 0, (long)PMENU, 0);  
    CALL (LIST_RAB , 0, from, YKEYWORD1, 2, 0, LP(gs), 0); 
    if(gs.w > WND->w) 
      WND->w = gs.w;  /* */ 
    WND->h = gs.h; 
 
    U->image = YImageGet (0, 0, WND->w, WND->h);  
  case YDRAW: 
    YWnd (&(U->hListRab), LIST_RAB, "", 0, 0, WND->w, WND->h,  
	  (long)PMENU, LP(U->sel), 0, 0, WIN->color);   
    strcpy (big_string, ""); 
    break; 
  case YNEW: 
    if(U->hSub != ID_NULL){ 
      YSend (U->hSub, YCLOSE, 0, 0, 0, 0); 
      YSetFocus (U->hListRab); 
      U->hSub = ID_NULL; 
    } 
    if((PMENU+mes3-1)->is_menu){ 
      strcpy (U->str, (PMENU+mes3-1)->string); 
      YSetMult(1, 1); 
      YWnd (&(U->hSub), MENU, "", WND->w, mes2, WND->w, WND->h,  
	    (PMENU+mes3-1)->long1, 0, 0, 0, WIN->color);    
    } 
    break; 
  case YSETFOCUS: 
    YSetFocus(U->hListRab);  
   break; 
/*   case YK_RIGHT:  */ 
  case YRIGHT:  
    if(U->hSub != ID_NULL){ 
      YSetFocus (U->hSub); 
      break; 
    }else{ 
      YSend (WND->parent, YJUMP, 0, 0, 0, 0);  
      break; 
    } 
/*   case YK_LEFT: */ 
  case YLEFT: 
    if(U->hSub != ID_NULL){ 
      YSend (U->hSub, YCLOSE, 0, 0, 0, 0); 
      YSetFocus (U->hListRab); 
      U->hSub = ID_NULL; 
      break; 
    } 
  case YJUMP: 
    YSend (WND->parent, message, mes1, mes2, mes3, mes4);   
    break; 
  case YMENU: 
    strcpy(prom_str, big_string); 
    strcpy (big_string, U->str); 
    strcat (big_string, "/"); 
    strcat (big_string, prom_str); 
    YSend (WND->parent, message, mes1, mes2, (long)big_string, 0);   
/*     YPost (WND->parent, message, mes1, mes2, (long)big_string, 0);   */ 
    break; 
  case YDATAMADE: 
    item = PMENU+mes3-1; 
    if(item->is_check) 
      item->long1 = !((YT_BOOL)item->long1); 
 
    if(!(item->is_menu)) {  
      strcpy (big_string, item->string); 
      YSend (WND->parent, YMENU, id, 0, (long)item->string, 0); 
    } 
    break; 
   case YCLOSE: 
/*     YCloseChild (id); */ 
    YWndClean (id); 
 
   /*  YImagePut (-1, -1, U->image); */ 
    YImagePut (0, 0, U->image); 
    YImageFree (U->image); 
/*     YWndCloseMe (id); */ 
    break; 
  default:  ;;; 
  } 
 
  RETURN_TRUE; 
#undef PMENU 
} 
/*-----------------------------YMeta_dlg------------------------------------*/ 
long 
YMeta_dlg (PFUNC_VAR) 
{ 
  static int hOk, hCancel; 
  static char fname[50]; 
  static int hEdit; 
  /* static YT_META *meta; */ 
  static void *meta; 
  static int xx = 0, xx_min = 0, xx_max = 300, yy = 0, yy_min = 0, yy_max = 300; 
  static YT_BOOL is_xy_p16 = FALSE, is_xy_p12 = FALSE; 
  static int hXX, hYY; 
 
  char *p_fname; 
 
  switch (message) { 
  case YGETSIZE: 
    MYSIZE_IS (W_DLG, H_DLG); 
    break; 
  case YOPEN: 
    /* meta = (YT_META *) YGet("wlong1"); */ 
    meta = (void *) YGet("wlong1"); 
 
    if (!meta) { 
      YBeep (); 
      strcpy (fname, ""); 
    } else { 
      p_fname = YMetaGetInfo (meta, &xx, &yy); 
      strcpy (fname, p_fname); 
/*       strcpy (fname, meta->save_fname); */ 
/*       xx = meta->save_xx; */ 
/*       yy = meta->save_yy; */ 
    } 
    YGoto (YKEYWORD3, 0, 0, 0, 0); 
  case YDRAW: 
    YDrawRectF (0, 0, WND->w, WND->h, WIN->color); 
    YWnd (&hEdit, EDIT, "", 25,15, WND->w-50,20, (long)fname,0,0,0, CLR_DEF); 
 
    YBeginGroup ("Size (mm)", 25, 50, 125, 85, YColor("")); 
    YWnd (&hXX, DECIMAL, "", 10,10, 40,20, 0,xx_min,xx_max,LP(xx), CLR_DEF); 
    YWnd (&hYY, DECIMAL, "", 50,10, 40,20, 0,yy_min,yy_max,LP(yy), CLR_DEF); 
    YWndGroupBegin (); 
      YWnd (/* PH */Ph(), CHECK, "1:2 page", 10,35, 20,20, 0,LP(is_xy_p12),0,0, CLR_DEF); 
      YWnd (/* PH */Ph(), CHECK, "2:6 page", 10,60, 20,20, 0,LP(is_xy_p16),0,0, CLR_DEF); 
    YWndGroupEnd (); 
    YEndGroup (); 
 
    YWnd (&hOk,     PUSH, "OK",     WND->w/2-70,145, 60,20, 0,0,0,0, CLR_DEF); 
    YWnd (&hCancel, PUSH, "Cancel", WND->w/2+10,145, 60,20, 0,0,0,0, CLR_DEF); 
    break; 
  case YCHECK: 
    YGoto (YKEYWORD2, 0, 0, 0, 0); 
    YWndUpdate (hXX); 
    YWndUpdate (hYY); 
    break; 
  case YKEYWORD2: 
    if (is_xy_p12) { 
      xx = 140; 
      yy = 110; 
    } else if (is_xy_p16) { 
      xx = 90; 
      yy = 70; 
    } 
    break; 
  case YPUSH: 
    if (mes1 == hCancel) 
      YDlgEnd (FALSE); 
    else if (mes1 == hOk) { 
      strcpy (fname, (char *) YSend (hEdit, YGETDATA, 0, 0, 0, 0)); 
      if (!meta) { 
	YBeep (); 
	YDlgEnd (TRUE); 
      } 
      YMetaPutInfo (meta, fname, xx, yy); 
/*       meta->save_fname = fname; */ 
/*       meta->save_xx = xx; */ 
/*       meta->save_yy = yy; */ 
      YGoto (YKEYWORD2, 0, 0, 0, 0); 
      YDlgEnd (TRUE); 
    } 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  } 
 
  RETURN_TRUE; 
} 
/*------------------------------SCREEN------------------------------------*/ 
long 
SCREEN (PFUNC_VAR) 
{ 
  typedef struct { 
    void   *meta; 
    char   *butts[15]; 
    int     x, y, w, h; 
    int     id_from; 
  } YT_USR; 
 
  YT_RECT *rect; 
  static char   *butts[] = { "Print","", NULL}; 
 
  switch (message) { 
  case YMETADRAW: 
    U->id_from = mes2; 
    U->meta = (void *) mes3; 
    YGoto (YREDRAW, mes1, 0, 0, 0); 
    break; 
  case YCREATE: /* !!! */ 
    break; 
  case YOPEN: 
    U_MALLOC; 
    U->meta = NULL; 
    U->id_from = ID_NULL; 
 
    WIN->color = YColor("white"); 
  case YDRAW: 
    U->x = 5; 
    U->y = 5; 
    U->w = WND->w - 10; 
    U->h = WND->h - 10; 
    YDrawRectF (0,0, WND->w,WND->h, WIN->color); 
    YGoto (YREDRAW, TRUE, 0, 0, 0); 
    break; 
  case YGETRECT: 
    rect = (YT_RECT *) mes3; 
    rect->x = U->x; 
    rect->y = U->y; 
    rect->w = U->w; 
    rect->h = U->h; 
    break;; 
  case YREDRAW: 
    if (!U->meta) 
      break; 
    YSetCliping (TRUE, 0, 0, WND->w, WND->h); 
    YMetaToScr (U->meta, U->x, U->y, U->w, U->h, mes1, WIN->color); 
    YSetCliping (FALSE, 0, 0, 0, 0); 
    break; 
  case YLBUTT: 
    YSWITCH ((char*)mes3/* phrase */) { 
    YCASE ("Print")   /* ???  */  
      if (!YDlg (YMeta_dlg, "Save to PostScript", 0,0,0,0, (long)U->meta,0,0,0, CLR_DEF, TRUE))  
	break; 
      if (U->meta == NULL) 
	break; 
      YMetaToPS (U->meta); /* ??? , file_name */  
    YCASE ("++") 
      U->w = U->w * 1.2; 
      U->h = U->h * 1.2; 
      YGoto (YREDRAW, TRUE, 0, 0, 0); 
    YCASE ("--") 
      U->w = U->w / 1.2; 
      U->h = U->h / 1.2; 
      YGoto (YREDRAW, TRUE, 0, 0, 0); 
    YDEFAULT 
      YSend (U->id_from, message, mes1, mes2, mes3, mes4); 
    } 
    break; 
  case YRBUTT: 
    YSWITCH ((char*)mes3/* phrase */) { 
    YCASE ("++") 
      U->x += 20; 
      U->y += 20; 
      YGoto (YREDRAW, TRUE, 0, 0, 0); 
    YCASE ("--") 
      U->x -= 20; 
      U->y -= 20; 
      YGoto (YREDRAW, TRUE, 0, 0, 0); 
    YDEFAULT 
      YSend (U->id_from, message, mes1, mes2, mes3, mes4); 
    } 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  case YGETSIZE: 
    break; 
  case YGETCOMS: 
    *((long*)mes3) = (long)butts; 
    break; 
  default: 
    YSend (U->id_from, message, mes1, mes2, mes3, mes4); 
  } 
 
  RETURN_TRUE; 
} 
/*---------------------------------WAIT-------------------------------------*/ 
long 
WAIT (PFUNC_VAR) 
{ 
  static char string[50]; 
  static int hString, hDone; 
  static int done; 
  static char done_s[20]; 
 
  int    wait_done; 
  /* char  *wait_string; */ 
 
  switch (message) { 
  case YGETSIZE: 
    MYSIZE_IS (W_DLG, H_DLG); 
    break; 
  case YOPEN: 
    done = 0; 
    sprintf (done_s, "%d", done); 
    strcpy (string, MORE->WAIT_string); 
  case YDRAW: 
    YDrawRectF (0, 0, WND->w, WND->h, WIN->color); 
    YWnd (&hString, YTEXT, "",             50,25, 140,20, (long)string,0,0,0, CLR_DEF); 
    YDrawRectB (25, 65, 190, 3, YColorV("vc_black")); 
    YWnd (&hDone,   YTEXT, "Done (%) : ", 120,80, 70,20, (long)done_s,0,0,0, CLR_DEF); 
    YDrawRectB (25, 110, 190, 3, YColorV("vc_black")); 
 
    YWnd (Ph(), PUSH, "Cancel", 90,140, 70,20, 0,0,0,0, CLR_DEF); 
    break; 
  case YREDRAW: 
    wait_done   = mes1; 
    /* wait_string = (char*)mes3; */ 
 
    if (done != wait_done) { 
      done = wait_done; 
      sprintf (done_s, "%d", done); 
      YWndUpdate (hDone); 
    } 
    break; 
  case YCLOSE: 
    YWndClean (id); 
    break; 
  case YPUSH: 
    MORE->WAIT_cancel = TRUE; 
    break; 
  default: ;;;; 
  } 
 
  RETURN_TRUE; 
} 
/*-------------------------------YPtInWind----------------------------------*/ 
YT_BOOL 
YPtInWind (int x0, int y0, int x, int y, int w, int h) 
{ 
  CALCXY (x0, y0); 
  CALCXY (x, y); 
  CALCWH (w, h); 
  return (YPtInRect (x0, y0, x, y, w, h)); 
} 
/*****************************************************************************/ 
 
 
#endif 
 
 
/*****************************************************************************/ 
