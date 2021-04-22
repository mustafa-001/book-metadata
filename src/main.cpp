#include <cstdio>
#include <memory>

#include <iostream>
#include <string>
#include <filesystem>

#include "book.h"
#include "pdf.h"
#include "epub.h"
#include "djvu.h"
using namespace std;
namespace fs = std::filesystem;

class CommandLine {
public:
  CommandLine(int argc, char **argv);
  string author;
  string title;
  string filename_to_set;
  string filename;
  bool read_author = false;
  bool read_title = false;
  bool read = true;
  bool write = false;
  bool complete_from_database = false;
  bool interactive = false;

private:
  int argc;
  char **argv;
  static auto is_read(int argc, char **argv) -> bool;
};
CommandLine::CommandLine(int argc, char **argv) {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "--read") == 0 || strcmp(argv[i], "-r") == 0) {
      this->read = true;
    } else if (strcmp(argv[i], "--write") == 0 || strcmp(argv[i], "-w") == 0) {
      this->write = true;
      if (is_read(argc, argv)) {
        // TODO Error
      }
    } else if (strncmp(argv[i], "--author", strlen(argv[i])) == 0 ||
               strncmp(argv[i], "-a", strlen(argv[i])) == 0) {
      if (is_read(argc, argv)) {
        this->read_author = true;
        // TODO error
      } else {
        if (i + 1 > argc) {
          // TODO erro
        }
        this->author = string{argv[i + 1]};
        this->read_author = false;
        i++;
      }
    } else if (strncmp(argv[i], "--title", strlen(argv[i])) == 0 ||
               strncmp(argv[i], "-t", strlen(argv[i])) == 0) {
      if (is_read(argc, argv)) {
        this->read_title = true;
        // TODO error
      } else {
        if (i + 1 > argc) {
          // TODO erro
        }
        this->title = string{argv[i + 1]};
        this->read_title = false;
        i++;
      }
    } else if (strncmp(argv[i], "--filename", strlen(argv[i])) == 0) {
      if (is_read(argc, argv)) {
        // TODO Error
      } else {
        this->filename_to_set = string{argv[i + 1]};
        i++;
      }
      // Positional argument
    } else {
      this->filename = string{argv[i]};
    }
  }
}
auto CommandLine::is_read(int argc, char **argv) -> bool {
  for (int i = 0; i < argc; i++) {
    if (strcmp(argv[i], "--write") == 0 || strcmp(argv[i], "-w") == 0) {
      return false;
    }
  }
  return true;
}

using namespace std;

auto main(int argc, char **argv) -> int {
  CommandLine cmd{argc, argv};
  PoDoFo::PdfError::EnableDebug(
      false); // turn it off to better view the output from this app!
  PoDoFo::PdfError::EnableLogging(false);
  cout << "argument count is: " << argc << endl;
  cout << "author passed: " << cmd.author << endl;
  cout << "read author: " << cmd.read_author << endl;
  cout << "title passed: " << cmd.title << endl;
  cout << "filename: " << cmd.filename << endl;
  cout << "read: " << !cmd.write << endl;
  string ext{fs::path{cmd.filename}.extension().string()};
  printf( "Extension: %s \n", ext.c_str());
  std::unique_ptr<BookMetadata> book(nullptr);
  if (ext == ".pdf") {
    book = std::make_unique<PdfBook>(cmd.filename, !cmd.write);
  } else if (ext == ".djvu") {
    book = std::make_unique<DJVuMetadata>(cmd.filename, !cmd.write);
  } else {
    book = std::make_unique<EpubBook>(cmd.filename, !cmd.write);
  }
  if (!cmd.write) {
    if (cmd.read_author) {
      std::string w{};
      for (auto &&a : book->get_authors()) {
        w.append(a);
        w.append("\n");
      }
      w.pop_back();
      std::cout << w << std::endl;
      cout << book->get_title() << endl;
    }
    if (cmd.read_title) {
      cout << book->get_title() << endl;
    }
  } else if (cmd.write) {
    if (!cmd.author.empty()) {
      book->set_authors(vector{cmd.author});
    }
    if (!cmd.title.empty()) {
      book->set_title(cmd.title);
    }
    if (!cmd.filename_to_set.empty()) {
      book->set_filename(cmd.filename_to_set);
    }
  }
}