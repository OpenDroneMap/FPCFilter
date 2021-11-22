/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <filesystem>
#include <fstream>

#include "vendor/curly/headers/curly.hpp/curly.hpp"

namespace net = curly_hpp;
namespace fs = std::filesystem;

class TestArea
{
    std::string name;
public:
    TestArea(const std::string &name, bool recreateIfExists = false);

    fs::path getFolder(const fs::path &subfolder = "");
    fs::path downloadTestAsset(const std::string &url, const std::string &filename, bool overwrite = false);
};

class FileDownloader : public net::download_handler {
public:
    FileDownloader(std::string& filename)
    : stream_(filename.c_str(), std::ofstream::binary) {}

    std::size_t write(const char* src, std::size_t size) override {
        stream_.write(src, static_cast<std::streamsize>(size));
        return size;
    }
private:
    std::ofstream stream_;
};

