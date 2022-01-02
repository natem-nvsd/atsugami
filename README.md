Atsugami
========
[![.](https://github.com/natem-nvsd/atsugami/blob/master/readme_header_picture.png)]

### Development goals
0.70:	Icon view, searching, tag preview, editing, working tabs, favourites, bulk import (maybe), the wiki, settings dialog, help dialog  
0.80:	Fix broken things, clean up things, there will be 0 GTK warnings or errors or incompatible pointers  
0.90:	...  
1.0:	Everything will be working and implemented. Builds for Linux, Mac, and Windows will be in [Releases](https://github.com/natem-nvsd/atsugami/releases)  

### Notes
*	Limit of 999,999x999,999px  

### Dependencies
PostgreSQL (Tested on 14 and 14.1)  
GTK3  
Clang  
[BSD make (bmake)](https://github.com/natem-nvsd/bmake)

### Building and installation
```
# FreeBSD:
make
sudo make install

# Linux:
bmake
sudo bmake install
```  

### Colours (will be implemented later)
```
### Colours:
ff8a8b: Artist  
c797ff: Copyright  
35c64a: Character  
009be6: Generic tag  
ead084: Meta    
```
