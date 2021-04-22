import xml.etree.ElementTree as ET
import zipfile
import sys
import tempfile
ns = {'dc': 'http://purl.org/dc/elements/1.1/',
'rns': 'http://www.idpf.org/2007/opf' ,
'container_rns': "urn:oasis:names:tc:opendocument:xmlns:container" }

def remove_authors(tree):
    for c in metadata.findall('./dc:creator', ns):
        print(c)
        metadata.remove(c)
    print("Removed all creators")
    for c in metadata.findall('dc:creator', ns):
        print("shouldn't happen", c.text)
    return tree
    

def append_author(metadata, author):
    a = ET.Element('{'+ns['dc']+'}creator')
    a.text = author
    metadata.append(a)
    return metadata

def set_title(metadata, title):
    metadata.find('dc:title', ns).text = title
    return metadata

def get_opfpath(container):
    root = ET.parse(container).getroot()
    path = root.find(".//container_rns:rootfile", ns)
    return path.attrib['full-path']

book = zipfile.ZipFile('book.epub')
for i in book.namelist():
    print(i)
with book.open("META-INF/container.xml") as contents:
    opfpath = get_opfpath(contents)
print("Opf path:\t", opfpath)
with book.open(opfpath) as opffile:
    tree = ET.parse(opffile)
root = tree.getroot()

print("\n----Before writing --- ")
for child in root.findall('.//dc:title', ns):
    print("Title:\t", child.text)
for child in root.findall('.//dc:creator', ns):
    print("Author:\t", child.text)

metadata = tree.find('./rns:metadata', ns)
mt = remove_authors(metadata) 
mt = set_title(mt, "test title")
root.set('rns:metadata', append_author(metadata, "test"))
tree._setroot(root)
# for i in root.find("./rns:metadata", ns):
#     print(i)
print("--- After writing --- ")
for child in root.findall('.//dc:creator', ns):
    print("Author:\t", child.text)
for child in root.findall('.//dc:title', ns):
    print("Title:\t", child.text)
with tempfile.TemporaryDirectory() as dir:
    book.extractall(dir)
    tree.write(dir.name+opfpath)
    zipfile.ZipFile("book.epub").compression
book = zipfile.ZipFile('book.epub', 'a')
tree.write(zipfile.Path(book, opfpath).open('w'))
book.close()

