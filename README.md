# book-metadata

book-metadata is an command-line program for managing metadata of .pdf, .epub, .azw3 and .djvu files. It uses [djvulibre](http://djvu.sourceforge.net/), [PoDoFo](http://podofo.sourceforge.net/) and [libxml++](https://developer-old.gnome.org/libxml++-tutorial/5.0/) and [Boost::program_options]() libraries via [vcpkg](https://vcpkg.io/en/index.html).

## Usage
```
$ ./book-metadata -A "Author 1" -T "Title; Subtitle" book1.epub 
$ ./book-metadata --author --title book1.epub 
Author 1
Title; Subtitle
$ ./book-metadata -A "Author 2" book3.djvu 
$ ./book-metadata --author book3.djvu 
Author 2
```
## Building 


- Set up cmake and vcpkg.(https://vcpkg.io/en/getting-started.html)
- Inttall podofo, libxml, zip and boost::program_options.

```
vcpkg install podofo libxmlpp zlib boost-program-options
```
-Configure and build with cmake.
```
cmake -S . -B build -DCMAKE_TOOLCHAIN_FILE=path/to/your/vcpkg/toolchain/file
cmake --build build
```

## TODO
- Find metadata from https://developers.google.com/books using filename.
- Implement ISBN metadata option.
