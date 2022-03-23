#include <cstdio>
#include <memory>

#include <iostream>
#include <string>
#include <filesystem>
#include <codecvt>
#include <boost/program_options.hpp>
#include <fmt/core.h>

#include <fmt/xchar.h>
#include "book.h"
#include "pdf.h"
#include "epub.h"
#include "djvu.h"
namespace fs = std::filesystem;
namespace po = boost::program_options;

auto main(int argc, char** argv) -> int {
	/*CommandLine cmd{ argc, argv };*/
	setlocale(LC_ALL, "en_US.utf8");
	//SetConsoleCP(65001);
	//SetConsoleOutputCP(65001);
	po::options_description desc("Edit metadata of bookmarks");
	int u16_argc;
	auto u16_argv = CommandLineToArgvW(GetCommandLineW(), &u16_argc);
	auto u16_argv_2 = po::split_winmain(GetCommandLineW());
	//std::string u8_argv = std::wstring_convert<
		//std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes();
	std::vector<std::string> u8_argv_2{};
	for (auto&& i : u16_argv_2) {
		std::string u8_argv = std::wstring_convert<
			std::codecvt_utf8_utf16<wchar_t>>{}.to_bytes(i);
			u8_argv_2.push_back(u8_argv);
	}
	u8_argv_2.erase(u8_argv_2.begin(), u8_argv_2.begin()+1);
	desc.add_options()
		("title,t", po::bool_switch(), "show title")
		("author,a", po::bool_switch(), "show author")
		("ctitle,T", boost::program_options::value<std::string>()->default_value("None 😧"), "if present change title")
		("cauthor,A", boost::program_options::value<std::string>()->default_value("None 😧"), "if present change author")
		("file-path,f", boost::program_options::value<std::string>()->required(), "path of the book");

	po::positional_options_description pos;
	pos.add("file-path", 1);
	po::variables_map vm;
	po::store(po::command_line_parser(u8_argv_2).options(desc).positional(pos).run(), vm);
	po::notify(vm);

	PoDoFo::PdfError::EnableDebug(
		false); // turn it off to better view the output from this app.
	PoDoFo::PdfError::EnableLogging(false);
	//cout << "argument count is: " << argc << endl;
	//cout << "author passed: " << vm.author << endl;
	//cout << "read author: " << po.read_author << endl;
	//cout << "title passed: " << cmd.title << endl;
	//cout << "filename: " << cmd.filename << endl;
	//cout << "read: " << !cmd.write << endl;
	fs::path file_path{ vm["file-path"].as<std::string>() };

	/*
	The C++11 standard introduced std::codecvt_utf8_utf16.
	Although it is deprecated as of C++17, according to this paper it will
	be available "until a suitable replacement is standardized"
	https://stackoverflow.com/a/46141528/6817513*/
	std::string u8_file_path = std::wstring_convert<
		std::codecvt_utf8_utf16<char16_t>, char16_t>{}.to_bytes(file_path.u16string());
	auto conv = std::wstring_convert<
		std::codecvt_utf8_utf16<wchar_t>, wchar_t>{};
	fmt::print("Author: {}", vm["cauthor"].as<std::string>());
	fmt::print("Title: {}", vm["ctitle"].as<std::string>());
	if (!fs::exists(file_path)) {
		fmt::print("File cannot be found {} \n", u8_file_path);
		std::exit(-1);
	}
	fmt::print("File path is  {} \n", u8_file_path);
	bool readonly = !(vm.count("ctitle") || vm.count("cauthor"));
	std::string ext{ file_path.extension().string() };
	std::unique_ptr<BookMetadata> book(nullptr);
	if (ext == ".pdf") {
		book = std::make_unique<PdfBook>(file_path, readonly);
	}
	else if (ext == ".djvu") {
		book = std::make_unique<DJVuMetadata>(file_path, readonly);
	}
	else if (ext == ".epub") {
		book = std::make_unique<EpubBook>(file_path, readonly);
	}
	else {
		cout << "Extension " << ext << "is not supported" << endl;
	}
	if (vm.count("author")) {
		std::string w{};
		for (auto&& a : book->get_authors()) {
			w.append(a);
			w.append("\n");
		}
		w.pop_back();
		fmt::print("{}\n", w);
	}
	if (vm.count("title")) {
		fmt::print("{}\n", book->get_title());
	}

	if (vm.count("cauthor")) {
		book->set_authors(vector{ vm["cauthor"].as<std::string>() });
	}
	if (vm.count("ctitle")) {
		book->set_title(vm["ctitle"].as<std::string>());

	}

}