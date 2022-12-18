# File System

### A simple way to test it:
Run the mfs program and use the following commands:
```bash
createfs fs.image
open fs.image
put testfile
savefs fs.image
closefs
quit
```
Restart your program
```bash
openfs fs.image
get testfile testfile.new
closefs
quit
```

Now type : <code>diff testfile testfile.new</code>
It should return nothing which means it matches.