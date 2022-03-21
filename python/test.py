import os
executable = "out/build/x64-Debug/src/book-metadata.exe"
books_dir = "books"
print(os.getcwd())
for i in (os.scandir(books_dir)):
    print("Testing: ", i.path)
    print(os.spawnl(os.P_WAIT, executable, "--author", "--title", "\""+i.path+"\""))
    print("\n")
