#include <iostream>
#include <cmath>
#include <GL/glut.h>
#include <time.h> 
#include <stdlib.h>
#include <unistd.h>
#include <chrono>
using namespace std;
using namespace std::chrono;
#define VERTS 15
#define ELAST_X 0.25
#define ELAST_Y 0.5
#define ELAST_COL 0.5
float delta;

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

		void move(){
			pos[0] += delta*vel[0];
			pos[1] += delta*vel[1];
		}

		void applyForce(float* acc){
			vel[0]+=delta*acc[0]/mass;
			vel[1]+=delta*acc[1]/mass;
		}

		void applyGrav(){
			vel[1]-=delta*9.8f;
		}

		void applyAcc(float* acc){
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

class Rod{
	public:
		float* points[2];  //0=start 1=end
		Particle* attached;
		int pAttached;
		bool movable[2];
		float length;

		void init(Particle p1, Particle p2,float len){
			points[0]= p2.pos;
			points[1]=p1.pos;
			length=len;
			attached=new Particle[2];
			attached[0]=p1;
			attached[1]=p2;
			pAttached=2;
		}
		void init(float* startPoint, Particle p1,float len){
			points[0]= startPoint;
			points[1]=p1.pos;
			length=len;
			attached=new Particle[1];
			attached[0]=p1;
			pAttached=1;
		}

		void adjustParticles(){
			float* d=new float[2];
			d[0]=points[0][0]-points[1][0];
			d[1]=points[0][1]-points[1][1];
			float hyp = hypot(d[0],d[1]);
			cout << hyp<<endl;
			d[0]/=hyp;
			d[1]/=hyp;
			for (int i=0;i<pAttached;i++){
				float magnitude = attached[i].vel[0]*d[0] + attached[i].vel[1]*d[1];
				attached[i].vel[0]-=d[0]*magnitude;
				attached[i].vel[1]-=d[1]*magnitude;
			}
		}
		void Render(){
			glColor3f(0,1,0);
			glBegin(GL_LINES);
			glVertex2d(points[0][0],points[0][1]);
			glVertex2d(points[1][0],points[1][1]);
			glEnd();

		}
};

class Spring{
	public:
		float *start;
		float *end;
		Particle *attached;
		int pAttached;
		float elsticity;
		float natLen;
		bool spring; //true=spring false=string;

		void init(Particle p1, Particle p2,float elast,float natl,bool isSpring){
			start = p2.pos;
			end=p1.pos;
			elsticity=elast;
			natLen=natl;
			spring=isSpring;
			attached=new Particle[2];
			attached[0]=p1;
			attached[1]=p2;
			pAttached=2;
		}
		void init(float* startPoint, Particle p1,float elast,float natl,bool isSpring){
			start = startPoint;
			end=p1.pos;
			elsticity=elast;
			natLen=natl;
			spring=isSpring;
			attached=new Particle[1];
			attached[0]=p1;
			pAttached=1;
		}

		float* tension(){
			float *diff = new float[2];
			diff[0] = start[0]-end[0];
			diff[1] = start[1]-end[1];
			float clen= hypot(diff[0],diff[1]);
			float ext = clen-natLen;
			if(ext<0 && !spring){
				ext=0;
			}
			//diff becomes the tension vector, this is set to be applied to end
			//to apply it to start *-1 it
			diff[0]*=ext*elsticity/(clen*natLen);
			diff[1]*=ext*elsticity/(clen*natLen);
			return diff;	
		}

		void Render(){
			glColor3f(elsticity/100,1-elsticity/100,0);
			glBegin(GL_LINES);
			glVertex2d(start[0],start[1]);
			glVertex2d(end[0],end[1]);
			glEnd();

		}
};

Rod* rods;
Particle* particles;
Spring* springs;
int numRods;
int numParts;
int numSprings;


void draw(){
	for (int i =0;i<numParts;i++){
		particles[i].Render();	
	}
	for (int i=0;i<numRods;i++){
		rods[i].Render();	
	}
	for (int i=0;i<numSprings;i++){
		springs[i].Render();	
	}
}

void disInit(){
	glClearColor(0.1f, 0.1f, 0.1f, 0); // Set background color to black and opaque
	glClear(GL_COLOR_BUFFER_BIT);         // Clear the color buffer
	draw();
	glFlush();
}

high_resolution_clock::time_point lastTime;
void run(){
	duration<double> duration = high_resolution_clock::now() - lastTime;
	delta = duration.count();
	lastTime = high_resolution_clock::now();
	//do shit
	for (int i=0;i<numSprings;i++){
		float* ten = springs[i].tension();
		for(int j=0;j<springs[i].pAttached;j++){
			springs[i].attached[j].applyForce(ten);
			ten[0]*=-1;
			ten[1]*=-1;
		}
	}
	for (int i=0;i<numParts;i++){
		particles[i].applyGrav();
	}
	for (int i=0;i<numRods;i++){
		rods[i].adjustParticles();
	}
	for (int i =0;i<numParts;i++){
		particles[i].move();
		particles[i].bounceOffWalls();

		for(int j=i+1;j<numParts;j++){
			if(hypot(particles[i].pos[0]-particles[j].pos[0],particles[i].pos[1]-particles[j].pos[1])<=(0.1)){
				particles[i].collisionWithParticle(particles[j]);
			}
		}
	}

	glClear(GL_COLOR_BUFFER_BIT);
	draw();
	glFlush();
}


int main(int argc, char** argv) {
	numParts=4;
	particles = new Particle[4];
	particles[0].init(1,0.05,0.1,0,0,0);
	particles[1].init(1,0.05,-0.1,0,0,0);
	particles[2].init(1,0.05,-0.1,0.2,0,0);
	particles[3].init(1,0.05,0.1,0.2,0,0);
	numSprings=0;
	rods = new Rod[6];
	numRods=6;
	springs=new Spring[0];
	rods[0].init(particles[0],particles[1],0.6);
	rods[1].init(particles[0],particles[2],0.6);
	rods[2].init(particles[0],particles[3],0.6);
	rods[3].init(particles[1],particles[2],0.6);
	rods[4].init(particles[1],particles[3],0.6);
	rods[5].init(particles[2],particles[3],0.6);
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
