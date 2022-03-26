#include "djvu.h"
#include "spdlog/spdlog.h"
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <BSByteStream.h>
#include <ByteStream.h>
#include <DjVuAnno.h>
#include <DjVuDocEditor.h>
#include <DjVuDocument.h>
#include <DjVuFile.h>
#include <DjVuPort.h>
#include <GOS.h>
#include <GSmartPointer.h>
#include <GString.h>
#include <IFFByteStream.h>
#include <ddjvuapi.h>
#include <miniexp.h>
#include <map>
#include <ostream>
#include <string>

namespace fs = std::filesystem;
using namespace DJVU;
DJVuMetadata::DJVuMetadata(const fs::path& path, const bool read_only)
	: path{ path }, filename{ path.filename() }, read_only{ read_only } {

	djvu_doc = DjVuDocEditor::create_wait(GURL::Filename::Native(path.string().c_str()));
	if (!djvu_doc->get_shared_anno_file()) {
		std::cout << "File doesn't contain a shared annotation page.\n";
		djvu_doc->create_shared_anno_file();
	}
	auto file = djvu_doc->get_shared_anno_file();
	if (!file->contains_anno()) {
		std::cout << "Shared annotation page doesn't contain annotation itself.\n";
	}
	else {
		const auto ann_r = file->get_anno();
		auto ann2_r = IFFByteStream::create(ann_r);
		GUTF8String chkid;
		auto size = ann2_r->get_chunk(chkid);
		spdlog::debug("chunk id {}, size: {}", chkid, size);
		auto ann3 = BSByteStream::create(ann2_r->get_bytestream());
		ann3->seek(0);
		const char* buffer = (char*)malloc(10000);
		int read_bytes = 1;
		while (read_bytes != 0) {
			read_bytes = ann3->readall((void*)buffer, 1000);
			ser_annotations.append(buffer, read_bytes);
		}
	}
	metadata = deserialize_metadata(ser_annotations);
	// printf("title: %s\n", metadata["title"].c_str());
	// std::cout << serialize_metadata(metadata) << "\n";
	// metadata.insert(std::pair("pub", "torrent"));
	// auto tmp_str = serialize_metadata(metadata);
	// std::string new_ann{};
	// std::string key{"metadata"};
	// std::cout << set_field(new_ann, key, tmp_str) << "\n" << std::endl;
}

auto DJVuMetadata::deserialize_metadata(const std::string& contents)
-> std::map<std::string, std::string> {
	std::map<std::string, std::string> metadata{};
	auto p = contents.find_first_of("(metadata");
	if (p == std::string::npos) {
		return metadata;
	}
	while (true) {
		auto k = contents.find_first_of('(', p + 1) + 1;
		k = contents.find_first_not_of(' ', k);
		auto ke = contents.find_first_of(' ', k + 1);
		auto v = contents.find_first_not_of(' ', ke);
		auto clp = contents.find_first_of(')', v + 1);
		// For now clp equeals ve but this can be wrong eg value_x  ). But it's
		// harmless and implementing rfind_first_not_of() is isn't worth it.
		auto ve = clp;
		auto key = contents.substr(k, ke - k);
		auto value = contents.substr(v, ve - v);
		if (value.starts_with('\"') && value.ends_with('\"')) {
			value.erase(0, 1);
			value.pop_back();
		}
		p = clp + 1;
		// std::cout<< "key: " << key << " value: " << value << "\n" << std::endl;
		metadata.insert(std::pair(key, value));
		// Close starting paranthesis
		auto c = contents.find_first_of(')', clp + 1);
		auto o = contents.find_first_of('(', clp + 1);
		if (o == std::string::npos || c < o) {
			break;
		};
	}
	return metadata;
}

