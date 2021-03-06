#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>
#include "xtensor-python/pytensor.hpp"

#include "nifty/cgp/geometry.hxx"
#include "nifty/cgp/bounds.hxx"

#include "nifty/cgp/features/geometric_features.hxx"
#include "nifty/cgp/features/topological_features.hxx"

namespace py = pybind11;



namespace nifty{
namespace cgp{


    void exportFeatures(py::module & module) {


        {
            typedef Cell1CurvatureFeatures2D Op;
            const auto clsName = std::string("Cell1CurvatureFeatures2D");
            auto pyCls = py::class_<Op>(module, clsName.c_str());

            pyCls
            .def(
                py::init< const std::vector<float> & >(),
                py::arg("sigmas")    = std::vector<float>({1.0f, 2.0f, 4.0f})
            )
            .def("__call__",
                [](
                    const Op & op,
                    const CellGeometryVector<2,1> &  cell1GeometryVector,
                    const CellBoundedByVector<2,1> & cell1BoundedByVector
                ){
                    const int64_t nFeatures = op.numberOfFeatures();
                    const int64_t nCells1   = cell1GeometryVector.size();

                    xt::pytensor<float, 2> out = xt::zeros<float>({nCells1, nFeatures});

                    op(cell1GeometryVector, cell1BoundedByVector, out);

                    return  out;
                },
                py::arg("cell1GeometryVector"),
                py::arg("cell1BoundedByVector")
            )
            .def("names", &Op::names)
            ;
        }
        {
            typedef Cell1LineSegmentDist2D Op;
            const auto clsName = std::string("Cell1LineSegmentDist2D");
            auto pyCls = py::class_<Op>(module, clsName.c_str());

            pyCls
            .def(
                py::init< const std::vector<std::size_t> &>(),
                py::arg("dists")  =  std::vector<std::size_t>({std::size_t(3),std::size_t(5),std::size_t(7)})
            )
            .def("__call__",
                [](
                    const Op & op,
                    const CellGeometryVector<2,1> &  cell1GeometryVector
                ){
                    const int64_t nFeatures = op.numberOfFeatures();
                    const int64_t nCells1   = cell1GeometryVector.size();

                    xt::pytensor<float, 2> out({nCells1, nFeatures});

                    op(cell1GeometryVector, out);

                    return  out;
                },
                py::arg("cell1GeometryVector")
            )
            .def("names", &Op::names)
            ;
        }
        {
            typedef Cell1BasicGeometricFeatures2D Op;
            const auto clsName = std::string("Cell1BasicGeometricFeatures2D");
            auto pyCls = py::class_<Op>(module, clsName.c_str());

            pyCls
            .def( py::init< >())
            .def("__call__",
                [](
                    const Op & op,
                    const CellGeometryVector<2,1>   & cell1GeometryVector,
                    const CellGeometryVector<2,2>   & cell2GeometryVector,
                    const CellBoundsVector<2,1>     & cell1BoundsVector
                ){
                    const int64_t nFeatures = op.numberOfFeatures();
                    const int64_t nCells1   = cell1GeometryVector.size();

                    xt::pytensor<float, 2> out({nCells1, nFeatures});

                    op( cell1GeometryVector,
                        cell2GeometryVector,
                        cell1BoundsVector,
                        out
                    );

                    return  out;
                },
                py::arg("cell1GeometryVector"),
                py::arg("cell2GeometryVector"),
                py::arg("cell1BoundsVector")
            )
            .def("names", &Op::names)
            ;
        }
        {
            typedef Cell1BasicTopologicalFeatures Op;
            const auto clsName = std::string("Cell1BasicTopologicalFeatures2D");
            auto pyCls = py::class_<Op>(module, clsName.c_str());

            pyCls
            .def( py::init< >())
            .def("__call__",
                [](
                    const Op & op,
                    const CellBoundsVector<2,0>     & cell0BoundsVector,
                    const CellBoundsVector<2,1>     & cell1BoundsVector,
                    const CellBoundedByVector<2,1>  & cell1BoundedByVector,
                    const CellBoundedByVector<2,2>  & cell2BoundedByVector
                ){
                    const int64_t nFeatures = op.numberOfFeatures();
                    const int64_t nCells1   = cell1BoundsVector.size();

                    xt::pytensor<float, 2> out({nCells1, nFeatures});

                    op( cell0BoundsVector,
                        cell1BoundsVector,
                        cell1BoundedByVector,
                        cell2BoundedByVector,
                        out
                    );

                    return  out;
                },
                py::arg("cell0BoundsVector"),
                py::arg("cell1BoundsVector"),
                py::arg("cell1BoundedByVector"),
                py::arg("cell2BoundedByVector"))

            .def("names", &Op::names)
            ;
        }
    }

}
}
