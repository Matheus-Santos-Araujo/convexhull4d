#pragma once

#include <math.h>
#include <vector>

using namespace std;

struct Point {

    struct { double x, y, z; };
    double p[3];

    void norm() {
        double norm = 1.0 / sqrtf(x * x + y * y + z * z);
        x *= norm; y *= norm; z *= norm;
    }

    Point unit() {
        double norm = 1.0f / sqrtf(x * x + y * y + z * z);
        return Point{ x * norm, y * norm, z *= norm };
    }
};

Point cross(const Point& p1, const Point& p2) {
    return Point{ p1.y * p2.z - p1.z * p2.y, p1.z * p2.x - p1.x * p2.z, p1.x * p2.y - p1.y * p2.x };
}

float dot(const Point& p1, const Point& p2) {
    return p1.x * p2.x + p1.y * p2.y + p1.z * p2.z;
}

double get3x3Determinant(std::vector<std::vector<double>>& m)
{
    double det = 0;
    std::vector<std::vector<double>> extendedDet(3, std::vector<double>(5));
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            extendedDet[i][j] = m[i][j % 3];
        }
    }
    for (int i = 0; i < 3; i++)
    {
        double downDiagonalProduct = 1;
        double upDiagonalProduct = 1;
        for (int j = 0; j < 3; j++)
        {
            downDiagonalProduct *= extendedDet[j][j + i];
            upDiagonalProduct *= extendedDet[j][4 - j - i];
        }
        det += downDiagonalProduct - upDiagonalProduct;
    }
    return det;
}