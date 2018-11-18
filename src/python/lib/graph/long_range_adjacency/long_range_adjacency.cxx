#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <pybind11/stl.h>

#include "nifty/python/converter.hxx"

#ifdef WITH_HDF5
#include "nifty/hdf5/hdf5_array.hxx"
#endif

#include "nifty/graph/long_range_adjacency/long_range_adjacency.hxx"

namespace py = pybind11;

namespace nifty{
namespace graph{

    using namespace py;

    template<class CLS, class BASE>
    void removeFunctions(py::class_<CLS, BASE > & clsT){
        clsT
            .def("insertEdge", [](CLS * self,const uint64_t u,const uint64_t ){
                throw std::runtime_error("cannot insert edges into 'LongRangeAdjacency'");
            })
            .def("insertEdges",[](CLS * self, py::array_t<uint64_t> pyArray) {
                throw std::runtime_error("cannot insert edges into 'LongRangeAdjacency'");
            })
        ;
    }


    template<class LABELS>
    void exportLongRangeAdjacencyT(
        py::module & module,
        const std::string & clsName,
        const std::string & facName
    ){

        typedef LABELS Labels;

        typedef UndirectedGraph<> BaseGraph;
        typedef LongRangeAdjacency<Labels> AdjacencyType;

        auto clsT = py::class_<AdjacencyType, BaseGraph>(module, clsName.c_str());
        clsT
            .def("numberOfEdgesInSlice", [](const AdjacencyType & self){
                size_t nSlices = self.shape(0);
                std::vector<size_t> out(nSlices);
                for(size_t slice = 0; slice < nSlices; ++slice){
                    out[slice] = self.numberOfEdgesInSlice(slice);
                }
                return out;
            })
            .def("edgeOffset", [](const AdjacencyType & self){
                size_t nSlices = self.shape(0);
                std::vector<size_t> out(nSlices);
                for(size_t slice = 0; slice < nSlices; ++slice){
                    out[slice] = self.edgeOffset(slice);
                }
                return out;
            })
            .def("serialize",[](const AdjacencyType & self){
                nifty::marray::PyView<uint64_t, 1> out({self.serializationSize()});
                auto ptr = &out(0);
                self.serialize(ptr);
                return out;
            })
        ;
        removeFunctions<AdjacencyType, BaseGraph>(clsT);

        // from labels
        module.def(facName.c_str(),
            [](
               Labels labels,
               const size_t range,
               const size_t numberOfLabels,
               const bool ignoreLabel,
               const int numberOfThreads
            ){
                auto ptr = new AdjacencyType(labels, range, numberOfLabels, ignoreLabel, numberOfThreads);
                return ptr;
            },
            py::return_value_policy::take_ownership,
            py::arg("labels"),
            py::arg("range"),
            py::arg("numberOfLabels"),
            py::arg_t<bool>("ignoreLabel", false),
            py::arg_t<int>("numberOfThreads", -1)
        );

        // from labels + serialization
        module.def(facName.c_str(),
            [](
               Labels labels,
               nifty::marray::PyView<uint64_t, 1> serialization
            ){

                auto startPtr = &serialization(0);
                auto lastElement = &serialization(serialization.size() - 1);
                auto d = lastElement - startPtr + 1;

                NIFTY_CHECK_OP(d,==,serialization.size(), "serialization must be contiguous");

                auto ptr = new AdjacencyType(labels, startPtr);
                return ptr;
            },
            py::return_value_policy::take_ownership,
            py::arg("labels"),
            py::arg("serialization")
        );

    }

    void exportLongRangeAdjacency(py::module & module) {

        typedef marray::PyView<uint32_t, 3> ExplicitLabels;
        exportLongRangeAdjacencyT<ExplicitLabels>(
            module,
            "ExplicitLabelsLongRangeAdjacency",
            "explicitLabelsLongRangeAdjacency"
        );

        #ifdef WITH_HDF5
        typedef hdf5::Hdf5Array<uint32_t> Hdf5Labels;
        exportLongRangeAdjacencyT<Hdf5Labels>(
            module,
            "Hdf5LabelsLongRangeAdjacency",
            "hdf5LabelsLongRangeAdjacency"
        );
        #endif
    }
}
}