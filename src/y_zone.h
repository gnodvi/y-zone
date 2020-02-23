/****************************************************************************
 *                                                                          *
 *  y_zone.h  -- declarations for "Library Yzone";                          *  
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
           
#ifndef _Y_ZONE_H
#define _Y_ZONE_H
 
#ifdef __cplusplus
extern "C" {
#endif

/****************************************************************************/

#include  <stdio.h>
#include  <stdlib.h>  
#include  <string.h>

#include  <math.h>
#include  <time.h>  
#include  <ctype.h>  
  

/****************************************************************************
 *                                                                          *
 *                 Declarations for section [OPEN]                          *  
 *                                                                          *
 ****************************************************************************
 */                                                                          

typedef  int     YT_BOOL;                                                          
typedef  double  YT_SPACE;

#define  YNULL  0
#define  FALSE  0                                                              
#define  TRUE   1  
#define  YERROR(str)  {fprintf (stderr, "%s \n"); exit (EXIT_FAILURE);}
typedef  size_t  YT_SIZET;
                                                           
#define  YMAXSPACE 1.00E+30
#define  YMINSPACE 1.00E-30 

#define  YMAX(a,b)  (((a) > (b)) ? (a) : (b))
#define  YMIN(a,b)  (((a) < (b)) ? (a) : (b))
#define  YABS(x)    (((x) < (0)) ? (-x) : (x))
#define  YRAND_S    srand((unsigned)time(NULL))
#define  YRAND(imin,imax) (imin+rand()%(imax-imin+1))

int      YInt (float);
YT_BOOL  YPtInRect (int, int, int, int, int, int);

float    YRandF (float, float);
void     YBeep (void);
char   **YDirRead (void);
YT_BOOL  YDirChange (char *);
char*    YTimeString (void);
char*    YTimeStringMin (void);
long     YTimeGet (void);
void     YTimePause (long);

/****************************************************************************/

typedef struct {
  int     id_last;
  int     x0_old, y0_old;
  float   old_xmult, old_ymult;
  int     id_dialog;
} YT_CONTEXT;

#define YKEY_LEVEL0    10
#define YKEY_LEVEL1  1000
#define YKEY_LEVEL2  2000
#define YKEY_LEVEL3  3000
#define YKEY_LEVEL4  4000
#define YKEY_LEVEL5  5000
#define YKEY_LOCALS  9000

#define YKEY_DEFINE 10000

enum keys_open_y { 
  YLEVEL0_BEGIN = YKEY_LEVEL0, 

  YCREATE, YOPEN, YCLOSE, YDESTROY, YFINAL, 
  YCOLORED, YGRAYED, YINVERSE, 

  YGETDATA, YSETFOCUS, YKILLFOCUS, 
  YDRAW,  YDRAWITEM, YREDRAW, YGETSIZE, YUPDATE,                                                          
  YGET, YGETITEM, YMREGIONOUT, YZOOMME, 

  YKEYWORD0, YKEYWORD1, YKEYWORD2, YKEYWORD3, YKEYWORD4, 
  YTIMER, PROCESS_BEGIN, PROCESS_STEP, PROCESS_END,
  YLMOUSEDOWN, YLMOUSEUP, YLMOUSEDRAG, 
  YRMOUSEDOWN, YRMOUSEUP, YRMOUSEDRAG, 
  YLMOUSEDOUBLE, YLRMOUSEDRAG,
  YKEYBOARD,
  
  YLEVEL0_END 
};


#define  ID_NULL   20000 
#define  LP(val) (long)&(val)
                                                            
#define  PFUNC_PAR_OLD   int, int, int, int, long, long
#define  PFUNC_PAR   int, int, int, int, int, long, long
#define  PFUNC_VAR   int id, int from, int message, int mes1, int mes2, long mes3, long mes4 
#define  PFUNC_VAL   id, from, message, mes1, mes2, mes3, mes4 
typedef  long (*YT_PFUNC) (PFUNC_PAR);                             

typedef unsigned long YT_COLOR; 

