#include "video/Renderable.hpp"
using namespace firefly;
using namespace firefly::video;
std::list<Renderable *> Renderable::_renderList;
Renderable::Renderable() { _renderList.push_back(this); }
Renderable::~Renderable() { std::erase(_renderList, this); }