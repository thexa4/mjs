#include "defs.h"
#include "mms.h"
#include "colors.h"
#include "struct.h"
#include "proto.h"
#include "extern.h"

/*
 * There are 64 possible color_pairs, 0-63. This is 6 bits worth of colors,
 * and since there are only 64 possible color combos then why not use them
 * all for easy translation. So, in 8 bits, the format is as follows:
 * 00xxxyyy, where xxx is the foreground color (bits) and yyy is the background
 * color. Everything starts at zero of course, otherwise that wouldn't fit
 * naturally into our 0-63 scheme.
 * However, ncurses hard-codes the color pair zero to be what we would call
 * 56 (white on black, 7<<3), so we fix color pair 56 to be black on black
 * (what 0<<3|0 would have been). Doh.
 */

void
init_ansi_pair (void)
{
	u_int8_t fore, back;
	for (fore = 0; fore < COLORS; fore++)
		for (back = 0; back < COLORS; back++)
			init_pair(fore<<3 | back, fore, back);
}	

int
my_mvwprintw (WINDOW *win, int y, int x, u_int32_t attribs, const u_char *format, ...)
{
	u_int16_t i;
	va_list args;
	u_char buf[BUFFER_SIZE+1], *p;
	memset(buf, 0, BUFFER_SIZE+1);
	va_start(args, format);
	i = vsnprintf(buf, BUFFER_SIZE, format, args);
	va_end(args);
	wmove(win, y, x);
	p = buf;
	while (*p)
		waddch(win, *p++ | attribs);
	return i;
}

int
my_wprintw (WINDOW *win, u_int32_t attribs, const u_char *format, ...)
{
	u_int16_t i;
	va_list args;
	u_char buf[BUFFER_SIZE+1], *p;
	memset(buf, 0, BUFFER_SIZE+1);
	va_start(args, format);
	i = vsnprintf(buf, BUFFER_SIZE, format, args);
	va_end(args);
	p = buf;
	while (*p)
		waddch(win, *p++ | attribs);
	return i;
}

int
my_mvwnprintw (WINDOW *win, int y, int x, u_int32_t attribs, int n, const u_char *format, ...)
{
	u_int16_t i;
	va_list args;
	u_char buf[n+1], *p;
	memset(buf, 0, n+1);
	va_start(args, format);
	i = vsnprintf(buf, n, format, args);
	va_end(args);
	wmove(win, y, x);
	p = buf;
	while (*p)
		waddch(win, *p++ | attribs);
	return i;
}

int
my_mvwnprintw2 (WINDOW *win, int y, int x, u_int32_t attribs, int n, const u_char *format, ...)
{
	u_int16_t i;
	va_list args;
	u_char buf[n+1], *p;
	memset(buf, 0, n+1);
	va_start(args, format);
	i = vsnprintf(buf, n, format, args);
	va_end(args);
	wmove(win, y, x);
	p = buf;
	while (*p) {
		waddch(win, *p++ | attribs);
		n--;
	}
	while (n) {
		waddch(win, ' ' | attribs);
		n--;
	}
	return i;
}

int
my_wnprintw (WINDOW *win, u_int32_t attribs, int n, const u_char *format, ...)
{
	u_int16_t i;
	va_list args;
	u_char buf[n+1], *p;
	memset(buf, 0, n+1);
	va_start(args, format);
	i = vsnprintf(buf, n, format, args);
	va_end(args);
	p = buf;
	while (*p)
		waddch(win, *p++ | attribs);
	return i;
}

int
my_mvwnaddstr (WINDOW *win, int y, int x, u_int32_t attribs, size_t n, const u_char *str)
{
	u_char *s = NULL;

	wmove(win, y, x);
	if (str && *str)
		for (s = (char *)str; *s && n; n--)
			waddch(win, *s++ | attribs);
	for (; n; n--)
		waddch(win, ' ' | attribs);
	return OK;
}

int
my_wnaddstr (WINDOW *win, u_int32_t attribs, size_t n, const u_char *str)
{
	u_char *s = NULL;

	if (str && *str)
		for (s = (char *)str; *s && n; n--)
			waddch(win, *s++ | attribs);
	for (; n; n--)
		waddch(win, ' ' | attribs);
	return OK;
}

int
my_waddstr (WINDOW *win, u_int32_t attribs, const u_char *str)
{
	u_char *s = NULL;
	
	if (!str || !*str)
		return OK;
	for (s = (u_char *)str; *s; waddch(win, *s++ | attribs));
	return OK;
}

int
my_mvwaddstr (WINDOW *win, int y, int x, u_int32_t attribs, const u_char *str)
{
	wmove(win, y, x);
	return my_waddstr(win, attribs, str);
}

