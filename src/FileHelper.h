//
// Created by qm210 on 14.05.2025.
//

#ifndef DLTROPHY_SIMULATOR_FILEHELPER_H
#define DLTROPHY_SIMULATOR_FILEHELPER_H

#include <string>
#include <filesystem>
#include <fstream>
#include <format>

class FileHelper {
public:
    static void ensure(const std::string& path) {
        // helper that could be moved out of this class, but nevermindelidoo.
        if (std::filesystem::exists(path))
            return;

        auto absolute_path =
                std::filesystem::absolute(
                        std::filesystem::path(path)
                ).string();
        auto message = std::format(
                "File can not be read under {0}",
                absolute_path
        );
        throw std::runtime_error(message);
    }
};

#endif //DLTROPHY_SIMULATOR_FILEHELPER_H