#define  YLINER(x,x1,y1,x2,y2) ((y1)+(float)((x)-(x1))*((y2)-(y1))/((x2)-(x1)))
#define  RETURN_TRUE { from++; mes1++; mes2++; mes3++; mes4++; return(TRUE); }
#define  CALL(proc, id,from,message,mes1,mes2,mes3,mes4) ((*(proc))(id,from,message,mes1,mes2,mes3,mes4))

long     MAIN  (PFUNC_PAR);
YT_BOOL  YMainParse (int argc, char **argv); 

int     *Ph (void);                                                                  
void     YBigWindow (int *, YT_PFUNC, char *, int,int, int,int, long,long,long,long, YT_COLOR);

void     YWnd (int *, YT_PFUNC, char *, int,int, int,int, long,long,long,long, YT_COLOR); 
void     YWndCreate (int *, YT_PFUNC, char *, long, long, long, long);
void     YWndOpen (int, int, int, int, int, YT_COLOR);
void     YWndClose (int);                                                          
void     YWndDelete (int id);  
 
#define  YUnWnd YWndDelete
void     YWndFree (int id);
void     YModClean (int parent_c);
void     YWndClean (int parent);

int      YFindModByName (char *name);
YT_PFUNC YProcByName(char *proc_name);

void     YWndGroupBegin (void);
void     YWndGroupEnd (void);
void     YZoomMe (int id);

long     YSendFrom (PFUNC_PAR);
YT_BOOL  YPostFrom (PFUNC_PAR);
long     YSend (PFUNC_PAR_OLD);
YT_BOOL  YPost (PFUNC_PAR_OLD);   
long     YGoto (int, int, int, long, long);

void     YSetTimer (int, long);
void     YKillTimer (void);
void     YSetProcess (int, int);
void     YKillProcess (void);   

long     YGet_DPY (void);
long     YGet_SCR (void);
long     YGet_WIN (void);
void     YExit (long);

void     YCheckSystem (void);

/* typedef struct { */
/* } YT_WINDOW_S; */

#define MAX_CHILD  40
typedef struct {
  int      *pid;
  int       numer;
  int       ch[MAX_CHILD], ch_i;
  YT_PFUNC  proc;
  char     *name;
  int       parent_c;
  int       parent;
  void     *usr;  
  /* void     *sys; */
  /* ------ */
  YT_BOOL   opened;
  int       x, y, w, h;
  float     x_mult, y_mult;
  /* ------ */
  YT_COLOR  color;
  long      image;
  int       id_goto;
  int       bigw_i;
  long      long1,   long2,   long3,   long4;
  int       int1,    int2,    int3,    int4;
  long      w_long1, w_long2, w_long3, w_long4;
  /* ------ */
	long      fltk_o;
	long      ywin;
} YT_WINDOW;

#define  WINI_X_MULT(id) ((WNDI((id)))->x_mult)
#define  WINI_Y_MULT(id) ((WNDI((id)))->y_mult)

#define  WNDI(i)  ((YT_WINDOW*)(PWINDOW(i)))
#define  WND      (WNDI(Y_IDLAST))

#define  U   ((YT_USR*)(WND->usr))
#define  U_MALLOC   {if (!(WND->usr = malloc(sizeof(YT_USR)))) YError("usr");}

#define  FLAI_BEG    1
#define  FLAI_NUM  300 
#define  FLAI_END  (FLAI_BEG+FLAI_NUM-1)

#define  PWINDOW(i)   (KERN->wnd[(i)-FLAI_BEG])

#define  CLR_MAX     256
#define  CLR_DEF   CLR_MAX+1

#define  MYSIZE_IS(ww,hh)  Mysize_is(mes3,ww,hh)
void     Mysize_is (long mes3, int ww, int hh);

int      YGetFocus (void);
void     YSetFocus (int);
void     YWndUpdate (int);
long     YGetData (int);
void     YMouseHide (void);
void     YMouseShow (void);
void     YMouseForm (char *);

