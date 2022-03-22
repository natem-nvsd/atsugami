Atsugami
========
![Images not included](https://github.com/natem-nvsd/atsugami/blob/master/readme_header_picture.png)
Images not included.  

TODO:
-----
*	Prevent SQL injection				[✘]
*	Wiki pages					[✘]
*	Reload iconview function			[✘]
*	Cached thumbnails				[✔]
*	Settings page					[✘]
*	Edit tags page					[✘]
*	Delete tags function				[✘]
*	Delete image function				[✘]
*	Bulk import					[✘]
*	Bulk edit					[✘]
*	Import tags from Danbooru function		[✘]
*	Re-orderable tabs				[✔]
*	Hotkeys						[✔]
*	Favourite image function			[✘]
*	Search						[✘]
*	Open image in external viewer function		[✘]
*	Advanced tooltip				[✘]
*	Open image in new tab				[✘]
*	Safe mode					[✘]
*	Export function					[✘]
*	Tag list					[✔]
*	Tag blacklisting				[✘]
*	Add "Image" menu item				[✘]
*	Update "View" menu item				[✔]
*	Context menus					[✔]
*	Tag colours					[✔]
*	Parent-child border colours in GTK icon view	[✘]
*	Add "Parent" field to `import_wizard.c`		[✘]
*	Open at a reasonable size on desktop environments	[✘]
*	Use different colour schemes for light and dark themes	[✘]
*	Add icons to tabs labels			[✘]

Dependencies
------------

* PostgreSQL (Tested on 14 and 14.1)  
* GTK3  
* libmd (Should be preinstalled on BSD systems)
Build-time dependencies:  
* pkgconf
* C99 compatible C compiler (under tests, GCC failed)
* [BSD make (bmake) (mirror)](https://github.com/natem-nvsd/bmake)

Building and installation
-------------------------

```
# FreeBSD:
make
sudo make install

# Linux:
bmake
sudo bmake install

# Windows:
Good luck
```  
