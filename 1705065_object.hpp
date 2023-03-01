#ifndef OBJECT_H
#define OBJECT_H

#include<bits/stdc++.h>
#include "1705065_Point.hpp"
#include "1705065_pointLight.hpp"

using namespace std;

extern point pos, u, r, l;
extern vector<pointLight*> pointLights;


class object{
    public:
        point reference_point;
        double height, width, length;
        double color[3];
        double coefficients[4]; // amb, diff, spec, refl
        int shine;

        Object() {

        }

        virtual void draw() {}

        virtual double intersect(ray rei, double* color, int level){ 
            return -1.0; 
        }

        bool shadow(ray r, point p); 
};

extern vector<object*> obj_list;

bool object::shadow(ray r, point p) {
    double tmin = 10000000000;
    int nearest = -1;

    for(int k = 0; k < obj_list.size(); k++){
        double dummy[3];
        double t = obj_list[k]->intersect(r, dummy, 0);
        if(t>=0 && t<tmin){
            tmin = t;
            nearest = k;
        }
    }

    if(nearest != -1) {
        point intersectionPoint = r.start + r.dir*tmin;
        return !(intersectionPoint==p);
    }

    return false;
}


class sphere : public object{
    public:
        sphere(point center, double radius) {
            reference_point = center;
            length = radius;
        }

        void draw() {
            int stacks = 50;
            int slices = 100;
            point points[100][100];
            int i,j;
            double h,r;
            //generate points
            for(i=0;i<=stacks;i++)
            {
                h=length*sin(((double)i/(double)stacks)*(pi/2));
                r=length*cos(((double)i/(double)stacks)*(pi/2));
                for(j=0;j<=slices;j++)
                {
                    points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
                    points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
                    points[i][j].z=h;
                }
            }
            //draw quads using generated points
            for(i=0;i<stacks;i++)
            {
                glColor3f(color[0], color[1], color[2]);
                for(j=0;j<slices;j++)
                {
                    glTranslatef(reference_point.x, reference_point.y, reference_point.z);
                    
                    glBegin(GL_QUADS);{
                        //upper hemisphere
                        glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
                        glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
                        glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
                        glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                        //lower hemisphere
                        glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
                        glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
                        glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
                        glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
                    }glEnd();
                
                    glTranslatef(-reference_point.x, -reference_point.y, -reference_point.z);
                }
            }
        }

        double intersect(ray rei, double* color, int level){ 
            rei.start = rei.start - reference_point;
            double a = 1;
            double b = 2*rei.dir.DOTProduct(rei.start);
            double c = rei.start.DOTProduct(rei.start) - (length*length);
            rei.start = rei.start + reference_point;

            double result = b*b - 4*a*c;

            if(result < 0){
                return -1.0;
            }

            double d = sqrt(result);
            double t1,t2;
            double tmin;

            t1 = (-b+d)/(2*a);
            t2 = (-b-d)/(2*a);
            
            if(t1>0 && t2>0) {
                tmin = t2;
            }
            else if(t1>0){
                tmin = t1;
            }
            else if(t2>0){
                tmin = t2;
            }
            else {
                return -1;
            }

            if(level == 0){
                return tmin;
            }

            point intersectionPoint = rei.start + rei.dir*tmin;
            double intersectionPointColor[3];
            intersectionPointColor[0] = this->color[0];
            intersectionPointColor[1] = this->color[1];
            intersectionPointColor[2] = this->color[2];

            color[0] = intersectionPointColor[0]*coefficients[0];
            color[1] = intersectionPointColor[1]*coefficients[0];
            color[2] = intersectionPointColor[2]*coefficients[0];

            point intersect_normal;
            intersect_normal = intersectionPoint - reference_point;
            intersect_normal = intersect_normal.normalize();

            for(int i = 0; i < pointLights.size(); i++){
                ray r(pointLights[i]->light_pos, intersectionPoint - pointLights[i]->light_pos);
                // if(shadow(r, intersectionPoint)) 
                //     continue;
                
                double lambert_val = r.dir.DOTProduct(intersect_normal);

                point reflectedDir = r.dir - intersect_normal*(r.dir.DOTProduct(intersect_normal) * 2);
                reflectedDir = reflectedDir.normalize();
                ray reflected(intersectionPoint, reflectedDir);
                
                double phong_val = reflected.dir.DOTProduct(rei.dir);

                color[0] += intersectionPointColor[0] * pointLights[i]->color[0] * coefficients[1] * lambert_val;
                color[1] += intersectionPointColor[1] * pointLights[i]->color[1] * coefficients[1] * lambert_val;
                color[2] += intersectionPointColor[2] * pointLights[i]->color[2] * coefficients[1] * lambert_val;

                color[0] += intersectionPointColor[0] * pointLights[i]->color[0] * coefficients[2] * pow(phong_val,this->shine);
                color[1] += intersectionPointColor[1] * pointLights[i]->color[1] * coefficients[2] * pow(phong_val,this->shine);
                color[2] += intersectionPointColor[2] * pointLights[i]->color[2] * coefficients[2] * pow(phong_val,this->shine);

                if(color[0]>1) color[0] = 1;
                if(color[1]>1) color[1] = 1;
                if(color[2]>1) color[2] = 1;

                if(color[0]<0) color[0] = 0;
                if(color[1]<0) color[1] = 0;
                if(color[2]<0) color[2] = 0;
            }
            
            return tmin;
        }
};