#define  YK_NO	       0
#define  YK_ENTER      13
#define  YK_ESCAPE     27
#define  YK_SPACE      32
#define  YK_DELETE      1
#define  YK_PAGEUP      2
#define  YK_PAGEDOWN    3
#define  YK_UP          4  
#define  YK_DOWN        5
#define  YK_RIGHT       6
#define  YK_LEFT        7
#define  YK_BACK        8
#define  YK_TAB         9
#define  YK_HOME       10
#define  YK_END        11

#define  CLR_NULL  CLR_MAX+2	/*not real color! */
#define  CLR_XOR   CLR_MAX+3

void     YKillFocus (void);
void     YSetMouseRegion (int, int, int, int, int);

void     YDrawContext (YT_BOOL is_begin, int id);
void     YSetOrigin (int, int);
void     YGetOrigin (int *, int *);
void     YGetContext (YT_CONTEXT *s);
void     YSetContext (YT_CONTEXT *s);

void     YSetMult (float, float);
void     YGetMult (float *, float *);

YT_COLOR YColorV(char *virt_name);
void     YColorToRGB (YT_COLOR color, int *r, int *g, int *b);
YT_COLOR YColor (char *);
YT_COLOR YColer (int);
YT_COLOR YScale(int n);
YT_COLOR YGetColor (int x, int y);
int      YColorToScale (YT_COLOR);

void     YSetCliping (YT_BOOL, int, int, int, int);

void     YPaintPixel (int, int, YT_COLOR);
void     YPaintLine (int x1, int y1, int x2, int y2, YT_COLOR color);
void     YPaintRectF (int x, int y, int w, int h, YT_COLOR f_color);
void     YPaintRectB (int, int, int, int, YT_COLOR);
void     YPaintRectFB (int, int, int, int, YT_COLOR, YT_COLOR);
void     YPaintPolyF (int, int *, YT_COLOR);
void     YPaintString (char *text, int x, int y, YT_COLOR color);
void     YPaintArcF (int, int, int, int,  int, int, YT_COLOR);
void     YPaintArcB (int, int, int, int,  int, int, YT_COLOR);
       
void     YSetFont (char *);
int      YStringW (char *);
int      YStringH (char *);

long     YMakeImage (int wreal, int hreal);
void     YSetXorMode (YT_BOOL xor);
void     YPutPixel (long image, int x, int y, YT_COLOR color);
long     YImageGet (int, int, int, int);
void     YImagePut (int, int, long);
void     YImageFree (long);

typedef struct {
  YT_PFUNC proc;
  char    *name;
  long     long1, long2, long3, long4;
  YT_COLOR color;
} YT_DEFWND;

YT_DEFWND *YSetWnd (YT_PFUNC, char *, long, long, long, long, YT_COLOR);

long     YDlg (YT_PFUNC, char *, int,int,int,int, long,long,long,long, YT_COLOR, long);
void     YDlgEnd (long);
#define  W_DLG    240
#define  H_DLG    170
void     YOpenDialog (int*, YT_PFUNC, char*, int,int,int,int, long,long,long,long, YT_COLOR,long);
void     YCloseDialog (int id);
void     YSetDialog (YT_PFUNC dial);

void     YSetMult (float, float);
int      YIdGoto (void);

/* #define  WINI(i)       ((YT_WINDOW_S*)(WNDI(i)->sys)) */
#define  WINI(i)       (WNDI(i))
#define  WIN           (WINI(Y_IDLAST))

typedef struct {
  YT_BOOL flag;
  int     w, h;
} YT_GETSIZE;

#define  MULT(x,y)        {x=YInt((x)*(KERN->x_mult)); y=YInt((y)*(KERN->y_mult));}
#define  ANTIMULTX(x)     {x=YInt((x)/(KERN->x_mult));}
#define  ANTIMULTY(y)     {y=YInt((y)/(KERN->y_mult));}
#define  ANTIMULT(x,y)    {ANTIMULTX(x);  ANTIMULTY(y);}

