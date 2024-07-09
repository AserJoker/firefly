#include "video/ElementBuffer.hpp"
#include "video/Buffer.hpp"
using namespace firefly::video;
ElementBuffer::ElementBuffer(const std::vector<Face> &data,
                             const BufferUsage &usage)
    : Buffer(data.size() * sizeof(Face), (void *)data.data(),
             BufferType::ELEMENT_ARRAY, usage) {}