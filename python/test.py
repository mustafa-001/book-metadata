import os, io, shlex
import random
import string
import subprocess

executable = "out/build/x64-Debug/src/book-metadata.exe"
books_dir = "books"
##os.chdir("C:/Users/flc/kod/book-metadata/")
print(os.getcwd())


def test_read(ext):
    for i in os.scandir(books_dir):
        if ext != None and not i.path.endswith(ext):
            continue
        path = '"{}"'.format(i.path)
        print(path)
        res = subprocess.run(
            [executable, "--verbose", "--author", "--title", i.path],
            stdout=subprocess.PIPE,
            encoding="utf-8",
        ).stdout
        ##os.spawnl(os.P_WAIT, executable, "--author", "\""+"books\İlber Ortaylı & İsmail Küçükkaya - Cumhuriyet'in İlk Yüzyılı (1923-2023)__575в5К.epub"+"\"")
        print(res, "\n\n")


def test_write_author():
    for i in os.scandir(books_dir):
        print("Testing: ", i.path)
        path = "{}".format((i.path))
        rand_str = "".join(
            random.choice(
                "qwertyuopasldfkgjhzbcxzvnm1234567890öĂřƜƗǅƈťŸãľƅñĒŽŴĹōēƮÿžēĒƑĐĝǪƷǬǇ"
            )
            for _ in range(10)
        )
        subprocess.run([executable, "--verbose", "--cauthor", rand_str, path])
        res = subprocess.run([executable, "--author", path], stdout=subprocess.PIPE)
        if res.stdout.decode("utf-8") == rand_str:
            print("Success \n")
        print(res.stdout.decode("utf-8"))


def test_write_author(ext):
    for i in os.scandir(books_dir):
        if ext != None and not i.path.endswith(ext):
            continue
        print("Testing: ", i.path)
        path = "{}".format(i.path)
        rand_str = "".join(
            random.choice(
                "qwertyuopasldfkgjhzbcxzvnm1234567890öĂřƜƗǅƈťŸãľƅñĒŽŴĹōēƮÿžēĒƑĐĝǪƷǬǇ"
            )
            for _ in range(10)
        )
        subprocess.run([executable, "--verbose", "--cauthor", rand_str, path])
        res = subprocess.run([executable, "--author", path], stdout=subprocess.PIPE)
        if res.stdout.decode("utf-8").rstrip() == rand_str:
            print("Success \n")
        else:
            print(
                "Failure when changing author: \n",
                path,
                "\n target value:\t",
                rand_str,
                "\tcurrent output:\t",
                res.stdout,
            )


       
test_write_author("pdf")
#test_read("djvu")
