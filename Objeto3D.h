#ifndef OBJETO_3D
#define OBJETO_3D

#include "Ponto.h"

#include "TrianglePlane.h"

#include <vector>
#include <string>

class Objeto3D
{
private:
    std::vector<Ponto> vertices;
    std::vector<std::vector <size_t>> faces;
    std::vector<Ponto> centroides;
    std::vector<Ponto> animationIncrement;

    std::vector<size_t> quads;
    std::vector<size_t> tris;
    std::vector<size_t> ngons;

    Ponto position;
    float rotation[4] {0.0f, 0.0f, 0.0f, 0.0f};

public:
    Objeto3D();
    Objeto3D(std::string filename);

    Objeto3D(const Objeto3D& other);
    // Objeto3D& operator=(const Objeto3D& other);

    size_t getNFaces();
    size_t getNTris();
    size_t getNQuads();
    size_t getNNgons();

    size_t getNVertices();
    Ponto* getVertice(size_t i);
    std::vector <size_t>* getFace(size_t iv);

    // Plane getPlaneFromFace(size_t iv);
    TrianglePlane getTrianglePlane(size_t iv);

    float getRotationAngle();
    void setRotation(float angle, float x, float y, float z);
    void LoadFile(std::string file);
    void DesenhaVertices();
    void DesenhaWireframe();
    void Desenha();

    Ponto CalculaCentroide(size_t faceIndex);
    void RecalculaCentroides();
    void DesenhaCentroides();

    void SubdivideFaceEm(size_t faceIndex, size_t times);
    void SubdivideFace(size_t faceIndex);
    void SubdivideMesh(size_t n_times);
    void TriangulaQuad(size_t faceIndex);
    void TriangulaFace(size_t faceIndex);
    void TriangulaMesh();

    void findAnimationIncrement(Objeto3D *obj2, size_t n_frames);
    void animate(bool reverse);
};

#endif