#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>


#include "xtensor-python/pyarray.hpp"
#include "xtensor-python/pytensor.hpp"
#include "xtensor-python/pyvectorize.hpp"


#include "nifty/tools/runtime_check.hxx"
#include "nifty/python/converter.hxx"
#include "nifty/python/graph/undirected_list_graph.hxx"
#include "nifty/python/graph/undirected_grid_graph.hxx"
#include "nifty/python/graph/agglo/export_agglomerative_clustering.hxx"
#include "nifty/graph/graph_maps.hxx"
#include "nifty/graph/agglo/agglomerative_clustering.hxx"


#include "nifty/graph/agglo/cluster_policies/fixation_cluster_policy.hxx"
#include "nifty/graph/agglo/cluster_policies/detail/merge_rules.hxx"

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);

namespace nifty{
namespace graph{
namespace agglo{


    template<class GRAPH, class ACC_0, bool WITH_UCM>
    void exportfixationClusterPolicyTT(py::module & aggloModule) {

        typedef GRAPH GraphType;
        const auto graphName = GraphName<GraphType>::name();
        typedef xt::pytensor<float, 1>   PyViewFloat1;
        typedef xt::pytensor<uint8_t, 1> PyViewUInt8_1;
        const std::string withUcmStr =  WITH_UCM ? std::string("WithUcm") :  std::string() ;

        {
            // name and type of cluster operator
            typedef FixationClusterPolicy<GraphType, ACC_0, WITH_UCM> ClusterPolicyType;
            const auto clusterPolicyBaseName = std::string("FixationClusterPolicy") +  withUcmStr;
            const auto clusterPolicyBaseName2 = clusterPolicyBaseName + ACC_0::staticName();
            const auto clusterPolicyClsName = clusterPolicyBaseName + graphName + ACC_0::staticName();
            const auto clusterPolicyFacName = lowerFirst(clusterPolicyBaseName);

            // the cluster operator cls
            auto clusterClass = py::class_<ClusterPolicyType>(aggloModule, clusterPolicyClsName.c_str())
                //.def_property_readonly("mergePrios", &ClusterPolicyType::mergePrios)
                //.def_property_readonly("notMergePrios", &ClusterPolicyType::notMergePrios)
                //.def_property_readonly("edgeSizes", &ClusterPolicyType::edgeSizes)
            ;

            clusterClass
                    .def("exportAgglomerationData", [](
                            ClusterPolicyType * self
                         ){
                             return self->exportAgglomerationData();
                         }
                    );


            // factory
            aggloModule.def(clusterPolicyFacName.c_str(),
                [](
                    const GraphType & graph,
                    const PyViewFloat1 & signedWeights,
                    const PyViewUInt8_1 & isLocalEdge,
                    const PyViewFloat1 & edgeSizes,
                    const PyViewFloat1 & nodeSizes,
                    const typename ClusterPolicyType::Acc0SettingsType updateRule0,
                    const uint64_t numberOfNodesStop,
                    const double sizeRegularizer,
                    const bool addNonLinkConstraints
                ){
                    typename ClusterPolicyType::SettingsType s;
                    s.numberOfNodesStop = numberOfNodesStop;
                    s.sizeRegularizer = sizeRegularizer;
                    s.updateRule0 = updateRule0;
                    s.addNonLinkConstraints = addNonLinkConstraints;
                    auto ptr = new ClusterPolicyType(graph, signedWeights, isLocalEdge, edgeSizes, nodeSizes, s);
                    return ptr;
                },
                py::return_value_policy::take_ownership,
                py::keep_alive<0,1>(), // graph
                py::arg("graph"),
                py::arg("signedWeights"),
                py::arg("isMergeEdge"),
                py::arg("edgeSizes"),
                py::arg("nodeSizes"),
                py::arg("updateRule0"),
                py::arg("numberOfNodesStop") = 1,
                py::arg("sizeRegularizer") = 0.,
                py::arg("addNonLinkConstraints") = false,
            );

            // export the agglomerative clustering functionality for this cluster operator
            exportAgglomerativeClusteringTClusterPolicy<ClusterPolicyType>(aggloModule, clusterPolicyBaseName2);
        }
    }


//    template<class GRAPH, class ACC_0>
//    void exportfixationClusterPolicyT(py::module & aggloModule) {
//        exportfixationClusterPolicyTT<GRAPH, ACC_0, false>(aggloModule);
//        //exportfixationClusterPolicy<GRAPH, ACC_0, true >(aggloModule);
//    }


    void exportFixationAgglomerativeClustering(py::module & aggloModule) {
        {
            typedef PyUndirectedGraph GraphType;

            typedef merge_rules::SumEdgeMap<GraphType, float >  SumAcc;
            typedef merge_rules::ArithmeticMeanEdgeMap<GraphType, float >  ArithmeticMeanAcc;
            typedef merge_rules::GeneralizedMeanEdgeMap<GraphType, float > GeneralizedMeanAcc;
            typedef merge_rules::SmoothMaxEdgeMap<GraphType, float >       SmoothMaxAcc;
            typedef merge_rules::RankOrderEdgeMap<GraphType, float >       RankOrderAcc;
            typedef merge_rules::MaxEdgeMap<GraphType, float >             MaxAcc;
            typedef merge_rules::MinEdgeMap<GraphType, float >             MinAcc;
            typedef merge_rules::MutexWatershedEdgeMap<GraphType, float >             MWSAcc;


            exportfixationClusterPolicyTT<GraphType, SumAcc, false >(aggloModule);
            exportfixationClusterPolicyTT<GraphType, ArithmeticMeanAcc, false >(aggloModule);
            exportfixationClusterPolicyTT<GraphType, SmoothMaxAcc, false>(aggloModule);
            exportfixationClusterPolicyTT<GraphType, GeneralizedMeanAcc, false>(aggloModule);
            exportfixationClusterPolicyTT<GraphType, RankOrderAcc , false>(aggloModule);
            exportfixationClusterPolicyTT<GraphType, MaxAcc, false>(aggloModule);
            exportfixationClusterPolicyTT<GraphType, MinAcc, false>(aggloModule);
            exportfixationClusterPolicyTT<GraphType, MWSAcc, false>(aggloModule);


        }

    }

} // end namespace agglo
} // end namespace graph
} // end namespace nifty
