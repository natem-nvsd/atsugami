Atsugami
========
![Images not included](https://github.com/natem-nvsd/atsugami/blob/master/readme_header_picture.png)
Images not included.  

TODO:
-----

*	Wiki pages
*	Page titles with Pango
*	Reload iconview function
*	Cached thumbnails
*	Settings page
*	Edit file tags page
*	Delete tags function
*	Delete image function
*	Bulk import
*	Bulk edit
*	Import tags from Danbooru function
*	Re-orderable tabs
*	Hotkeys
*	Favourite image function
*	Search
*	Open image in external viewer function
*	Advanced tooltip
*	Open image in new tab
*	Safe mode
*	Export function
*	Tag list
*	Tag blacklisting
*	Locking on files being imported

Dependencies
------------

* PostgreSQL (Tested on 14 and 14.1)  
* GTK3  
* pkgconf (build-time ony)
* libmd (Should be preinstalled on BSD systems)
* [BSD make (bmake)](https://github.com/natem-nvsd/bmake) (build-time only)
	\* gmake _may_ work

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

Colours (will be implemented later)
-----------------------------------

```
### Colours:
ff8a8b: Artist  
c797ff: Copyright  
35c64a: Character  
009be6: Generic tag  
ead084: Meta    
```
