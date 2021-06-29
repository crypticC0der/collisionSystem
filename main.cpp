#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
using namespace std;
#define VERTS 15
#define ELAST_X 0.25
#define ELAST_Y 0.5
#define ELAST_COL 0.5

class Particle{
	public:
		float *pos;
		float *vel;
		float mass;
		float radius;

		void init(float m,float r, float x, float y, float vx, float vy){
			pos = new float[2];
			vel = new float[2];
			radius = r;
			mass =m;
			pos[0] =x;
			pos[1] =y;
			vel[0] = vx;
			vel[1] = vy;
		}

		void move(float delta){
			pos[0] += delta*vel[0];
			pos[1] += delta*vel[1];
		}

		void applyAcc(float* acc,float delta){
			vel[0]+=delta*acc[0];	
			vel[1]+=delta*acc[1];	
		}

		void bounceOffWalls(){
			if((pos[1]>=1-radius && vel[1]>0)||(pos[1]<=-1+radius&&vel[1]<0)){
				vel[1]*=-ELAST_Y;	
			}
			if((pos[0]>=1-radius && vel[0]>0)||(pos[0]<=-1+radius&&vel[0]<0)){
				vel[0]*=-ELAST_X;	
			}
		}

		void collisionWithParticle(Particle p){
			float* nvec = new float[2];
			nvec[0] = pos[0] - p.pos[0];
			nvec[1] = pos[1] - p.pos[1];
			float h = hypot(nvec[0],nvec[1]);
			nvec[0]/=h;
			nvec[1]/=h;
			float v[4]; //velocity before parrellel and perpendicular to the line of collision
			v[0] = vel[1]*nvec[0]-nvec[1]*vel[0];
			v[1] = vel[0]*nvec[0]+nvec[1]*vel[1];
			v[2] = p.vel[1]*nvec[0]-nvec[1]*p.vel[0];
			v[3] = p.vel[0]*nvec[0]+nvec[1]*p.vel[1];
			float w[4]; //velocity after parrellel and perpendicular to the line of collision
			w[0] = v[0];
			w[2] = v[2];
			w[1] = (mass*v[1] + p.mass*v[3] + p.mass*ELAST_COL*(v[3]-v[1]))/(mass+p.mass);
			w[3] = (mass*v[1] + p.mass*v[3] + mass*ELAST_COL*(v[1]-v[3]))/(mass+p.mass);
			vel[0]=w[1]*nvec[0]-w[0]*nvec[1];
			vel[1]=w[0]*nvec[0]+w[1]*nvec[1];
			p.vel[0]=w[3]*nvec[0]-w[2]*nvec[1];
			p.vel[1]=w[2]*nvec[0]+w[3]*nvec[1];
		}

		void Render(){ 
			glColor3f(mass/100,1-mass/100,0);
			glBegin(GL_POLYGON);
			for (int i =0;i<VERTS;i++){
				glVertex2d(pos[0] + radius*sin(2*i*M_PI/VERTS),pos[1]+ radius*cos(2*i*M_PI/VERTS));
			}
			glEnd();
		}
};

Particle* particles;
int numParts;


void draw(){
	for (int i =0;i<numParts;i++){
		particles[i].Render();	
	}
}

void disInit(){
	glClearColor(0.1f, 0.1f, 0.1f, 0); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	draw();
	glFlush();
}
using namespace std::chrono;
high_resolution_clock::time_point lastTime;
void run(){
	duration<double> duration = high_resolution_clock::now() - lastTime;
	double delta = 10*duration.count();
	//do shit
	for (int i=0;i<numParts;i++){
		float* acc = new float[2];
		acc[0]=(i*-1);
		acc[1]=0;
		particles[i].applyAcc(acc,delta);
		particles[i].move(delta);
		particles[i].bounceOffWalls();
		free(acc);
		for(int j=i+1;j<numParts;j++){
			if(hypot(particles[i].pos[0]-particles[j].pos[0],particles[i].pos[1]-particles[j].pos[1])<=(0.1)){
				particles[i].collisionWithParticle(particles[j]);
				cout<<"collided"<<endl;
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);
	draw();
	glFlush();
	lastTime = high_resolution_clock::now();
}


int main(int argc, char** argv) {
	numParts=2;
	particles = new Particle[2];
	particles[0].init(50,0.05,-0.5,0,0,0);
	particles[1].init(1,0.05,0.5,0,0,0);
	srand (time(NULL));
	glutInit(&argc, argv);		// Initialize GLUT
    glutInitWindowSize(800,800);   // Set the window's initial width & height
    glutCreateWindow("Lines"); // Create a window with the given title
    glutInitWindowPosition(50, 50); // Position the window's initial top-left corner
    glutDisplayFunc(disInit); // Register display callback handler for window re-paint
	lastTime = high_resolution_clock::now();
	glutIdleFunc(run);
	glutMainLoop();
	return 0;
}
