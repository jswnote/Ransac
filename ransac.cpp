#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include <string>
#include <random>
#include <algorithm>
#include <cmath>

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

void extract_random_3_fast(const string& filename, vector<point_3d>& cand, vector<point_3d>& out_cand)
{
    ifstream file(filename);
    string line;
    bool data_section = false;

    vector<point_3d> reservoir;
    reservoir.reserve(3);

    vector<point_3d> all_points;

    random_device rd;
    mt19937 gen(rd());

    vector<int> reservoir_index;
    reservoir_index.reserve(3);
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

        all_points.push_back(p);

        if (count < 3) {
            reservoir.push_back(p);
            reservoir_index.push_back(count);
        }
        else {
            std::uniform_int_distribution<int> dist(0, count);
            int r = dist(gen);
            if (r < 3) {
                reservoir[r] = p;
                reservoir_index[r] = count;
            }
        }

        count++;
    }
    cand.clear();
    cand = reservoir;

    out_cand.clear();
    out_cand.reserve(all_points.size() - reservoir.size());

    for (int i = 0; i < all_points.size(); i++)
    {
        if (i == reservoir_index[0] ||
            i == reservoir_index[1] ||
            i == reservoir_index[2])
            continue;

        out_cand.push_back(all_points[i]);
    }
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

void build_plane(const vector<point_3d>& rand_point, plane &coeff)
{
    point_3d first = rand_point[0];
    point_3d second = rand_point[1];
    point_3d third = rand_point[2];

    cal_coefficient(first, second, third, coeff);
}

void ransac(const string& filename, vector<point_3d>& cand, plane& coeff, vector<point_3d>& out_cand, int repeat, float threshold, plane& final_coeff)
{
    int max_idx = 0;
    int total_count = 0;

    for (int Num = 0; Num < repeat; Num++)
    {
        
        extract_random_3_fast(filename, cand, out_cand);
        build_plane(cand, coeff);

        int cnt = 0;

        for (int i = 0; i < out_cand.size(); i++)
        {
            float x = out_cand[i].x;
            float y = out_cand[i].y;
            float z = out_cand[i].z;

            float distance = (abs(coeff.a * x + coeff.b * y + coeff.c * z + coeff.d) / sqrt(pow(coeff.a, 2) + pow(coeff.b, 2) + pow(coeff.c, 2)));

            if (distance < threshold)
            {
                cnt++;
            }          
        }

        if (total_count < cnt)
        {
            total_count = cnt;
            max_idx = Num;
            final_coeff = coeff;
        }
    }

}


int main()
{
    string filename = "abcd";
    vector<point_3d> candidate;
    vector<point_3d> out_candidate;
    plane final_plane;

    int re = 100;
    float th = 0.3;


    plane candidate_coeffs;

    ransac(filename, candidate, candidate_coeffs, out_candidate, re, th, final_plane);

    return 0;
}
