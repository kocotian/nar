# nar
kocotiaN ARchiver is a tiny tool that will pack your files into a small archive.

# Installation
First, clone the repo
```bash
$ git clone https://github.com/kocotian/nar.git
$ cd nar
```
Next, make (compile) the package
```bash
$ make
```
Finally, install compiled app:
```bash
# make install
```

# Usage
## Archive files:
To archive files write in your terminal:
```bash
$ nar -a [files]
```
Note that nar default redirect output to stdout, so if you want to write archive to file you must redirect output to file, eg.
```bash
$ nar -a essay.md scr*.png > mydocs.nar
```
## Unarchive files:
To unpack files from archive, simply write:
```bash
$ nar -u [archive name]
```
For example - you have archive photos.nar and you want to unpack the archive - write
```bash
$ nar -u photos.nar
```
and files will be unpacked. Note that you don't need to redirect output
