/* spin.c */


/*
 * Spinning box.  This program is in the public domain.
 *
 * Brian Paul
 */


#include <math.h>
#include <stdio.h>

#include <GL/glx.h> 
#include <GL/gl.h> 
#include "ui.h"




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



void reshape( int width, int height )
{
   glViewport(0, 0, (GLint)width, (GLint)height);
   glMatrixMode(GL_PROJECTION);
   glLoadIdentity();
   glFrustum( -1.0, 1.0, -1.0, 1.0, 5.0, 15.0 );
   glMatrixMode(GL_MODELVIEW);
}


GLenum key(int k, GLenum mask)
{
   switch (k) {
   case KEY_ESCAPE:
       exit(0);
   }
   return GL_FALSE;
}


void draw( void )
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
   tkSwapBuffers();
}


void idle( void )
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

void init(void)
{
   Object = make_object();
   glCullFace( GL_BACK );
/*   glEnable( GL_CULL_FACE );*/
   glDisable( GL_DITHER );
   glShadeModel( GL_FLAT );
/*   glEnable( GL_DEPTH_TEST ); */

   Xrot = Yrot = Zrot = 0.0;
   Xstep = Step;
   Ystep = Zstep = 0.0;
}


int main( int argc, char *argv[] )
{
    return ui_loop(argc, argv, "spin");
}
