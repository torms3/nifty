#include <pybind11/pybind11.h>
#include <iostream>

namespace py = pybind11;

PYBIND11_DECLARE_HOLDER_TYPE(T, std::shared_ptr<T>);


namespace nifty{
namespace graph{


    void exportUndirectedListGraph(py::module &);
    void exportEdgeContractionGraphUndirectedGraph(py::module & );

    void initSubmoduleMulticut(py::module &);
    void initSubmoduleRag(py::module &);
    void initSubmoduleGala(py::module &);

    void initSubmoduleGraph(py::module &niftyModule) {
        auto graphModule = niftyModule.def_submodule("graph","graph submodule");


        exportUndirectedListGraph(graphModule);
        exportEdgeContractionGraphUndirectedGraph(graphModule);

        initSubmoduleMulticut(graphModule);
        initSubmoduleRag(graphModule);
        initSubmoduleGala(graphModule);
    }

}
}
