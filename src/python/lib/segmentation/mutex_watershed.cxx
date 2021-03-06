#include "pybind11/pybind11.h"
#include "pybind11/stl.h"

#include "xtensor-python/pytensor.hpp"
#include "xtensor-python/pyarray.hpp"

#include "nifty/segmentation/mutex_watershed.hxx"
#include "nifty/segmentation/connected_components.hxx"

namespace py = pybind11;

namespace nifty {
    namespace segmentation {

        void exportMutexWatershed(py::module & m) {
            m.def("compute_mws_clustering",[](const uint64_t number_of_labels,
                                              const xt::pytensor<uint64_t, 2> & uvs,
                                              const xt::pytensor<uint64_t, 2> & mutex_uvs,
                                              const xt::pytensor<float, 1> & weights,
                                              const xt::pytensor<float, 1> & mutex_weights){
                      xt::pytensor<uint64_t, 1> node_labeling = xt::zeros<uint64_t>({(int64_t) number_of_labels});
                      {
                          py::gil_scoped_release allowThreads;
                          compute_mws_clustering(number_of_labels, uvs,
                                                               mutex_uvs, weights,
                                                               mutex_weights, node_labeling);
                      }
                      return node_labeling;
                  }, py::arg("number_of_labels"),
                  py::arg("uvs"), py::arg("mutex_uvs"),
                  py::arg("weights"), py::arg("mutex_weights"));


            m.def("compute_mws_segmentation_impl",[](const xt::pytensor<int64_t, 1> & sorted_flat_indices,
                                                     const xt::pytensor<bool, 1> & valid_edges,
                                                     const std::vector<std::vector<int>> & offsets,
                                                     const size_t number_of_attractive_channels,
                                                     const std::vector<int> & image_shape){
                      int64_t number_of_nodes = 1;
                      for (auto & s: image_shape){
                          number_of_nodes *= s;
                      }
                      xt::pytensor<uint64_t, 1> node_labeling = xt::zeros<uint64_t>({number_of_nodes});
                      {
                          py::gil_scoped_release allowThreads;
                          compute_mws_segmentation(sorted_flat_indices,
                                                                 valid_edges,
                                                                 offsets,
                                                                 number_of_attractive_channels,
                                                                 image_shape,
                                                                 node_labeling);
                      }
                      return node_labeling;
                  }, py::arg("sorted_flat_indices"),
                  py::arg("valid_edges"),
                  py::arg("offsets"),
                  py::arg("number_of_attractive_channels"),
                  py::arg("image_shape"));


            m.def("compute_divisive_mws_segmentation_impl",[](const xt::pytensor<int64_t, 1> & sorted_flat_indices,
                                                              const xt::pytensor<bool, 1> & valid_edges,
                                                              const std::vector<std::vector<int>> & offsets,
                                                              const size_t number_of_attractive_channels,
                                                              const std::vector<int> & image_shape){
                      int64_t number_of_nodes = 1;
                      for (auto & s: image_shape){
                          number_of_nodes *= s;
                      }
                      xt::pytensor<uint64_t, 1> node_labeling = xt::zeros<uint64_t>({number_of_nodes});
                      {
                          py::gil_scoped_release allowThreads;
                          compute_divisive_mws_segmentation(sorted_flat_indices,
                                                                          valid_edges,
                                                                          offsets,
                                                                          number_of_attractive_channels,
                                                                          image_shape,
                                                                          node_labeling);
                      }
                      return node_labeling;
                  }, py::arg("sorted_flat_indices"),
                  py::arg("valid_edges"),
                  py::arg("offsets"),
                  py::arg("number_of_attractive_channels"),
                  py::arg("image_shape"));

            m.def("compute_mws_prim_segmentation_impl",[](const xt::pytensor<float, 1> & edge_weights,
                                                          const xt::pytensor<bool, 1> & valid_edges,
                                                          const std::vector<std::vector<int>> & offsets,
                                                          const size_t number_of_attractive_channels,
                                                          const std::vector<int> & image_shape){
                      int64_t number_of_nodes = 1;
                      for (auto & s: image_shape){
                          number_of_nodes *= s;
                      }
                      xt::pytensor<uint64_t, 1> node_labeling = xt::zeros<uint64_t>({number_of_nodes});
                      {
                          py::gil_scoped_release allowThreads;
                          compute_mws_prim_segmentation(edge_weights,
                                                                      valid_edges,
                                                                      offsets,
                                                                      number_of_attractive_channels,
                                                                      image_shape,
                                                                      node_labeling);
                      }
                      return node_labeling;
                  }, py::arg("edge_weights"),
                  py::arg("valid_edges"),
                  py::arg("offsets"),
                  py::arg("number_of_attractive_channels"),
                  py::arg("image_shape"));


        }

    }
}
