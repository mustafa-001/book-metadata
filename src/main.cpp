#include <cstdio>
#include <memory>

#include <iostream>
#include <string>
#include <filesystem>
#include <boost/program_options.hpp>
#include "book.h"
#include "pdf.h"
#include "epub.h"
#include "djvu.h"
namespace fs = std::filesystem;
namespace po = boost::program_options;

auto main(int argc, char** argv) -> int {
	/*CommandLine cmd{ argc, argv };*/
	po::options_description desc("Edit metadata of bookmarks");
	desc.add_options()
		("title", po::bool_switch(), "show title")
		("author", po::bool_switch(), "show author")
		("file-path", "path of the book")
		("ctitle", "if present change title")
		("cauthor", "if present change author");

	po::positional_options_description pos;
	pos.add("file-path", 1);
	po::variables_map vm;
	po::store(po::command_line_parser(argc, argv).options(desc).positional(pos).run(), vm);
	po::notify(vm);

	PoDoFo::PdfError::EnableDebug(
		false); // turn it off to better view the output from this app!
	PoDoFo::PdfError::EnableLogging(false);
	//cout << "argument count is: " << argc << endl;
	//cout << "author passed: " << vm.author << endl;
	//cout << "read author: " << po.read_author << endl;
	//cout << "title passed: " << cmd.title << endl;
	//cout << "filename: " << cmd.filename << endl;
	//cout << "read: " << !cmd.write << endl;
	cout << vm["file-path"].as<std::string>() << endl;
	fs::path file_path{ vm["file-path"].as<std::string>() };
	if (!fs::exists(file_path))
		std::exit(-1);
	bool readonly = true;
	std::string ext{ file_path.extension().string() };
	printf("Extension: %s \n", ext.c_str());
	std::unique_ptr<BookMetadata> book(nullptr);
	if (ext == ".pdf") {
		book = std::make_unique<PdfBook>(file_path, readonly);
	}
	else if (ext == ".djvu") {
		book = std::make_unique<DJVuMetadata>(file_path, readonly);
	}
	else {
		book = std::make_unique<EpubBook>(file_path, readonly);
		if (vm.count("author")) {
			std::string w{};
			for (auto&& a : book->get_authors()) {
				w.append(a);
				w.append("\n");
			}
			w.pop_back();
			std::cout << w << std::endl;
		}
		if (vm.count("title")) {
			cout << book->get_title() << endl;
		}
		if (vm.count("cauthor")) {
			book->set_authors(vector{ vm["cauthor"].as<std::string>() });
		}
		if (vm.count("ctitle")) {
			book->set_title(vm["ctitle"].as<std::string>());
		}
		//if (!vm["file-path"].as<std::string>()_to_set.empty()) {
			//book->set_filename(vm["file-path"].as<std::string>()_to_set);
		//}
	}
}