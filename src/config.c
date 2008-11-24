/*
*      config.c
*      Copyright © 2008 Martin Duquesnoy <xorg62@gmail.com>
*      All rights reserved.
*
*      Redistribution and use in source and binary forms, with or without
*      modification, are permitted provided that the following conditions are
*      met:
*
*      * Redistributions of source code must retain the above copyright
*        notice, this list of conditions and the following disclaimer.
*      * Redistributions in binary form must reproduce the above
*        copyright notice, this list of conditions and the following disclaimer
*        in the documentation and/or other materials provided with the
*        distribution.
*      * Neither the name of the  nor the names of its
*        contributors may be used to endorse or promote products derived from
*        this software without specific prior written permission.
*
*      THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*      "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*      LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*      A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*      OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*      SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*      LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*      DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*      THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*      (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*      OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "wmfs.h"

#define FILE_NAME   ".config/wmfs/wmfsrc"

cfg_t *cfg;
cfg_t *cfg_misc;
cfg_t *cfg_bar;
cfg_t *cfg_variables;
cfg_t *cfg_root;
cfg_t *cfg_client;
cfg_t *cfg_layouts;
cfg_t *cfg_tags;
cfg_t *cfg_keys;
cfg_t *cfgtmp;

static cfg_opt_t misc_opts[] =
{
     CFG_STR("font",             "sans-9",  CFGF_NONE),
     CFG_BOOL("raisefocus",      cfg_false, CFGF_NONE),
     CFG_BOOL("raiseswitch",     cfg_true,  CFGF_NONE),
     CFG_END()
};

static cfg_opt_t bar_opts[] =
{
     CFG_STR("bg",        "#090909", CFGF_NONE),
     CFG_STR("fg",        "#6289A1", CFGF_NONE),
     CFG_STR("position",  "top",     CFGF_NONE),
     CFG_END()
};

static cfg_opt_t mouse_button_opts[] =
{
     CFG_INT("tag",    -1,        CFGF_NONE),
     CFG_STR("button", "Button1", CFGF_NONE),
     CFG_STR("func",   "",        CFGF_NONE),
     CFG_STR("cmd",    "",        CFGF_NONE),
     CFG_END()
};

static cfg_opt_t root_opts[] =
{
     CFG_STR("background_command", "",                CFGF_NONE),
     CFG_SEC("mouse",              mouse_button_opts, CFGF_MULTI),
     CFG_END()
};

static cfg_opt_t titlebar_opts[] =
{
     CFG_INT("height", 0,                 CFGF_NONE),
     CFG_STR("fg",     "#FFFFFF",         CFGF_NONE),
     CFG_SEC("mouse",  mouse_button_opts, CFGF_MULTI),
     CFG_END()
};

static cfg_opt_t client_opts[]=
{
     CFG_INT("border_height",         1,                  CFGF_NONE),
     CFG_STR("border_normal",         "#354B5C",          CFGF_NONE),
     CFG_STR("border_focus",          "#6286A1",          CFGF_NONE),
     CFG_STR("resize_corner_normal",  "#ff0000",          CFGF_NONE),
     CFG_STR("resize_corner_focus",   "#ff0000",          CFGF_NONE),
     CFG_STR("modifier",              "Alt",              CFGF_NONE),
     CFG_SEC("mouse",                 mouse_button_opts,  CFGF_MULTI),
     CFG_SEC("titlebar",              titlebar_opts,      CFGF_NONE),
     CFG_END()
};

static cfg_opt_t layout_opts[] =
{
     CFG_STR("type",   "", CFGF_NONE),
     CFG_STR("symbol", "", CFGF_NONE),
     CFG_END()
};

static cfg_opt_t layouts_opts[] =
{
     CFG_STR("fg",           "#FFFFFF",   CFGF_NONE),
     CFG_STR("bg",           "#292929",   CFGF_NONE),
     CFG_SEC("layout",       layout_opts, CFGF_MULTI),
     CFG_END()
};

static cfg_opt_t tag_opts[] =
{
     CFG_STR("name",        "",           CFGF_NONE),
     CFG_FLOAT("mwfact",    0.65,         CFGF_NONE),
     CFG_INT("nmaster",     1,            CFGF_NONE),
     CFG_STR("layout",      "tile_right", CFGF_NONE),
     CFG_BOOL("resizehint", cfg_false,    CFGF_NONE),
     CFG_END()
};

static cfg_opt_t tags_opts[] =
{
     CFG_STR("sel_fg",       "#FFFFFF", CFGF_NONE),
     CFG_STR("sel_bg",       "#354B5C", CFGF_NONE),
     CFG_STR("border",       "#090909", CFGF_NONE),
     CFG_SEC("tag",          tag_opts,  CFGF_MULTI),
     CFG_END()
};

static cfg_opt_t key_opts[] =
{
     CFG_STR_LIST("mod", "{Control}", CFGF_NONE),
     CFG_STR("key",      "None",      CFGF_NONE),
     CFG_STR("func",     "",          CFGF_NONE),
     CFG_STR("cmd",      "",          CFGF_NONE),
     CFG_END()
};

static cfg_opt_t keys_opts[] =
{
     CFG_SEC("key", key_opts, CFGF_MULTI),
     CFG_END()
};

static cfg_opt_t variable_opts[] =
{
     CFG_STR("content", "", CFGF_NONE),
     CFG_END()
};

static cfg_opt_t variables_opts[] =
{
     CFG_SEC("var", variable_opts, CFGF_TITLE | CFGF_MULTI),
     CFG_END()
};

static cfg_opt_t opts[] =
{
     CFG_SEC("misc",      misc_opts,      CFGF_NONE),
     CFG_SEC("variables", variables_opts, CFGF_NONE),
     CFG_SEC("root",      root_opts,      CFGF_NONE),
     CFG_SEC("client",    client_opts,    CFGF_NONE),
     CFG_SEC("bar",       bar_opts,       CFGF_NONE),
     CFG_SEC("layouts",   layouts_opts,   CFGF_NONE),
     CFG_SEC("tags",      tags_opts,      CFGF_NONE),
     CFG_SEC("keys",      keys_opts,      CFGF_NONE),
     CFG_END()
};


func_name_list_t func_list[] =
{
     {"spawn",                   uicb_spawn },
     {"client_kill",             uicb_client_kill },
     {"client_prev",             uicb_client_prev },
     {"client_next",             uicb_client_next },
     {"toggle_max",              uicb_togglemax },
     {"layout_next",             uicb_layout_next },
     {"layout_prev",             uicb_layout_prev },
     {"tag",                     uicb_tag },
     {"tag_next",                uicb_tag_next },
     {"tag_prev",                uicb_tag_prev },
     {"tag_transfert",           uicb_tagtransfert },
     {"set_mwfact",              uicb_set_mwfact },
     {"set_nmaster",             uicb_set_nmaster },
     {"quit",                    uicb_quit },
     {"toggle_infobar_position", uicb_infobar_togglepos },
     {"mouse_move",              uicb_mouse_move },
     {"mouse_resize",            uicb_mouse_resize },
     {"client_raise",            uicb_client_raise },
     {"tile_switch",             uicb_tile_switch },
     {"toggle_free",             uicb_togglefree },
     {"reload",                  uicb_reload }
};

func_name_list_t layout_list[] =
{
     {"tile_right", tile },
     {"tile_left", tile_left },
     {"tile_top", tile_top },
     {"tile_bottom", tile_bottom },
     {"tile_grid", grid},
     {"max",  maxlayout },
     {"free", freelayout }
};

key_name_list_t key_list[] =
{
     {"Control", ControlMask },
     {"Shift",   ShiftMask },
     {"Lock",    LockMask },
     {"Alt",     Mod1Mask },
     {"Mod2",    Mod2Mask },
     {"Mod3",    Mod3Mask },
     {"Mod4",    Mod4Mask },
     {"Mod5",    Mod5Mask },
     {NULL,      NoSymbol }
};

name_to_uint_t mouse_button_list[] =
{
     {"Button1", Button1 },
     {"Button2", Button2 },
     {"Button3", Button3 },
     {"Button4", Button4 },
     {"Button5", Button5 },
     {"1", Button1 },
     {"2", Button2 },
     {"3", Button3 },
     {"4", Button4 },
     {"5", Button5 },
};

void*
name_to_func(char *name, func_name_list_t l[])
{
     int i;

     if(name)
          for(i = 0; l[i].name ; ++i)
               if(!strcmp(name, l[i].name))
                    return l[i].func;

     return NULL;
}

ulong
char_to_modkey(char *name)
{
     int i;

     if(name)
          for(i = 0; key_list[i].name; ++i)
               if(!strcmp(name, key_list[i].name))
                    return key_list[i].keysym;

     return NoSymbol;
}

uint
char_to_button(char *name)
{
     int i;

     if(name)
          for(i = 0; mouse_button_list[i].name; ++i)
               if(!strcmp(name, mouse_button_list[i].name))
                    return mouse_button_list[i].button;

     return 0;
}

Layout
layout_name_to_struct(Layout lt[], char *name, int n)
{
     int i;

     for(i = 0; i < n; ++i)
          if(lt[i].func == name_to_func(name, layout_list))
               return lt[i];

     return lt[0];
}

char*
var_to_str(char *conf_choice)
{
     int i;
     char *tmpchar = NULL;

     if(!conf_choice)
          return 0;

     for(i = 0; confvar[i].name; i++)
          if(!strcmp(conf_choice, confvar[i].name))
               tmpchar = confvar[i].content;
     if(tmpchar)
          return strdup(tmpchar);
     else
          return strdup(conf_choice);

     return NULL;
}

void
mouse_section(MouseBinding mb[], cfg_t *cfg, int ns)
{
     int i;
     cfg_t *tmp;

     for(i = 0; i < ns; ++i)
     {
          tmp          = cfg_getnsec(cfg, "mouse", i);
          mb[i].tag    = cfg_getint(tmp, "tag");
          mb[i].button = char_to_button(cfg_getstr(tmp, "button"));
          mb[i].func   = name_to_func(cfg_getstr(tmp, "func"), func_list);
          mb[i].cmd    = strdup(var_to_str(cfg_getstr(tmp, "cmd")));
     }

     return;
}

/** Configuration initialization
 * \todo Make functions for all section (tag, client, layout...)
*/
void
init_conf(void)
{
     char final_path[128];
     int ret, i, j, l;

     sprintf(final_path, "%s/%s",
             strdup(getenv("HOME")),
             strdup(FILE_NAME));

     cfg = cfg_init(opts, CFGF_NONE);
     ret = cfg_parse(cfg, final_path);

     if(ret == CFG_FILE_ERROR
        || ret == CFG_PARSE_ERROR)
     {
          fprintf(stderr, "WMFS: parsing configuration file (%s) failed\n", final_path);
          sprintf(final_path, "%s/wmfs/wmfsrc", XDG_CONFIG_DIR);
          fprintf(stderr, "Use the default configuration (%s).\n", final_path);
          cfg = cfg_init(opts, CFGF_NONE);
          ret = cfg_parse(cfg, final_path);
     }

     cfg_misc      = cfg_getsec(cfg, "misc");
     cfg_variables = cfg_getsec(cfg, "variables");
     cfg_root      = cfg_getsec(cfg, "root");
     cfg_client    = cfg_getsec(cfg, "client");
     cfg_bar       = cfg_getsec(cfg, "bar");
     cfg_layouts   = cfg_getsec(cfg, "layouts");
     cfg_tags      = cfg_getsec(cfg, "tags");
     cfg_keys      = cfg_getsec(cfg, "keys");

     if((cfg_size(cfg_variables, "var")) > 256)
     {
          fprintf(stderr,"WMFS Configuration: Too many variables !\n");
          exit(EXIT_FAILURE);
     }

     for(i = 0; i < cfg_size(cfg_variables, "var"); ++i)
     {
          cfgtmp             = cfg_getnsec(cfg_variables, "var", i);
          confvar[i].name    = strdup(cfg_title(cfgtmp));
          confvar[i].content = strdup(cfg_getstr(cfgtmp, "content"));
     }

     /* misc */
     conf.font          = var_to_str(strdup(cfg_getstr(cfg_misc, "font")));
     conf.raisefocus    = cfg_getbool(cfg_misc, "raisefocus");
     conf.raiseswitch   = cfg_getbool(cfg_misc, "raiseswitch");

     /* bar */
     conf.colors.bar  = getcolor(var_to_str(cfg_getstr(cfg_bar, "bg")));
     conf.colors.text = strdup(var_to_str(cfg_getstr(cfg_bar, "fg")));
     conf.bartop      = (strcmp(strdup(cfg_getstr(cfg_bar, "position")), "top") == 0) ? True : False;

     /* root */
     conf.root.background_command = strdup(var_to_str(cfg_getstr(cfg_root, "background_command")));
     conf.root.nmouse = cfg_size(cfg_root, "mouse");
     conf.root.mouse = emalloc(conf.root.nmouse, sizeof(MouseBinding));
     mouse_section(conf.root.mouse, cfg_root, conf.root.nmouse);

     /* client */
     conf.client.borderheight        = (cfg_getint(cfg_client, "border_height"))
          ? cfg_getint(cfg_client, "border_height") : 1;
     conf.client.bordernormal        = getcolor(var_to_str(cfg_getstr(cfg_client, "border_normal")));
     conf.client.borderfocus         = getcolor(var_to_str(cfg_getstr(cfg_client, "border_focus")));
     conf.client.resizecorner_normal = getcolor(var_to_str(cfg_getstr(cfg_client, "resize_corner_normal")));
     conf.client.resizecorner_focus  = getcolor(var_to_str(cfg_getstr(cfg_client, "resize_corner_focus")));
     conf.client.mod                |= char_to_modkey(cfg_getstr(cfg_client, "modifier"));
     conf.client.nmouse              = cfg_size(cfg_client, "mouse");
     conf.client.mouse               = emalloc(conf.client.nmouse, sizeof(MouseBinding));
     mouse_section(conf.client.mouse, cfg_client, conf.client.nmouse);

     /* titlebar (into the client section) */
     cfgtmp                = cfg_getsec(cfg_client, "titlebar");
     conf.titlebar.height  = cfg_getint(cfgtmp, "height");
     conf.titlebar.fg      = var_to_str(cfg_getstr(cfgtmp, "fg"));
     conf.titlebar.nmouse  = cfg_size(cfgtmp, "mouse");
     conf.titlebar.mouse   = emalloc(conf.titlebar.nmouse, sizeof(MouseBinding));
     mouse_section(conf.titlebar.mouse, cfgtmp, conf.titlebar.nmouse);

     /* layout */
     /* Set conf.layout NULL for conf reload */
     for(i = 0; i < NUM_OF_LAYOUT; ++i)
     {
          conf.layout[i].symbol = NULL;
          conf.layout[i].func = NULL;
     }

     conf.colors.layout_fg  = strdup(var_to_str(cfg_getstr(cfg_layouts, "fg")));
     conf.colors.layout_bg  = getcolor(var_to_str(cfg_getstr(cfg_layouts, "bg")));

     if((conf.nlayout = cfg_size(cfg_layouts, "layout")) > NUM_OF_LAYOUT
          || !(conf.nlayout = cfg_size(cfg_layouts, "layout")))
     {
          fprintf(stderr, "WMFS Configuration: Too many or no layouts (%d)\n", conf.nlayout);
          conf.nlayout          = 1;
          conf.layout[0].symbol = strdup("TILE");
          conf.layout[0].func   = tile;
     }

     if(!conf.layout[0].symbol
        && !conf.layout[0].func)
     {
          for(i = 0; i < conf.nlayout; ++i)
          {
               cfgtmp = cfg_getnsec(cfg_layouts, "layout", i);
               if(!name_to_func(strdup(cfg_getstr(cfgtmp, "type")), layout_list))
               {
                    fprintf(stderr, "WMFS Configuration: Unknow Layout type: \"%s\"\n",
                            strdup(cfg_getstr(cfgtmp, "type")));
                    exit(EXIT_FAILURE);
               }
               else
               {
                    conf.layout[i].symbol = strdup(var_to_str(cfg_getstr(cfgtmp, "symbol")));
                    conf.layout[i].func = name_to_func(strdup(cfg_getstr(cfgtmp, "type")), layout_list);
               }
          }
     }

     /* tag
      * if there is no tag in the conf or more than
      * MAXTAG (32) print an error and create only one.
      */
     conf.colors.tagselfg  = strdup(var_to_str(cfg_getstr(cfg_tags, "sel_fg")));
     conf.colors.tagselbg  = getcolor(var_to_str(cfg_getstr(cfg_tags, "sel_bg")));
     conf.colors.tagbord   = getcolor(var_to_str(cfg_getstr(cfg_tags, "border")));

     conf.ntag = cfg_size(cfg_tags, "tag");
     if(!conf.ntag  || conf.ntag > MAXTAG)
     {
          fprintf(stderr, "WMFS Configuration: Too many or no tag"
                  " (%d) in the configration file\n", conf.ntag);
          conf.ntag = 1;
          conf.tag[0].name       = strdup("WMFS");
          conf.tag[0].mwfact     = 0.65;
          conf.tag[0].nmaster    = 1;
          conf.tag[0].resizehint = False;
          conf.tag[0].layout     = layout_name_to_struct(conf.layout, "tile_right", conf.nlayout);
     }
     else
     {
          for(i = 0; i < conf.ntag; ++i)
          {
               cfgtmp                  = cfg_getnsec(cfg_tags, "tag", i);
               if(strlen(strdup(cfg_getstr(cfgtmp, "name"))) > 256)
                    fprintf(stderr, "WMFS Configuration: name of tag %d too long !\n", i);
               conf.tag[i].name        = strdup(cfg_getstr(cfgtmp, "name"));
               conf.tag[i].mwfact      = cfg_getfloat(cfgtmp, "mwfact");
               conf.tag[i].nmaster     = cfg_getint(cfgtmp, "nmaster");
               conf.tag[i].resizehint  = cfg_getbool(cfgtmp, "resizehint");
               conf.tag[i].layout      = layout_name_to_struct(conf.layout, cfg_getstr(cfgtmp, "layout"), conf.nlayout);
          }
     }

     seltag = 1;
     prevtag = 0;
     for(i = 0; i < conf.ntag; ++i)
          tags[i + 1] = conf.tag[i];

     /* keybind */
     conf.nkeybind = cfg_size(cfg_keys, "key");
     keys = emalloc(conf.nkeybind, sizeof(Key));

     for(j = 0; j <  conf.nkeybind; ++j)
     {
          cfgtmp = cfg_getnsec(cfg_keys, "key", j);

          for(l = 0; l < cfg_size(cfgtmp, "mod"); ++l)
               keys[j].mod |= char_to_modkey(cfg_getnstr(cfgtmp, "mod", l));

          keys[j].keysym = XStringToKeysym(cfg_getstr(cfgtmp, "key"));
          keys[j].func = name_to_func(cfg_getstr(cfgtmp, "func"), func_list);
          if(keys[j].func == NULL)
          {
               fprintf(stderr, "WMFS Configuration error: Unknow Function \"%s\"\n",
                       cfg_getstr(cfgtmp, "func"));
               exit(EXIT_FAILURE);
          }
          keys[j].cmd = (!strdup(var_to_str((cfg_getstr(cfgtmp, "cmd"))))
                                 ?  NULL : strdup(var_to_str(cfg_getstr(cfgtmp, "cmd"))));
     }
     cfg_free(cfg);


     return;
}
