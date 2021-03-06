# sphinx_gallery_thumbnail_number = 4
from __future__ import absolute_import
from . import _graph as __graph
from ._graph import *

from .. import Configuration
from . import opt

from . opt import multicut
from . opt import lifted_multicut
from . opt import mincut
from . opt import minstcut

import numpy
from functools import partial
import types
import sys
import warnings

__all__ = []

for key in __graph.__dict__.keys():
    try:
        __graph.__dict__[key].__module__='nifty.graph'
    except:
        pass
    __all__.append(key)


UndirectedGraph.__module__ = "nifty.graph"


ilpSettings = multicut.ilpSettings





# multicut objective
UndirectedGraph.MulticutObjective                     = multicut.MulticutObjectiveUndirectedGraph
UndirectedGraph.EdgeContractionGraph                  = EdgeContractionGraphUndirectedGraph
EdgeContractionGraphUndirectedGraph.MulticutObjective = multicut.MulticutObjectiveEdgeContractionGraphUndirectedGraph


UndirectedGraph.MincutObjective                     = mincut.MincutObjectiveUndirectedGraph
UndirectedGraph.EdgeContractionGraph                = EdgeContractionGraphUndirectedGraph
EdgeContractionGraphUndirectedGraph.MincutObjective = mincut.MincutObjectiveEdgeContractionGraphUndirectedGraph

# #minstcut objective
# UndirectedGraph.MinstcutObjective                     = minstcut.MinstcutObjectiveUndirectedGraph
# UndirectedGraph.EdgeContractionGraph                = EdgeContractionGraphUndirectedGraph
# EdgeContractionGraphUndirectedGraph.MinstcutObjective = minstcut.MinstcutObjectiveEdgeContractionGraphUndirectedGraph

# lifted multicut objective
UndirectedGraph.LiftedMulticutObjective = lifted_multicut.LiftedMulticutObjectiveUndirectedGraph




def randomGraph(numberOfNodes, numberOfEdges):
    g = UndirectedGraph(numberOfNodes)

    uv = numpy.random.randint(low=0, high=numberOfNodes-1, size=numberOfEdges*2)
    uv = uv.reshape([-1,2])

    where = numpy.where(uv[:,0]!=uv[:,1])[0]
    uv = uv[where,:]


    g.insertEdges(uv)
    while( g.numberOfEdges < numberOfEdges):
        u,v = numpy.random.randint(low=0, high=numberOfNodes-1, size=2)
        if u != v:
            g.insertEdge(int(u),int(v))
    return g




class EdgeContractionGraphCallback(EdgeContractionGraphCallbackImpl):
    def __init__(self):
        super(EdgeContractionGraphCallback, self).__init__()

        try:
            self.contractEdgeCallback = self.contractEdge
        except AttributeError:
            pass

        try:
            self.mergeEdgesCallback = self.mergeEdges
        except AttributeError:
            pass

        try:
            self.mergeNodesCallback = self.mergeNodes
        except AttributeError:
            pass

        try:
            self.contractEdgeDoneCallback = self.contractEdgeDone
        except AttributeError:
            pass

def edgeContractionGraph(g, callback):
    Ecg = g.__class__.EdgeContractionGraph
    ecg = Ecg(g, callback)
    return ecg





def undirectedGraph(numberOfNodes):
    return UndirectedGraph(numberOfNodes)

def undirectedGridGraph(shape, simpleNh=True):
    if not simpleNh:
        raise RuntimeError("currently only simpleNh is implemented")
    s = [int(s) for s in shape]
    if(len(s) == 2):
        return UndirectedGridGraph2DSimpleNh(s)
    elif(len(s) == 3):
        return UndirectedGridGraph3DSimpleNh(s)
    else:
        raise RuntimeError("currently only 2D and 3D grid graph is exposed to python")

gridGraph = undirectedGridGraph

