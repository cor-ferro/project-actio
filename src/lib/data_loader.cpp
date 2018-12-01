#include "data_loader.h"

#include <boost/filesystem.hpp>
#include <boost/filesystem/path.hpp>
#include "utils.h"

namespace loader {
    Base::Base(const Path rootPath) : rootPath(rootPath) {};

    Filesystem::Filesystem(const Path rootPath) : Base(rootPath) {};

    byte *Filesystem::load(Path name) {
        Path fileName = rootPath;
        fileName /= name;

        byte *data = nullptr;

        if (boost::filesystem::exists(fileName)) {
            std::ifstream scriptFile(fileName.string());

            scriptFile.seekg(0, std::ios::end);
            auto size = static_cast<size_t>(scriptFile.tellg());

            scriptFile.seekg(0, std::ios::beg);

            data = new byte[size]; // @todo: use allocator

            auto eos = std::istreambuf_iterator<char>();
            auto it = std::istreambuf_iterator<char>(scriptFile);

            size_t i = 0;
            while (it != eos) {
                data[i] = reinterpret_cast<byte>(*it);
                it++;
                i++;
            }

            assert(i == size && "size not match");
        }

        return data;
    }

    Package::Package(const Path rootPath) : Base(rootPath) {};

    byte *Package::load(Path name) {
        return nullptr;
    }
}