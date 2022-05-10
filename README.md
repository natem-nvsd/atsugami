Atsugami
========
![Images not included](https://github.com/natem-nvsd/atsugami/blob/master/readme_header_picture.png)
Images not included.  

TODO:
-----
*	Prevent SQL injection
*	Wiki pages
*	Reload iconview
*	Settings page
*	Edit tags page
*	Delete tags
*	Delete image
*	Bulk import
*	Bulk edit
*	Import tags from Danbooru
*	Favourite image
*	Search
*	Advanced tooltip
*	Open image in new tab
*	Export
*	Tag list
*	Tag blacklisting
*	Context menus
*	Parent-child border colours in GTK icon view
*	Add "Parent" field to `import_wizard.c`
*	Open at a reasonable size on desktop environments
*	Use different colour schemes for light and dark themes
*	Memory management
*	"Recycle bin"-like page
*	Ability to expunge images (delete file from disk)
*	dtext parser for wiki pages
*	Coloured borders for parent and child files in icon views
*	Make certain GtkAccelerators apply only in specific tabs
*	Make the "general" tag colour the same as the GTK theme's primary colour
*	Pixiv Ugoria support via libzip and GdkPixbuf PixbufSimpleAnim
*	Gnome Keyring support for database passwords

Icons to add
----------------
Preferences: preferences-system  
Tag list: view-list  
Recycle bin: user-trash/user-trash-full  
Edit tag: document-edit-symbolic  
Edit image: document-edit-symbolic  
New wiki page: document-new  
Edit wiki page: document-edit-symbolic  
Image Viewer: image-x-generic

Dependencies
------------
* PostgreSQL (Tested on 14 and 14.1)  
* GTK 3  
* libmd (Should be preinstalled on BSD systems)  
<!--* libdiff (Should be preinstalled on BSD systems)  -->
<!--* gtkimageviewer 2.0 -->
* pkgconf
* C99 compatible C compiler
* [BSD make (bmake) (mirror)](https://github.com/natem-nvsd/bmake)

Building and installation
-------------------------

```
# (Free)BSD:
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
