/************************************************************************
     File:        TrainView.cpp

     Author:     
                  Michael Gleicher, gleicher@cs.wisc.edu

     Modifier
                  Yu-Chi Lai, yu-chi@cs.wisc.edu
     
     Comment:     
						The TrainView is the window that actually shows the 
						train. Its a
						GL display canvas (Fl_Gl_Window).  It is held within 
						a TrainWindow
						that is the outer window with all the widgets. 
						The TrainView needs 
						to be aware of the window - since it might need to 
						check the widgets to see how to draw

	  Note:        we need to have pointers to this, but maybe not know 
						about it (beware circular references)

     Platform:    Visio Studio.Net 2003/2005

*************************************************************************/

#include <iostream>
#include <Fl/fl.h>

// we will need OpenGL, and OpenGL needs windows.h
#include <windows.h>
//#include "GL/gl.h"
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "GL/glu.h"
#include <cmath>
#include "TrainView.H"
#include "TrainWindow.H"
#include "Utilities/3DUtils.H"


#ifdef EXAMPLE_SOLUTION
#	include "TrainExample/TrainExample.H"
#endif	
//My function
Pnt3f GMT(Pnt3f p1, Pnt3f p2, Pnt3f p3, Pnt3f p4, float mode, float t) {
	glm::mat4x4 G = {
		{p1.x,p2.x,p3.x,p4.x},
		{p1.y,p2.y,p3.y,p4.y},
		{p1.z,p2.z,p3.z,p4.z},
		{1,1,1,1}
	};
	G = glm::transpose(G);
	glm::mat4x4 M;
	if (mode == 1) {
		M = { 0, 0, 0, 0,
			0, 0, -1, 1,
			0, 0, 1, 0,
			0, 0, 0, 0 };
	}
	else if (mode == 2) {
		M = {
				{-1.0f,2.0f,-1.0f,0.0f},
				{3.0f,-5.0f,0.0f,2.0f},
				{-3.0f,4.0f,1.0f,0.0f},
				{1.0f,-1.0f,0.0f,0.0f}
		};
		M /= 2.0f;
	}
	else if (mode == 3) {
		M = {
			{-1.0f,3.0f,-3.0f,1.0f},
			{3.0f,-6.0f,0.0f,4.0f},
			{-3.0f,3.0f,3.0f,1.0f},
			{1.0f,0.0f,0.0f,0.0f}
		};
		M /= 6.0f;
	}
	M = glm::transpose(M);
	glm::vec4 T = { pow(t,3),pow(t,2),pow(t,1),pow(t,0) };
	glm::vec4 result = G * M * T;
	return Pnt3f(result[0], result[1], result[2]);
}
void glVertex3f_Simplify(Pnt3f q0) {
	glVertex3f(q0.x,q0.y,q0.z);
}

void initDirLight() {
	float noAmbient[] = { 0.0f,0.0f,0.0f ,0.0f ,1.0f };
	float whiteDiffuse[] = { 1.0,1.0f ,1.0f ,1.0f };
	float position[] = { 0.5,1.0f ,0.0f ,0.0f };

	glLightfv(GL_LIGHT0, GL_AMBIENT, noAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteDiffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, position);
}

void initPosLight() {
	float yellowAmbientDiffuse[] = { 0.0f,0.0f ,1.0f ,1.0f };
	float whiteDiffuse[] = { 1.0,0.0f ,0.0f ,1.0f };
	float position[] = { -2.0f,1.0f ,0.0f ,1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, yellowAmbientDiffuse);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, whiteDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, position);
}

