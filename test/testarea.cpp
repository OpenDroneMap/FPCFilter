/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "testarea.h"

#include <chrono>

TestArea::TestArea(const std::string &name, bool recreateIfExists)
    : name(name){
    const auto root = getFolder();

    if (name.find("..") != std::string::npos) throw std::runtime_error ("Cannot use .. in name");;
    
    if (recreateIfExists){
        if (exists(root)){
        	std::cout << "Removing " << root << std::endl;
            std::cout << "Removed " << fs::remove_all(root) << " files/folders" << std::endl;
        }
    }
}

fs::path TestArea::getFolder(const fs::path &subfolder){
    const fs::path root = fs::temp_directory_path() / "ddb_test_areas" / fs::path(name);
    auto dir = root;
    if (!subfolder.empty()) dir = dir / subfolder;

    if (!exists(dir)){
	    create_directories(dir);
        std::cout << "Created test folder " << dir << std::endl;
    }
    return dir;
}

fs::path TestArea::downloadTestAsset(const std::string &url, const std::string &filename, bool overwrite){
    fs::path destination = getFolder() / fs::path(filename);

    if (exists(destination) && file_size(destination)) {
        if (!overwrite) return destination;
        fs::remove(destination);
    }

    net::performer performer;

    net::request_builder()
	    .url(url)
	    .downloader<file_dowloader>(destination.string().c_str())
	    .verification(false)
	    .send().wait();

    return destination;
}

