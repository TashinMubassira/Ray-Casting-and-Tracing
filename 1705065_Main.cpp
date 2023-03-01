#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<bits/stdc++.h>

#define pi (2*acos(0.0))

#include <windows.h>
#include <GL/glut.h>

#include "bitmap_image.hpp"

#include "1705065_Point.hpp"
#include "1705065_object.hpp"
#include "1705065_pointLight.hpp"

using namespace std;


double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;


vector<object*> obj_list;
vector<pointLight*> pointLights;


// eye == pos
point pos;
point u,r,l;
double displacement;
double theta;
double inc;

int rec_level,pixel_count;
int obj_count;
int point_light_count;
int spotlight_count;

int windowHeight = 600, windowWidth = 600;
double viewAngle = 80;


int img_count;


void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 1.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);

			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);

			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}

void capture(){
    bitmap_image image(pixel_count, pixel_count);
    for(int i = 0; i < pixel_count; i++){
        for(int j = 0; j < pixel_count; j++){
            image.set_pixel(i,j, 0,0,0);
        }
    }

    double planeDistance =  (windowHeight/2.0) / tan((viewAngle/2.0)* (pi/180));
    point topLeft = pos + l*planeDistance  - r*(windowWidth/2) + u*(windowHeight/2);

    double du = double(windowWidth)/pixel_count;
    double dv = double(windowHeight)/pixel_count;

    int nearest;
    double t,tmin;
    for(int i = 0; i < pixel_count; i++){
        for(int j = 0; j < pixel_count; j++){
            //calculate currpixel
            point cur_pixel;
            cur_pixel = topLeft + r* (i*du + (du/2)) - u*(j*dv + (dv/2));

            ray rei(pos,(cur_pixel - pos));

            double *color = new double[3];
            double *dummy = new double[3];

            tmin = 10000000000;
            nearest = -1;

            for(int k = 0; k < obj_list.size(); k++){
                t = obj_list[k]->intersect(rei, dummy, 0);
                if(t>=0 && t<tmin){
                    tmin = t;
                    nearest = k;
                }
            }

            if(nearest != -1) {
                obj_list[nearest]->intersect(rei, color, 1);
                image.set_pixel(i, j, color[0]*255, color[1]*255, color[2]*255);
            }

            delete color;
            delete dummy;
        }
    }

    stringstream str;
    str << img_count;
    string str_val;
    str >> str_val;
    image.save_image("output_" + str_val + ".bmp");    
    cout << "image captured" << endl;
}