void DrawSleeper(Pnt3f qt0, Pnt3f qt1, Pnt3f cross_t,Pnt3f orient_t) {

	
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt1 + cross_t);
	glVertex3f_Simplify(qt1 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 - orient_t + cross_t);
	glVertex3f_Simplify(qt1 - orient_t + cross_t);
	glVertex3f_Simplify(qt1 - orient_t - cross_t);
	glVertex3f_Simplify(qt0 - orient_t - cross_t);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - orient_t - cross_t);
	glVertex3f_Simplify(qt0 - orient_t + cross_t);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt1 + cross_t);
	glVertex3f_Simplify(qt1 - cross_t);
	glVertex3f_Simplify(qt1 - orient_t - cross_t);
	glVertex3f_Simplify(qt1 - orient_t + cross_t);
	glEnd();
	glBegin(GL_QUADS);
	glVertex3f_Simplify(qt1 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - orient_t - cross_t);
	glVertex3f_Simplify(qt1 - orient_t - cross_t);
	glEnd();
}
void draw_sleeper(Pnt3f front, Pnt3f back, Pnt3f cross_t, Pnt3f up, bool doingShadows) {
	Pnt3f sleeper_up0 = front - up;
	Pnt3f sleeper_up1 = back - up;
	Pnt3f forward_nor = (front - back);
	forward_nor.normalize();
	Pnt3f cross_nor = cross_t;
	cross_nor.normalize();
	Pnt3f up_nor = up;
	up_nor.normalize();
	if (!doingShadows) glColor3ub(25, 25, 25);
	glLineWidth(1);
	glBegin(GL_LINES);
	glVertex3f_Simplify(front + cross_t);
	glVertex3f_Simplify(back + cross_t);

	glVertex3f_Simplify(back + cross_t);
	glVertex3f_Simplify(back - cross_t);

	glVertex3f_Simplify(back - cross_t);
	glVertex3f_Simplify(front - cross_t);

	glVertex3f_Simplify(front - cross_t);
	glVertex3f_Simplify(front + cross_t);

	//------------------------------
	glVertex3f_Simplify(front + cross_t);
	glVertex3f_Simplify(sleeper_up0 + cross_t);

	glVertex3f_Simplify(front - cross_t);
	glVertex3f_Simplify(sleeper_up0 - cross_t);

	glVertex3f_Simplify(back + cross_t);
	glVertex3f_Simplify(sleeper_up1 + cross_t);

	glVertex3f_Simplify(back - cross_t);
	glVertex3f_Simplify(sleeper_up1 - cross_t);

	//------------------------------
	glVertex3f_Simplify(sleeper_up0 + cross_t);
	glVertex3f_Simplify(sleeper_up1 + cross_t);

	glVertex3f_Simplify(sleeper_up1 + cross_t);
	glVertex3f_Simplify(sleeper_up1 - cross_t);

	glVertex3f_Simplify(sleeper_up1 - cross_t);
	glVertex3f_Simplify(sleeper_up0 - cross_t);

	glVertex3f_Simplify(sleeper_up0 - cross_t);
	glVertex3f_Simplify(sleeper_up0 + cross_t);
	glEnd();
	if (!doingShadows) glColor3ub(101, 50, 0);
	glBegin(GL_QUADS);
	glVertex3f_Simplify(front + cross_t);
	glVertex3f_Simplify(back + cross_t);
	glVertex3f_Simplify(back - cross_t);
	glVertex3f_Simplify(front - cross_t);

	glVertex3f_Simplify(sleeper_up0 + cross_t);
	glVertex3f_Simplify(sleeper_up1 + cross_t);
	glVertex3f_Simplify(sleeper_up1 - cross_t);
	glVertex3f_Simplify(sleeper_up0 - cross_t);

	glVertex3f_Simplify(sleeper_up0 - cross_t);
	glVertex3f_Simplify(front - cross_t);
	glVertex3f_Simplify(back - cross_t);
	glVertex3f_Simplify(sleeper_up1 - cross_t);

	glVertex3f_Simplify(sleeper_up0 + cross_t);
	glVertex3f_Simplify(front + cross_t);
	glVertex3f_Simplify(back + cross_t);
	glVertex3f_Simplify(sleeper_up1 + cross_t);

	glVertex3f_Simplify(front - cross_t);
	glVertex3f_Simplify(front + cross_t);
	glVertex3f_Simplify(sleeper_up0 + cross_t);
	glVertex3f_Simplify(sleeper_up0 - cross_t);

	glVertex3f_Simplify(back - cross_t);
	glVertex3f_Simplify(back + cross_t);
	glVertex3f_Simplify(sleeper_up1 + cross_t);
	glVertex3f_Simplify(sleeper_up1 - cross_t);
	glEnd();
}
//************************************************************************
//
// * Constructor to set up the GL window
//========================================================================
TrainView::
TrainView(int x, int y, int w, int h, const char* l) 
	: Fl_Gl_Window(x,y,w,h,l)
