/*                                                                           
 *      _____  __  _____  __     ____                                   
 *     / ___/ / / /____/ / /    /    \   FieldKit
 *    / ___/ /_/ /____/ / /__  /  /  /   (c) 2010, FIELD. All rights reserved.              
 *   /_/        /____/ /____/ /_____/    http://www.field.io           
 *   
 *	 Created by Marcus Wendt on 21/05/2010.
 */

/*
 
 Results on my Mac Pro 2x.26Ghz Quad-Core Nehalem
 
 Debug Mode
 
 ---- Iteration Test ----
 initialising 10000 particles
 running tests 100000 times
 running StaticArrayTest: 85.021642 s 
 running STLObjectListTest: 102.046584 s 
 running STLPointerListTest: 103.400822 s 
 running STLPointerVectorTest: 107.725085 s 
 running STLObjectVectorTest: 111.854316 s 
 ---- Done ----
 
 
 Release Mode
 
 ---- Iteration Test ----
 initialising 10000 particles
 running tests 100000 times
 running StaticArrayTest: 38.603858 s 
 running STLObjectListTest: 38.412148 s 
 running STLPointerListTest: 39.084085 s 
 running STLPointerVectorTest: 38.526468 s 
 running STLObjectVectorTest: 38.399052 s 
 ---- Done ----
 */

#include <list>
#include <vector>
#include <boost/foreach.hpp>
#include "cinder/app/AppBasic.h"

using namespace std;
using namespace ci;
using namespace ci::app;

inline float randomRange(float a, float b) {
	return ((b-a)*((float)rand()/RAND_MAX))+a;
}

class Particle {
	Vec3f steer;
	Vec3f velocity;
	Vec3f position;
	
public:
	void update() {
		steer.x += randomRange(-100, 100);
		steer.y += randomRange(-100, 100);
		steer.z += randomRange(-100, 100);
		
		velocity += steer;
		position += velocity;
		velocity *= 0.97f;
		steer.zero();
	}
};

class Test {
public:
	const char* name;
	
	virtual void init(int numParticles) = 0;
	virtual void update() = 0;
};


class StaticArrayTest : public Test {
public:
	Particle* particles;
	int numParticles;
	
	void init(int numParticles) {
		this->name = "StaticArrayTest";
		this->numParticles = numParticles;
		particles = new Particle[numParticles];
	}
	
	void update() {
		for(int i=0; i<numParticles; i++) {
			particles[i].update();
		}
	}
};

class STLObjectListTest : public Test {
	list<Particle> particles;
	
	void init(int numParticles) {
		this->name = "STLObjectListTest";
		for(int i=0; i<numParticles; i++) {
			particles.push_back(Particle());
		}
	}
	
	void update() {
		for(list<Particle>::iterator p = particles.begin(); p != particles.end(); ++p) {	
			p->update();
		}
	}
};


class STLPointerListTest : public Test {
	list<Particle*> particles;
	
	void init(int numParticles) {
		this->name = "STLPointerListTest";
		for(int i=0; i<numParticles; i++) {
			particles.push_back(new Particle());
		}
	}
	
	void update() {
		for(list<Particle*>::iterator p = particles.begin(); p != particles.end(); ++p) {	
			(*p)->update();
		}
	}
};


class STLPointerVectorTest : public Test {
	vector<Particle*> particles;
	
	void init(int numParticles) {
		this->name = "STLPointerVectorTest";
		for(int i=0; i<numParticles; i++) {
			particles.push_back(new Particle());
		}
	}
	
	void update() {
		for(vector<Particle*>::iterator p = particles.begin(); p != particles.end(); ++p) {	
			(*p)->update();
		}
	}
};


class STLObjectVectorTest : public Test {
	vector<Particle> particles;
	
	void init(int numParticles) {
		this->name = "STLObjectVectorTest";
		for(int i=0; i<numParticles; i++) {
			particles.push_back(Particle());
		}
	}
	
	void update() {
		for(vector<Particle>::iterator p = particles.begin(); p != particles.end(); ++p) {	
			p->update();
		}
	}
};

int main(const char* args) {
	printf("---- Iteration Test ----\n");
	
	// config
	int numParticles = 10000;
	int numIterations = 100000;
	Timer* timer = new Timer();
	
	// create tests
	list<Test*> tests;
	tests.push_back(new StaticArrayTest());
	tests.push_back(new STLObjectListTest());
	tests.push_back(new STLPointerListTest());
	tests.push_back(new STLPointerVectorTest());
	tests.push_back(new STLObjectVectorTest());	
	
	// init tests
	printf("initialising %i particles\n", numParticles);
	BOOST_FOREACH(Test* t, tests) {
		t->init(numParticles);
	}
	
	// run tests
	printf("running tests %i times\n", numIterations);
	BOOST_FOREACH(Test* t, tests) {
		printf("running %s: ", t->name);
		timer->start();
		for(int i=0; i<numIterations; i++) {
			t->update();
		}
		timer->stop();
		printf("%f s \n", timer->getSeconds());
	}
	
	printf("---- Done ----\n");
	return 0;
}