
#ifdef _WIN32
#include <windows.h>
#endif

#include "stdlib.h"
#include "stdio.h"
#include <openGL/gl.h>
#include <GLUT/glut.h>
#include <ctype.h>
#include <unistd.h>


//
// Axes object and methods to render it
//

typedef struct {
	float axisSize;
	float vertexArray[18];
	float colorArray[18];
} Axes;

void initAxes( Axes * axes );
void createArrayData( Axes * axes);

void drawAxes( Axes * axes );
// Draw with glBegin and glEnd
void drawAxesLegacy( Axes * axes );
// Draw with glDrawArrays
void drawAxesModern( Axes * axes );

void changeAxisSize( Axes * axes, float newSize );



//
// Scene handling and rendering
//

typedef struct {

	// Objects in the scene
	Axes * axes;

	GLfloat Observer_distance;
	GLfloat Observer_angle_x;
	GLfloat Observer_angle_y;

	GLfloat Width, Height, Front_plane, Back_plane;

} Scene;

void initScene( Scene * scene, int UI_window_width, int UI_window_height );

void drawScene( Scene * scene );
void drawObjects( Scene * scene );

void changeProjection( Scene * scene );
void changeObserver( Scene * scene );
void reshapeScene( Scene * scene, int newWidth, int newHeight );

int keyPress( Scene * scene, unsigned char key, int x, int y );
void specialKeyPress( Scene * scene, int key, int x, int y );
void idleAnimation( Scene * scene );



Scene * scene;



void mainDrawScene(){

	if ( scene != NULL )
		drawScene( scene );

	glutSwapBuffers();
}


void changeWindowSize( int newWidth, int newHeight ){

	if ( scene != NULL )
		reshapeScene( scene, newWidth, newHeight );

	glutPostRedisplay();
}



void normalKeys( unsigned char key, int x, int y ){

	int exitCode = 0;

	if ( scene != NULL )
		exitCode = keyPress( scene, key, x, y );

	if ( exitCode ){
		free( scene );
		exit(0);
	} else
		glutPostRedisplay();
}


void specialKeys( int key, int x, int y ){

	if ( scene != NULL )
		specialKeyPress( scene, key, x, y );

	glutPostRedisplay();
}




int main( int argc, char ** argv ){


	// Glut initialization
	glutInit( &argc, argv );

	// Desired attributes for OpenGL rendering
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

	// Window size and position
	int
		UI_window_pos_x = 50,
		UI_window_pos_y = 50,
		UI_window_width = 500,
		UI_window_height=500;

	// Window's bottom left corner
	glutInitWindowPosition( UI_window_pos_x, UI_window_pos_y );
	// Window size
	glutInitWindowSize( UI_window_width, UI_window_height );

	// Create the window
	glutCreateWindow( "Minimum OpenGL program" );

	// Assign functions to specific behaviours
	glutDisplayFunc( mainDrawScene );
	glutReshapeFunc( changeWindowSize );
	glutKeyboardFunc( normalKeys );
	glutSpecialFunc( specialKeys );
	//glutIdleFunc( [] { if ( scene != NULL ) scene -> idleAnimation(); } );


	// Initialize the scene
	scene = (Scene*) malloc( sizeof( Scene ) );
	initScene( scene, UI_window_width, UI_window_height );


	// Main event loop
	glutMainLoop();


	return 0;
}






//
// Axes
//


void initAxes( Axes * axes ){

	axes[0] = (Axes) {};
    axes->axisSize = 1000;
}


void createArrayData( Axes * axes ){

	axes->colorArray[0] = 1; axes->colorArray[1] = 0; axes->colorArray[2] = 0;
	axes->colorArray[3] = 1; axes->colorArray[4] = 0; axes->colorArray[5] = 0;
	axes->colorArray[6] = 0; axes->colorArray[7] = 1; axes->colorArray[8] = 0;
	axes->colorArray[9] = 0; axes->colorArray[10] = 1; axes->colorArray[11] = 0;
	axes->colorArray[12] = 0; axes->colorArray[13] = 0; axes->colorArray[14] = 1;
	axes->colorArray[15] = 0; axes->colorArray[16] = 0; axes->colorArray[17] = 1;

	axes->vertexArray[0] = -axes->axisSize; axes->vertexArray[1] = 0; axes->vertexArray[2] = 0;
	axes->vertexArray[3] = axes->axisSize; axes->vertexArray[4] = 0; axes->vertexArray[5] = 0;
	axes->vertexArray[6] = 0; axes->vertexArray[7] = -axes->axisSize; axes->vertexArray[8] = 0;
	axes->vertexArray[9] = 0; axes->vertexArray[10] = axes->axisSize; axes->vertexArray[11] = 0;
	axes->vertexArray[12] = 0; axes->vertexArray[13] = 0; axes->vertexArray[14] = -axes->axisSize;
	axes->vertexArray[15] = 0; axes->vertexArray[16] = 0; axes->vertexArray[17] = axes->axisSize;
}