#define  CALCXY(x,y)      {MULT(x,y);  x=x+(KERN->x0); y=y+(KERN->y0);}
#define  ANTICALCXY(x,y)  {x=x-(KERN->x0);  y=y-(KERN->y0); ANTIMULT(x,y);}

#define  CALCWH(w,h)      {MULT(w,h); }
#define  ANTICALCWH(w,h)  {ANTIMULT(w,h);}

#define  SIZE_COLER  18 /*  16 */

typedef struct {
  int         sizescale;
  YT_COLOR    colors[CLR_MAX];
  int         display_w, display_h;
  char       *main_font;
  int         x0, y0;
  float       x_mult, y_mult;
  int         id_wl_proc;
  int         id_dialog;
  YT_WINDOW  *wnd[FLAI_NUM]; 
  int         numer;
  int         id_last;
  long        w_long1, w_long2, w_long3, w_long4;
  YT_PFUNC    sys_proc;
  YT_BOOL     wmode;
  YT_BOOL     second_draw;
  char       *font;
	int         font_size;
} YT_KERN;

extern  YT_KERN *kern;

#define  KERN (kern) 
 
#define  ID_DIALOG    (KERN->id_dialog)
#define  SECOND_DRAW  (KERN->second_draw)
#define  Y_FONT       (KERN->font)
#define  Y_FONT_SIZE  (KERN->font_size)
#define  Y_IDLAST     (KERN->id_last)
#define  Y_WMODE      (KERN->wmode)
#define  Y_WLONG1     (KERN->w_long1)
#define  Y_WLONG2     (KERN->w_long2)
#define  Y_WLONG3     (KERN->w_long3)
#define  Y_WLONG4     (KERN->w_long4)
#define  Y_WLPROC     (KERN->id_wl_proc)
#define  Y_WDISPLAY   (KERN->display_w)
#define  Y_HDISPLAY   (KERN->display_h)
#define  SIZE_SCALE   (KERN->sizescale)
#define  CLR_SCALE(n) (KERN->colors[(n)])

void     YFinalYlib ();

/****************************************************************************/
void     YRedrawAll (void); 

                                                                               

/****************************************************************************
 *                                                                          *
 *                 Declarations for section [MORE]                          *  
 *                                                                          *
 ****************************************************************************
 */                                                                          

void     YInitMORE (void);

typedef struct {
  char    *yswitch;
  YT_BOOL  in_case;
  char     rab_string[100];
  FILE    *ystderr;
  FILE    *print_fout;
  char    *print_fname;
  YT_BOOL  print_is_add;
  char    *WAIT_string;
  int      WAIT_flag;
  YT_BOOL  WAIT_cancel;
} YT_MORE;

extern   YT_MORE *impr;
#define  MORE (impr)  

enum keys_y_impr {
  YMORE_BEGIN = YKEY_LEVEL2, 

  YSEND,  YREAD, YAPPLY, YDRAWALL,
  YITEM1, YITEM2, YITEM3, YITEM4, 
  YSEND_BEG, YSEND_END, YGETCOMS,  
  YUP, YNEXT, YPREV, YDOWN, YHOME,  YEND, YRIGHT,
  YMAKE, YOFFSET, YADD, 
  YLEFT, YNONE, 

  YINDEX, YBACK,
  YDATAMADE, 
  YWND2MOD, YMOD2WND, 
  YCHAR, YSHORT, YINT, YDOUBLE, YCENTER,
  YSIZE, YMAIN, YXOR, YCUBE, 
  YGETPOINT, YGETRECT, 
  YNAMES, YPUTDATA, YWORK,
  YSET,
  YSPACE, YFLOAT,            
  YREAL, 
  YPUT, YGOTOFROM, YHORZ, YVERT,
  YKEYTAB, 
  YMETA, YPOST,  YASCII, YBINARY,
  YSTRING, 
  YPUTWND, YDESTITEM, YRECTF, YARCB, YARCF, YPIXEL, YLINE, YPOLYF, 
  YBOOL, YCOLMAP, YNEW, YNORMAL, YBACKUP,
  YBIN, YTXT,  YWRITE, 
  YBEGIN, YMAIL, YHAND,  YPRINT,    YSCANF,
  YGRAF,  YJUMP, YTEST,  YMETADRAW,           

