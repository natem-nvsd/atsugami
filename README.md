Atsugami
========
![Images not included](https://github.com/natem-nvsd/atsugami/blob/master/readme_header_picture.png)
Images not included.  

TODO
----

### Classes
* Finish About class
* Finish Console class
* Finish List Tags class
* Browser class
* Manga class
* Pool class
* Post Wizard class
* Settings Class
* Tag Wizard class
* Trash class
* Viewer class
* Wiki class

### UI - Control Elements
* Edit tags page
* Delete tags
* Delete image
* Bulk import
* Bulk edit
* Export
* Tag blacklisting
* Ability to expunge images (delete file from disk)

### UI - Miscellaneous
* Reload iconview
* Favourite image
* Open image in new tab
* Undo/Redo in entry fields
* File drag-and-drop to import
* File drag-and-drop to export
* Coloured borders for parent and child files in icon views
* Make certain GtkAccelerators apply only in specific tabs
* Make the "general" tag colour the same as the GTK theme's primary colour
* Context menus
* Parent-child border colours in GTK icon view
* Add "Parent" field to `import_wizard.c`
* Open at a reasonable size on desktop environments

### Data Processing
* Danbooru importer
* dtext parser for wiki pages and artist commentary
* Gnome Keyring support for database passwords
* Pixiv Ugoria support via libzip and GdkPixbuf PixbufSimpleAnim
* Search callback

### Miscellaneous
* Documentation

FIXME:
------
* Advanced tooltip displays relative to icon\_view
* Advanced tooltip produces the error: `Gtk-CRITICAL **: gtk_tree_model_get_iter: assertion 'path != NULL' failed`
* Advanced tooltip scrolled window does scroll
* Tag list check buttons do nothing
* Tag list columns of the wrong size
* Blank tags, or tags consisting of spaces are produced

Icons to add
----------------
* Preferences: preferences-system
* Tag list: view-list
* Recycle bin: user-trash/user-trash-full
* Edit tag: document-edit-symbolic
* Edit image: document-edit-symbolic
* New wiki page: document-new
* Edit wiki page: document-edit-symbolic
* Image Viewer: image-x-generic

Dependencies
------------
* PostgreSQL (Tested on 14 and 14.1)  
* libpq-fe  
* GTK 3  
* libmd (Should be preinstalled on BSD systems)  
<!--* libdiff (Should be preinstalled on BSD systems)  -->
* [gtkimageview](https://github.com/natem-nvsd/gtkimageview)
* pkgconf
* C11-compatible C compiler
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