void keyboardListener(unsigned char key, int x,int y){
	point temp;
	switch(key){

        case '0':
            capture();
            img_count++;
            break;

		case '1':
			// drawgrid=1-drawgrid;
			temp = l;

			l.x = l.x*cos(-theta) + r.x*sin(-theta);
			l.y = l.y*cos(-theta) + r.y*sin(-theta);
			l.z = l.z*cos(-theta) + r.z*sin(-theta);

			r.x = r.x*cos(-theta) - temp.x*sin(-theta);
			r.y = r.y*cos(-theta) - temp.y*sin(-theta);
			r.z = r.z*cos(-theta) - temp.z*sin(-theta);
			break;

		case '2':
			temp = l;

			l.x = l.x*cos(theta) + r.x*sin(theta);
			l.y = l.y*cos(theta) + r.y*sin(theta);
			l.z = l.z*cos(theta) + r.z*sin(theta);

			r.x = r.x*cos(theta) - temp.x*sin(theta);
			r.y = r.y*cos(theta) - temp.y*sin(theta);
			r.z = r.z*cos(theta) - temp.z*sin(theta);
			break;

		case '3':
			temp = l;

			l.x = l.x*cos(theta) + u.x*sin(theta);
			l.y = l.y*cos(theta) + u.y*sin(theta);
			l.z = l.z*cos(theta) + u.z*sin(theta);

			u.x = u.x*cos(theta) - temp.x*sin(theta);
			u.y = u.y*cos(theta) - temp.y*sin(theta);
			u.z = u.z*cos(theta) - temp.z*sin(theta);
			break;			

		case '4':
			temp = l;

			l.x = l.x*cos(-theta) + u.x*sin(-theta);
			l.y = l.y*cos(-theta) + u.y*sin(-theta);
			l.z = l.z*cos(-theta) + u.z*sin(-theta);

			u.x = u.x*cos(-theta) - temp.x*sin(-theta);
			u.y = u.y*cos(-theta) - temp.y*sin(-theta);
			u.z = u.z*cos(-theta) - temp.z*sin(-theta);
			break;	
		
		case '5':
			temp = u;

			u.x = u.x*cos(theta) + r.x*sin(theta);
			u.y = u.y*cos(theta) + r.y*sin(theta);
			u.z = u.z*cos(theta) + r.z*sin(theta);

			r.x = r.x*cos(theta) - temp.x*sin(theta);
			r.y = r.y*cos(theta) - temp.y*sin(theta);
			r.z = r.z*cos(theta) - temp.z*sin(theta);
			break;

		case '6':
			temp = u;

			u.x = u.x*cos(-theta) + r.x*sin(-theta);
			u.y = u.y*cos(-theta) + r.y*sin(-theta);
			u.z = u.z*cos(-theta) + r.z*sin(-theta);

			r.x = r.x*cos(-theta) - temp.x*sin(-theta);
			r.y = r.y*cos(-theta) - temp.y*sin(-theta);
			r.z = r.z*cos(-theta) - temp.z*sin(-theta);
			break;	

		case '7' :
			//look-up left
			temp = l;

			l.x = l.x*cos(-theta) + r.x*sin(-theta);
			l.y = l.y*cos(-theta) + r.y*sin(-theta);
			l.z = l.z*cos(-theta) + r.z*sin(-theta);

			r.x = r.x*cos(-theta) - temp.x*sin(-theta);
			r.y = r.y*cos(-theta) - temp.y*sin(-theta);
			r.z = r.z*cos(-theta) - temp.z*sin(-theta);	

			l.x = l.x*cos(theta) + u.x*sin(theta);
			l.y = l.y*cos(theta) + u.y*sin(theta);
			l.z = l.z*cos(theta) + u.z*sin(theta);

			u.x = u.x*cos(theta) - temp.x*sin(theta);
			u.y = u.y*cos(theta) - temp.y*sin(theta);
			u.z = u.z*cos(theta) - temp.z*sin(theta);
			break;

		case '8':
			//look-up right
			temp = l;

			l.x = l.x*cos(theta) + r.x*sin(theta);
			l.y = l.y*cos(theta) + r.y*sin(theta);
			l.z = l.z*cos(theta) + r.z*sin(theta);

			r.x = r.x*cos(theta) - temp.x*sin(theta);
			r.y = r.y*cos(theta) - temp.y*sin(theta);
			r.z = r.z*cos(theta) - temp.z*sin(theta);

			l.x = l.x*cos(theta) + u.x*sin(theta);
			l.y = l.y*cos(theta) + u.y*sin(theta);
			l.z = l.z*cos(theta) + u.z*sin(theta);

			u.x = u.x*cos(theta) - temp.x*sin(theta);
			u.y = u.y*cos(theta) - temp.y*sin(theta);
			u.z = u.z*cos(theta) - temp.z*sin(theta);
			break;

		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	double new_max_a, new_side;

	switch(key){
		case GLUT_KEY_DOWN:		//down arrow key
			pos.x -= l.x*displacement;
			pos.y -= l.y*displacement;
			pos.z -= l.z*displacement;		
			break;
		case GLUT_KEY_UP:		// up arrow key
			pos.x += l.x*displacement;
			pos.y += l.y*displacement;
			pos.z += l.z*displacement;
			break;

		case GLUT_KEY_RIGHT:
			pos.x += r.x*displacement;
			pos.y += r.y*displacement;
			pos.z += r.z*displacement;
			break;
		case GLUT_KEY_LEFT:
			pos.x -= r.x*displacement;
			pos.y -= r.y*displacement;
			pos.z -= r.z*displacement;
			break;

		case GLUT_KEY_PAGE_UP:
			pos.x += u.x*displacement;
			pos.y += u.y*displacement;
			pos.z += u.z*displacement;
			break;
		case GLUT_KEY_PAGE_DOWN:
			pos.x -= u.x*displacement;
			pos.y -= u.y*displacement;
			pos.z -= u.z*displacement;
			break;		

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;

		case GLUT_KEY_END:
			break;

		default:
			break;
	}
	glutPostRedisplay();
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}



void display(){

	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0,0,0,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?  pos
	//2. where is the camera looking?	l
	//3. Which direction is the camera's UP direction? u
	gluLookAt(
		pos.x, pos.y, pos.z,	
		pos.x+l.x, pos.y+l.y, pos.z+l.z,	
		u.x, u.y, u.z
	);


	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);


	/****************************
	/ Add your objects from here
	****************************/
	//add objects

	drawAxes();

    for(int i = 0; i < obj_list.size(); i++) {
        object* ob = obj_list[i];
        ob->draw();
    }


	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){
	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=1;
	drawaxes=1;
	// cameraHeight=150.0;
	// cameraAngle=1.0;
	angle=0;
	//pos
	pos.x = 0;
	pos.y = -40;
	pos.z = 5;

	displacement = 3;
	theta = 0.05;

	//u
	u.x = 0;
	u.y = 0;
	u.z = 1;

	//r
	r.x = 1;
	r.y = 0;
	r.z = 0;

	//l
	l.x = 0;
	l.y = 1;
	l.z = 0;

	inc = 1;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(viewAngle ,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

void loadData(){
    ifstream scene;
    scene.open("scene.txt");
    scene >> rec_level;
    scene >> pixel_count;
    scene >> obj_count;
    for(int i = 0; i < obj_count; i++){
        string typ;
        scene >> typ;
        if(typ == "sphere"){
            double x, y, z;
            scene >> x >> y >> z;
            double radius;
            scene >> radius;
            double r, g, b;
            scene >> r >> g >> b;
            double ambient, diffuse, specular, reflection;
            scene >> ambient >> diffuse >> specular >> reflection;
            int shine;
            scene >> shine;
        
            sphere* sp = new sphere(point(x,y,z), radius);
            sp->color[0] = r;
            sp->color[1] = g;
            sp->color[2] = b;

            sp->coefficients[0] = ambient;
            sp->coefficients[1] = diffuse;
            sp->coefficients[2] = specular;
            sp->coefficients[3] = reflection;
            
            sp->shine = shine;

            obj_list.push_back(sp);
        }
        else if(typ == "triangle"){
            double p1_x,p1_y,p1_z;
            double p2_x,p2_y,p2_z;
            double p3_x,p3_y,p3_z;
            scene >> p1_x >> p1_y >> p1_z;
            scene >> p2_x >> p2_y >> p2_z;
            scene >> p3_x >> p3_y >> p3_z;

            double r, g, b;
            scene >> r >> g >> b;
            double ambient, diffuse, specular, reflection;
            scene >> ambient >> diffuse >> specular >> reflection;
            int shine;
            scene >> shine; 

            object* ob = new triangle(point(p1_x,p1_y,p1_z), point(p2_x,p2_y,p2_z), point(p3_x,p3_y,p3_z));

            ob->color[0] = r;
            ob->color[1] = g;
            ob->color[2] = b;
    
            ob->coefficients[0] = ambient;
            ob->coefficients[1] = diffuse;
            ob->coefficients[2] = specular;
            ob->coefficients[3] = reflection;
    
            ob->shine = shine;

            obj_list.push_back(ob);
        }
        else {
            double A,B,C,D,E,F,G,H,I,J;
            scene >> A >> B >> C >> D >> E;
            scene >> F >> G >> H >> I >> J;

            double cube_ref, length, width, height;
            scene >> cube_ref >> length >> width >> height;

            double r, g, b;
            scene >> r >> g >> b;
            double ambient, diffuse, specular, reflection;
            scene >> ambient >> diffuse >> specular >> reflection;
            int shine;
            scene >> shine; 
        }
    }

    scene >> point_light_count;
    for(int i = 0; i < point_light_count; i++){
        double p_x, p_y, p_z;
        scene >> p_x >> p_y >> p_z;
        double r, g, b;
        scene >> r >> g >> b;
        pointLight *light = new pointLight(point(p_x,p_y,p_z), r, g, b);
        pointLights.push_back(light);
    }

    scene >> spotlight_count;
    for(int i = 0; i < spotlight_count; i++){
        double p_x, p_y, p_z;
        scene >> p_x >> p_y >> p_z;
        double r, g, b;
        scene >> r >> g >> b;
        double dir_x, dir_y, dir_z;
        scene >> dir_x >> dir_y >> dir_z;
        double cutoff_angle;
        scene >> cutoff_angle;
    }

    cout << "Input taken" << endl;

}


int main(int argc, char **argv){   

    object *ob = new Floor(1000, 20);
    ob->coefficients[0] = 0.5;
    ob->coefficients[1] = 0.3;
    ob->coefficients[2] = 0.5;
    ob->coefficients[3] = 0.2;
    ob->shine = 7;
    obj_list.push_back(ob);
    loadData();

	glutInit(&argc,argv);
	glutInitWindowSize(windowHeight, windowWidth);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("Task 1");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

    delete ob;
    for(int i = 0; i < obj_list.size(); i++){
        delete obj_list[i];
    }

	return 0;
}