void drawAxes( Axes * axes ){

	drawAxesLegacy( axes );
}


void drawAxesLegacy( Axes * axes ){

	glLineWidth( 1 );

	glBegin( GL_LINES );
		// X axis, in red
		glColor3f( 1, 0, 0 );
		glVertex3f( -axes->axisSize, 0, 0 );
		glVertex3f( axes->axisSize, 0, 0 );
		// Y axis, in green
		glColor3f( 0, 1, 0 );
		glVertex3f( 0, -axes->axisSize, 0 );
		glVertex3f( 0, axes->axisSize, 0 );
		// Z axis, in blue
		glColor3f( 0, 0, 1 );
		glVertex3f( 0, 0, -axes->axisSize );
		glVertex3f( 0, 0, axes->axisSize );
	glEnd();
}


void drawAxesModern( Axes * axes ){

	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, 0, axes->vertexArray );
	glEnableClientState( GL_COLOR_ARRAY );
	glColorPointer( 3, GL_FLOAT, 0, axes->colorArray );
	glDrawArrays( GL_LINES, 0, 6 ) ;
}



void changeAxisSize( Axes * axes, float newSize ){

	axes->axisSize = newSize;
	createArrayData( axes );
}





//
// Scene
//



void initScene( Scene * scene, int UI_window_width,int UI_window_height ){

    scene->Front_plane = 10;
    scene->Back_plane = 100;
    scene->Observer_distance = 4 * scene->Front_plane;
    scene->Observer_angle_x = scene->Observer_angle_y = 0;

	scene->axes = (Axes*) malloc( sizeof( Axes) );
	initAxes( scene->axes );

    // Color to clear the scene in every frame
	glClearColor(1,1,1,1);

    // Enable z-buffer
	glEnable(GL_DEPTH_TEST);

	changeProjection( scene );
	scene->Width = UI_window_width / 10;
	scene->Height = UI_window_height / 10;
	glViewport( 0, 0, UI_window_width, UI_window_height );
}



void drawScene( Scene * scene ){

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
	changeObserver( scene );
	drawObjects( scene );
}

void drawObjects( Scene * scene ){

	drawAxes( scene->axes );
}


void changeProjection( Scene * scene ){

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum(
        -scene->Width, scene->Width,
        -scene->Height, scene->Height,
        scene->Front_plane, scene->Back_plane
    );
}

void changeObserver( Scene * scene ) {

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0, 0, -scene->Observer_distance );
    glRotatef( scene->Observer_angle_x, 1, 0, 0 );
    glRotatef( scene->Observer_angle_y, 0, 1, 0 );
}

void reshapeScene( Scene * scene, int newWidth,int newHeight ){

    changeProjection( scene );
    scene->Width = newWidth / 10;
    scene->Height = newHeight / 10;
    glViewport( 0, 0, newWidth, newHeight );
}


int keyPress( Scene * scene, unsigned char key, int x, int y ){

	if ( key == 'Q' || key == 'q' )
        return 1;

	return 0;
}

void specialKeyPress( Scene * scene, int key, int x, int y ){

    switch ( key ){
    	case GLUT_KEY_LEFT: scene->Observer_angle_y -= 10; break;
    	case GLUT_KEY_RIGHT: scene->Observer_angle_y += 10; break;
    	case GLUT_KEY_UP: scene->Observer_angle_x -= 10; break;
    	case GLUT_KEY_DOWN: scene->Observer_angle_x += 10; break;
    	case GLUT_KEY_PAGE_UP: scene->Observer_distance *= 1.2; break;
    	case GLUT_KEY_PAGE_DOWN: scene->Observer_distance /= 1.2; break;
	}
}

void idleAnimation( Scene * scene ){

	static int fps = 20;

	usleep( 1000000 / fps );
	//object -> idleAnimation();

	glutPostRedisplay();
}
