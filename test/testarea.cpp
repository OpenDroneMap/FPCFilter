/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */
#include "testarea.h"

#define LOGURU_WITH_STREAMS 1
#include "../vendor/loguru/loguru.hpp"

TestArea::TestArea(const std::string &name, bool recreateIfExists)
    : name(name){
    const auto root = getFolder();

    if (name.find("..") != std::string::npos) throw std::runtime_error ("Cannot use .. in name");;


    if (recreateIfExists){
        if (fs::exists(root)){
        	LOG_S(INFO) << "Removing " << root;
            LOG_S(INFO) << "Removed " << fs::remove_all(root) << " files/folders";
        }
    }
}

fs::path TestArea::getFolder(const fs::path &subfolder){
    const fs::path root = fs::temp_directory_path() / "ddb_test_areas" / fs::path(name);
    auto dir = root;
    if (!subfolder.empty()) dir = dir / subfolder;

    if (!fs::exists(dir)){
        fs::create_directories(dir);
        LOG_S(INFO) << "Created test folder " << dir;
    }
    return dir;
}

fs::path TestArea::downloadTestAsset(const std::string &url, const std::string &filename, bool overwrite){
    fs::path destination = getFolder() / fs::path(filename);

    if (fs::exists(destination)){
        if (!overwrite) return destination;
        else fs::remove(destination);
    }

    net::request_builder()
        .url(url)
        .downloader<FileDownloader>(destination.string())
        .send().wait();
/*
    net::Request r = net::GET(url);
    r.verifySSL(false);
    r.downloadToFile(destination.string());
*/
    return destination;
}