//========================================================================
{
	mode( FL_RGB|FL_ALPHA|FL_DOUBLE | FL_STENCIL );

	resetArcball();
}

//************************************************************************
//
// * Reset the camera to look at the world
//========================================================================
void TrainView::
resetArcball()
//========================================================================
{
	// Set up the camera to look at the world
	// these parameters might seem magical, and they kindof are
	// a little trial and error goes a long way
	arcball.setup(this, 40, 250, .2f, .4f, 0);
}

//************************************************************************
//
// * FlTk Event handler for the window
//########################################################################
// TODO: 
//       if you want to make the train respond to other events 
//       (like key presses), you might want to hack this.
//########################################################################
//========================================================================
int TrainView::handle(int event)
{
	// see if the ArcBall will handle the event - if it does, 
	// then we're done
	// note: the arcball only gets the event if we're in world view
	if (tw->worldCam->value())
		if (arcball.handle(event)) 
			return 1;

	// remember what button was used
	static int last_push;

	switch(event) {
		// Mouse button being pushed event
		case FL_PUSH:
			last_push = Fl::event_button();
			// if the left button be pushed is left mouse button
			if (last_push == FL_LEFT_MOUSE  ) {
				doPick();
				damage(1);
				return 1;
			};
			break;

	   // Mouse button release event
		case FL_RELEASE: // button release
			damage(1);
			last_push = 0;
			return 1;

		// Mouse button drag event
		case FL_DRAG:

			// Compute the new control point position
			if ((last_push == FL_LEFT_MOUSE) && (selectedCube >= 0)) {
				ControlPoint* cp = &m_pTrack->points[selectedCube];

				double r1x, r1y, r1z, r2x, r2y, r2z;
				getMouseLine(r1x, r1y, r1z, r2x, r2y, r2z);

				double rx, ry, rz;
				mousePoleGo(r1x, r1y, r1z, r2x, r2y, r2z, 
								static_cast<double>(cp->pos.x), 
								static_cast<double>(cp->pos.y),
								static_cast<double>(cp->pos.z),
								rx, ry, rz,
								(Fl::event_state() & FL_CTRL) != 0);

				cp->pos.x = (float) rx;
				cp->pos.y = (float) ry;
				cp->pos.z = (float) rz;
				damage(1);
			}
			break;

		// in order to get keyboard events, we need to accept focus
		case FL_FOCUS:
			return 1;

		// every time the mouse enters this window, aggressively take focus
		case FL_ENTER:	
			focus(this);
			break;

		case FL_KEYBOARD:
		 		int k = Fl::event_key();
				int ks = Fl::event_state();
				if (k == 'p') {
					// Print out the selected control point information
					if (selectedCube >= 0) 
						printf("Selected(%d) (%g %g %g) (%g %g %g)\n",
								 selectedCube,
								 m_pTrack->points[selectedCube].pos.x,
								 m_pTrack->points[selectedCube].pos.y,
								 m_pTrack->points[selectedCube].pos.z,
								 m_pTrack->points[selectedCube].orient.x,
								 m_pTrack->points[selectedCube].orient.y,
								 m_pTrack->points[selectedCube].orient.z);
					else
						printf("Nothing Selected\n");

					return 1;
				};
				break;
	}

	return Fl_Gl_Window::handle(event);
}

