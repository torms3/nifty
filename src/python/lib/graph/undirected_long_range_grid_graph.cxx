
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pytensor.hpp"
#include "xtensor-python/pyvectorize.hpp"

#include <cstddef>
//#include "nifty/python/converter.hxx"
#include "nifty/python/graph/undirected_long_range_grid_graph.hxx"
#include "nifty/graph/rag/grid_rag.hxx"



namespace py = pybind11;


namespace nifty{
namespace graph{



    using namespace py;

    template<class CLS, class BASE>
    void removeFunctions(py::class_<CLS, BASE > & clsT){
        clsT
            .def("insertEdge", [](CLS * self,const uint64_t u,const uint64_t ){
                throw std::runtime_error("cannot insert edges into 'UndirectedLongRangeGridGraph'");
            })
            .def("insertEdges",[](CLS * self, py::array_t<uint64_t> pyArray) {
                throw std::runtime_error("cannot insert edges into 'UndirectedLongRangeGridGraph'");
            })
        ;
    }


    template<std::size_t DIM, class RAG>
    void exportComputeLongRangeEdgesFromRagAndOffsets(
            py::module & graphModule
    ){
        graphModule.def("computeLongRangeEdgesFromRagAndOffsets_impl",
                      [](
                              const RAG & rag,
//                              std::array<int, DIM>       shape,
                              xt::pytensor<int64_t, 2> offsets,
//                              xt::pytensor<int64_t, 1> longRangeStrides,
//            xt::pytensor<uint32_t, DIM> &
                              xt::pytensor<uint64_t, DIM> & labels,
                              xt::pytensor<float, 1> offsetsProbs,
                              xt::pytensor<float, DIM+1> & randomProbs,
                              const int numberOfThreads
                      ){
                          typedef typename std::vector<array::StaticArray<int64_t, DIM>> OffsetVectorType;
                          typedef typename std::vector<float> OffsetProbsType;

                          OffsetVectorType offsetVector(offsets.shape()[0]);
                          OffsetProbsType offsetProbsVector(offsetsProbs.shape()[0]);

                          for(auto i=0; i<offsetVector.size(); ++i){
                              offsetProbsVector[i] = offsetsProbs(i);
                              for(auto d=0; d<DIM; ++d){
                                  offsetVector[i][d] = offsets(i, d);
                              }
                          }

                         // FIXME: no brackets???
//                          xt::pytensor<uint64_t, 2> edgeOut;
//                          {
//                              py::gil_scoped_release release;
//                              edgeOut = computeLongRangeEdgesFromRagAndOffsets(rag, offsetVector, labels, numberOfThreads);
//                          }
//                          return edgeOut;
                            py::gil_scoped_release release;
                          return computeLongRangeEdgesFromRagAndOffsets(rag, offsetVector, labels,  offsetProbsVector, randomProbs, numberOfThreads);

                      },
                      py::arg("rag"),
                      py::arg("offsets"),
                        py::arg("labels"),
                        py::arg("offsetsProbs"),

                      py::arg("randomProbs"),
                      py::arg("numberOfThreads") = 1
        );
    };


