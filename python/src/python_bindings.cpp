// pybind11 bindings for levelset2d_bilinear_measure, scoped to exactly
// what this project itself provides: common_geometry's Vec2d (via
// pybind11/eigen.h, auto-converting to/from NumPy, no explicit binding
// needed) and Edge2d (bound below, since FindPolygonCrossings takes a
// list of them), plus the bilinear-measurement primitives (now
// implemented in common_geometry, bound here under their original
// Python names). Deliberately does NOT depend on (or bind) any
// extraction-algorithm project (levelset2d_polygon) -- matching this
// project's own C++ design, which only depends on common_geometry and
// operates on already-extracted geometry supplied by the caller.

#include <pybind11/eigen.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "common_geometry/edge.hpp"
#include "levelset2d_bilinear_measure/levelset2d_bilinear_measure.hpp"

namespace py = pybind11;
using namespace ns_cg;

PYBIND11_MODULE(_levelset2d_bilinear_measure, m) {
  m.doc() =
      "Python bindings for levelset2d_bilinear_measure: measurement primitives for "
      "level-set-derived geometry (bilinear interpolation crossings, "
      "polygon-segment ray intersections).";

  py::class_<Edge2d>(m, "Edge2d")
      .def(py::init<>())
      .def(py::init<Vec2d, Vec2d>(), py::arg("start"), py::arg("end"))
      .def_property(
          "start", [](Edge2d& e) -> Vec2d& { return e.GetStart(); },
          [](Edge2d& e, Vec2d v) { e.GetStart() = std::move(v); })
      .def_property(
          "end", [](Edge2d& e) -> Vec2d& { return e.GetEnd(); },
          [](Edge2d& e, Vec2d v) { e.GetEnd() = std::move(v); });

  py::list corner_offsets;
  for (const auto& o : ns_cg::kCellCornerOffset)
    corner_offsets.append(py::make_tuple(o[0], o[1]));
  m.attr("CELL_CORNER_OFFSET") = corner_offsets;

  m.def("bilinear_value", &ns_cg::BilinearValue, py::arg("v"), py::arg("x"), py::arg("y"),
        "v: the 4 cell-corner values, ordered per CELL_CORNER_OFFSET.");

  m.def("find_bilinear_crossings", &ns_cg::FindBilinearCrossings, py::arg("v"),
        py::arg("origin"), py::arg("dir"), py::arg("samples") = 4000,
        "Every t in [0,1] where the bilinear interpolant of `v` crosses "
        "zero along the line origin + t*dir.");

  m.def("ray_segment_intersect", &ns_cg::RaySegmentIntersect, py::arg("origin"),
        py::arg("dir"), py::arg("a"), py::arg("b"),
        "The ray parameter t where segment [origin, origin+dir] crosses "
        "segment (a,b), or None.");

  m.def("find_polygon_crossings", &ns_cg::FindPolygonCrossings, py::arg("segments"),
        py::arg("origin"), py::arg("dir"),
        "Every t in [0,1] where the segment origin + t*dir crosses one of "
        "`segments`.");
}
