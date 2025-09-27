#include "model.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <algorithm>

model::model(const std::string filename)
{
    std::ifstream in;
    in.open(filename);
    if(in.fail()) return;
    std::string line;
    while(!in.eof())
    {
        std::getline(in, line);
        std::istringstream iss(line);
        char trash;
        if(!line.compare(0, 2, "v "))
        {
            iss >> trash;
            vec3 v{};
            for(int i : {0, 1, 2}) iss >> v[i];
            verts.push_back(v);
        }
        else if(!line.compare(0, 2, "f "))
        {
            int v, t, n, count{0};
            iss >> trash;
            while(iss >> v >> trash >> t >> trash >> n)
            {
                faces.push_back(--v );
                ++count;
            }
            if (3!=count) 
            {
                std::cerr << "Error: the obj file is supposed to be triangulated" << std::endl;
                return;
            }
        }
    }
    std::vector<int> idx(nfaces());
    for (int i = 0; i < nfaces(); ++i) idx[i] = i;
    std::sort(idx.begin(), idx.end(), [&](const int& a, const int& b){
        double aMinZ = std::min(std::min(vert(a, 0).z, vert(a, 1).z), vert(a, 2).z);
        double bMinZ = std::min(std::min(vert(b, 0).z, vert(b, 1).z), vert(b, 2).z);
        return aMinZ < bMinZ;
    });
    std::vector<int> faces2(nfaces() * 3);
    for (int i = 0; i < nfaces(); ++i)
    {
        for (int j = 0; j < 3; ++j) faces2[i * 3 + j] = faces[idx[i] * 3 + j];
    }
    faces = faces2;
    std::cout << "verts: " <<verts.size() << std::endl;
    std::cout << "faces: " <<faces.size() << std::endl;
}
int model::nverts() const{return verts.size();}
int model::nfaces() const{return faces.size() / 3;}
vec3 model::vert(const int i) const {return verts[i];}
vec3 model::vert(const int iface, const int nthvert) const 
{return verts[faces[iface*3+nthvert]];}