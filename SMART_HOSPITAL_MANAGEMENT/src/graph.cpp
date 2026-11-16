#include "../include/graph.h"
#include <fstream>
#include <sstream>
#include <climits>

// Graph Edge Constructor
GraphEdge::GraphEdge(string destination, int dist)
{
    destinationID = destination;
    distance = dist;
    next = NULL;
}

// Graph Vertex Constructor
GraphVertex::GraphVertex(string id, string name)
{
    locationID = id;
    locationName = name;
    edges = NULL;
    next = NULL;
}

// Constructor
HospitalGraph::HospitalGraph()
{
    head = NULL;
    vertexCount = 0;
    loadGraph();
}

// Destructor
HospitalGraph::~HospitalGraph()
{
    clearGraph();
}

// Clear Graph
void HospitalGraph::clearGraph()
{
    GraphVertex* current = head;

    while (current != NULL)
    {
        GraphEdge* edge = current->edges;

        while (edge != NULL)
        {
            GraphEdge* tempEdge = edge;
            edge = edge->next;
            delete tempEdge;
        }

        GraphVertex* temp = current;
        current = current->next;
        delete temp;
    }

    head = NULL;
    vertexCount = 0;
}

// Search Location
GraphVertex* HospitalGraph::searchVertex(
    const string& locationID) const
{
    GraphVertex* current = head;

    while (current != NULL)
    {
        if (current->locationID == locationID)
            return current;

        current = current->next;
    }

    return NULL;
}

// Get Vertex Index
int HospitalGraph::getVertexIndex(
    const string& locationID) const
{
    GraphVertex* current = head;
    int index = 0;

    while (current != NULL)
    {
        if (current->locationID == locationID)
            return index;

        current = current->next;
        index++;
    }

    return -1;
}

// Add Location
bool HospitalGraph::addLocation(
    const string& id,
    const string& name)
{
    if (id.empty() || name.empty() ||
        id.find('|') != string::npos ||
        name.find('|') != string::npos ||
        id.find('\n') != string::npos ||
        name.find('\n') != string::npos ||
        searchVertex(id) != NULL)
    {
        cout << "Invalid or duplicate location.\n";
        return false;
    }

    GraphVertex* newVertex = new GraphVertex(id, name);

    if (head == NULL)
    {
        head = newVertex;
    }
    else
    {
        GraphVertex* current = head;

        while (current->next != NULL)
            current = current->next;

        current->next = newVertex;
    }

    vertexCount++;

    if (!saveGraph())
    {
        cout << "Warning: Location added but file could not be saved.\n";
    }

    return true;
}

// Add Route (Undirected Graph)
bool HospitalGraph::addRoute(
    const string& sourceID,
    const string& destinationID,
    int distance)
{
    if (sourceID == destinationID || distance <= 0)
        return false;

    GraphVertex* source = searchVertex(sourceID);
    GraphVertex* destination = searchVertex(destinationID);

    if (source == NULL || destination == NULL)
        return false;

    GraphEdge* edge = source->edges;

    while (edge != NULL)
    {
        if (edge->destinationID == destinationID)
        {
            edge->distance = distance;
            break;
        }

        edge = edge->next;
    }

    if (edge == NULL)
    {
        GraphEdge* newEdge =
            new GraphEdge(destinationID, distance);

        newEdge->next = source->edges;
        source->edges = newEdge;
    }

    edge = destination->edges;

    while (edge != NULL)
    {
        if (edge->destinationID == sourceID)
        {
            edge->distance = distance;
            break;
        }

        edge = edge->next;
    }

    if (edge == NULL)
    {
        GraphEdge* newEdge =
            new GraphEdge(sourceID, distance);

        newEdge->next = destination->edges;
        destination->edges = newEdge;
    }

    if (!saveGraph())
        cout << "Warning: Route added but file could not be saved.\n";

    return true;
}

// Display Locations
void HospitalGraph::displayLocations() const
{
    GraphVertex* current = head;

    cout << "\n===== HOSPITAL LOCATIONS =====\n";

    if (current == NULL)
    {
        cout << "No locations available.\n";
        return;
    }

    while (current != NULL)
    {
        cout << current->locationID << " - "
             << current->locationName << endl;

        current = current->next;
    }
}

