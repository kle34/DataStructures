// application.cpp <Starter Code>
// <Kevin Le, Sean Atangan>
//
// University of Illinois at Chicago
// CS 251: Fall 2020
// Project #7 - Openstreet Maps
//
// References:
// TinyXML: https://github.com/leethomason/tinyxml2
// OpenStreetMap: https://www.openstreetmap.org
// OpenStreetMap docs:
//   https://wiki.openstreetmap.org/wiki/Main_Page
//   https://wiki.openstreetmap.org/wiki/Map_Features
//   https://wiki.openstreetmap.org/wiki/Node
//   https://wiki.openstreetmap.org/wiki/Way
//   https://wiki.openstreetmap.org/wiki/Relation
//

#include <iostream>
#include <iomanip>  /*setprecision*/
#include <string>
#include <vector>
#include <map>
#include <stack>
#include <queue>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <limits>
#include "graph.h"
#include "tinyxml2.h"
#include "dist.h"
#include "osm.h"

using namespace std;
using namespace tinyxml2;


class prioritize {
 public:
    bool operator()(const pair<long long, double>& p1,
                  const pair<long long, double>& p2) const {
    if (p1.second > p2.second) {
      return true;
    } else if (p1.second < p2.second) {
      return false;
    } else {  // values are the same, decide based on keys:
      return p1.first > p2.first;
    }
  }
};


double getDistance(long long A, long long B,
map<long long, Coordinates>  Nodes) {
    double LatA = Nodes[A].Lat;
    double LonA = Nodes[A].Lon;
    double LatB = Nodes[B].Lat;
    double LonB = Nodes[B].Lon;
    double distance = distBetween2Points(LatA, LonA, LatB, LonB);
    return distance;
}

void putVertices(graph<long long, double> &G,
map<long long, Coordinates>  Nodes) {
  for (auto &e : Nodes) {
      G.addVertex(e.first);  // Add each node into the graph
  }
}

void putEdges(graph<long long, double> &G, vector<FootwayInfo> Footways,
map<long long, Coordinates> Nodes) {
    double distance;
    for (long unsigned int i = 0; i < Footways.size(); i++) {
        if (Footways[i].Nodes.size() >= 2) {
            for (long unsigned int j = 0; j < Footways[i].Nodes.size(); j++) {
                if (j+1 != Footways[i].Nodes.size()) {
                    distance = getDistance(Footways[i].Nodes[j],
                    Footways[i].Nodes[j+1], Nodes);
                    G.addEdge(Footways[i].Nodes[j],
                    Footways[i].Nodes[j+1], distance);
                    G.addEdge(Footways[i].Nodes[j+1],
                    Footways[i].Nodes[j], distance);
                }
            }
        }
    }
}

void printBuildingInfo(string name, double lat, double lon) {
    cout << " " << name << endl;
    cout << " " << "(" << lat << ", " << lon << ")" << endl;
}

void findBuilding(string startBuilding, string destBuilding,
vector<BuildingInfo> Buildings, int &startI, int &destI) {
    bool startFound = false;
    bool destFound = false;
    for (long unsigned int i = 0; i < Buildings.size(); i++) {
        if ((startBuilding.compare(Buildings[i].Abbrev) == 0)
        && startBuilding.length() == Buildings[i].Abbrev.length()) {
            startFound = true;
            startI = i;
            break;
        } else if (Buildings[i].Fullname.find(startBuilding)
            != std::string::npos) {
            startFound = true;
            startI = i;
            break;
        }
    }
    for (long unsigned int i = 0; i < Buildings.size(); i++) {
        if (destBuilding == Buildings[i].Abbrev) {
            destFound = true;
            destI = i;
            break;
        } else if (Buildings[i].Fullname.find(destBuilding)
            != std::string::npos) {
            destFound = true;
            destI = i;
            break;
        }
    }
    if (!startFound) {
        cout << "Starting point not found." << endl;
        return;
    }
    if (!destFound) {
        cout << "Destination point not found." << endl;
        return;
    }
    cout << "Starting point:" << endl;
    printBuildingInfo(Buildings[startI].Fullname,
    Buildings[startI].Coords.Lat, Buildings[startI].Coords.Lon);
    cout << "Destination point:" << endl;
    printBuildingInfo(Buildings[destI].Fullname,
    Buildings[destI].Coords.Lat, Buildings[destI].Coords.Lon);
}

long long findNearestNode(vector<FootwayInfo> Footways,
map<long long, Coordinates>  Nodes, vector<BuildingInfo> Buildings, int A) {
    double distance = -1;
    double next;
    long long nodeID;
    for (long unsigned int i = 0; i < Footways.size(); i++) {
        for (long unsigned int j = 0; j < Footways[i].Nodes.size(); j++) {
            long long a = Footways[i].Nodes[j];
            next = distBetween2Points(Buildings[A].Coords.Lat,
            Buildings[A].Coords.Lon, 
            Nodes[a].Lat, Nodes[a].Lon);
            if (distance == -1) {
                distance = next + 1;
            }
            if (next < distance) {
                distance = next;
                nodeID = Nodes[a].ID;
            }
        }
    }
    return nodeID;
}

void printCoords(long long nodeID, map<long long, Coordinates> Nodes) {
    cout << " (" << Nodes[nodeID].Lat << ", "
    << Nodes[nodeID].Lon << ")" << endl;
}

