#include "book.h"
#include "libxml++/document.h"
#include "libxml++/parsers/domparser.h"
#include <filesystem>
#include <string>
#include <zip.h>
#undef _GLIB_HAVE_BUILTIN_OVERFLOW_CHECKS

class EpubBook final : public BookMetadata {
private:
  std::filesystem::path file_name;
  std::filesystem::path path;
  std::filesystem::path content_opf_path;
  std::string content;
  zip_t *epub_file;
  zip_file_t *content_opf_file;
  xmlpp::Document *xml_doc;
  xmlpp::Node::PrefixNsMap ns_map;
  xmlpp::DomParser parser;

public:
  const bool read_only;
  EpubBook(const std::filesystem::path &path, bool read_only = true);

  auto get_authors() -> std::vector<std::string> final;

  void set_authors(const std::vector<std::string> &authors) final;

  auto get_title() -> std::string override;

  void set_title(const std::string &title) final;

  void set_filename(const std::filesystem::path &filename) final;

  void write_to_disk() final;

  ~EpubBook() final;
};