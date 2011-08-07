/*
 *  wmfs2 by Martin Duquesnoy <xorg62@gmail.com> { for(i = 2011; i < 2111; ++i) ©(i); }
 *  For license, see COPYING.
 */

#include "screen.h"
#include "util.h"

static Scr33n*
screen_new(Geo *g)
{
     Scr33n *s = xcalloc(1, sizeof(Scr33n));

     s->geo = *g;
     s->seltag = NULL;

     SLIST_INIT(&s->tags);

     SLIST_INSERT_HEAD(&W->h.screen, s, next);

     /* Set as selected screen */
     W->screen = s;

     return s;
}

void
screen_init(void)
{
     Scr33n *s;
     Geo g;

     g.x = 0;
     g.y = 0;
     g.w = DisplayWidth(W->dpy, W->xscreen);
     g.h = DisplayHeight(W->dpy, W->xscreen);

     SLIST_INIT(&W->h.screen);

     s = screen_new(&g);
     tag_screen(s, tag_new(s, "tag"));
}

void
screen_free(void)
{
     FREE_LIST(Scr33n, W->h.screen);
}
