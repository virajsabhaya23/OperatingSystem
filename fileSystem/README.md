# File System

###A simple way to test it:
 - Start your program
 - createfs fs.image
 - open fs.image
 - put testfile
 - savefs fs.image
 - closefs
 - quit
 - Restart your program
 - openfs fs.image
 - get testfile testfile.new
 - closefs
 - quit
 - Then:
 - diff testfile testfile.new

It should return nothing which means it matches.