/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "gfx.h"

#define GL3D_WINDOW_SIZE		120		// Limits zbuffer size to program runs in 192k limit on STMF407
#define FRAME_DELAY				20		// 20ms should be approx 50 frames per second less CPU overheads.

/* The handle for our Window */
GHandle gh;

static GLfloat Xrot, Xstep;
static GLfloat Yrot, Ystep;
static GLfloat Zrot, Zstep;
static GLfloat Step = 5.0;
static GLfloat Scale = 1.0;
static GLuint Object;

static GLuint make_object( void )
{
   GLuint list;

   list = glGenLists( 1 );

   glNewList( list, GL_COMPILE );

   glBegin( GL_LINE_LOOP );
   glColor3f( 1.0, 1.0, 1.0 );
   glVertex3f(  1.0,  0.5, -0.4 );
   glColor3f( 1.0, 0.0, 0.0 );
   glVertex3f(  1.0, -0.5, -0.4 );
   glColor3f( 0.0, 1.0, 0.0 );
   glVertex3f( -1.0, -0.5, -0.4 );
   glColor3f( 0.0, 0.0, 1.0 );
   glVertex3f( -1.0,  0.5, -0.4 );
   glEnd();

   glColor3f( 1.0, 1.0, 1.0 );

   glBegin( GL_LINE_LOOP );
   glVertex3f(  1.0,  0.5, 0.4 );
   glVertex3f(  1.0, -0.5, 0.4 );
   glVertex3f( -1.0, -0.5, 0.4 );
   glVertex3f( -1.0,  0.5, 0.4 );
   glEnd();

   glBegin( GL_LINES );
   glVertex3f(  1.0,  0.5, -0.4 );   glVertex3f(  1.0,  0.5, 0.4 );
   glVertex3f(  1.0, -0.5, -0.4 );   glVertex3f(  1.0, -0.5, 0.4 );
   glVertex3f( -1.0, -0.5, -0.4 );   glVertex3f( -1.0, -0.5, 0.4 );
   glVertex3f( -1.0,  0.5, -0.4 );   glVertex3f( -1.0,  0.5, 0.4 );
   glEnd();


   glEndList();

   return list;
}

static void draw( void )
{
   glClear( GL_COLOR_BUFFER_BIT );

   glPushMatrix();

   glTranslatef( 0.0, 0.0, -10.0 );
   glScalef( Scale, Scale, Scale );
   if (Xstep) {
      glRotatef( Xrot, 1.0, 0.0, 0.0 );
   }
   else if (Ystep) {
      glRotatef( Yrot, 0.0, 1.0, 0.0 );
   }
   else {
      glRotatef( Zrot, 0.0, 0.0, 1.0 );
   }

   glCallList( Object );

   glPopMatrix();

   glFlush();
   gwinRedraw(gh);
}


static void spin( void )
{
   Xrot += Xstep;
   Yrot += Ystep;
   Zrot += Zstep;

   if (Xrot>=360.0) {
      Xrot = Xstep = 0.0;
      Ystep = Step;
   }
   else if (Yrot>=360.0) {
      Yrot = Ystep = 0.0;
      Zstep = Step;
   }
   else if (Zrot>=360.0) {
      Zrot = Zstep = 0.0;
      Xstep = Step;
   }
   draw();
}

int main(void) {
    /* Initialize and clear the display */
    gfxInit();
    gdispClear(White);

    /* Create the 3D window */
    {
    	GWindowInit	wi;

		gwinClearInit(&wi);
    	wi.show = TRUE; wi.x = (gdispGetWidth()-GL3D_WINDOW_SIZE)/2; wi.y = (gdispGetHeight()-GL3D_WINDOW_SIZE)/2; wi.width = GL3D_WINDOW_SIZE; wi.height = GL3D_WINDOW_SIZE;
        gh = gwinGL3DCreate(0, &wi);
    }

    /* Init the 3D stuff */
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
    glMatrixMode(GL_MODELVIEW);

    Object = make_object();
    glCullFace( GL_BACK );
   	//glEnable( GL_CULL_FACE );
    glDisable( GL_DITHER );
    glShadeModel( GL_FLAT );
   	//glEnable( GL_DEPTH_TEST );
    Xrot = Yrot = Zrot = 0.0;
    Xstep = Step;
    Ystep = Zstep = 0.0;


    while(TRUE) {

    	// rate control
    	gfxSleepMilliseconds(FRAME_DELAY);

    	// move and redraw
    	spin();
    }   
}