__inline__ void
my_wnclear (WINDOW *win, int n)
{
	int x, y;
	getyx(win, y, x);
	for (; n > 0; n--)
		waddch(win, ' ');
	wmove(win, y, x);
}

int
active_win (Window *window)
{
	WINDOW *win = window->win;
	PANEL *panel = window->panel;
	const u_char *title = window->title;
	int i, x, y;
	wborder(win, '�', '�', '�', '�', '�', '�', '�', '�');
	getmaxyx(win, y, x);
	mvwchgat(win, 0, 0, x, A_ALTCHARSET | colors[ACTIVE], 0, NULL);
	mvwchgat(win, y-1, 0, x, A_ALTCHARSET | colors[ACTIVE], 0, NULL);
	for (i = 0; i < y; i++) {
		mvwchgat(win, i, 0, 1, A_ALTCHARSET | colors[ACTIVE], 0, NULL);
		mvwchgat(win, i, x-1, 1, A_ALTCHARSET | colors[ACTIVE], 0, NULL);
	}	
	if (title && ((i = strlen(title)+4) < x))
		my_mvwprintw(win, 0, x-i-(x-i)/2, colors[TITLE], "[ %s ]", title);
	top_panel(panel);
	update_panels();
	return 1;
}

int
inactive_win (Window *window)
{
	WINDOW *win = window->win;
	const u_char *title = window->title;
	int i, x, y;
	wborder(win, 0, 0, 0, 0, 0, 0, 0, 0);
	getmaxyx(win, y, x);
	mvwchgat(win, 0, 0, x, A_ALTCHARSET | colors[INACTIVE], 0, NULL);
	mvwchgat(win, y-1, 0, x, A_ALTCHARSET | colors[INACTIVE], 0, NULL);
	for (i = 0; i < y; i++) {
		mvwchgat(win, i, 0, 1, A_ALTCHARSET | colors[INACTIVE], 0, NULL);
		mvwchgat(win, i, x-1, 1, A_ALTCHARSET | colors[INACTIVE], 0, NULL);
	}	
	if (title && ((i = strlen(title)+4) < x))
		my_mvwprintw(win, 0, x-i-(x-i)/2, colors[TITLE], "[ %s ]", title);
	update_panels();
	return 1;
}

int inactive_edit (Window *window)
{
	/* this is kind of stupid but its how its gotta be! */
	return hide_panel(window->panel);
}

void
do_scrollbar (Window *window)
{
	int i = 1, offscreen, x, y; /* window dimensions, etc */
	int top, bar, bottom; /* scrollbar portions */
	double value; /* how much each notch represents */
	wlist *wtmp = window->contents.list;
	flist *ftmp = NULL, *selected = wtmp->selected;
	WINDOW *win = window->win;
	if (wtmp->length < 1)
		return;
	getmaxyx(win, y, x);
	y -= 2;
	x -= 1;
	value = wtmp->length / (double) y;
	for (ftmp = wtmp->top; ftmp != selected; ftmp = ftmp->next)
		i++;
	/* now we calculate the number preceeding the screenfull */
	offscreen = wtmp->where - i;

	/* calculate the sizes of our status bar */
	if (value < 1) {
		top = 0;
		bar = y;
		bottom = 0;
	} else {
		/* drago's code */
		double toptmp;
		toptmp=offscreen / value;
		top= (int) (double)(int)toptmp/1 == toptmp ? toptmp : (double)(int)toptmp+(double)1;
		bar=(int)((y / value)+(double).5);
		bottom = y - top - bar;
		/* end drago's code =) */
	}
	/* because of rounding we may end up with too much, correct for that */
	if (bottom < 0)
		top += bottom;
	mvwvline(win, 1, x, ACS_CKBOARD | A_ALTCHARSET | colors[SCROLL], top);
	mvwvline(win, 1+top, x, ACS_BLOCK | A_ALTCHARSET | colors[SCROLLBAR], bar);
	if (bottom > 0)
		mvwvline(win, 1+top+bar, x, ACS_CKBOARD | A_ALTCHARSET | colors[SCROLL], bottom);
#ifdef GPM_SUPPORT
	mvwaddch(win, 0, x, ACS_UARROW | A_ALTCHARSET | colors[ARROWS]);
	mvwaddch(win, y+1, x, ACS_DARROW | A_ALTCHARSET | colors[ARROWS]);
#endif
	update_panels();
}

void
free_list (flist *list)
{
	flist *ftmp;
	
	for (ftmp = list; ftmp; ftmp = ftmp->next) {
		free(ftmp->filename);
		free(ftmp->artist);
		free(ftmp->title);
		free(ftmp->path);
		free(ftmp->prev);
	}
	free(ftmp);
}
