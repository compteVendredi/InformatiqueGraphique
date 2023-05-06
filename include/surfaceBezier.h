#ifndef SURFACEBEZIER_H
#define SURFACEBEZIER_H


#include <vector>
#include <glm/glm.hpp>
#include <courbeBezier.h>
#include <point.h>
#include <math.h>


class SurfaceBezier {
    public:
        SurfaceBezier(const std::vector<std::vector<glm::vec3>>& pointsControles, float pasU, float pasV);
        Point evaluer(float u, float v) const;
        void charger(unsigned int *VBO, unsigned int *VAO, unsigned int *EBO);
        void afficher() const;
        void afficherPolygoneControle() const;
        

    private:
        std::pair<int,int> generer(float pasU, float pasV);
        std::vector<glm::vec3> pointsControles;
        std::vector<Point> points;
        std::vector<unsigned int> indicesPointsControles, indices;
        unsigned int *VAO, taillePointsControlesLigne, taillepointsControlesColonne;
};


SurfaceBezier::SurfaceBezier(const std::vector<std::vector<glm::vec3>>& pointsControles, float pasU, float pasV)
{
    taillePointsControlesLigne = pointsControles.size();
    taillepointsControlesColonne = pointsControles[0].size();

    for(int i = 0; i < taillePointsControlesLigne; i++){
        for(int j = 0; j < taillepointsControlesColonne; j++){
            this->pointsControles.push_back(pointsControles[i][j]);
            if(i < taillePointsControlesLigne-1){
                indicesPointsControles.push_back(i*taillepointsControlesColonne+j);
                indicesPointsControles.push_back((i+1)*taillepointsControlesColonne+j);
            }
            if(j < taillepointsControlesColonne-1){
                indicesPointsControles.push_back(i*taillepointsControlesColonne+j);
                indicesPointsControles.push_back(i*taillepointsControlesColonne+j+1);
            } 
        }
    }

    std::pair<int,int> res = generer(pasU, pasV);

    int tailleU = res.first;
    int tailleV = res.second;

    for(int i = 0; i < tailleU; i++){
        for(int j = 0; j < tailleV; j++){
            if(i < tailleU-1 && j < tailleV-1){
                indices.push_back(i*tailleV+j);
                indices.push_back(i*tailleV+j+1);
                indices.push_back((i+1)*tailleV+j);

                indices.push_back((i+1)*tailleV+j+1);
                indices.push_back((i+1)*tailleV+j);
                indices.push_back(i*tailleV+j+1);
            }                
        }
    }

}


Point SurfaceBezier::evaluer(float u, float v) const
{
    std::vector<glm::vec3> curvePointsU;
    for (int i = 0; i < taillePointsControlesLigne; i++) {
        std::vector<glm::vec3> curvePointsV(taillepointsControlesColonne);
        for (int j = 0; j < taillepointsControlesColonne; j++) {
            curvePointsV[j] = pointsControles[i*taillepointsControlesColonne+j];
        }

        CourbeBezier curve(curvePointsV, 1);
        glm::vec3 curvePoint = curve.evaluer(v);
        curvePointsU.push_back(curvePoint);
    }
    CourbeBezier curveU(curvePointsU, 1);
    glm::vec3 point = curveU.evaluer(u);

    std::vector<glm::vec3> curvePointsV;
    for (int i = 0; i < taillepointsControlesColonne; i++) {
        std::vector<glm::vec3> curvePointsU(taillePointsControlesLigne);
        for (int j = 0; j < taillePointsControlesLigne; j++) {
            curvePointsU[j] = pointsControles[j*taillePointsControlesLigne+i];
        }

        CourbeBezier curve(curvePointsU, 1);
        glm::vec3 curvePoint = curve.evaluer(u);
        curvePointsV.push_back(curvePoint);
    }
    CourbeBezier curveV(curvePointsV, 1);

    glm::vec3 normale = glm::normalize(glm::cross(curveU.evaluer(u+EPSILON)-point,
    curveV.evaluer(v+EPSILON)-point));
    return Point(point, normale, u, v);
}



std::pair<int,int> SurfaceBezier::generer(float pasU, float pasV){
    float tU = 0, tV = 0;
    int i, j;

    for(i = 0; i < (int)1/pasU; i++){
        for(j = 0; j < (int)1/pasV; j++){
            Point point = Point(evaluer(tU, tV));
            points.push_back(point);
            tV += pasV;
        }
        points.push_back(evaluer(tU, 1.0f));
        tV = 0;
        tU += pasU;
    }
    i++;

    for(j = 0; j < (int)1/pasV; j++){
        Point point = Point(evaluer(1.0f, tV));
        points.push_back(point);
        tV += pasV;
    }
    points.push_back(evaluer(1.0f, 1.0f));
    j++;    

    return std::make_pair(i,j);
}


void SurfaceBezier::charger(unsigned int *VBO, unsigned int *VAO, unsigned int *EBO){
    this->VAO = VAO;
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);
    glGenBuffers(2, EBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(Point), points.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[0]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Point), (void*)(2*sizeof(glm::vec3)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(VAO[1]);

    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, pointsControles.size() * sizeof(glm::vec3), pointsControles.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indicesPointsControles.size()*sizeof(unsigned int), indicesPointsControles.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void SurfaceBezier::afficher() const{
    glBindVertexArray(VAO[0]);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void SurfaceBezier::afficherPolygoneControle() const{
    glBindVertexArray(VAO[1]);
    glDrawElements(GL_LINES, indicesPointsControles.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}



#endif
