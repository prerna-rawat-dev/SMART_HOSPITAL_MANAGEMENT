#ifndef GRAPH_H
#define GRAPH_H

#include <iostream>
#include <string>
using namespace std;

// Adjacency List Edge
struct GraphEdge
{
    string destinationID;
    int distance;
    GraphEdge* next;

    GraphEdge(string destination, int dist);
};

// Graph Vertex
struct GraphVertex
{
    string locationID;
    string locationName;
    GraphEdge* edges;
    GraphVertex* next;

    GraphVertex(string id, string name);
};

// Hospital Graph Manager
class HospitalGraph
{
private:
    GraphVertex* head;
    int vertexCount;

    GraphVertex* searchVertex(const string& locationID) const;

    int getVertexIndex(const string& locationID) const;

    void clearGraph();

    bool saveGraph() const;

    bool loadGraph();

public:
    HospitalGraph();
    ~HospitalGraph();

    HospitalGraph(const HospitalGraph& other) = delete;
    HospitalGraph& operator=(const HospitalGraph& other) = delete;

    bool addLocation(const string& id, const string& name);

    bool addRoute(
        const string& sourceID,
        const string& destinationID,
        int distance
    );

    void displayLocations() const;

    void displayRoutes() const;

    bool findRouteBFS(
        const string& sourceID,
        const string& destinationID
    ) const;

    bool findShortestRoute(
        const string& sourceID,
        const string& destinationID
    ) const;

    int getLocationCount() const;
};

#endif