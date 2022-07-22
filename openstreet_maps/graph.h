// graph.h <Starter Code>
// < Kevin Le, Sean Atangan>
//
// Basic graph class using adjacency list representation.
// Uses a map of a map to impement the list.
//
// University of Illinois at Chicago
// CS 251: Fall 2020
// Project #7 - Openstreet Maps
//

#pragma once

#include <iostream>
#include <stdexcept>
#include <vector>
#include <set>
#include <map> 

using namespace std;

template<typename VertexT, typename WeightT>
class graph {
 private:

  map<VertexT, map<VertexT, WeightT>> MapData;
  int edgeCount = 0;
  
 public:

  //
  // NumVertices
  //
  // Returns the # of vertices currently in the graph.
  //
  int NumVertices() const {
    return MapData.size();
  }

  //
  // NumEdges
  //
  // Returns the # of edges currently in the graph.
  //
  int NumEdges() const {

    int count = 0;  //use private member variable for this instead of looping
    for (auto &e: MapData) {
         count = count + e.second.size();
    }
    return count;
  }

  //
  // addVertex
  //
  // Adds the vertex v to the graph if there's room, and if so
  // returns true.  If the graph is full, or the vertex already
  // exists in the graph, then false is returned.
  //
  bool addVertex(VertexT v) { // False if Vertex already exists
    if (MapData.count(v) > 0) {
        return false;
    }
    MapData[v];  // Initialize vertices with empty edges and weights
    return true;
  }

  //
  // addEdge
  //
  // Adds the edge (from, to, weight) to the graph, and returns
  // true.  If the vertices do not exist or for some reason the
  // graph is full, false is returned.
  //
  // NOTE: if the edge already exists, the existing edge weight
  // is overwritten with the new edge weight.
  //
  bool addEdge(VertexT from, VertexT to, WeightT weight) {
    typename map<VertexT, map<VertexT, WeightT>> :: iterator bar;
    if (MapData.count(from) == 0) { // False if vertex doesn't exist
        return false;
    }
    if (MapData.count(to) == 0) { // False if vertex doesn't exist
        return false;
    }
    MapData[from][to] = weight;  // overwrites
    return true;
  }

  //
  // getWeight
  //
  // Returns the weight associated with a given edge.  If
  // the edge exists, the weight is returned via the reference
  // parameter and true is returned.  If the edge does not
  // exist, the weight parameter is unchanged and false is
  // returned.
  //
  bool getWeight(VertexT from, VertexT to, WeightT& weight) const {
    if (MapData.count(from) == 0) { // False if vertex doesn't exist
        return false;
    }
    if (MapData.count(to) == 0) { // False if vertex doesn't exist
        return false;
    }
    map<VertexT,WeightT> Edge;
    Edge = MapData.at(from);
    for (auto &e : Edge) {  // Edge exists
      if (e.first == to) {
          weight = e.second;
          return true;
      }
    }
    return false;
  }

  //
  // neighbors
  //
  // Returns a set containing the neighbors of v, i.e. all
  // vertices that can be reached from v along one edge.
  // Since a set is returned, the neighbors are returned in
  // sorted order; use foreach to iterate through the set.
  //
  set<VertexT> neighbors(VertexT v) const {
    set<VertexT>  S;
    if (MapData.count(v) == 0) { // Returns empty set if vertex doesn't exist
        return S;
    }
    map<VertexT,WeightT> Edge;
    Edge = MapData.at(v);
    for (auto &e: Edge) {
        S.insert(e.first);
    }
    return S;
  }

  //
  // getVertices
  //
  // Returns a vector containing all the vertices currently in
  // the graph.
  //
  vector<VertexT> getVertices() const {
    vector<VertexT> V;
    for (auto &e : MapData) {  
        V.push_back(e.first);
    }
    return V;
  }

  //
  // dump
  //
  // Dumps the internal state of the graph for debugging purposes.
  //
  // Example:
  //    graph<string,int>  G(26);
  //    ...
  //    G.dump(cout);  // dump to console
  //
  void dump(ostream& output) const {
    output << "***************************************************" << endl;
    output << "********************* GRAPH ***********************" << endl;
    output << "**Num vertices: " << this->NumVertices() << endl;
    output << "**Num edges: " << this->NumEdges() << endl;
    output << endl;
    output << "**Vertices:" << endl;
    int i = 0;
    for (auto &e : MapData) {
        output << " " << i << ". " << e.first << endl;
        i++;
    }
    output << endl;
    output << "**Edges:" << endl;
    for (auto &e : MapData) {
        output << " " << e.first << ": ";
        for (auto &f : e.second) {
            output << "( " << e.first << "," << f.first << "," << f.second << ") ";
        }
        output << endl;
    }
    output << "**************************************************" << endl;
  }
};