#ifndef LIB_PATH_H_
#define LIB_PATH_H_

#include <boost/filesystem/path.hpp>

typedef boost::filesystem::path Path;

template <typename... Types>
Path createPath(Types... args) {
    Path p;

    using expander = int[];
    (void) expander{ (p/= std::forward<Types>(args), void(), 0)... };

    return p;
}

#endif