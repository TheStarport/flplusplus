/*
  WheelScroll.cpp - Scroll some windows using the mouse wheel.

  Jason Hood, 16 & 17 February, 2016.

  Install:
	Copy WheelScroll.dll to your EXE directory and add it to the
	[Libraries] section of dacom.ini.
*/

//Adapted for use in flplusplus 2020

#include "adoxacommon.h"


#define ADDR_LOADSAVE_WHEEL 0x5D1DA4	// Load or Save Game
#define ADDR_NAVMAP_WHEEL   0x5D2FCC	// waypoints
#define ADDR_KEY_WHEEL	    0x5D4AA4	// key list
#define ADDR_TEXT_WHEEL     0x5E47B4	// info, stats, log
#define ADDR_CTRL_WHEEL     0x5E4F6C	// text control
#define ADDR_SERVER_WHEEL   0x5E5164	// servers
#define ADDR_SCROLL_WHEEL   0x5E55F4	// scroll bar
#define ADDR_DESC_WHEEL     0x5E591C	// server description
#define ADDR_LOAD_WHEEL     0x5E5CFC	// Load Game (from the main menu)
#define ADDR_LIST_WHEEL     0x5E6194	// inventory, jobs, news


struct Scroll
{
  BYTE	x1[0x6C];
  BYTE	flags;
  BYTE	x2[0x3B8-0x6D];
  int	dir;
  float delay;
};

#define VISIBLE 2


struct Window
{
  PVOID   vftable;
  Window* parent;
  BYTE	  x1[8];
  char*   type;
  int	  name_len;
  char	  name[32];
  
};

#define InvList_Scroll	0x3D0
#define MisnList_Scroll 0x428
#define NewsList_Scroll 0x648
#define Text_Scroll	0x3F0
#define Load_Scroll	0xD4
#define LoadSave_Scroll 0x1758
#define Keymap_Scroll	0x4B8
#define Server_Scroll	0xC8
#define Waypoint_Scroll 0xAE0
#define Base_Scroll	0xFC4


typedef void (FASTCALL *ScrollUpdate)( Scroll* );


bool FASTCALL Window_Wheel(Window* _this, int EDX_dummy, int dir )
{
  for (Window* win = _this; win != NULL; win = win->parent)
  {
    int ofs = -1;

    if (win->type != NULL &&
	strcmp( win->type, "UIScroll" ) == 0)
    {
      ofs = 0;
    }
    else if (win->type != NULL &&
	     (strcmp( win->type, "RichTextWin" ) == 0 ||
	      strcmp( win->type, "UIRichTextWin" ) == 0))
    {
      ofs = Text_Scroll;
    }
    else if (strcmp( win->name, "NN_Inventory" ) == 0 ||
	     strcmp( win->name, "NN_Dealer" ) == 0)
    {
      ofs = InvList_Scroll;
    }
    else if (strcmp( win->name, "NN_MissionVendor" ) == 0)
    {
      ofs = MisnList_Scroll;
    }
    else if (strcmp( win->name, "NN_News" ) == 0)
    {
      ofs = NewsList_Scroll;
    }
    else if (strcmp( win->name, "NN_LoadSave" ) == 0)
    {
      ofs = LoadSave_Scroll;
    }
    else if (strcmp( win->name, "CharSelDesc" ) == 0 ||
	     strcmp( win->name, "CharSelTime" ) == 0)
    {
      win = win->parent;
      ofs = Load_Scroll;
    }
    else if (strcmp( win->name, "ServerTable" ) == 0)
    {
      win = win->parent;
      ofs = Server_Scroll;
    }
    else if (strcmp( win->name, "NN_Preferences" ) == 0)
    {
      ofs = Keymap_Scroll;
    }
    else if (strcmp( win->name, "NeuroNetNavMap" ) == 0)
    {
      ofs = Waypoint_Scroll;
    }
    else if (strncmp( win->name, "NavMapBase", 10 ) == 0 ||
	     strcmp( win->name, "NavMapSystemTitleText" ) == 0)
    {
      win = win->parent;
      ofs = Base_Scroll;
    }
    if (ofs >= 0)
    {
      Scroll* scroll = (ofs == 0) ? (Scroll*)win : ((Scroll**)win)[ofs / 4];
      if (scroll != NULL && (scroll->flags & VISIBLE))
      {
	scroll->dir = (dir < 0) ? 1 : 0;
	scroll->delay = -1;
	((ScrollUpdate)0x597560)( scroll );
	scroll->delay = -1;
	((ScrollUpdate)0x597560)( scroll );
	scroll->dir = -1;
	return true;
      }
    }
  }

  return false;
}


void adoxa::wheelscroll()
{
  ProtectW( (PDWORD)ADDR_LOADSAVE_WHEEL, 4 );
  ProtectW( (PDWORD)ADDR_NAVMAP_WHEEL,	 4 );
  ProtectW( (PDWORD)ADDR_KEY_WHEEL,	 4 );
  ProtectW( (PDWORD)ADDR_TEXT_WHEEL,	 4 );
  //ProtectW( (PDWORD)ADDR_CTRL_WHEEL,	 4 );
  ProtectW( (PDWORD)ADDR_SERVER_WHEEL,	 4 );
  //ProtectW( (PDWORD)ADDR_SCROLL_WHEEL, 4 );
  //ProtectW( (PDWORD)ADDR_DESC_WHEEL,	 4 );
  //ProtectW( (PDWORD)ADDR_LOAD_WHEEL,	 4 );
  ProtectW( (PDWORD)ADDR_LIST_WHEEL,	 4 );

  typedef bool (FASTCALL *FWheel)( Window*, int, int );
  *(FWheel*)ADDR_LOADSAVE_WHEEL =
  *(FWheel*)ADDR_NAVMAP_WHEEL	=
  *(FWheel*)ADDR_KEY_WHEEL	=
  *(FWheel*)ADDR_TEXT_WHEEL	=
  *(FWheel*)ADDR_CTRL_WHEEL	=
  *(FWheel*)ADDR_SERVER_WHEEL	=
  *(FWheel*)ADDR_SCROLL_WHEEL	=
  *(FWheel*)ADDR_DESC_WHEEL	=
  *(FWheel*)ADDR_LOAD_WHEEL	=
  *(FWheel*)ADDR_LIST_WHEEL	= Window_Wheel;
}
 
