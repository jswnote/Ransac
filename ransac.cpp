#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>

using namespace std;

struct point_3d
{
	float x;
	float y;
	float z;
};

struct vec
{
    float x;
    float y;
    float z;
};

struct plane
{
    float a;
    float b;
    float c;
    float d;
};

void extract_random_3_fast(const string& filename, vector<point_3d>& result)
{
    ifstream file(filename);
    string line;
    bool data_section = false;

    vector<point_3d> reservoir;   // size 3
    reservoir.reserve(3);

    random_device rd;
    mt19937 gen(rd());

    int count = 0;

    while (std::getline(file, line))
    {
        if (line.rfind("DATA ascii", 0) == 0) {
            data_section = true;
            continue;
        }
        if (!data_section)
            continue;

        std::stringstream ss(line);
        point_3d p;
        ss >> p.x >> p.y >> p.z;

        if (count < 3) {
            reservoir.push_back(p);
        }
        else {
            std::uniform_int_distribution<int> dist(0, count);
            int r = dist(gen);
            if (r < 3) {
                reservoir[r] = p;   
            }
        }

        count++;
    }

    result = reservoir;
}

void cal_coefficient(const point_3d &a, const point_3d &b, const point_3d &c, plane &coeff)
{
    float i = (b.y - a.y) * (c.z - a.z) - (b.z - a.z) * (c.y - a.y);
    float j = (b.z - a.z) * (c.x - a.x) - (b.x - a.x) * (c.z - a.z);
    float k = (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
    float d = - (a.x * i + a.y * j + a.z * k);

    coeff.a = i;
    coeff.b = j;
    coeff.c = k;
    coeff.d = d;
}

void build_plane(const vector<point_3d>* rand_point, plane &coeff)
{
    point_3d first = (*rand_point)[0];
    point_3d second = (*rand_point)[1];
    point_3d third = (*rand_point)[2];

    cal_coefficient(first, second, third, coeff);
}
