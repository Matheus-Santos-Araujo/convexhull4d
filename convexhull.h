#pragma once
#include <algorithm>
#include <vector>
#include "vectorops.h"

struct Edge {
    Point* v1, * v2;

    bool equal(Point* v) const {
        bool h = v1->x == v->x && v1->y == v->y && v1->z == v->z;
        return h;
    }

    inline bool operator==(const Edge& e) const { return equal(e.v1) && equal(e.v2); }

    bool left(Point* other) {

        std::vector<std::vector<double>> m{
       {v1->x, v2->x, other->x},
       {v1->y, v2->y, other->y},
       {1, 1, 1} };

        if (get3x3Determinant(m) < 0) {
            return false;
        }
        else {
            return true;
        }
    }
};

struct EdgeStack {
    vector<Edge> data;

    Edge get() {
        Edge b = data.back();
        data.pop_back();

        return b;
    }
    void put(Edge e) {
        data.push_back(e);
    }

    int findIndex(const vector<Edge>& arr, Edge item) {

        for (int i = 0; i < arr.size(); i++) {
            if ((arr[i].v1->x == item.v1->x && arr[i].v1->y == item.v1->y && arr[i].v1->z == item.v1->z)
                && (arr[i].v2->x == item.v2->x && arr[i].v2->y == item.v2->y && arr[i].v2->z == item.v2->z)
                || (arr[i].v2->x == item.v1->x && arr[i].v2->y == item.v1->y && arr[i].v2->z == item.v1->z)
                && (arr[i].v1->x == item.v2->x && arr[i].v1->y == item.v2->y && arr[i].v1->z == item.v2->z)) {
                return i;
            }
        }

        return -1;
    }

    void putp(Edge e) {

        int index = findIndex(data, e);
        if (index == -1) {
            data.push_back(e);
        }
        else {
            data.erase(data.begin() + index);
        }
    }

    bool isEmpty() {
        return data.empty();
    }
};
struct Face {
    union {
        struct { Point* v1, * v2, * v3; };
        Point* p[3];
    };

    inline Point normal() const {
        return cross(Point{ v2->x - v1->x, v2->y - v1->y, v2->z - v1->z }, Point{ v3->x - v1->x, v3->y - v1->y, v3->z - v1->z }).unit();
    }

    inline Point center() const {
        return Point{ (v1->x + v2->x + v3->x) / 3.0,  (v1->y + v2->y + v3->y) / 3.0, (v1->z + v2->z + v3->z) / 3.0 };
    }

    bool outside(const Face& f, const Point& p) {
        return dot(f.normal(), (Point{ p.x - f.center().x, p.y - f.center().y, p.z - f.center().z }).unit()) <= 0;
    }
};

struct Hull {
    vector<Point*> vertices;
    vector<Face> faces;

    void clear() { vertices.clear(); faces.clear(); }
    void add_vertex(Point* v) { vertices.push_back(v); }
    inline void add_face(const Face& t) { faces.push_back(t); }
};

vector<Hull> convexhull(vector<Hull>& objs) {

    vector<Hull> hulls;
    for (Hull& o : objs) {
        Hull hull;
        hull.vertices = o.vertices;

        EdgeStack es = EdgeStack();

        // Random noise
        for (int k = 0; k < hull.vertices.size(); k++) {
            hull.vertices[k]->x = hull.vertices[k]->x + ((float)rand() / RAND_MAX) / 10;
            hull.vertices[k]->y = hull.vertices[k]->y + ((float)rand() / RAND_MAX) / 10;
            hull.vertices[k]->z = hull.vertices[k]->z + ((float)rand() / RAND_MAX) / 10;
        }

        // Search 2D
        double minY = 1000000000000;
        Point* startingPoint = hull.vertices[0];
        Point* endPoint = hull.vertices[0];
        for (int i = 0; i < hull.vertices.size(); i++) {
            if (hull.vertices[i]->y < minY) {
                minY = hull.vertices[i]->y;
                startingPoint = hull.vertices[i];
            }
        }
        for (int j = 0; j < hull.vertices.size(); j++) {
            Edge testEdge = Edge{ startingPoint, endPoint };
            if (testEdge.left(hull.vertices[j])) {
                endPoint = hull.vertices[j];
            }
        }

        Edge e = Edge{ startingPoint, endPoint };

        es.data.push_back(e);
        es.data.push_back(Edge{ e.v2, e.v1 });

        vector<Face> hullFaces;

        while (!es.isEmpty()) {
            e = es.get();

            // Search
            int i;

            for (i = 0; ((hull.vertices[i]->x == e.v1->x && hull.vertices[i]->y == e.v1->y && hull.vertices[i]->z == e.v1->z))
                || ((hull.vertices[i]->x == e.v2->x && hull.vertices[i]->y == e.v2->y && hull.vertices[i]->z == e.v2->z)); i++) {
            }
            Point* candidate = hull.vertices[i];
            int c = 0;
            Face candh = Face{ e.v1, e.v2, candidate };
            for (i++; i < hull.vertices.size(); i++) {
                bool b = !candh.outside(candh, *hull.vertices[i]);
                if ((hull.vertices[i]->x != e.v1->x || hull.vertices[i]->y != e.v1->y || hull.vertices[i]->z != e.v1->z)
                    && (hull.vertices[i]->x != e.v2->x || hull.vertices[i]->y != e.v2->y || hull.vertices[i]->z != e.v2->z)
                    && !candh.outside(candh, *hull.vertices[i])) {
                    candidate = hull.vertices[i];
                    candh = Face{ e.v1, e.v2, candidate };
                    c = i;
                }
            }
            Point* cand = candidate;
            hullFaces.push_back(Face{ e.v1, e.v2, cand });
            es.putp(Edge{ e.v1, cand });
            es.putp(Edge{ cand, e.v2 });
        }

        for (Face& f : hullFaces) {
            hull.faces.push_back(f);
        }
        hulls.push_back(hull);
    }

    return hulls;
}