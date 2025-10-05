#include <vector>
#include "geometry.h"
#include "tgaimage.h"

#pragma once
#ifndef MODEL_H_
#define MODEL_H_

    class model {
    private:
        std::vector<vec4> verts{};
        std::vector<vec4> norms{};
        std::vector<vec2> tex{};
        std::vector<int> facetVerts{};
        std::vector<int> faceNorms{};
        std::vector<int> faceTexs{};
        TGAImage normalMap{};
        TGAImage diffuseMap{};
        TGAImage specularMap{};

    public:
        model(const std::string filename);
        int nverts() const; // number of vertices
        int nnorms() const; // number of normals
        int nfaces() const; // number of triangles
        int nfaceNorms() const;
        vec4 vert(const int i) const;                          // 0 <= i < nverts()
        vec4 vert(const int iface, const int nthvert) const;
        vec4 normal(const int iface, const int nthvert) const;
        vec4 normal(const vec2 &uv) const;
        vec2 uv(const int iface, const int nthvert) const;
        const TGAImage& diffuse() const;
        const TGAImage& specular() const;
    };

#endif