// Display Routes
void HospitalGraph::displayRoutes() const
{
    GraphVertex* current = head;

    cout << "\n===== HOSPITAL ROUTES =====\n";

    while (current != NULL)
    {
        cout << "\n" << current->locationName << " connects to:\n";

        GraphEdge* edge = current->edges;

        while (edge != NULL)
        {
            GraphVertex* destination =
                searchVertex(edge->destinationID);

            if (destination != NULL)
            {
                cout << "  -> " << destination->locationName
                     << " (" << edge->distance << " metres)\n";
            }

            edge = edge->next;
        }

        current = current->next;
    }
}

// BFS - Breadth First Search
bool HospitalGraph::findRouteBFS(
    const string& sourceID,
    const string& destinationID) const
{
    int source = getVertexIndex(sourceID);
    int destination = getVertexIndex(destinationID);

    if (source == -1 || destination == -1)
        return false;

    if (source == destination)
    {
        cout << "You are already at this location.\n";
        return true;
    }

    int* queue = new int[vertexCount];
    int* visited = new int[vertexCount];
    int* parent = new int[vertexCount];

    for (int i = 0; i < vertexCount; i++)
    {
        visited[i] = 0;
        parent[i] = -1;
    }

    int front = 0, rear = 0;

    queue[rear++] = source;
    visited[source] = 1;

    while (front < rear)
    {
        int currentIndex = queue[front++];

        GraphVertex* current = head;

        for (int i = 0; i < currentIndex; i++)
            current = current->next;

        GraphEdge* edge = current->edges;

        while (edge != NULL)
        {
            int nextIndex = getVertexIndex(edge->destinationID);

            if (nextIndex != -1 && !visited[nextIndex])
            {
                visited[nextIndex] = 1;
                parent[nextIndex] = currentIndex;
                queue[rear++] = nextIndex;
            }

            edge = edge->next;
        }
    }

    if (!visited[destination])
    {
        cout << "No route found.\n";

        delete[] queue;
        delete[] visited;
        delete[] parent;

        return false;
    }

    int* path = new int[vertexCount];
    int pathCount = 0;

    int current = destination;

    while (current != -1)
    {
        path[pathCount++] = current;
        current = parent[current];
    }

    cout << "\nBFS Route (Minimum number of connections):\n";

    for (int i = pathCount - 1; i >= 0; i--)
    {
        GraphVertex* location = head;

        for (int j = 0; j < path[i]; j++)
            location = location->next;

        cout << location->locationName;

        if (i > 0)
            cout << " -> ";
    }

    cout << endl;

    delete[] path;
    delete[] queue;
    delete[] visited;
    delete[] parent;

    return true;
}

// Dijkstra's Algorithm
bool HospitalGraph::findShortestRoute(
    const string& sourceID,
    const string& destinationID) const
{
    int source = getVertexIndex(sourceID);
    int destination = getVertexIndex(destinationID);

    if (source == -1 || destination == -1)
        return false;

    int* distance = new int[vertexCount];
    int* visited = new int[vertexCount];
    int* parent = new int[vertexCount];

    for (int i = 0; i < vertexCount; i++)
    {
        distance[i] = INT_MAX;
        visited[i] = 0;
        parent[i] = -1;
    }

    distance[source] = 0;

    for (int count = 0; count < vertexCount; count++)
    {
        int minimum = INT_MAX;
        int currentIndex = -1;

        for (int i = 0; i < vertexCount; i++)
        {
            if (!visited[i] && distance[i] < minimum)
            {
                minimum = distance[i];
                currentIndex = i;
            }
        }

        if (currentIndex == -1)
            break;

        visited[currentIndex] = 1;

        GraphVertex* current = head;

        for (int i = 0; i < currentIndex; i++)
            current = current->next;

        GraphEdge* edge = current->edges;

        while (edge != NULL)
        {
            int nextIndex = getVertexIndex(edge->destinationID);

            if (nextIndex != -1 && !visited[nextIndex] &&
                distance[currentIndex] != INT_MAX &&
                distance[currentIndex] <= INT_MAX - edge->distance)
            {
                int newDistance =
                    distance[currentIndex] + edge->distance;

                if (newDistance < distance[nextIndex])
                {
                    distance[nextIndex] = newDistance;
                    parent[nextIndex] = currentIndex;
                }
            }

            edge = edge->next;
        }
    }

    if (distance[destination] == INT_MAX)
    {
        cout << "No route found.\n";

        delete[] distance;
        delete[] visited;
        delete[] parent;

        return false;
    }

    int* path = new int[vertexCount];
    int pathCount = 0;

    int current = destination;

    while (current != -1)
    {
        path[pathCount++] = current;
        current = parent[current];
    }

    cout << "\nShortest Hospital Route:\n";

    for (int i = pathCount - 1; i >= 0; i--)
    {
        GraphVertex* location = head;

        for (int j = 0; j < path[i]; j++)
            location = location->next;

        cout << location->locationName;

        if (i > 0)
            cout << " -> ";
    }

    cout << "\nTotal Distance: "
         << distance[destination] << " metres\n";

    delete[] path;
    delete[] distance;
    delete[] visited;
    delete[] parent;

    return true;
}

