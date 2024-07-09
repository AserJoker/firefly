#include "video/ElementBuffer.hpp"
#include "video/VBuffer.hpp"
using namespace firefly::video;
ElementBuffer::ElementBuffer(const std::vector<Face> &data,
                             const VBufferUsage &usage)
    : VBuffer(data.size() * sizeof(Face), (void *)data.data(),
              VBufferType::ELEMENT_ARRAY, usage) {}