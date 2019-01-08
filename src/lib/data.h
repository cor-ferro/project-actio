#ifndef ACTIO_LIB_DATA_H
#define ACTIO_LIB_DATA_H

#include <vector>
#include <fstream>

class Data {
public:
    Data() = default;

    explicit Data(std::ifstream &stream, const size_t& size);

    explicit Data(const unsigned char *ptr, const size_t& size);

    size_t size() const;

    bool empty() const;

    void clear();

    void set(std::ifstream &stream, const size_t& size);

    void set(const unsigned char *ptr, const size_t& size);

    const unsigned char * operator*() const;
private:
    std::vector<unsigned char> m_buffer;
};

#endif //ACTIO_LIB_DATA_H
