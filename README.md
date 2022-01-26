Atsugami
========
![Images not included](https://github.com/natem-nvsd/atsugami/blob/master/readme_header_picture.png)
Images not included.  

# IMPORTANT: Pre-1.0 builds are only recommended for development use, since there may be database structure changes

### Notes
*	Limit of 999,999x999,999px  

### Dependencies
* PostgreSQL (Tested on 14 and 14.1)  
* GTK3  
* GCR 3
* pkgconf
* [BSD make (bmake)](https://github.com/natem-nvsd/bmake)

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