  YMORE_END 
}; 

YT_BOOL  YStrCmp (char *, char *);
YT_BOOL  YStrCopy (char **, char **, int);

#define  Y_SWIT  (MORE->yswitch)  
#define  Y_CASE  (MORE->in_case)  
#define  YSTRCMP(s)  (YStrCmp((s), Y_SWIT))
#define  YSWITCH(sw)  (Y_SWIT=(sw)); if (FALSE) 
#define  YCASE(s)           } else if (YSTRCMP(s)) { 
#define  YCASE2(s1,s2)      } else if (YSTRCMP(s1) || YSTRCMP(s2)) { 
#define  YCASE3(s1,s2,s3)   } else if (YSTRCMP(s1) || YSTRCMP(s2) || YSTRCMP(s3)) { 
#define  YBREAK             ;;
#define  YDEFAULT           } else {    

#define  Y_STR     (MORE->rab_string)
#define  Y_STDERR  (MORE->ystderr)

#define  YOVERFLOW(i) ( ((double)i) != (i) )
#define  YMEMLOC(p,conv,size) {if (YOVERFLOW(size)) p=NULL; else p=(conv)calloc(1,size);}
#define  YMALLOC(p,type,num) {YMEMLOC(p,type*,(num)*sizeof(type)); /* if (p==NULL) YError("YMALLOC"); */}

#define  AA(ii,i,j) (((j)-1)*(ii) + ((i)-1))
#define  AAA(ii,jj,i,j,k) ((ii)*(jj)*((k)-1) + (ii)*((j)-1) + ((i)-1))
#define  AAAA(ii,jj,kk,i,j,k,l) ((ii)*(jj)*(kk)*((l)-1) + (ii)*(jj)*((k)-1) + (ii)*((j)-1) + ((i)-1))
#define  BB(ii,jj,i,j)       ((jj)*(i) + (j)) 
#define  BBB(ii,jj,kk,i,j,k) ((jj)*(kk)*(i) + (kk)*(j) + (k))
#define  BBBB(ii,jj,kk,ll,i,j,k,l) ((jj)*(kk)*(ll)*(i) + (kk)*(ll)*(j) + (ll)*(k) + (l))

#define  PARR2(p,ii,jj,i,j)  (p+BB(ii,jj,i,j))
#define  ARR2(p,ii,jj,i,j)   (*(PARR2(p,ii,jj,i,j)))

void     YPrintMode (char *fname, YT_BOOL is_add);
void     YPrintDo (char *string);
#define  OUTD(val)  (fprintf(Y_STDERR,"out_%d \n",(val)))
#define  OUTF(val)  (fprintf(Y_STDERR,"out_%f \n",(val)))
#define  OUTS(val)  (fprintf(Y_STDERR,"out_%s \n",(val)))
void     YError (char *);
void     YErrorS (char *warning, char *name);
void     YWarning (char *);
void     YWarningS(char *warning, char *name);

