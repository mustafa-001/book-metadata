#ifndef _BMD_DJVU_H
#define _BMD_DJVU_H
#include "book.h"
#include <filesystem>
#include <libdjvu/DjVuDocEditor.h>
#include <map>
#include <string>
#include <vector>

class DJVuMetadata final : public BookMetadata {
private:
  std::filesystem::path path;
  std::filesystem::path filename;
  DJVU::GP<DJVU::DjVuDocEditor> djvu_doc;
  std::map<std::string, std::string> metadata;
  std::string ser_annotations;

public:
  const bool read_only;

  DJVuMetadata(const std::filesystem::path &path, bool read_only = true);

  auto get_authors() -> std::vector<std::string> final;

  void set_authors(const std::vector<std::string> &authors) final;

  auto get_title() -> std::string final;

  void set_title(const std::string &title) final;

  void set_filename(const std::filesystem::path &filename) final;

  void write_to_disk() final;

  auto set_field(std::string &sexpr_table, const std::string &key, const std::string &value)
      -> std::string &;

  auto find_next_matching_close_paren(std::string &list, std::string::size_type  opening)
      -> std::string::size_type;

  auto deserialize_metadata(const std::string &contents)
      -> std::map<std::string, std::string>;

  auto serialize_metadata(const std::map<std::string, std::string> &metadata)
      -> std::string;

  ~DJVuMetadata() final;

private:
  void print_anno(DjVuFile &file);
};
#endif