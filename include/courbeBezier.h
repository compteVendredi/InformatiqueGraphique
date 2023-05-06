#ifndef COURBEBEZIER_H
#define COURBEBEZIER_H


#include <vector>
#include <glm/glm.hpp>
#include <define.h>


class CourbeBezier {
    public:
        CourbeBezier(const std::vector<glm::vec3>& pointsControles, int nbSegment);
        CourbeBezier(const std::vector<glm::vec3>& pointsControles, float longueur);
        glm::vec3 evaluer(float t) const;
        void charger(unsigned int *VBO, unsigned int *VAO);
        void afficherCourbe() const;
        void afficherPolygoneControle() const;
        

    private:
        std::vector<glm::vec3> pointsControles;
        std::vector<glm::vec3> points;
        unsigned int *VAO;
};


/*Mise en place de deux méthodes de discrétisation de l'espace paramétrique pour l'approximation de la courbe par des segments de droite*/


//Uniforme dans l'espace paramétrique (à partir du nombre de segment donné par l'utilisateur)
CourbeBezier::CourbeBezier(const std::vector<glm::vec3>& pointsControles, int nbSegment)
    : pointsControles(pointsControles)
{

    points.push_back(pointsControles[0]);

    float pas = 1.0/float(nbSegment);
    float t = pas;

    //https://stackoverflow.com/questions/13542220/why-for-i-0-1-i-1-0-i-0-1-doesnt-break-at-i-1-0/13543240#13543240
    for(int i = 1; i < nbSegment; i++){
        glm::vec3 point;
        point = evaluer(t);
        points.push_back(point);
        t += pas;
    }
    points.push_back(pointsControles[pointsControles.size()-1]);
}

//Uniforme en longueur de segment (à partir de la longueur d'un segment donné par l'utilisateur)
//Pour une précision de + ou - 1% de la longueur
CourbeBezier::CourbeBezier(const std::vector<glm::vec3>& pointsControles, float longueur)
    : pointsControles(pointsControles)
{
    float u = 0.0f;
    float e = longueur/100.0f;
    int last = 0;
    float pas = longueur;    
    
    points.push_back(pointsControles[0]);
    
    while(u < 1.0f){
        glm::vec3 point;  
        float distance;        
        bool estInf = true;
        bool estSup = true;   
        int compteur = 0;        
        while((estInf || estSup) && (compteur<MAX_COMPTEUR)){
            if(estSup){
                pas -= longueur/10000.0f;            
            }
            else{
                pas += longueur/10000.0f;            
            }
            point = evaluer(u+pas);
            distance = glm::length(points[last] - point);
            estInf = distance < longueur-e;
            estSup = distance > longueur+e;            
            compteur++;
        }
        //std::cout << distance << std::endl;
        last++;        
        u += pas;
        if(u < 1.0f)
            points.push_back(point);
    }
    
    points.push_back(pointsControles[pointsControles.size()-1]);
}


//https://developpement-informatique.com/article/527/calculer-le-coefficient-binomial
//complexité linéaire (n)
int comb(int n, int k){
    if(k>(n-k))
        k=n-k;
    int coef=1;
    for(int i=0;i<k;i++){
        coef *= (n-i);
        coef /= (i+1);
    }

    return coef;
}

float bernstein(int m, int i, float u){
    return comb(m,i)*pow(u,i)*pow(1-u,m-i);
}

glm::vec3 CourbeBezier::evaluer(float t) const
{
    glm::vec3 point(0.0f, 0.0f, 0.0f);
    int n = pointsControles.size();
    for (int i = 0; i < n; i++) {
        point +=  bernstein(n-1,i,t)*pointsControles[i];
    }

    return point;
}


/*Visualisation de la courbe et de son polygone de contrôle*/


void CourbeBezier::charger(unsigned int *VBO, unsigned int *VAO){
    this->VAO = VAO;
    glGenVertexArrays(2, VAO);
    glGenBuffers(2, VBO);

    glBindVertexArray(VAO[0]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(glm::vec3), points.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindVertexArray(VAO[1]);
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, pointsControles.size() * sizeof(glm::vec3), pointsControles.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}


void CourbeBezier::afficherCourbe() const{
    glBindVertexArray(VAO[0]);
    glDrawArrays(GL_LINE_STRIP, 0, points.size());
    glBindVertexArray(0);
}

void CourbeBezier::afficherPolygoneControle() const{
    glBindVertexArray(VAO[1]);
    glDrawArrays(GL_LINE_STRIP, 0, pointsControles.size());
    glBindVertexArray(0);
}


#endif
