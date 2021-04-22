#ifndef book_metadata_book_h
#define book_metadata_book_h
#include <filesystem>
#include <vector>
#include <string>

namespace fs = std::filesystem;
class BookMetadata {
private:
  fs::path path;
  fs::path new_filename;

public:

  virtual auto get_authors() -> std::vector<std::string> =0
;

  virtual void set_authors(const std::vector<std::string> &authors) =0;

  virtual auto get_title() -> std::string =0;

  virtual void set_title(const std::string &title) =0;

  virtual void set_filename(const fs::path &filename) =0;

  virtual void write_to_disk() =0;

  virtual ~BookMetadata() = default;
};
#endif