#include <string>
#pragma once
#include "book.h"
#include "podofo/podofo.h"
#include "podofo/podofo-base.h"

using namespace std;
namespace fs = std::filesystem;
class PdfBook final: public BookMetadata {
private:
    fs::path path;
    fs::path file_name;
    PoDoFo::PdfMemDocument pdf_doc;
    PoDoFo::PdfInfo* info;
public:
    const bool read_only;
    PdfBook(const fs::path& path, bool read_only = true);

    auto get_authors() -> std::vector<std::string> final;

    void set_authors(const std::vector<std::string> &authors) final;

    auto get_title() -> string final;

    void set_title(const string& title) final;

    void set_filename(const fs::path& filename) final;

    void write_to_disk() final;

    ~PdfBook() final;
};