void     YDrawPixel (int, int, YT_COLOR);
void     YDrawLine (int, int, int, int, YT_COLOR);
void     YDrawRectF (int, int, int, int, YT_COLOR);
void     YDrawRectB (int, int, int, int, YT_COLOR);
void     YDrawRectFB (int, int, int, int, YT_COLOR, YT_COLOR);
void     YDrawPolyF (int, int *, YT_COLOR);
void     YDrawPolyB (int num, int *points, YT_COLOR color);
void     YDrawArcF (int, int, int, int,  int, int, YT_COLOR);
void     YDrawArcB (int, int, int, int,  int, int, YT_COLOR);
void     YDrawCircB (int xc, int yc, int r, YT_COLOR color);
void     YDrawCircF (int xc, int yc, int r, YT_COLOR color);
void     YDrawString (char *, int, int, YT_COLOR);
void     YDrawColmap (void *, int, int, int, int);
void     YDrawColmapB (void *, int, int, int, int, YT_COLOR);
void     YDrawScale (int, int, int, int, int);
void     YDrawLogo (char *, int, int, int, int);
void     YDrawVString (char *, int, int, YT_COLOR);
void     YDrawStringImp (char *, int, int, YT_COLOR, int, int);
void     YDrawSymbol (char *, int, int, int, int, YT_COLOR);
void     YDrawPointer (int, int, int, YT_COLOR);
void     YDrawTriB (int, int, int, int, int, int, YT_COLOR);
void     YDrawTriF (int, int, int, int, int, int, YT_COLOR);
void     YDrawTriFB (int, int, int, int, int, int, YT_COLOR, YT_COLOR);
void     YDrawFourB (int, int, int, int, int, int, int, int, YT_COLOR);
void     YDrawFourF (int, int, int, int, int, int, int, int, YT_COLOR);
void     YDrawFourFB (int, int, int, int, int, int, int, int,YT_COLOR,YT_COLOR);
void     YDrawStrings (char **, int, int);
void     YDrawRectText (char *, int, int, int, int, YT_COLOR);
void     YDrawColer (int xx, int yy, int ww, int hh);
void     YDrawDownBar (int, int, int, int, YT_COLOR);
void     YDrawUpBar (int, int, int, int, YT_COLOR);
void     YDrawUpWindow (int, int, int, int, YT_COLOR, char *);
void     YDrawDownRect (int, int, int, int);
void     YDrawUpRect (int, int, int, int);
void     YDrawCheck(int x, int y, int w, int h, YT_COLOR color, YT_BOOL flag);
void     YDrawXorLine (int, int, int, int, YT_COLOR);

void     YSetXorDraw (YT_BOOL);

void     YBeginGroup (char*, int, int, int, int, YT_COLOR);
void     YEndGroup (void);

typedef struct {
  int     x, y, w, h;
} YT_RECT;

/* #define  YPI   (3.1415926535897932384626)  */
#define  YPI   (3.14159) /* !!!??? */
#define  UNITX (YStringW("W"))
#define  UNITY (YStringH("H"))

typedef struct {
  char *v_name;
  char *r_name;
} YT_V_NAME;

typedef struct {
  char     *proc_name;
  YT_PFUNC  proc;
} AT_PNAME1;

void     YSetPnames(AT_PNAME1 *pnames);
YT_PFUNC YProcByName(char *proc_name);
void     YSetVirtNames (YT_V_NAME*);
YT_BOOL  YSetVirtName (char *virt_name, char *real_name);
char    *VN (char *virt_name);
long     YGet (char *);
void     YSet (char *, long);
long     YGeti(int, char *);
void     YSeti(int, char *, long);
void     YCalcRects (int, int, YT_RECT *, int, int, int, int, int, int);
int      YPutInCicl (int, int, int);

void     YNextProcInit (void);
YT_PFUNC YNextProcGet (YT_PFUNC prev);
YT_BOOL  YNextProcSet (YT_PFUNC prev, YT_PFUNC next);

#define  CALL_NEXT (CALL(YNextProcGet(WND->proc), id, from, message, mes1, mes2, mes3, mes4))

void    *YColmapCreate (void);
void     YColmapInit (void *, int, int);
void     YColmapBeginDraw (void *, int, int, YT_COLOR);
void     YColmapDrawPixel (int, int, YT_COLOR);
void     YColmapEndDraw (void);
YT_COLOR YColmapGet (void *, int, int);
void     YColmapPut (void *, int, int, YT_COLOR);

void    *YMetaCreate (int);
void     YMetaInit (void *, int, int, int, int);
void     YMetaBegin (void *, char *);
void     YMetaEnd (void);
void     YMetaDrawTo (void *, int, YT_BOOL, int id_from);
void     YMetaToPS (void *);
void     YMetaToScr (void *, int, int, int, int, YT_BOOL, YT_COLOR);
int      YMetaW (void *);
char*    YMetaGetInfo (void *void_meta, int *xx, int *yy);
void     YMetaPutInfo (void *void_meta, char *save_fname, int xx, int yy);

