import os, io ,shlex
import random
import string
import subprocess
executable = "out/build/x64-Debug/src/book-metadata.exe"
books_dir = "books"
os.chdir("C:/Users/flc/kod/book-metadata/")
print(os.getcwd())
def test_read():
    for i in (os.scandir(books_dir)):
        path = "\"{}\"".format(i.path)
        print(path)
        res = subprocess.run([executable, "--author", "--title", i.path], 
                            stdout=subprocess.PIPE, encoding="utf-8" ).stdout
        ##os.spawnl(os.P_WAIT, executable, "--author", "\""+"books\İlber Ortaylı & İsmail Küçükkaya - Cumhuriyet'in İlk Yüzyılı (1923-2023)__575в5К.epub"+"\"")
        print(res, "\n\n")

def test_write_author():
    for i in (os.scandir(books_dir)):
        print("Testing: ", i.path)
        path = "{}".format((i.path))
        rand_str = "".join(random.choice("qwertyuopasldfkgjhzbcxzvnm1234567890") for _ in range(10))
        print(path, " : ", rand_str)
        subprocess.run([executable, "--cauthor", rand_str, path])
        res = subprocess.run([executable, "--author", path]).stdout
        print(res, "\n\n")


test_read()
