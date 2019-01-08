#include "data.h"

Data::Data(const unsigned char *ptr, const size_t& size) {
    set(ptr, size);
}

Data::Data(std::ifstream &stream, const size_t &size) {
    set(stream, size);
}

size_t Data::size() const {
    return m_buffer.size();
}

bool Data::empty() const {
    return m_buffer.empty();
}

void Data::clear() {
    m_buffer.clear();
    m_buffer.shrink_to_fit();
}

void Data::set(std::ifstream &stream, const size_t& size) {
    clear();

    if (stream) {
        m_buffer.reserve(size) ;
        m_buffer.resize(size);

        stream.read((char*)(&m_buffer.front()), size);
    }
}

void Data::set(const unsigned char *ptr, const size_t &size) {

}

const unsigned char *Data::operator*() const {
    return &m_buffer.front();
}