auto DJVuMetadata::serialize_metadata(
	const std::map<std::string, std::string>& metadata) -> std::string {
	std::string r;
	for (const auto& kv : metadata) {
		r.append("\n\t(");
		r.append(kv.first);
		r.append(" \"");
		r.append(kv.second);
		r.append("\")");
	}
	// std::cout << "in serialize_metadata()"<< r << "\n";
	return r;
}
auto DJVuMetadata::find_next_matching_close_paren(
	std::string& list, std::string::size_type opening)
	-> std::string::size_type {
	auto cp = list.find_first_of(')', opening + 1);
	auto op = list.find_first_of('(', opening + 1);
	// std::cout << "cp: " << cp << " op: " << op << " length: " << list.length()
	// << "\n";
	if (op < cp) {
		cp = find_next_matching_close_paren(list, op);
		return find_next_matching_close_paren(list, cp);
	}
	return cp;
}
auto DJVuMetadata::set_field(std::string& sexpr_table, const std::string& key,
	const std::string& value) -> std::string& {
	// std::cout << "table: \n" << sexpr_table << "\n";
	if (sexpr_table.empty()) {
		std::cout << "Table is empty!\n";
		sexpr_table.append("( )");
	}
	std::string a{ "(" };
	a += key;
	auto field_start = sexpr_table.find_first_of(a);
	if (field_start != std::string::npos) {
		auto field_end = find_next_matching_close_paren(sexpr_table, field_start);
		sexpr_table.erase(field_start, field_end - field_start + 1);
		spdlog::debug("sexpr table: {}", sexpr_table);
	}
	sexpr_table.append(a);
	sexpr_table.append("\n");
	sexpr_table.append(value);
	sexpr_table.append(")");
	return sexpr_table;
}

void DJVuMetadata::print_anno(DjVuFile& file) {
	const auto ann_r = file.get_anno();
	auto ann2_r = IFFByteStream::create(ann_r);
	GUTF8String chkid;
	ann2_r->get_chunk(chkid);
	// std::cout << "chunk id " << (const char *)chkid << "\n";
	auto ann3 = BSByteStream::create(ann2_r->get_bytestream());
	ann3->seek(0);
	const char* buffer = (char*)malloc(1000);
	while (0 != ann3->readall((void*)buffer, 1000)) {
		printf("%s\n", buffer);
	}
	// printf("%s\n", buffer);
	//
}

auto DJVuMetadata::get_authors() -> std::vector<std::string> {
	auto r = metadata.find("author");
	if (r != metadata.end()) {
		return std::vector{ r->second };
	}
	return std::vector{ std::string{""} };
}

void DJVuMetadata::set_authors(const std::vector<std::string>& authors) {
	std::string r;
	for (auto&& a : authors) {
		r.append(a);
		r.append(", ");
	}
	r.pop_back();
	r.pop_back();
	metadata.insert_or_assign("author", r);
}

auto DJVuMetadata::get_title() -> std::string {
	auto r = metadata.find("title");
	if (r != metadata.end()) {
		return r->second;
	}
	return std::string{ "" };
}

void DJVuMetadata::set_title(const std::string& title) {
	std::cout << "On set title.\n";
	metadata.insert_or_assign("title", title);
}

void DJVuMetadata::set_filename(const std::filesystem::path& filename) {
	this->filename = filename;
}

void DJVuMetadata::write_to_disk() {
	// TODO How to forward char* to string& accepting functions?
	std::string m{ "metadata" };
	std::string sm = serialize_metadata(metadata);
	// std::cout << "serialized annotations: " << ser_annotations << "\n";
	ser_annotations = set_field(ser_annotations, m, sm);

	// Write our serialized annotations to a ByteStream. BSByteStream here is for
	// compressing it.
	auto ann_bs(ByteStream::create());
	auto bzz = BSByteStream::create(ann_bs, 1000);
	for (int i = 0; i < ser_annotations.length(); i++) {
		bzz->write8(ser_annotations[i]);
	}
	//bzz->writall(ser_annotations.c_str(), ser_annotations.length());
	bzz = nullptr;
	// std::cout << (const char*) bzz->getAsUTF8() << "\n" <<  std::endl;

	// Write compressed Bytestream to new ByteStream. IFFByteStream here is for
	// navigating to proper chunk and writing necessary IFF chunk info stuff.
	ann_bs->seek(0);
	const auto ann = ByteStream::create();
	auto iff_ann = IFFByteStream::create(ann);
	iff_ann->put_chunk("ANTz");
	iff_ann->copy(*ann_bs);
	iff_ann->close_chunk();

	djvu_doc->get_shared_anno_file()->anno = ann;
	djvu_doc->get_shared_anno_file()->set_modified(true);

	if (djvu_doc->can_be_saved()) {
		djvu_doc->save();
	}
	else {
		printf("This file cannot be saved!\n");
		std::exit(-1);
	}
	/*if (filename.stem() != path.stem()) {
	  auto path2 = path;
	  path2.replace_filename(filename);
	  path2.replace_extension(".djvu");
	  fs::rename(path, path2);
	}*/
}

DJVuMetadata::~DJVuMetadata() {
	// std::cout << "On destructor." << read_only << "\n" << std::endl;
	if (!read_only) {
		write_to_disk();
	}
}