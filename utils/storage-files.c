#include <menu.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>

#define ARRAY_SIZE(a) (sizeof(a) / sizeof(a[0]))
#define CTRLD 	4

char name[256][256];
char command[100];
char *point;
int count = 0;

static int parse_iso(const struct dirent *dir)
{
 if(!dir)
   return 0;

 if(dir->d_type == DT_REG) { /* only deal with regular file */
     const char *ext = strrchr(dir->d_name,'.');
     if((!ext) || (ext == dir->d_name))
       return 0;
     else {
       if(strcmp(ext, ".iso") == 0)
         return 1;
     }
 }

 return 0;
}

static int parse_image(const struct dirent *dir)
{
 if(!dir)
   return 0;

 if(dir->d_type == DT_REG) { /* only deal with regular file */
     const char *ext = strrchr(dir->d_name,'.');
     if((!ext) || (ext == dir->d_name))
       return 0;
     else {
       if(strcmp(ext, ".image") == 0)
         return 1;
     }
 }

 return 0;
}

void attach_storage(const char *name)
{
	system("modprobe -r g_mass_storage");
point = strrchr(name,'.');
     if(strcmp(point,".image") == 0) {
        sprintf( command, "modprobe g_mass_storage \"file=%s\" stall=0 iManufacturer='MacPCZone'", name);
	} else {
  	sprintf( command, "modprobe g_mass_storage \"file=%s\" ro=1 cdrom=1 iManufacturer='MacPCZone'", name);
}
system(command);
}

int main()
{	ITEM **my_items;
	int c;				
	MENU *my_menu;
        int n_choices, i;
//	ITEM *cur_item;
   struct dirent **namelist;
   int n;
	
   n = scandir(".", &namelist, parse_iso, alphasort);
   if (n < 0) {
       perror("scandir");
       return 1;
   }
   else {
       while (n--) {
           strcpy(name[count++], namelist[n]->d_name);
           free(namelist[n]);
       }
       free(namelist);
   }

   n = scandir(".", &namelist, parse_image, alphasort);
   if (n < 0) {
       perror("scandir");
       return 1;
   }
   else {
       while (n--) {
           strcpy(name[count++], namelist[n]->d_name);
           free(namelist[n]);
       }
       free(namelist);
   }

	/* Initialize curses */	
	initscr();
	start_color();
        cbreak();
        noecho();
	keypad(stdscr, TRUE);
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_GREEN, COLOR_BLACK);
	init_pair(3, COLOR_MAGENTA, COLOR_BLACK);

	/* Initialize items */
        n_choices = ARRAY_SIZE(name);
        my_items = (ITEM **)calloc(n_choices + 1, sizeof(ITEM *));
        for(i = 0; i < n_choices; ++i)
                my_items[i] = new_item(name[i], " ");
	my_items[n_choices] = (ITEM *)NULL;

	/* Create menu */
	my_menu = new_menu((ITEM **)my_items);

	/* Set fore ground and back ground of the menu */
	set_menu_fore(my_menu, COLOR_PAIR(1) | A_REVERSE);
	set_menu_back(my_menu, COLOR_PAIR(2));
	set_menu_grey(my_menu, COLOR_PAIR(3));

	/* Post the menu */
	mvprintw(LINES - 3, 0, " Press <ENTER> to see the option selected");
	mvprintw(LINES - 2, 0, " Up and Down arrow keys to naviage (F1 to Exit)");
	post_menu(my_menu);
	refresh();

	while((c = getch()) != KEY_F(1))
	{       switch(c)
	        {	case KEY_DOWN:
				menu_driver(my_menu, REQ_DOWN_ITEM);
				break;
			case KEY_UP:
				menu_driver(my_menu, REQ_UP_ITEM);
				break;
			case 10: /* Enter */
				move(20, 0);
				clrtoeol();
				attach_storage(item_name(current_item(my_menu)));
				mvprintw(20, 0, "%s has been attached to the USB bus", 
						item_name(current_item(my_menu)));
				pos_menu_cursor(my_menu);
				break;
		}
	}	
	unpost_menu(my_menu);
	for(i = 0; i < n_choices; ++i)
		free_item(my_items[i]);
	free_menu(my_menu);
	endwin();
	return 0;
}
