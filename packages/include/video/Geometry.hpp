#pragma once
namespace firefly::video {
struct Geometry {
  struct Point2D {
    float x;
    float y;
  };
  struct Point3D {
    float x;
    float y;
    float z;
  };
  struct Size {
    float w, h;
  };
  struct Rect {
    Point2D position;
    Size size;
  };
  struct Rotation {
    Point3D center;
    float angle;
  };
};
} // namespace firefly::video