//************************************************************************
//
// * this is the code that actually draws the window
//   it puts a lot of the work into other routines to simplify things
//========================================================================
void TrainView::draw()
{

	//*********************************************************************
	//
	// * Set up basic opengl informaiton
	//
	//**********************************************************************
	//initialized glad
	if (gladLoadGL())
	{
		//initiailize VAO, VBO, Shader...
	}
	else
		throw std::runtime_error("Could not initialize GLAD!");

	// Set up the view port
	glViewport(0,0,w(),h());

	// clear the window, be sure to clear the Z-Buffer too
	glClearColor(0,0,.3f,0);		// background should be blue

	// we need to clear out the stencil buffer since we'll use
	// it for shadows
	glClearStencil(0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH);

	// Blayne prefers GL_DIFFUSE
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);

	// prepare for projection
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	setProjection();		// put the code to set up matrices here

	//######################################################################
	// TODO: 
	// you might want to set the lighting up differently. if you do, 
	// we need to set up the lights AFTER setting up the projection
	//######################################################################
	// enable the lighting
	glEnable(GL_COLOR_MATERIAL);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	// top view only needs one light
	if (tw->topCam->value()) {
		glDisable(GL_LIGHT1);
		glDisable(GL_LIGHT2);
	} else {
		glEnable(GL_LIGHT1);
		glEnable(GL_LIGHT2);
	}

	//*********************************************************************
	//
	// * set the light parameters
	//
	//**********************************************************************
	GLfloat lightPosition1[]	= {0,1,1,0}; // {50, 200.0, 50, 1.0};
	GLfloat lightPosition2[]	= {1, 0, 0, 0};
	GLfloat lightPosition3[]	= {0, -1, 0, 0};
	GLfloat yellowLight[]		= {0.5f, 0.5f, .1f, 1.0};
	GLfloat whiteLight[]			= {1.0f, 1.0f, 1.0f, 1.0};
	GLfloat blueLight[]			= {.1f,.1f,.3f,1.0};
	GLfloat grayLight[]			= {.3f, .3f, .3f, 1.0};
	/*initDirLight();
	initPosLight();*/
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition1);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, whiteLight);
	glLightfv(GL_LIGHT0, GL_AMBIENT, grayLight);

	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition2);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, yellowLight);

	glLightfv(GL_LIGHT2, GL_POSITION, lightPosition3);
	glLightfv(GL_LIGHT2, GL_DIFFUSE, blueLight);



	//*********************************************************************
	// now draw the ground plane
	//*********************************************************************
	// set to opengl fixed pipeline(use opengl 1.x draw function)
	glUseProgram(0);

	setupFloor();
	glDisable(GL_LIGHTING);
	drawFloor(200,10);


	//*********************************************************************
	// now draw the object and we need to do it twice
	// once for real, and then once for shadows
	//*********************************************************************
	glEnable(GL_LIGHTING);
	setupObjects();

	drawStuff();

	// this time drawing is for shadows (except for top view)
	if (!tw->topCam->value()) {
		setupShadows();
		drawStuff(true);
		unsetupShadows();
	}
}

