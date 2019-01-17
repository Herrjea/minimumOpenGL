
/*

Mirar de nuevo Vertex arrays de TheChernoProject

Mouse interaction:
	Move camera
	Detect selected object
	Animation support

Full basic engine functionality:
	Decent, 4.3 error handling
		How can I make it 4.3 or higher though? D=
	3D mesh import
	Animation import
	Texture import
	Lighting
	Camera movement
	Object movement
	Input control system
	Audio
	Normal map import
	Animation transition
	Text input box when pressing Enter, and execution when pressing Enter again:
		Nothing, if no text was entered, or only "\".
		Text commands if the line starts with \.
		Characters says the entered text inside a bubble.
	Path finding

*/


#ifdef _WIN32
#include <windows.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <openGL/gl.h>
//#include <GLUT/glut.h>
#include <ctype.h>
#include <unistd.h>


#define GLEW_STATIC



// Primitive error handling

#define GLCall(x) GLClearError();\
	x;\
	GLLogCall( #x, __FILE__, __LINE__ )

// Clear error stack
static void GLClearError(){
	while ( glGetError() != GL_NO_ERROR );
}

// Print error
static bool GLLogCall( const char* functionName, const char* fileName, int line ){

	while ( GLenum error = glGetError() ){
		printf(
			"OpenGL error %d\n\t%s\n\tat line %d\n\tin file %s\n",
			error,
			functionName,
			line,
		 	fileName
		);
		exit( -1 );
	}

	return true;
}


// Store a text file into a string
char* readFile( char* filePath );


//
// Initialize the shader programs
//

static GLuint compileShader( GLuint type, char* filePath );

static GLuint initShaders( char* vertShaderFileName, char* fragShaderFileName );


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

void draw( Axes * axes );
// Draw with glBegin and glEnd
void drawAxesLegacy( Axes * axes );
// Draw with glDrawArrays
void drawAxesModern( Axes * axes );

void changeAxisSize( Axes * axes, float newSize );



//
// Triangle object
//

typedef struct {
	GLuint vertexBuffer;
	GLuint vertexArray;
	GLuint indexBuffer;
	int vertexAmount;
	int indexAmount;
} Triangle;

void initTriangle( Triangle * triangle );

void draw( Triangle * triangle );



//
// Scene handling and rendering
//

typedef struct {

	// Objects in the scene
	Axes * axes;
	Triangle * triangle;

	GLfloat observerDistance;
	GLfloat observerAngleX;
	GLfloat observerAngleY;

	GLfloat width, height, frontPlane, backPlane;

} Scene;

void initScene( Scene * scene, int screenWidth, int screenHeight );

void drawScene( Scene * scene );
void drawObjects( Scene * scene );

void changeProjection( Scene * scene );
void changeObserver( Scene * scene );
void reshapeScene( Scene * scene, int newWidth, int newHeight );

int keyPress( Scene * scene, unsigned char key, int x, int y );
void specialKeyPress( Scene * scene, int key, int x, int y );
void idleAnimation( Scene * scene );



//
// Other methods to manipulate the scene
//

void changeWindowSize( int newWidth, int newHeight );

void normalKeys( unsigned char key, int x, int y );

void specialKeys( int key, int x, int y );


// GLFW input handling

void keyCallback( GLFWwindow* window, int key, int scanCode, int action, int modifierBits );

void charCallback( GLFWwindow* window, unsigned int codePoint );

void charModsCallback( GLFWwindow* window, unsigned int codePoint, int modifierBits );

void cursorPosCallback( GLFWwindow* window, double xPos, double yPos );

void mouseButtonCallback( GLFWwindow* window, int button, int action, int modifierBits );

void scrollCallBack( GLFWwindow* window, double xOffset, double yOffset );



// Main scene object
Scene * scene;



int main( int argc, char ** argv ){


	int screenWidthRaw = 600, screenHeightRaw = 600;
	int screenWidth, screenHeight;

	char* vertShaderFileName = "shader.vert";
	char* fragShaderFileName = "shader.frag";
	GLuint program;


	// Initialize GLFW

	if ( !glfwInit() ){
		printf( "Failed to initialize GLFW.\n" );
        return -1;
	}

    glfwWindowHint( GLFW_CONTEXT_VERSION_MAJOR, 3 );
    glfwWindowHint( GLFW_CONTEXT_VERSION_MINOR, 3 );
    glfwWindowHint( GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE );
    glfwWindowHint( GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE );
    glfwWindowHint( GLFW_RESIZABLE, GL_FALSE );

    GLFWwindow* window = glfwCreateWindow(
        screenWidthRaw,
        screenHeightRaw,
        "Minimum OpenGL program",
        NULL,
        NULL
    );

	if ( window == NULL ){
		printf( "Failed to create GLFW window.\n" );
		glfwTerminate();
		return -1;
	}

	// Get the window size that we're going to be working with.
	// Needed for the retina display of some Macs.
	// For other monitors the raw values will be returned
    glfwGetFramebufferSize( window, &screenWidth, &screenHeight );

    glfwMakeContextCurrent( window );
    glewExperimental = GL_TRUE;

	// Limit frames per second.
	// Vertical synchronization (vsync) with screen framerate
	glfwSwapInterval( 1 );


	// Initialize GLEW
    if ( glewInit() != GLEW_OK ){
        printf( "Failed to initialize GLEW.\n" );
        return -1;
    }


	// Set callback functions that will handle input events
	glfwSetKeyCallback( window, keyCallback );
	glfwSetCharCallback( window, charCallback );
	glfwSetCursorPosCallback( window, cursorPosCallback );
	glfwSetMouseButtonCallback( window, mouseButtonCallback );
	glfwSetScrollCallback( window, scrollCallBack );


	// Create and compile the shader programs
	program = initShaders( vertShaderFileName, fragShaderFileName );
	glUseProgram( program );


	// Get the location of uniform variables and assign them
	GLuint u_Color = glGetUniformLocation( program, "u_Color" );
	glUniform4f( u_Color, 0.8f, 0.5f, 0.2f, 1.0f );



	// Initialize all the objects in the scene
	scene = (Scene*) malloc( sizeof( Scene ) );
	initScene( scene, screenWidth, screenHeight );


	// Main loop of events

    while ( !glfwWindowShouldClose( window ) ){

            glfwPollEvents();

			glClear( GL_COLOR_BUFFER_BIT );

			drawScene( scene );

            glfwSwapBuffers( window );
    }

    glfwTerminate();

	// Glut initialization
	//glutInit( &argc, argv );

	// Desired attributes for OpenGL rendering
	//glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );

	// Window size and position
	int
		UI_window_pos_x = 50,
		UI_window_pos_y = 50,
		UI_window_width = 500,
		UI_window_height=500;

	// Window's bottom left corner
	//glutInitWindowPosition( UI_window_pos_x, UI_window_pos_y );
	// Window size
	//glutInitWindowSize( UI_window_width, UI_window_height );

	// Create the window
	//glutCreateWindow( "Minimum OpenGL program" );

	// Assign functions to specific behaviours
	//glutDisplayFunc( mainDrawScene );
	//glutReshapeFunc( changeWindowSize );
	//glutKeyboardFunc( normalKeys );
	//glutSpecialFunc( specialKeys );
	//glutIdleFunc( [] { if ( scene != NULL ) scene -> idleAnimation(); } );





	//glutInitDisplayMode( GLUT_3_2_CORE_PROFILE );
	//printf( "Supported GLSL version is %s.\n", (char*) glGetString( GL_SHADING_LANGUAGE_VERSION ) );


	// Create and compile the shader programs
	//program = initShaders( vertShaderFileName, fragShaderFileName );
	//glUseProgram( program );


	// Main event loop
	//glutMainLoop();


	return 0;
}



// Reading files

char* readFile( char* filePath ){

	char* buffer = 0;
	long fileLength;
	FILE * f = fopen( filePath, "r" );

	if ( f ){

		fseek( f, 0, SEEK_END );
		fileLength = ftell( f );
		fseek( f, 0, SEEK_SET );

		buffer = (char*) malloc( fileLength * sizeof( char ) );

		if ( buffer ){
			fread( buffer, 1, fileLength - 1, f );
			buffer[fileLength-1] = '\0';
		}

		fclose( f );

		if ( !buffer )
			printf( "Could not read data from file %s\n", filePath );

		//else printf( "\nRead file:\n\n%s\n\n", buffer );
	}
	else
		printf( "Could not open file %s\n", filePath );

	return buffer;
}


//
// Initialize the shader programs
//

static GLuint compileShader( GLuint type, char* filePath ){

	char* shaderSource = readFile( filePath );
	//printf( "%s\n", shaderSource );

	GLuint shaderId = glCreateShader( type );
	GLCall(glShaderSource( shaderId, 1, &shaderSource, NULL )); // NULL seguro?
	GLCall(glCompileShader( shaderId ));

	// Error handling
	int result;
	glGetShaderiv( shaderId, GL_COMPILE_STATUS, &result );
	if ( result == GL_FALSE ){
		int length;
		glGetShaderiv( shaderId, GL_INFO_LOG_LENGTH, &length );
		char* message = (char*) malloc( length * sizeof( char ) );
		glGetShaderInfoLog( shaderId, length, &length, message );
		printf(
			"Failed to compile %s shader \"%s\":\n%s\n",
			type == GL_VERTEX_SHADER ? "vertex" : "fragment",
			filePath,
			message
		);
		glDeleteShader( shaderId );
		free( message );
		return 0;
	}

	return shaderId;
}

static GLuint initShaders( char* vertShaderFileName, char* fragShaderFileName ){

	char* shaderFolder = "shaders/";
	char* filePath =
		(char*) malloc(
			(
				strlen( shaderFolder ) +
				( strlen( vertShaderFileName ) > strlen( fragShaderFileName ) ?
					strlen( vertShaderFileName ) :
					strlen( fragShaderFileName ) )
				+ 1 // for the '\0'
			) *
			sizeof( char ) // which is 1 Byte, but still, for clarity
		);
	GLuint program, vertShaderId, fragShaderId;

	program = glCreateProgram();

	strcpy( filePath, shaderFolder );
	strcat( filePath, vertShaderFileName );
	vertShaderId = compileShader( GL_VERTEX_SHADER, filePath );

	strcpy( filePath, shaderFolder );
	strcat( filePath, fragShaderFileName );
	fragShaderId = compileShader( GL_FRAGMENT_SHADER, filePath );

	GLCall(glAttachShader( program, vertShaderId ));
	GLCall(glAttachShader( program, fragShaderId ));
	GLCall(glLinkProgram( program ));
	GLCall(glValidateProgram( program ));

	// We can delete the shaders,
	// since they're already linked to out program
	GLCall(glDeleteShader( vertShaderId ));
	GLCall(glDeleteShader( fragShaderId ));

	free( filePath );

	return program;
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



void draw( Axes * axes ){

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

	glEnableClientState( GL_COLOR_ARRAY );
	glColorPointer( 3, GL_FLOAT, 0, axes->colorArray );
	glDrawArrays( GL_LINES, 0, 6 ) ;
}



void changeAxisSize( Axes * axes, float newSize ){

	axes->axisSize = newSize;
	createArrayData( axes );
}





//
// Triangle
//

void initTriangle( Triangle * triangle ){


	GLfloat positions[] = {
		-0.5, -0.5, 0.0,
		0.5, -0.5, 0.0,
		0.0, 0.5, 0.0,
		0.1, 0.7, 0.0,
		-0.1, 0.7, 0.0
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 4
	};
	int dimensions = 3;
	triangle->vertexAmount =
		sizeof( positions ) / sizeof( GLfloat ) / dimensions;
	triangle->indexAmount =
		sizeof( indices ) / sizeof( GLuint );


	// Create the vertex and index buffers and vertex array
	// and store their indices in the triangle structure
	GLCall(glGenVertexArrays( 1, &triangle->vertexArray ));
	GLCall(glGenBuffers( 1, &triangle->vertexBuffer ));
	GLCall(glGenBuffers( 1, &triangle->indexBuffer ));

	// Set the created objects as selected
	GLCall(glBindVertexArray( triangle->vertexArray ));
	GLCall(glBindBuffer( GL_ARRAY_BUFFER, triangle->vertexBuffer ));
	GLCall(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, triangle->indexBuffer ));


	// Upload data to the selected buffers
	GLCall(glBufferData(
		GL_ARRAY_BUFFER,
		triangle->vertexAmount * dimensions * sizeof( GLfloat ),
		positions,
		GL_STATIC_DRAW
	));
	GLCall(glBufferData(
		GL_ELEMENT_ARRAY_BUFFER,
		triangle->indexAmount * sizeof( GLuint ),
		indices,
		GL_STATIC_DRAW
	));

	// Tell OpenGL how to interpret the uploaded data
	GLCall(glVertexAttribPointer(
		// Starting at the beginning of our array
		0,
		// These elements per vertex
		dimensions,
		GL_FLOAT,
		// No need no normalize here
		GL_FALSE,
		// We only store the position information of each vertex
		// in the array, tightly packed, so no stride is necessary
		//0,
		sizeof( GLfloat ) * dimensions, // ???????????
		// The position information is the first information
		// from each vertex that we store in each slot,
		// so no need to jump inside a slot to access it
		(GLvoid *) 0
	));

	// This makes me question life itself,
	// but seems to be a required step
	GLCall(glEnableVertexAttribArray( 0 ));

	// Binding 0 is equivalent to having nothing bound,
	// since 0 can never be an index for this kind of objects
	GLCall(glBindBuffer( GL_ARRAY_BUFFER, 0 ));
	GLCall(glBindVertexArray( 0 ));
}


void draw( Triangle * triangle ){

	glBindVertexArray( triangle->vertexArray );
	GLCall(glDrawElements(
		GL_TRIANGLES,
		triangle->indexAmount,
		GL_UNSIGNED_INT,
		NULL
	));
	glBindVertexArray( 0 );
}




//
// Scene
//



void initScene( Scene * scene, int screenWidth, int screenHeight ){

    scene->frontPlane = 10;
    scene->backPlane = 100;
    scene->observerDistance = 4 * scene->frontPlane;
    scene->observerAngleX = scene->observerAngleY = 0;

	scene->axes = (Axes*) malloc( sizeof( Axes ) );
	initAxes( scene->axes );

	scene->triangle = (Triangle*) malloc( sizeof( Triangle ) );
	initTriangle( scene->triangle );

    // Color to clear the scene in every frame
	glClearColor( 0.2, 0.3, 0.4, 1.0 );

    // Enable z-buffer
	//glEnable( GL_DEPTH_TEST );

	//changeProjection( scene );
	scene->width = screenWidth / 10;
	scene->height = screenHeight / 10;
	glViewport( 0, 0, screenWidth, screenHeight );
}



void drawScene( Scene * scene ){

	glClear( GL_COLOR_BUFFER_BIT );//| GL_DEPTH_BUFFER_BIT );
	//changeObserver( scene );

	drawObjects( scene );
}

void drawObjects( Scene * scene ){

	draw( scene->axes );
	draw( scene->triangle );
}


void changeProjection( Scene * scene ){

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glFrustum(
        -scene->width, scene->width,
        -scene->height, scene->height,
        scene->frontPlane, scene->backPlane
    );
}

void changeObserver( Scene * scene ) {

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glTranslatef( 0, 0, -scene->observerDistance );
    glRotatef( scene->observerAngleX, 1, 0, 0 );
    glRotatef( scene->observerAngleY, 0, 1, 0 );
}

void reshapeScene( Scene * scene, int newWidth,int newHeight ){

    changeProjection( scene );
    scene->width = newWidth / 10;
    scene->height = newHeight / 10;
    glViewport( 0, 0, newWidth, newHeight );
}


int keyPress( Scene * scene, unsigned char key, int x, int y ){

	if ( key == 'Q' || key == 'q' )
        return 1;

	return 0;
}

void specialKeyPress( Scene * scene, int key, int x, int y ){

    /*switch ( key ){
    	case GLUT_KEY_LEFT: scene->observerAngleY -= 10; break;
    	case GLUT_KEY_RIGHT: scene->observerAngleY += 10; break;
    	case GLUT_KEY_UP: scene->observerAngleX -= 10; break;
    	case GLUT_KEY_DOWN: scene->observerAngleX += 10; break;
    	case GLUT_KEY_PAGE_UP: scene->observerDistance *= 1.2; break;
    	case GLUT_KEY_PAGE_DOWN: scene->observerDistance /= 1.2; break;
	}*/
}

void idleAnimation( Scene * scene ){

	static int fps = 20;

	usleep( 1000000 / fps );
	//object -> idleAnimation();

	//glutPostRedisplay();
}





//
// Other methods
//



void changeWindowSize( int newWidth, int newHeight ){

	if ( scene != NULL )
		reshapeScene( scene, newWidth, newHeight );

	//glutPostRedisplay();
}


void normalKeys( unsigned char key, int x, int y ){

	int exitCode = 0;

	if ( scene != NULL )
		exitCode = keyPress( scene, key, x, y );

	if ( exitCode ){
		free( scene );
		exit(0);
	} else
		;//glutPostRedisplay();
}


void specialKeys( int key, int x, int y ){

	if ( scene != NULL )
		specialKeyPress( scene, key, x, y );

	//glutPostRedisplay();
}


void keyCallback( GLFWwindow* window, int key, int scanCode, int action, int modifierBits ){

	if ( action == GLFW_PRESS ){
		printf( "Pressed key: %d.\n", key );
	}
	else if ( action == GLFW_REPEAT ){

	}
	else if ( action == GLFW_RELEASE ){

	}
}


void charCallback( GLFWwindow* window, unsigned int codePoint ){

	printf(
		"Character callback.\n\tCode point: %d\n",
		codePoint
	);
}


void charModsCallback( GLFWwindow* window, unsigned int codePoint, int modifierBits ){

	printf(
		"Character modifer callback.\n\tCode point: %d\n\tModifier: %d\n",
		codePoint,
		modifierBits
	);
}


void cursorPosCallback( GLFWwindow* window, double xPos, double yPos ){

	//printf( "%.0f %.0f\n", xPos, yPos );
}


void mouseButtonCallback( GLFWwindow*window, int button, int action, int modifierBits ){

	if ( button == GLFW_MOUSE_BUTTON_LEFT ){

		if ( action == GLFW_PRESS ){

			double xPos, yPos;
			glfwGetCursorPos( window, &xPos, &yPos );
			printf( "%.0f %.0f\n", xPos, yPos );
		}
	}

	/*
	if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS ){

		double xPos, yPos;
		glfwGetCursorPos( window, &xPos, &yPos );
		printf( "%.0f %.0f\n", xPos, yPos );
	}
	*/
}


void scrollCallBack( GLFWwindow* window, double xOffset, double yOffset ){

	printf( "%.0f %.0f\n", xOffset, yOffset );
}
