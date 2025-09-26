#include <vector>
#include "geometry.h"

    class model {
    private:
        std::vector<vec3> verts{};
        std::vector<int> faces{};
    public:
        model(const std::string filename);
        int nverts() const; // number of vertices
        int nfaces() const; // number of triangles
        vec3 vert(const int i) const;                          // 0 <= i < nverts()
        vec3 vert(const int iface, const int nthvert) const;
    };