Atsugami
========
![Images not included](https://github.com/natem-nvsd/atsugami/blob/master/readme_header_picture.png)
Images not included.  

### TODO:
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

### Notes
*	Limit of 999,999x999,999px  

### Dependencies
* PostgreSQL (Tested on 14 and 14.1)  
* GTK3  
* GCR 3 (build-time only)  
* pkgconf (build-time ony)
* [BSD make (bmake)](https://github.com/natem-nvsd/bmake) (build-time only)

### Building and installation
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

# FAQ
	Q: I imported a file, but it's not there any more. Where is it?  
	A: Atsugami automatically moves files which have been imported into a storage directory, to external prevent deletion.

### Colours (will be implemented later)
```
### Colours:
ff8a8b: Artist  
c797ff: Copyright  
35c64a: Character  
009be6: Generic tag  
ead084: Meta    
```
