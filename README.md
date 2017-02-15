Griffon-IDE
===========

Installation sources

 Ubuntu/Mint : 
 # apt-get install vte-2.91-dev webkitgtk-3.0-dev gtksourceview-3.0-dev libgtk-3-dev libnotify-dev scons
 
 Debian :
 # apt-get install libglib2.0-dev libvte-2.91-dev libwebkitgtk-3.0-dev libgtksourceview-3.0-dev libnotify-dev scons

Installation images and bin :

 # ./install-griffon

Uninstall :

 # scons -c install

For mounting FTP and SFTP (SSH), install sshfs curlftpfs :

# apt-get install curlftpfs sshfs

_____________________________________________
Library require :

gtk+-3.0
vte-2.91
webkitgtk-3.0
gtksourceview-3.0 (3.22.2-1) With the update for gtksourceview_map
libnotify