def undirectedLongRangeGridGraph(shape, offsets, is_local_offset, offsets_probabilities=None, labels=None, strides=None,
                                 number_of_threads=1, mask_used_edges=None):
    """
    :param offsets_probabilities: Probability that a repulsive long-range edge is intriduced. If None all long range connections are used
    :param labels: Indices of a passed segmentation (uint64)
    :param is_local_offset: boolean array of shape=nb_offsets
    :param mask_used_edges: Boolean array with expected shape (nb_offsets, x, y, z). If this argument is not None,
            `offsets_probabilities` is ignored. The mask on local edges is ignored (they are always part of the graph)
    """
    # TODO: bad design. Local edges could be skipped (especially in the masks...)
    offsets = numpy.require(offsets, dtype='int64')
    assert offsets.shape[0] == is_local_offset.shape[0]
    is_local_offset = is_local_offset.astype(numpy.bool)


    shape = list(shape)
    if len(shape) == 2:
        G = UndirectedLongRangeGridGraph2D
    elif len(shape) == 3:
        G = UndirectedLongRangeGridGraph3D
    else:
        raise RuntimeError("wrong dimension: undirectedLongRangeGridGraph is only implemented for 2D and 3D")

    if labels is None:
        labels = numpy.zeros(shape, dtype=numpy.uint32)
        start_from_labels = False
    else:
        raise NotImplementedError("Atm node labels are no longer supported")
        labels = labels.astype(numpy.uint32)
        start_from_labels = True


    randomProbsShape = tuple(shape) + (offsets.shape[0],)
    randomProbs = None
    if mask_used_edges is not None:
        assert mask_used_edges.ndim == 4
        mask_used_edges = numpy.rollaxis(mask_used_edges, axis=0, start=4)
        assert mask_used_edges.shape == randomProbsShape
        randomProbs = 1. - mask_used_edges.astype('float32')
        if offsets_probabilities is not None:
            warnings.warn("Offset probabilities are ignored when masked edges are passed")

    if offsets_probabilities is None:
        offsets_probabilities = numpy.ones((offsets.shape[0],), dtype='float')
        if randomProbs is None:
            randomProbs = numpy.ones(randomProbsShape, dtype='float') * 0.5
    else:
        if isinstance(offsets_probabilities, float):
            offsets_probabilities = numpy.ones((offsets.shape[0],), dtype='float') * offsets_probabilities
        else:
            assert offsets_probabilities.shape[0] == offsets.shape[0]
            offsets_probabilities = numpy.require(offsets_probabilities, dtype='float')
        # Randomly select which edges to add or not
        # It could be moved to C++ because atm we cannot initialize the graph in a 'thread-safe' way...
        if randomProbs is None:
            randomProbs = numpy.random.random(randomProbsShape)


    if strides is None:
        strides = numpy.ones(len(shape), dtype='int')
    else:
        if isinstance(strides, list):
            strides = numpy.array(strides)
        assert strides.shape[0] == len(shape)


    return G(shape, offsets, strides, offsets_probabilities, randomProbs, is_local_offset, number_of_threads)

longRangeGridGraph = undirectedLongRangeGridGraph


def compute_lifted_edges_from_rag_and_offsets(rag,
                                              labels,
                                        offsets,
                                        offsets_probabilities=None,
                                        number_of_threads=1,
                                              mask_used_edges=None):
    """
    :param offsets_probabilities: Probability that a repulsive long-range edge is intriduced. If None a "dense" graph is used
    :param labels: Indices of a passed segmentation (uint64)
    :param is_local_offset: boolean array of shape=nb_offsets
    :param affinity_mask: Skip some of the edges in the graph (bool array of shape (nb_offsets, x, y, z), False skips the edge)
    """
    # TODO: bad design. Local edges could be skipped.
    offsets = numpy.require(offsets, dtype='int64')

    shape = list(labels.shape)

    randomProbsShape = tuple(shape) + (offsets.shape[0],)

    randomProbs = None
    if mask_used_edges is not None:
        assert mask_used_edges.ndim == 4
        mask_used_edges = numpy.rollaxis(mask_used_edges, axis=0, start=4)
        assert mask_used_edges.shape == randomProbsShape, "{} {}".format(mask_used_edges.shape, randomProbsShape)
        randomProbs = mask_used_edges.astype('float32')
        if offsets_probabilities is not None:
            warnings.warn("Offset probabilities are ignored when masked edges are passed")

    if offsets_probabilities is None:
        offsets_probabilities = numpy.ones((offsets.shape[0],), dtype='float')
        if randomProbs is None:
            randomProbs = numpy.ones(randomProbsShape, dtype='float') * 0.5
    else:
        if isinstance(offsets_probabilities, float):
            offsets_probabilities = numpy.ones((offsets.shape[0],), dtype='float') * offsets_probabilities
        else:
            assert offsets_probabilities.shape[0] == offsets.shape[0]
            offsets_probabilities = numpy.require(offsets_probabilities, dtype='float')
        # Randomly select which edges to add or not
        # It could be moved to C++ because atm we cannot initialize the graph in a 'thread-safe' way...
        if randomProbs is None:
            randomProbs = numpy.random.random(randomProbsShape)

    return computeLongRangeEdgesFromRagAndOffsets_impl(rag, offsets,labels,offsets_probabilities,randomProbs,
                                                                   number_of_threads)



def drawGraph(graph, method='spring'):

    import networkx

    G=networkx.Graph()
    for node in graph.nodes():
        G.add_node(node)

    #uvIds = graph.uvIds()
    #for i in range(uvIds.shape[0]):
    #    u,v = uvIds[i,:]
    #    G.add_edge(u,v)
    for edge in graph.edges():
        u,v = graph.uv(edge)
        G.add_edge(u,v)

    nodeLabels = dict()

    for node in graph.nodes():
        nodeLabels[node] = str(node)
    if method == 'spring':
        networkx.draw_spring(G,labels=nodeLabels)
    else:
        networkx.draw(G, lables=nodeLabels)