YT_BOOL  YWaitBegin (YT_PFUNC, int, char *string); 
void     YWaitEnd (void);
YT_BOOL  YWaitSend (YT_BOOL flag);

int      YVertStringH (char *);


/****************************************************************************
 *                                                                          *
 *                 Declarations for section [WIND]                          *  
 *                                                                          *
 ****************************************************************************
 */                                                                          

void     YInitWIND (void);

enum keys_y_wind {
  YWIND_BEGIN = YMORE_END, 

  YSC_SET, YSCROLL, YFLOATS, 
  YPUSH, YRPUSH, 
  YLIST, YCHECK, YEDIT, YDECIMAL, 
  YLBUTT, YRBUTT,
  YMENU, YCOMBO, YPUSHDOWN, YRPUSHDOWN,

  YWIND_END 
}; 

typedef struct {
  char     *text;
  YT_PFUNC  proc;
  long      long1;
} YT_BOOK; 

#define  H_CAPTION 25 

long     YFindFile (void);
int      YMessageBox (char *, char *);
void     YHelpPages ();

long     YWL_MAIN (PFUNC_VAR);
long     DIALOG   (PFUNC_PAR);
#define  DLG_HCAP  18 
#define  DLG_WB     4 

long     F_PUSH     (PFUNC_PAR); 
long     F_CHECK    (PFUNC_PAR);
long     F_SCROLL   (PFUNC_PAR);
long     F_EDIT     (PFUNC_PAR);
long     F_CLOCK    (PFUNC_PAR);
long     F_LIST     (PFUNC_PAR);
long     F_TOPMENU  (PFUNC_PAR);

long     PUSH     (PFUNC_PAR); 
long     CHECK    (PFUNC_PAR);
long     SCROLL   (PFUNC_PAR);
long     SCROLL_H (PFUNC_PAR); 
long     EDIT     (PFUNC_PAR);
long     CLOCK    (PFUNC_PAR);
long     LIST     (PFUNC_PAR);
long     TOPMENU  (PFUNC_PAR);

long     DITTO    (PFUNC_PAR); 
long     BOOK     (PFUNC_PAR);
long     DECIMAL  (PFUNC_PAR);
long     COLORS   (PFUNC_PAR);
long     YTEXT    (PFUNC_PAR);
long     RAMKA    (PFUNC_PAR);
long     FLOATS   (PFUNC_PAR);
long     COMBO    (PFUNC_PAR);
long     PROCESS  (PFUNC_PAR);
long     SCREEN   (PFUNC_VAR);
long     FILES    (PFUNC_PAR);
long     ABOUT    (PFUNC_PAR); 
long     MESSAGE  (PFUNC_PAR);
long     IPUSH    (PFUNC_PAR);
long     FINDF    (PFUNC_PAR);
long     WAIT     (PFUNC_PAR);
long     BUTTS    (PFUNC_PAR);
long     LAYOUT   (PFUNC_PAR);
long     MENU     (PFUNC_PAR);

typedef struct {
  int     id;
  int     place;
  YT_DEFWND wnd;
} YT_PUTWND;

int      YLayPut (int id_lay, int, YT_PFUNC, char *, long, long, long, long, YT_COLOR);
YT_BOOL  YPtInWind (int, int, int, int, int, int);

typedef struct{
  int full_size,item_size,item_top;
} YT_LIST;

typedef struct {
  YT_BOOL is_string, is_check, is_gray, is_menu;
  char    string[50];
  long    long1;
} YT_MENU;

typedef struct {
  char    name[50];
  YT_MENU *items;
} YT_DMENU;

/****************************************************************************/

                                                                               
#ifdef __cplusplus
}
#endif
#endif /* _Y_ZONE_H */

/****************************************************************************/ 