vector<long long> Dijkstra1(graph<long long, double>& G,
  long long startV,
  map<long long, double>& distances,
  map<long long, long long>& predecessors) {
  vector<long long>  visited;
  set<long long>     visitedSet;
  priority_queue<pair<long long, double>,
    vector<pair<long long, double>>,
    prioritize>   unvisitedQueue;
    const double INF = numeric_limits<double>::max();
  auto vertices = G.getVertices();
  for (auto v : vertices) {
    unvisitedQueue.push(make_pair(v, INF));
    distances[v] = INF;
    predecessors[v] = -1;  // an invalid vertex:
  }
  distances[startV] = 0;
  // whenever we update the distances array, we also need
  // to update the priority queue:
  unvisitedQueue.push(make_pair(startV, 0));
  // run algorithm until no more vertices to visit:
  while (!unvisitedQueue.empty()) {
    // Pop the vertex from the queue with the *minimum* distance:
    auto kvpair = unvisitedQueue.top();
    unvisitedQueue.pop();
    auto curV = kvpair.first;
    if (distances[curV] == INF)  // unreachable:
      break;
    else if (kvpair.second == INF)  // remaining vertices in queue all INF:
      break;
    else if (visitedSet.count(curV) > 0)  // already visited?
      continue;  // if so, skip
    else {
      visited.push_back(curV);
      visitedSet.insert(curV);
    }
    // visit adjacent nodes:
    auto adjacent = G.neighbors(curV);
    for (auto adjV : adjacent) {
      double weight;

      if (!G.getWeight(curV, adjV, weight)) {
        cout << "**Error: internal error, unable to obtain weight?" << endl;
        continue;
      }
      double altDist = distances[curV] + weight;
      if (altDist < distances[adjV]) {
        distances[adjV] = altDist;
        unvisitedQueue.push(make_pair(adjV, altDist));
        predecessors[adjV] = curV;
      }
    }
  }
  return visited;
}

int main() {
  // maps a Node ID to it's coordinates (lat, lon)
  map<long long, Coordinates>  Nodes;
  // info about each footway, in no particular order
  vector<FootwayInfo>          Footways;
  // info about each building, in no particular order
  vector<BuildingInfo>         Buildings;
  XMLDocument                  xmldoc;

  int A;
  int B;

  cout << "** Navigating UIC open street map **" << endl;
  cout << endl;
  cout << std::setprecision(8);

  string def_filename = "map.osm";
  string filename;
  long long startNearestNode;
  long long destNearestNode;

  cout << "Enter map filename> ";
  getline(cin, filename);

  if (filename == "") {
    filename = def_filename;
  }

  //
  // Load XML-based map file
  //
  if (!LoadOpenStreetMap(filename, xmldoc)) {
    cout << "**Error: unable to load open street map." << endl;
    cout << endl;
    return 0;
  }
  // Read the nodes, which are the various known positions on the map:
  int nodeCount = ReadMapNodes(xmldoc, Nodes);
  // Read the footways, which are the walking paths:
  int footwayCount = ReadFootways(xmldoc, Footways);
  // Read the university buildings:
  int buildingCount = ReadUniversityBuildings(xmldoc, Nodes, Buildings);
  // Stats
  assert(nodeCount == (int)Nodes.size());
  assert(footwayCount == (int)Footways.size());
  assert(buildingCount == (int)Buildings.size());

  cout << endl;
  cout << "# of nodes: " << Nodes.size() << endl;
  cout << "# of footways: " << Footways.size() << endl;
  cout << "# of buildings: " << Buildings.size() << endl;

  graph<long long, double> G;
  putVertices(G, Nodes);
  putEdges(G, Footways, Nodes);
  
  cout << "# of vertices: " << G.NumVertices() << endl;
  cout << "# of edges: " << G.NumEdges() << endl;
  cout << endl;

  //
  // Navigation from building to building
  //
  string startBuilding, destBuilding;

  cout << "Enter start (partial name or abbreviation), or #> ";
  getline(cin, startBuilding);

  while (startBuilding != "#") {
    cout << "Enter destination (partial name or abbreviation)> ";
    getline(cin, destBuilding);

    findBuilding(startBuilding, destBuilding, Buildings, A, B);

    startNearestNode = findNearestNode(Footways, Nodes, Buildings, A);
    destNearestNode = findNearestNode(Footways, Nodes, Buildings, B);

    cout << endl;
    cout << "Nearest start node:" << endl <<
    " " << startNearestNode << endl;
    printCoords(startNearestNode, Nodes);
    cout << "Nearest destination node:" << endl <<
    " " << destNearestNode << endl;
    printCoords(destNearestNode, Nodes);

    map<long long, double> distances;
    map<long long, long long> predecessors;
    vector<long long> V;
    stack<long long> path;
    V = Dijkstra1(G, startNearestNode, distances, predecessors);
    cout << endl;
    cout << "Navigating with Dijkstra..." << endl;
    cout << "Distance to dest: " << distances[destNearestNode] << " miles" << endl;
    cout << "Path: ";

    long long key = predecessors[destNearestNode];
    path.push(destNearestNode);
    while (key != startNearestNode) {
        path.push(key);
        key = predecessors[key];
    }
    cout << startNearestNode;
    while (!path.empty()) {
        cout << "->" << path.top();
        path.pop();
    }
    cout << endl;

    cout << endl;
    cout << "Enter start (partial name or abbreviation), or #> ";
    getline(cin, startBuilding);
  }

  //
  // done:
  //
  cout << "** Done **" << endl;
  return 0;
}