// Save Graph to File
bool HospitalGraph::saveGraph() const
{
    ofstream file("DATA/hospital_map.txt");

    if (!file)
        return false;

    GraphVertex* current = head;

    while (current != NULL)
    {
        file << "V|" << current->locationID
             << "|" << current->locationName << "\n";

        current = current->next;
    }

    current = head;

    while (current != NULL)
    {
        GraphEdge* edge = current->edges;

        while (edge != NULL)
        {
            if (current->locationID < edge->destinationID)
            {
                file << "E|" << current->locationID
                     << "|" << edge->destinationID
                     << "|" << edge->distance << "\n";
            }

            edge = edge->next;
        }

        current = current->next;
    }

    file.close();
    return !file.fail();
}

// Load Graph from File
bool HospitalGraph::loadGraph()
{
    ifstream file("DATA/hospital_map.txt");

    if (!file)
        return false;

    string line;

    // First pass: Load locations
    while (getline(file, line))
    {
        if (line.length() < 3 || line[0] != 'V' ||
            line[1] != '|')
            continue;

        size_t separator = line.find('|', 2);

        if (separator == string::npos)
            continue;

        string id = line.substr(2, separator - 2);
        string name = line.substr(separator + 1);

        if (id.empty() || name.empty() ||
            searchVertex(id) != NULL)
            continue;

        GraphVertex* newVertex = new GraphVertex(id, name);

        if (head == NULL)
        {
            head = newVertex;
        }
        else
        {
            GraphVertex* current = head;

            while (current->next != NULL)
                current = current->next;

            current->next = newVertex;
        }

        vertexCount++;
    }

    file.close();

    // Second pass: Load routes
    file.open("DATA/hospital_map.txt");

    if (!file)
        return false;

    while (getline(file, line))
    {
        if (line.length() < 3 || line[0] != 'E' ||
            line[1] != '|')
            continue;

        stringstream ss(line.substr(2));

        string sourceID, destinationID, distanceText;

        if (!getline(ss, sourceID, '|') ||
            !getline(ss, destinationID, '|') ||
            !getline(ss, distanceText, '|'))
            continue;

        int distance = 0;

        stringstream conversion(distanceText);
        if (!(conversion >> distance) || distance <= 0)
            continue;

        GraphVertex* source = searchVertex(sourceID);
        GraphVertex* destination = searchVertex(destinationID);

        if (source == NULL || destination == NULL ||
            sourceID == destinationID)
            continue;

        GraphEdge* edge = new GraphEdge(destinationID, distance);
        edge->next = source->edges;
        source->edges = edge;

        edge = new GraphEdge(sourceID, distance);
        edge->next = destination->edges;
        destination->edges = edge;
    }

    file.close();
    return true;
}

// Get Location Count
int HospitalGraph::getLocationCount() const
{
    return vertexCount;
}