    template<std::size_t DIM, class LABELS>
    void exportUndirectedLongRangeGridGraphT(
        py::module & ragModule
    ){
        typedef UndirectedGraph<> BaseGraph;
        typedef UndirectedLongRangeGridGraph<DIM> GraphType;

        const auto clsName = GraphName<GraphType>::name();
        auto clsT = py::class_<GraphType,BaseGraph>(ragModule, clsName.c_str());

        // TODO: add input numberOfThreads and second constructor implementation
        // TODO: probably for the lables I need py::arg("labels").noconvert()
        clsT.def(py::init([](
            std::array<int, DIM>    shape,
            xt::pytensor<int64_t, 2> offsets,
            xt::pytensor<int64_t, 1> longRangeStrides,
//            xt::pytensor<uint32_t, DIM> &
            xt::pytensor<float, 1> offsetsProbs,
            xt::pytensor<float, DIM+1> & randomProbs,
            xt::pytensor<bool, 1> isLocalOffset,
//            bool startFromLabelSegm,
            const uint64_t numberOfThreads
                 ){
                    typedef typename GraphType::OffsetVector OffsetVector;
                    typedef typename GraphType::OffsetProbsType OffsetProbsType;
                     typedef typename GraphType::StridesType StridesType;
                    typedef typename GraphType::BoolVectorType isLocalType;
                    typedef typename GraphType::ShapeType ShapeType;

                    ShapeType s;
                    std::copy(shape.begin(), shape.end(), s.begin());
                    NIFTY_CHECK_OP(offsets.shape()[1], == , DIM, "offsets has wrong shape");

                     OffsetVector offsetVector(offsets.shape()[0]);
                    OffsetProbsType offsetProbsVector(offsetsProbs.shape()[0]);
                     isLocalType isLocalVector(isLocalOffset.shape()[0]);

                    for(auto i=0; i<offsetVector.size(); ++i){
                        offsetProbsVector[i] = offsetsProbs(i);
                        isLocalVector[i] = isLocalOffset(i);
                        for(auto d=0; d<DIM; ++d){
                            offsetVector[i][d] = offsets(i, d);
                        }
                    }

                    StridesType stridesVector(longRangeStrides.shape()[0]);
                    for(auto d=0; d<DIM; ++d){
                         stridesVector[d] = longRangeStrides(d);
                     }

                     // FIXME: no brackets???
                     py::gil_scoped_release release;
                    return new GraphType(s, offsetVector, stridesVector, offsetProbsVector, isLocalVector, randomProbs, numberOfThreads);
        }),
        py::arg("shape"),
        py::arg("offsets"),
        py::arg("longRangeStrides"),
//         py::arg("nodeLabels"),
         py::arg("offsetsProbs"),
         py::arg("randomProbs"),
         py::arg("isLocalOffset"),
         py::arg("numberOfThreads") = 1
                )
        // FIXME: I need to check that the edges indeed exists
//        .def("nodeFeatureDiffereces", [](
//            const GraphType & g,
//            xt::pytensor<float, DIM+1> nodeFeatures
//        ){
//            return g.nodeFeatureDiffereces(nodeFeatures);
//        })
//        .def("nodeFeatureDiffereces2", [](
//            const GraphType & g,
//            xt::pytensor<float, DIM+2> nodeFeatures
//        ){
//            return g.nodeFeatureDiffereces2(nodeFeatures);
//        })
        .def("edgeValues", [](
            const GraphType & g,
            xt::pytensor<float, DIM+1> nodeFeatures
        ){
                return g.edgeValues(nodeFeatures);
        })
        .def("nodeValues", [](
                const GraphType & g,
                xt::pytensor<float, DIM> nodeFeatures
        ){
            return g.nodeValues(nodeFeatures);
        })
        .def("mapEdgesIDToImage", [](
                const GraphType & g
        ){
            return g.mapEdgesIDToImage();
        })
        .def("mapNodesIDToImage", [](
                const GraphType & g
        ){
            return g.mapNodesIDToImage();
        })
        .def("edgeOffsetIndex", [](const GraphType & g){
            return g.edgeOffsetIndex();
        })
        .def("findLocalEdges", [](
                const GraphType & g,
                xt::pytensor<int64_t, DIM> & nodeLabels
        ){
            return g.findLocalEdges(nodeLabels);
        })
        ;

        removeFunctions<GraphType, BaseGraph>(clsT);


    }





    void exportUndirectedLongRangeGridGraph(py::module & graphModule) {

        typedef xt::pytensor<uint32_t, 3> ExplicitLabels3D;
        typedef GridRag<3, ExplicitLabels3D> Rag3d;
        exportComputeLongRangeEdgesFromRagAndOffsets<3, Rag3d >(graphModule);

        exportUndirectedLongRangeGridGraphT<2, uint32_t>(graphModule);
        exportUndirectedLongRangeGridGraphT<3, uint32_t>(graphModule);


  
    }


} // end namespace graph
} // end namespace nifty
