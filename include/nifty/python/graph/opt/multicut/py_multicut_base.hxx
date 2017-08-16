#pragma once

#include "nifty/graph/opt/multicut/multicut_base.hxx"

namespace nifty {
namespace graph {
namespace opt{
namespace multicut{






template<class OBJECTIVE>
class PyMulticutBase : public MulticutBase<OBJECTIVE> {
public:
    /* Inherit the constructors */
    // using MulticutFactory<Objective>::MulticutFactory;

    typedef OBJECTIVE Objective;
    typedef MulticutVisitorBase<OBJECTIVE> VisitorBaseType;
    typedef MulticutBase<Objective> McBase;
    typedef typename Objective::Graph Graph;
    typedef typename Graph:: template EdgeMap<uint8_t>  EdgeLabels;
    typedef typename Graph:: template NodeMap<uint64_t> NodeLabelsType;


    /* Trampoline (need one for each virtual function) */
    void optimize(NodeLabelsType & nodeLabels, VisitorBaseType * visitor) {
        PYBIND11_OVERLOAD_PURE(
            void,                  /* Return type */
            McBase,                /* Parent class */
            optimize,              /* Name of function */
            nodeLabels,  visitor   /* Argument(s) */
        );
    }

    const NodeLabelsType & currentBestNodeLabels()  {
        PYBIND11_OVERLOAD_PURE(
            const NodeLabelsType &,                 /* Return type */
            McBase,                             /* Parent class */
            currentBestNodeLabels,              /* Name of function */
        );
    }

    double currentBestEnergy()  {
        PYBIND11_OVERLOAD_PURE(
            double,                  /* Return type */
            McBase,                  /* Parent class */
            currentBestEnergy,       /* Name of function */
        );
    }

    const Objective & objective() const {
        PYBIND11_OVERLOAD_PURE(
            const Objective & ,    /* Return type */
            McBase,                /* Parent class */
            objective              /* Name of function */
        );
    }

    std::string name() const{
        PYBIND11_OVERLOAD_PURE(
            std::string ,    /* Return type */
            McBase,          /* Parent class */
            name             /* Name of function */
        );
    }
};

} // namespace multicut
} // namespace opt
} // namespace graph
} // namespace nifty
