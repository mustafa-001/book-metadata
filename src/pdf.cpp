#include "pdf.h"
#include <iostream>
#include "podofo/podofo-base.h"
#include "podofo/podofo.h"
#include <string>
#include <vector>

using namespace std;
using namespace PoDoFo;
namespace fs = std::filesystem;
PdfBook::PdfBook(const fs::path &path, bool read_only) : path{path}, pdf_doc{path.c_str()}, read_only{read_only}{
  file_name = path.filename(); // TODO Make this work with directories.
  info = pdf_doc.GetInfo();
}

auto PdfBook::get_authors() -> std::vector<std::string> {
  string r{info->GetAuthor().GetString()};
  return std::vector<std::string>{r};
}

void PdfBook::set_authors(const std::vector<std::string>& author) {
  PdfString psa = {author[0].c_str()};
  info->SetAuthor(psa);
}

auto PdfBook::get_title() -> std::string {
  return std::string{info->GetTitle().GetStringUtf8()};
}

void PdfBook::set_title(const std::string &title) {
  PdfString pst = {title.c_str()};
  info->SetTitle(pst);
}
void PdfBook::set_filename(const fs::path& filename) {
  this->file_name = filename;
}

void PdfBook::write_to_disk() {
  fs::path oldpath{path};
  //Create a temp file in the oldpath directory, write to it.
  fs::path temp_file{oldpath};
  temp_file.replace_extension("XXXXXX");
  auto* temp_file_path = (char *)malloc(temp_file.string().length());
  strcpy(temp_file_path, temp_file.string().c_str());
  if (-1 == mkstemp(temp_file_path)) {
    std::cout << "Error opening a temp file!  " << temp_file << endl;
    exit(-1);
  };
  pdf_doc.Write(temp_file_path);
  //Remove the first file we started with.
  if (oldpath.filename() == path.filename()){
    //remove(oldpath.c_str());
  }
  //Update this classes path to use new file. file_name variable only used to hold new filename until write_to_disk is called.
  path.replace_filename(file_name).replace_extension(".pdf");
  //rename(temp_file_path, path.c_str());
}

PdfBook::~PdfBook() {
  if (!read_only) {
    write_to_disk();
  }
}