//************************************************************************
//
// * This sets up both the Projection and the ModelView matrices
//   HOWEVER: it doesn't clear the projection first (the caller handles
//   that) - its important for picking
//========================================================================
void TrainView::
setProjection()
//========================================================================
{
	// Compute the aspect ratio (we'll need it)
	float aspect = static_cast<float>(w()) / static_cast<float>(h());

	// Check whether we use the world camp
	if (tw->worldCam->value())
		arcball.setProjection(false);
	// Or we use the top cam
	else if (tw->topCam->value()) {
		float wi, he;
		if (aspect >= 1) {
			wi = 110;
			he = wi / aspect;
		} 
		else {
			he = 110;
			wi = he * aspect;
		}

		// Set up the top camera drop mode to be orthogonal and set
		// up proper projection matrix
		glMatrixMode(GL_PROJECTION);
		glOrtho(-wi, wi, -he, he, 200, -200);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(-90,1,0,0);
	}
	else {
		trainCamView(this,aspect);
	}
}
void TrainView::trainCamView(TrainView* train, float aspect) {
	float percent = 1.0f / 20.0f;
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(120, aspect, 1, 500);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	float t = t_time;
	int i = floor(t);
	t -= i;
	ControlPoint p0 = m_pTrack->points[(i - 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
	ControlPoint p1 = m_pTrack->points[(i + m_pTrack->points.size()) % m_pTrack->points.size()];
	ControlPoint p2 = m_pTrack->points[(i + 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
	ControlPoint p3 = m_pTrack->points[(i + 2 + m_pTrack->points.size()) % m_pTrack->points.size()];

	Pnt3f train_ori_center = GMT(p0.orient, p1.orient, p2.orient, p3.orient, tw->splineBrowser->value(), t-percent);
	Pnt3f train_ori_forward = GMT(p0.orient, p1.orient, p2.orient, p3.orient, tw->splineBrowser->value(), t);

	Pnt3f train_center = GMT(p0.pos, p1.pos, p2.pos, p3.pos, tw->splineBrowser->value(), t-percent) + train_ori_center * 2.0f;
	Pnt3f train_forward = GMT(p0.pos, p1.pos, p2.pos, p3.pos, tw->splineBrowser->value(), t) + train_ori_forward * 2.0f;
	gluLookAt(train_center.x, train_center.y, train_center.z,
		train_forward.x, train_forward.y, train_forward.z, train_ori_center.x, train_ori_center.y, train_ori_center.z);
}
//************************************************************************
//
// * this draws all of the stuff in the world
//
//	NOTE: if you're drawing shadows, DO NOT set colors (otherwise, you get 
//       colored shadows). this gets called twice per draw 
//       -- once for the objects, once for the shadows
//########################################################################
// TODO: 
// if you have other objects in the world, make sure to draw them
//########################################################################
//========================================================================
void TrainView::drawStuff(bool doingShadows)
{
	
	// Draw the control points
	// don't draw the control points if you're driving 
	// (otherwise you get sea-sick as you drive through them)
	if (!tw->trainCam->value()) {
		for (size_t i = 0; i < m_pTrack->points.size(); ++i) {
			if (!doingShadows) {
				if (((int)i) != selectedCube)
					glColor3ub(240, 60, 60);
				else
					glColor3ub(240, 240, 30);
			}
			m_pTrack->points[i].draw();
		}
	}
	// draw the track
	//####################################################################
	// TODO: 
	// call your own track drawing code
	//####################################################################
	drawTrack(this, doingShadows);
	// draw the train
	//####################################################################
	// TODO: 
	//	call your own train drawing code
	//####################################################################
	if (!tw->trainCam->value())
		drawTrain(this, doingShadows);
}

// 
//************************************************************************
//
// * this tries to see which control point is under the mouse
//	  (for when the mouse is clicked)
//		it uses OpenGL picking - which is always a trick
//########################################################################
// TODO: 
//		if you want to pick things other than control points, or you
//		changed how control points are drawn, you might need to change this
//########################################################################
//========================================================================
void TrainView::drawTrack(TrainView* TrainV, bool doingShadows) {
	float T = 0.0f;
	float percent = 1.0f / DIVIDE_LINE;
	Pnt3f lastqt;
	bool Draw_Sleeper = false;
	for (size_t i = 0; i < m_pTrack->points.size(); i++) {
		ControlPoint& p1 = m_pTrack->points[(i - 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
		ControlPoint& p2 = m_pTrack->points[(i + m_pTrack->points.size()) % m_pTrack->points.size()];
		ControlPoint& p3 = m_pTrack->points[(i + 1 + m_pTrack->points.size()) % m_pTrack->points.size()];
		ControlPoint& p4 = m_pTrack->points[(i + 2 + m_pTrack->points.size()) % m_pTrack->points.size()];
		float t = percent;
		for (size_t j = 0; j < DIVIDE_LINE; j++) {
			Pnt3f qt0 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), t);
			Pnt3f orient_t= GMT(p1.orient, p2.orient, p3.orient, p4.orient,TrainV->tw->splineBrowser->value(), t);
			t += percent;
			Pnt3f qt1 = GMT(p1.pos, p2.pos, p3.pos, p4.pos, TrainV->tw->splineBrowser->value(), t);
			Pnt3f forward = qt1 - qt0;
			Pnt3f cross_t = forward * orient_t;
			cross_t.normalize();
			orient_t = cross_t * forward;
			orient_t.normalize();
			cross_t = cross_t * 2.5f;
			
			if (!doingShadows) {
				glColor3ub(77, 19, 0);
			}
			glLineWidth(4);
			glBegin(GL_LINES);
			glVertex3f_Simplify(qt0 + cross_t);
			glVertex3f_Simplify(qt1 + cross_t);
			glVertex3f_Simplify(qt0 - cross_t);
			glVertex3f_Simplify(qt1 - cross_t);
			glEnd();
			//補畫鐵軌斷裂處
			if (j != 0) {
				glBegin(GL_LINES);
				glVertex3f_Simplify(lastqt + cross_t);
				glVertex3f_Simplify(qt1 + cross_t);
				glVertex3f_Simplify(lastqt - cross_t);
				glVertex3f_Simplify(qt1 - cross_t);
				glEnd();
			}	
			
			T+= sqrtf(forward.x * forward.x + forward.y * forward.y + forward.z * forward.z);
			if (!Draw_Sleeper &&T >= Sleeper_Length) {
				if (!doingShadows) {
					glColor3ub(101, 50, 0);
				}
				forward.normalize();
				//DrawSleeper(qt0, qt0 - forward * track_interval, cross_t, orient_t);
				draw_sleeper(qt0 - forward * Sleeper_Length, qt0, cross_t, orient_t, doingShadows);
				T -= Sleeper_Length;
			}
			else if (Draw_Sleeper && T >= Sleeper_Interval) {
				T -= Sleeper_Interval;
				Draw_Sleeper = !Draw_Sleeper;
			}
			/*if (j % 2 == 0) {
				if (!doingShadows) {
					glColor3ub(101, 50, 0);
				}
				DrawSleeper(qt0, qt1, cross_t, orient_t);
			}*/
			/*else if (!sleeper && T >= track_interval) {
				T -= track_interval;
				sleeper = !sleeper;
			}*/
			lastqt = qt0;
		}
	}
}

void TrainView::drawTrain(TrainView* TrainV, bool doingShadows) {
	float percent = 1.0f / DIVIDE_LINE;
	float t = t_time;
	int i = floor(t);
	t -= i;
	Pnt3f cp_pos_p1 = m_pTrack->points[(i - 1 + m_pTrack->points.size()) % m_pTrack->points.size()].pos;
	Pnt3f cp_pos_p2 = m_pTrack->points[(i + m_pTrack->points.size()) % m_pTrack->points.size()].pos;
	Pnt3f cp_pos_p3 = m_pTrack->points[(i + 1 + m_pTrack->points.size()) % m_pTrack->points.size()].pos;
	Pnt3f cp_pos_p4 = m_pTrack->points[(i + 2 + m_pTrack->points.size()) % m_pTrack->points.size()].pos;
	// orient
	Pnt3f cp_orient_p1 = m_pTrack->points[(i - 1 + m_pTrack->points.size()) % m_pTrack->points.size()].orient;
	Pnt3f cp_orient_p2 = m_pTrack->points[(i + m_pTrack->points.size()) % m_pTrack->points.size()].orient;
	Pnt3f cp_orient_p3 = m_pTrack->points[(i + 1 + m_pTrack->points.size()) % m_pTrack->points.size()].orient;
	Pnt3f cp_orient_p4 = m_pTrack->points[(i + 2 + m_pTrack->points.size()) % m_pTrack->points.size()].orient;

	Pnt3f orient_t = GMT(cp_orient_p1, cp_orient_p2, cp_orient_p3, cp_orient_p4, TrainV->tw->splineBrowser->value(), t);
	Pnt3f qt0 = GMT(cp_pos_p1, cp_pos_p2, cp_pos_p3, cp_pos_p4, TrainV->tw->splineBrowser->value(), t);
	t += percent;
	Pnt3f qt1 = GMT(cp_pos_p1, cp_pos_p2, cp_pos_p3, cp_pos_p4, TrainV->tw->splineBrowser->value(), t);
	Pnt3f cross_t = (qt1 - qt0) * orient_t;
	cross_t.normalize();
	orient_t = cross_t * (qt1 - qt0);
	orient_t.normalize();
	cross_t = cross_t * 2.5f;
	Pnt3f up = orient_t * 5;
	Pnt3f forward = (qt1 - qt0);
	forward.normalize();
	forward =forward * 10;

	if (!doingShadows) {
		glColor3ub(64, 94, 15);
	}
	//上
	glBegin(GL_QUADS);
	glNormal3f(up.x, up.y, up.z);
	glVertex3f_Simplify(qt0 + cross_t+up);
	glVertex3f_Simplify(qt0 +forward + cross_t+up);
	glVertex3f_Simplify(qt0+forward- cross_t+up);
	glVertex3f_Simplify(qt0 - cross_t+up);
	glEnd();
	//下
	glBegin(GL_QUADS);
	glNormal3f(-up.x,-up.y, -up.z);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glEnd();
	//前
	glBegin(GL_QUADS);
	glNormal3f(forward.x, forward.y, forward.z);
	glVertex3f_Simplify(qt0 + forward - cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glEnd();
	//後
	glBegin(GL_QUADS);
	glNormal3f(-forward.x, -forward.y, -forward.z);
	glVertex3f_Simplify(qt0  - cross_t + up);
	glVertex3f_Simplify(qt0  + cross_t + up);
	glVertex3f_Simplify(qt0  + cross_t);
	glVertex3f_Simplify(qt0  - cross_t);
	glEnd();
	//左
	glBegin(GL_QUADS);
	glNormal3f(-cross_t.x, -cross_t.y, -cross_t.z);
	glVertex3f_Simplify(qt0 + forward - cross_t + up);
	glVertex3f_Simplify(qt0 + forward - cross_t);
	glVertex3f_Simplify(qt0 - cross_t);
	glVertex3f_Simplify(qt0 - cross_t+up);
	glEnd();
	//右
	glBegin(GL_QUADS);
	glNormal3f(cross_t.x, cross_t.y, cross_t.z);
	glVertex3f_Simplify(qt0 + forward + cross_t + up);
	glVertex3f_Simplify(qt0 + forward + cross_t);
	glVertex3f_Simplify(qt0 + cross_t);
	glVertex3f_Simplify(qt0 + cross_t + up);
	glEnd();
}

void TrainView::
doPick()
//========================================================================
{
	// since we'll need to do some GL stuff so we make this window as 
	// active window
	make_current();		

	// where is the mouse?
	int mx = Fl::event_x(); 
	int my = Fl::event_y();

	// get the viewport - most reliable way to turn mouse coords into GL coords
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);

	// Set up the pick matrix on the stack - remember, FlTk is
	// upside down!
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity ();
	gluPickMatrix((double)mx, (double)(viewport[3]-my), 
						5, 5, viewport);

	// now set up the projection
	setProjection();

	// now draw the objects - but really only see what we hit
	GLuint buf[100];
	glSelectBuffer(100,buf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);

	// draw the cubes, loading the names as we go
	for(size_t i=0; i<m_pTrack->points.size(); ++i) {
		glLoadName((GLuint) (i+1));
		m_pTrack->points[i].draw();
	}

	// go back to drawing mode, and see how picking did
	int hits = glRenderMode(GL_RENDER);
	if (hits) {
		// warning; this just grabs the first object hit - if there
		// are multiple objects, you really want to pick the closest
		// one - see the OpenGL manual 
		// remember: we load names that are one more than the index
		selectedCube = buf[3]-1;
	} else // nothing hit, nothing selected
		selectedCube = -1;

	printf("Selected Cube %d\n",selectedCube);
}