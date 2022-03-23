Atsugami
========
![Images not included](https://github.com/natem-nvsd/atsugami/blob/master/readme_header_picture.png)
Images not included.  

TODO:
-----
*	Prevent SQL injection					[✘]
*	Wiki pages						[✘]
*	Reload iconview						[✘]
*	Cached thumbnails					[✓]
*	Settings page						[✘]
*	Edit tags page						[✘]
*	Delete tags						[✘]
*	Delete image						[✘]
*	Bulk import						[✘]
*	Bulk edit						[✘]
*	Import tags from Danbooru				[✘]
*	Re-orderable tabs					[✓]
*	Hotkeys							[✓]
*	Favourite image						[✘]
*	Search							[✘]
*	Open image in external viewer				[✘]
*	Advanced tooltip					[✘]
*	Open image in new tab					[✘]
*	Safe mode						[✘]
*	Export							[✘]
*	Tag list						[✘]
*	Tag blacklisting					[✘]
*	Add "Image" menu item					[✓]
*	Update "View" menu item					[✓]
*	Context menus						[✘]
*	Tag colours						[✓]
*	Parent-child border colours in GTK icon view		[✘]
*	Add "Parent" field to `import_wizard.c`			[✘]
*	Open at a reasonable size on desktop environments	[✘]
*	Use different colour schemes for light and dark themes	[✘]
*	Add icons to tabs labels				[✓]
*	Memory management					[✘]
*	"Recycle bin"-like page					[✘]
*	Ability to expunge images (delete file from disk)	[✘]
*	dtext parser for wiki pages				[✘]
*	Coloured borders for parent and child files in icon views [✘]

Icons to add
----------------
Import: insert-image  					DONE  
New tag: list-add  					DONE  
Console: utilities-terminal  				DONE  
Preferences: preferences-system  
Icon view: view-list  					DONE  
Tag list: view-list  
Recycle bin: trash/trash-full  
Wiki: text-x-generic  
Image viewer: image-x-generic  				DONE  
Edit tag: gtk-edit  
Edit image: gtk-edit  
Export: document-save-as  
Bulk export: document-save-as  
Bulk import: emblem-photos  
Open image: image-x-generic  
Open image in new tab: image-x-generic  
Open image in external viewer: image-x-generic  
New wiki page: document-new  
Edit wiki page: document-page-setup  
Safe mode: preferences-system-privacy-symbolic  

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
./setup.sh
make
sudo make install

# macOS and Linux:
./setup.sh
bmake
sudo bmake install

# Windows:
Good luck
```  
