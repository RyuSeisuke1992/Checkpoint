/*
*   This file is part of Checkpoint
*   Copyright (C) 2017-2018 Bernardo Giordano
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   (at your option) any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*
*   Additional Terms 7.b and 7.c of GPLv3 apply to this file:
*       * Requiring preservation of specified reasonable legal notices or
*         author attributions in that material or in the Appropriate Legal
*         Notices displayed by works containing it.
*       * Prohibiting misrepresentation of the origin of that material,
*         or requiring that modified versions of such material be marked in
*         reasonable ways as different from the original version.
*/

#include "configuration.hpp"

Configuration::Configuration(void)
{
    // check for existing config.json files on the sd card, BASEPATH
    if (!io::fileExists(Archive::sdmc(), StringUtils::UTF8toUTF16(BASEPATH.c_str())))
    {
        store();
    }

    // load json config file
    std::ifstream i(BASEPATH);
    i >> mJson;
    i.close();

    int version_major = mJson["version_major"];
    int version_minor = mJson["version_minor"];
    int version_micro = mJson["version_micro"];
    if (version_major != VERSION_MAJOR || version_minor != VERSION_MINOR || version_micro != VERSION_MICRO)
    {
        store();
    }

    // parse filters
    std::vector<std::string> filter = mJson["filter"];
    for (auto& id : filter)
    {
        mFilterIds.emplace(strtoull(id.c_str(), NULL, 16));
    }

    // parse nand saves
    mNandSaves = mJson["nand_saves"];

    // parse additional save folders
    auto js = mJson["additional_save_folders"];
    for (auto it = js.begin(); it != js.end(); ++it)
    {
        std::vector<std::string> folders = it.value()["folders"];
        std::vector<std::u16string> u16folders;
        for (auto& folder : folders)
        {
            u16folders.push_back(StringUtils::UTF8toUTF16(folder.c_str()));
        }
        mAdditionalSaveFolders.emplace(strtoull(it.key().c_str(), NULL, 16), u16folders);
    }

    // parse additional extdata folders
    auto je = mJson["additional_extdata_folders"];
    for (auto it = je.begin(); it != je.end(); ++it)
    {
        std::vector<std::string> folders = it.value()["folders"];
        std::vector<std::u16string> u16folders;
        for (auto& folder : folders)
        {
            u16folders.push_back(StringUtils::UTF8toUTF16(folder.c_str()));
        }
        mAdditionalExtdataFolders.emplace(strtoull(it.key().c_str(), NULL, 16), u16folders);
    }
}

void Configuration::store(void)
{
    std::ifstream src("romfs:/config.json");
    std::ofstream dst(BASEPATH);
    dst << src.rdbuf();
    src.close();
    dst.close();
}

bool Configuration::filter(u64 id)
{
    return mFilterIds.find(id) != mFilterIds.end();
}

bool Configuration::nandSaves(void)
{
    return mNandSaves;
}

std::vector<std::u16string> Configuration::additionalSaveFolders(u64 id)
{
    std::vector<std::u16string> emptyvec;
    auto folders = mAdditionalSaveFolders.find(id);
    return folders == mAdditionalSaveFolders.end() ? emptyvec : folders->second;
}

std::vector<std::u16string> Configuration::additionalExtdataFolders(u64 id)
{
    std::vector<std::u16string> emptyvec;
    auto folders = mAdditionalExtdataFolders.find(id);
    return folders == mAdditionalExtdataFolders.end() ? emptyvec : folders->second;
}