class triangle : public object{
    public: 
        point points[3];

        triangle(point px, point py, point pz){
            points[0] = px;
            points[1] = py;
            points[2] = pz;
        }

        void draw(){
            glColor3f(color[0], color[1], color[2]);
            glBegin(GL_TRIANGLES);{
                for(int i = 0; i < 3; i++)
                    glVertex3f(points[i].x, points[i].y, points[i].z);
            }glEnd();
        }

        double intersect(ray rei, double* color, int level){
            double tmin = -1;
            const double EPSILON = 0.0000001;

            point vertex0 = this -> points[0];
            point vertex1 = this -> points[1];  
            point vertex2 = this -> points[2];

            point edge1, edge2, h, s, q;
            double a,f,u,v;
            edge1 = vertex1 - vertex0;
            edge2 = vertex2 - vertex0;

            h = rei.dir.crossProduct(edge2);
            a = edge1.DOTProduct(h);
            if (a > -EPSILON && a < EPSILON)
                return -1;    // This ray is parallel to this triangle.
            f = 1.0/a;
            s = rei.start - vertex0;
            u = f * s.DOTProduct(h);
            if (u < 0.0 || u > 1.0)
                return -1;
            q = s.crossProduct(edge1);
            v = f * rei.dir.DOTProduct(q);
            if (v < 0.0 || u + v > 1.0)
                return -1;
            // At this stage we can compute t to find out where the intersection point is on the line.
            double t = f * edge2.DOTProduct(q);
            if (t > EPSILON) // ray intersection
            {  
                color[0] = this->color[0] * this->coefficients[0];
                color[1] = this->color[1] * this->coefficients[0];
                color[2] = this->color[2] * this->coefficients[0]; 
                tmin = t;

                point intersectionPoint = rei.start + rei.dir*tmin;
                double intersectionPointColor[3];
                intersectionPointColor[0] = this->color[0];
                intersectionPointColor[1] = this->color[1];
                intersectionPointColor[2] = this->color[2];
                
                point intersect_normal = edge1.crossProduct(edge2);
                if(intersect_normal.DOTProduct(l) > 0){
                    intersect_normal = intersect_normal*(-1);
                }
                intersect_normal = intersect_normal.normalize();

                for(int i = 0; i < pointLights.size(); i++){
                    ray r(pointLights[i]->light_pos, intersectionPoint - pointLights[i]->light_pos);
                    // if(shadow(r, intersectionPoint)) 
                    //     continue;
                    
                    double lambert_val = r.dir.DOTProduct(intersect_normal);

                    point reflectedDir = r.dir - intersect_normal*(r.dir.DOTProduct(intersect_normal) * 2);
                    reflectedDir = reflectedDir.normalize();
                    ray reflected(intersectionPoint, reflectedDir);
                    
                    double phong_val = reflected.dir.DOTProduct(rei.dir);

                    color[0] += intersectionPointColor[0] * pointLights[i]->color[0] * coefficients[1] * lambert_val;
                    color[1] += intersectionPointColor[1] * pointLights[i]->color[1] * coefficients[1] * lambert_val;
                    color[2] += intersectionPointColor[2] * pointLights[i]->color[2] * coefficients[1] * lambert_val;

                    color[0] += intersectionPointColor[0] * pointLights[i]->color[0] * coefficients[2] * pow(phong_val,this->shine);
                    color[1] += intersectionPointColor[1] * pointLights[i]->color[1] * coefficients[2] * pow(phong_val,this->shine);
                    color[2] += intersectionPointColor[2] * pointLights[i]->color[2] * coefficients[2] * pow(phong_val,this->shine);

                    if(color[0]>1) color[0] = 1;
                    if(color[1]>1) color[1] = 1;
                    if(color[2]>1) color[2] = 1;

                    if(color[0]<0) color[0] = 0;
                    if(color[1]<0) color[1] = 0;
                    if(color[2]<0) color[2] = 0;
                }

                return tmin;
            }
            else // This means that there is a line intersection but not a ray intersection.
                return -1;
        }
};


