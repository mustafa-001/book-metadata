#include "epub.h"
#include "spdlog/spdlog.h"
#include "libxml++/attribute.h"
#include "libxml++/attributenode.h"
#include "libxml++/nodes/contentnode.h"
#include "libxml++/nodes/node.h"
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <libxml++/document.h>
#include <libxml++/libxml++.h>
#include <libxml++/parsers/domparser.h>
#include <libxml++/parsers/parser.h>
#include <list>
#include <ostream>
#include <string>
#include <zip.h>

const char* container_xml_path = "META-INF/container.xml";

namespace fs = std::filesystem;
EpubBook::EpubBook(const fs::path& path, bool read_only)
	:file_name{ path.filename() }, path{ path }, parser(), read_only{ read_only } {
	int flags = ZIP_CHECKCONS;
	if (read_only) {
		flags |= ZIP_RDONLY;
	}
	int error_p = 0;
	epub_file = zip_open(reinterpret_cast<const char*>(path.generic_u8string().c_str()), flags, &error_p);
	if (epub_file == nullptr) {
		spdlog::error("Error while opening epub file: \n");
		std::exit(-1);
	}

	auto* zstat = (zip_stat_t*)malloc(sizeof(zip_stat_t));
	if (-1 == zip_stat(epub_file, container_xml_path, 0, zstat)) {
		spdlog::error("Error while reading container.xml stats.");
		std::exit(-1);
	}
	char* buffer = (char*)malloc(zstat->size);
	zip_file_t* container_file = zip_fopen(epub_file, container_xml_path, 0);
	if (container_file == nullptr) {
		spdlog::error("Error while opening container file.");
		std::exit(-1);
	}
	int read_bytes = zip_fread(container_file, buffer, zstat->size);
	if (-1 == read_bytes) {
		spdlog::error("Error while reading container.xml file.\n");
		std::exit(-1);
	}
	int e = zip_fclose(container_file);
	if (e != 0) {
		spdlog::error("Error while closing container.xml file.\n");
		std::exit(-1);
	}
	ns_map = xmlpp::Node::PrefixNsMap{};
	ns_map.emplace("dc", "http://purl.org/dc/elements/1.1/");
	ns_map.emplace("opf", "http://www.idpf.org/2007/opf");
	ns_map.emplace("cn", "urn:oasis:names:tc:opendocument:xmlns:container");

	std::string c{ buffer, static_cast<size_t>(read_bytes) };
	parser.parse_memory(c.c_str());
	xml_doc = parser.get_document();
	auto* root = xml_doc->get_root_node();
	auto title_nodes = root->find("cn:rootfiles/cn:rootfile/@full-path", ns_map);
	auto* t = static_cast<xmlpp::AttributeNode*>(
		title_nodes[0]);
	content_opf_path = fs::path{ t->get_value().c_str() };
	// content_opf_path = fs::path{"OEBPS/content.opf"};

	zstat = (zip_stat_t*)malloc(sizeof(zip_stat_t));
	if (-1 == zip_stat(epub_file, content_opf_path.string().c_str(), 0, zstat)) {
		spdlog::error("Error while reading opf file stats.");
		std::exit(-1);
	}
	buffer = (char*)malloc(zstat->size);
	content_opf_file = zip_fopen(epub_file, content_opf_path.string().c_str(), 0);
	if (content_opf_file == nullptr) {
		spdlog::error("Error while opening opf file.");
		std::exit(-1);
	}
	read_bytes = zip_fread(content_opf_file, buffer, zstat->size);
	if (-1 == read_bytes) {
		spdlog::error("Error while reading container.xml file.\n");
		std::exit(-1);
	}

	content = std::string{ buffer, static_cast<size_t>(read_bytes) };
	parser.parse_memory(content.c_str());
	xml_doc = parser.get_document();
}

auto EpubBook::get_authors() -> std::vector<std::string> {
	auto* root = xml_doc->get_root_node();
	auto title_nodes = root->find("./opf:metadata/dc:creator", ns_map);
	std::vector<std::string> r{};
	for (auto&& c : title_nodes) {
		auto* t = static_cast<xmlpp::ContentNode*>(c->get_first_child("text"));
		r.push_back(std::string{ t->get_content() });
	}
	return r;
}

void EpubBook::set_authors(const std::vector<std::string>& authors) {
	auto* root = xml_doc->get_root_node();
	auto* metadata_node = root->find("./opf:metadata", ns_map)[0];
	root->set_namespace_declaration("http://www.idpf.org/2007/opf", "opf");
	for (auto&& c : metadata_node->find("./dc:creator", ns_map)) {
		spdlog::debug("Removing child {}", c->get_name().c_str());
		xmlpp::Node::remove_node(c);
	}
	for (auto&& a : authors) {
		auto* creator = static_cast<xmlpp::Element*>(metadata_node)->add_child_element("creator", "dc");
		creator->set_first_child_text(a);
		creator->set_attribute("opf:role", "aut");
	}
}

auto EpubBook::get_title() -> std::string {
	auto* root = xml_doc->get_root_node();
	auto title_nodes = root->find("./opf:metadata/dc:title", ns_map);
	auto* t = static_cast<xmlpp::ContentNode*>(
		title_nodes[0]->get_first_child("text"));
	return std::string{ t->get_content() };
}

void EpubBook::set_title(const std::string& title) {
	auto* root = xml_doc->get_root_node();
	root->set_namespace_declaration("http://www.idpf.org/2007/opf", "opf");
	auto* metadata_node = root->find("./opf:metadata", ns_map)[0];
	for (auto&& c : metadata_node->find("./dc:title", ns_map)) {
		spdlog::debug("Removing child {}", c->get_name().c_str());
		xmlpp::Node::remove_node(c);
	}
	auto* new_node = static_cast<xmlpp::Element*>(metadata_node)->add_child_element("title", "dc");
	new_node->set_first_child_text(title);
}

void EpubBook::set_filename(const fs::path& filename) { file_name = filename; }

void EpubBook::write_to_disk() {
	std::string serialized_output = xml_doc->write_to_string();
	if (serialized_output.length() == 0) {
		spdlog::error("Empty xml content!\n");
		std::exit(-1);
	}
	auto* zstat = (zip_stat_t*)malloc(sizeof(zip_stat_t));
	if (-1 == zip_stat(epub_file, content_opf_path.string().c_str(), 0, zstat)) {
		spdlog::error("Error while reading opf file stats.");
		std::exit(-1);
	}
	zip_source_t* src = zip_source_buffer(epub_file, serialized_output.c_str(), serialized_output.length(), 0);
	if (src == nullptr) {
		spdlog::error("Error while creating source buffer: %s\n", zip_strerror(epub_file));
		std::exit(-1);
	}
	if (-1 == zip_file_replace(epub_file, zstat->index, src, 0)) {
		spdlog::error("Error while replacing file: {}", zip_strerror(epub_file));
		std::exit(-1);
	}
	zip_fclose(content_opf_file);
	if (-1 == zip_close(epub_file)) {
		spdlog::error("Error while closing epub file: {}.", zip_strerror(epub_file));
		std::exit(-1);
	}
	// If filename changed move rename the file to new name. Dont bother creating
	// a new archive and deleting old one.
	if (file_name.stem() != path.stem()) {
		auto path2 = path;
		path2.replace_filename(file_name);
		path2.replace_extension(".epub");
		fs::rename(path, path2);
	}
}

EpubBook::~EpubBook() {
	if (!read_only) {
		write_to_disk();
	}
	else {
		zip_discard(epub_file);
	}
}