class Floor : public object{
    public:
        double floorWidth, tileWidth;

        Floor(double floorWidth, double tileWidth) {
            reference_point = point(-floorWidth/2, -floorWidth/2, 0);
            length = tileWidth;

            this->floorWidth = floorWidth;
            this->tileWidth = tileWidth;
        }

        void draw() {
            bool colcolor = 0;
            for(double y = -floorWidth/2; y < floorWidth/2; y += tileWidth){
                bool rowcolor = colcolor;
                for(double x = -floorWidth/2; x < floorWidth/2; x += tileWidth ) {
                    if(rowcolor) glColor3f(1, 1, 1);
                    else glColor3f(0, 0, 0);
                    
                    glBegin(GL_QUADS);{
                        glVertex3f(x, y, 0);
                        glVertex3f(x+tileWidth, y, 0);
                        glVertex3f(x+tileWidth, y+tileWidth, 0);
                        glVertex3f(x, y+tileWidth, 0);
                    }glEnd();
                
                    rowcolor = 1-rowcolor;
                }
                colcolor = 1-colcolor;
            }
        }

        double intersect(ray rei, double* color, int level){
            double D = 0;
            point floor_normal(0,0,1);
            double t, tmin;

            const double EPSILON = 1e-9;
            if(-EPSILON<rei.dir.z && rei.dir.z<EPSILON){
                color[0] = 0;
                color[1] = 0;
                color[2] = 0;
                return -1;
            }            

            t = -(D + floor_normal.DOTProduct(rei.start))/(floor_normal.DOTProduct(rei.dir));

            point intersecting;
            intersecting = rei.start + ((rei.dir)*t);


            bool colcolor = 0;
            for(double y = -floorWidth/2; y < floorWidth/2; y += tileWidth){
                bool rowcolor = colcolor;
                for(double x = -floorWidth/2; x < floorWidth/2; x += tileWidth ) {
                    if(rowcolor) { //white 
                        color[0] = 1;
                        color[1] = 1;
                        color[2] = 1;
                    }

                    else{ //black 
                        color[0] = 0;
                        color[1] = 0;
                        color[2] = 0;
                    }
                
                    rowcolor = 1-rowcolor;

                    if(intersecting.x >= x && intersecting.x <= x+tileWidth){
                        if(intersecting.y >= y && intersecting.y <= y+tileWidth){
                            //intersecting point in the specific tile
                            tmin = t;

                            point intersectionPoint = rei.start + rei.dir*tmin;
                            double intersectionPointColor[3];
                            intersectionPointColor[0] = color[0];
                            intersectionPointColor[1] = color[1];
                            intersectionPointColor[2] = color[2];

                            color[0] = intersectionPointColor[0]*coefficients[0];
                            color[1] = intersectionPointColor[1]*coefficients[0];
                            color[2] = intersectionPointColor[2]*coefficients[0];

                            point intersect_normal(0,0,1);

                            for(int i = 0; i < pointLights.size(); i++){
                                ray r(pointLights[i]->light_pos, intersectionPoint - pointLights[i]->light_pos);
                                // if(shadow(r, intersectionPoint)) 
                                //     continue;
                                
                                double lambert_val = r.dir.DOTProduct(intersect_normal);

                                point reflectedDir = r.dir - intersect_normal*(r.dir.DOTProduct(intersect_normal) * 2);
                                reflectedDir = reflectedDir.normalize();
                                ray reflected(intersectionPoint, reflectedDir);
                                
                                double phong_val = reflected.dir.DOTProduct(rei.dir);

                                color[0] += intersectionPointColor[0] * pointLights[i]->color[0] * coefficients[1] * lambert_val;
                                color[1] += intersectionPointColor[1] * pointLights[i]->color[1] * coefficients[1] * lambert_val;
                                color[2] += intersectionPointColor[2] * pointLights[i]->color[2] * coefficients[1] * lambert_val;

                                color[0] += intersectionPointColor[0] * pointLights[i]->color[0] * coefficients[2] * pow(phong_val,this->shine);
                                color[1] += intersectionPointColor[1] * pointLights[i]->color[1] * coefficients[2] * pow(phong_val,this->shine);
                                color[2] += intersectionPointColor[2] * pointLights[i]->color[2] * coefficients[2] * pow(phong_val,this->shine);
                                
                                if(color[0]>1) color[0] = 1;
                                if(color[1]>1) color[1] = 1;
                                if(color[2]>1) color[2] = 1;

                                if(color[0]<0) color[0] = 0;
                                if(color[1]<0) color[1] = 0;
                                if(color[2]<0) color[2] = 0;
                            }

                            return tmin;
                        }
                    }
                }
                colcolor = 1-colcolor;
            }

            return t;
        }




};

#endif