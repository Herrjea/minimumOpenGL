
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
#include <ctype.h>
#include <unistd.h>

// OpenGL and windowing
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Load image onto byte array
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

// Matrix and vector math
#include "cglm/cglm.h"


#define GLEW_STATIC



// Primitive error handling

#define GLCall(x) GLClearError();\
	x;\
	GLLogCall( #x, __FILE__, __LINE__ )

// Clear error stack
static void GLClearError(){
	while ( glGetError() != GL_NO_ERROR );
}

// Print error.
// Program terminates if any error is found
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
// Renderer objects
//

typedef struct {
	GLuint rendererId;
} VertexBuffer;

void init( VertexBuffer * vertexBuffer, unsigned int size, const void* data );

void bind( VertexBuffer * vertexBuffer );
void unbind(  VertexBuffer * vertexBuffer  );


typedef struct {
	GLuint rendererId;
	unsigned int size;
} IndexBuffer;

void init( IndexBuffer * indexBuffer, unsigned int size, const GLuint* data );

void bind( IndexBuffer * indexBuffer );
void unbind(  IndexBuffer * indexBuffer  );


// Each element that can be stored in a vertex buffer
typedef struct {
	GLenum type;
	unsigned int count;
	GLuint normalized;
	GLsizei stride;
	unsigned int typeSize;
} VertexBufferLayoutElement;


// Which elements are stored in each position of a vertex buffer
typedef struct {
	VertexBufferLayoutElement * elements;
	unsigned int elementCount;
	unsigned int reservedElements;
	unsigned int stride;
} VertexBufferLayout;

void init( VertexBufferLayout * vertexBufferLayout );

void push( VertexBufferLayout * vertexBufferLayout, unsigned int count, GLenum type );


// Which raw data we want to be rendered (vertex buffer)
// and how should OpenGL interpret that raw data (vertex buffer layout)
typedef struct {
	GLuint rendererId;
	VertexBuffer * vertexBuffers;
	unsigned int vertexBufferCount;
	unsigned int reservedVertexBuffers;
} VertexArray;

void init( VertexArray * vertexArray );

void bind( VertexArray * vertexArray );

void unbind( VertexArray * vertexArray );

void push( VertexArray * vertexArray, VertexBuffer * buffer, VertexBufferLayout * layout );


// Shader object
typedef struct {
	GLuint rendererId;
} Shader;

void init( Shader * shader, char* vertShaderFileName, char* fragShaderFileName );

GLuint compileShader( Shader * shader, GLuint type, char* filePath );

void setUniform1i( Shader * shader, GLint location, GLint value );

void setUniform4f( Shader * shader, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 );

void setUniformMatrix4fv( Shader * shader, GLint location, mat4 matrix );

GLint getUniformLocation( Shader * shader, char* name );

void bind( Shader * shader );

void unbind( Shader * shader );


// Finally, the actual renderer object.
// Currently empty, but it's ready to be expanded
// and used in the project as if it were already
typedef struct {

} Renderer;

void init( Renderer * renderer );

void draw( Renderer * renderer, VertexArray * vertexArray, IndexBuffer * indexBuffer, Shader * shader );



//
// Textures
//

typedef struct {
	GLuint rendererId;
	int width, height, bpp;
} Texture;

void init( Texture * texture, const char* filePath );

void bind( Texture * texture, GLuint slot = 0 );

void unbind( Texture * texture );



//
// Axes object and methods to render it
//

typedef struct {
	float axisSize;
	float vertexArray[18];
	float colorArray[18];
} Axes;

void init( Axes * axes );
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
	VertexArray * vertexArray;
	IndexBuffer * indexBuffer;
	Shader * shader;
	Texture * texture;
	GLint u_Texture;
	int vertexCount;
	int indexCount;
} Triangle;

void init( Triangle * triangle, Shader * shader );

void draw( Triangle * triangle, Renderer * renderer );



//
// Scene handling
//

typedef struct {

	// Objects in the scene
	Axes * axes;
	Triangle * triangle;
	// .

	float cameraAngleX;
	float cameraAngleY;

	float cursorSpeed;
	double prevCursorX;
	double prevCursorY;

	GLfloat observerDistance;
	GLfloat observerAngleX;
	GLfloat observerAngleY;

	GLfloat width, height, frontPlane, backPlane;

} Scene;

void initScene( Scene * scene, int screenWidth, int screenHeight, Shader * shader );

void drawScene( Scene * scene, Renderer * renderer );
void drawObjects( Scene * scene, Renderer * renderer );

void changeProjection( Scene * scene );
void changeObserver( Scene * scene );
void reshapeScene( Scene * scene, int newWidth, int newHeight );

int keyPress( Scene * scene, unsigned char key, int x, int y );
void specialKeyPress( Scene * scene, int key, int x, int y );
void idleAnimation( Scene * scene );


//
// Input handling
//

void update( Scene * scene, GLFWwindow * window );



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


	int screenWidthRaw = 800, screenHeightRaw = 600;
	int screenWidth, screenHeight;
	float aspectRatio;

	char* vertShaderFileName = "shader.vert";
	char* fragShaderFileName = "shader.frag";

	Shader * shader = (Shader*) malloc( sizeof( Shader ) );

	Renderer * renderer = (Renderer*) malloc( sizeof( Renderer ) );

	mat4 viewMatrix, projectionMatrix, mvpMatrix;
	vec3 xAxis = { 1.0, 0.0, 0.0 },
		yAxis = { 0.0, 1.0, 0.0 },
		zAxis = { 0.0, 0.0, 1.0 };
	float cameraAngleX = 0, cameraAngleY = 0;


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
	aspectRatio =  (float) screenWidth / screenHeight;

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
	glfwSetInputMode( window, GLFW_STICKY_KEYS, 1 );


	// Create and compile the shader programs
	init( shader, vertShaderFileName, fragShaderFileName );
	bind( shader );

	// Get the location of uniform variables and assign them
	GLint u_Color = getUniformLocation( shader, "u_Color" );
	setUniform4f( shader, u_Color, 0.2, 0.3, 0.4, 1.0 );


	// Initialize the renderer
	init( renderer );


	// Initialize all the objects in the scene
	scene = (Scene*) malloc( sizeof( Scene ) );
	initScene( scene, screenWidth, screenHeight, shader );


	// Set the view and projection matrix and pass it to the renderer

	glm_ortho(
		-aspectRatio,		// left
		aspectRatio,		// right
		-1.0,				// bottom
		1.0,				// up
		-1.0,				// near
		1.0,				// far
		projectionMatrix
	);

	glm_mat4_identity( viewMatrix );
	/*for ( int i = 0; i < 4; i++ ){
		for ( int j = 0; j < 4; j++ )
			printf( "%.1f   ", viewMatrix[i][j] );
		printf( "\n\n" );
	}*/



	// Main loop of events

    while ( !glfwWindowShouldClose( window ) ){

            glfwPollEvents();

			glm_rotate_make( viewMatrix, scene->cameraAngleX, xAxis );
			glm_rotate( viewMatrix, scene->cameraAngleY, yAxis );
			glm_mat4_mul( projectionMatrix, viewMatrix, mvpMatrix );
			GLint u_MVP = getUniformLocation( shader, "u_MVP" );
			setUniformMatrix4fv( shader, u_MVP, mvpMatrix );

			glClear( GL_COLOR_BUFFER_BIT );

			update( scene, window );

			drawScene( scene, renderer );

            glfwSwapBuffers( window );
    }


    glfwTerminate();

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
// Renderer
//

void init( VertexBuffer * vertexBuffer, unsigned int size, const void* data ){

	// Create the vertex buffer and store its index
	GLCall(glGenBuffers( 1, &vertexBuffer->rendererId ));

	// Set the created object as selected
	GLCall(glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer->rendererId ));

	// Upload data to the selected buffer
	GLCall(glBufferData( GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW ));
}


void bind( VertexBuffer * vertexBuffer ){

	GLCall(glBindBuffer( GL_ARRAY_BUFFER, vertexBuffer->rendererId ));
}

void unbind(  VertexBuffer * vertexBuffer  ){

	GLCall(glBindBuffer( GL_ARRAY_BUFFER, 0 ));
}


void init( IndexBuffer * indexBuffer, unsigned int size, const GLuint* data ){

	indexBuffer->size = size;

	// Create the index buffer and store its index
	GLCall(glGenBuffers( 1, &indexBuffer->rendererId ));

	// Set the created object as selected
	GLCall(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer->rendererId ));

	// Upload data to the selected buffer
	GLCall(glBufferData( GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW ));
}


void bind( IndexBuffer * indexBuffer ){

	GLCall(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, indexBuffer->rendererId ));
}

void unbind(  IndexBuffer * indexBuffer  ){

	GLCall(glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 ));
}


void init( VertexBufferLayout * vertexBufferLayout ){

	vertexBufferLayout->reservedElements = 1;

	vertexBufferLayout->elements = (VertexBufferLayoutElement*)
		malloc(
			sizeof( VertexBufferLayoutElement ) * vertexBufferLayout->reservedElements
		);

	vertexBufferLayout->elementCount = 0;
	vertexBufferLayout->stride = 0;
}

void push( VertexBufferLayout * vertexBufferLayout, unsigned int count, GLenum type ){

	// Reserve more space

	if ( vertexBufferLayout->elementCount == vertexBufferLayout->reservedElements ){

		VertexBufferLayoutElement * tmp = (VertexBufferLayoutElement*)
			malloc(
				sizeof( VertexBufferLayoutElement ) * vertexBufferLayout->reservedElements * 2
			);

		for ( int i = 0; i < vertexBufferLayout->reservedElements; i++ )
			tmp[i] = vertexBufferLayout->elements[i];

		free( vertexBufferLayout->elements );

		vertexBufferLayout->elements = tmp;
		vertexBufferLayout->reservedElements *= 2;
	}


	// Push the new element

	unsigned int typeSize =
		type == GL_FLOAT ?
			sizeof( GLfloat ) :
		type == GL_UNSIGNED_INT ?
			sizeof( GLuint ) :
		sizeof( GLubyte );

	VertexBufferLayoutElement newElement = (VertexBufferLayoutElement) {
		.type = type,
		.count = count,
		.normalized = GL_FALSE,
		.typeSize = typeSize
	};
	vertexBufferLayout->elements[vertexBufferLayout->elementCount] = newElement;
	vertexBufferLayout->elementCount++;


	// Update the whole size of each vertex

	vertexBufferLayout->stride += typeSize * count;
}


void init( VertexArray * vertexArray ){

	vertexArray->reservedVertexBuffers = 1;
	vertexArray->vertexBuffers = (VertexBuffer*)
		malloc( sizeof( VertexBuffer ) * vertexArray->reservedVertexBuffers );
	vertexArray->vertexBufferCount = 0;

	// Create the vertex array and store its index
	GLCall(glGenVertexArrays( 1, &vertexArray->rendererId ));
}

void bind( VertexArray * vertexArray ){

	GLCall(glBindVertexArray( vertexArray->rendererId ));
}

void unbind( VertexArray * vertexArray ){

	GLCall(glBindVertexArray( 0 ));
}

void push( VertexArray * vertexArray, VertexBuffer * buffer, VertexBufferLayout * layout ){

	unsigned int offset = 0;
	VertexBufferLayoutElement element;

	// Set the current array and buffer as selected
	bind( vertexArray );
	bind( buffer );

	for ( int i = 0; i < layout->elementCount; i++ ){

		element = layout->elements[i];
		GLCall(glEnableVertexAttribArray( i ));
		GLCall(glVertexAttribPointer(
			i,
			element.count,
			element.type,
			element.normalized,
			layout->stride,
			(const void *) offset
		));

		offset += element.count * element.typeSize;
	}
}


void init( Shader * shader, char* vertShaderFileName, char* fragShaderFileName ){

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

	// Get a new shader program
	GLCall(shader->rendererId = glCreateProgram());

	// Read and compile the vertex shader
	strcpy( filePath, shaderFolder );
	strcat( filePath, vertShaderFileName );
	vertShaderId = compileShader( shader, GL_VERTEX_SHADER, filePath );

	// Read and compile the fragment shader
	strcpy( filePath, shaderFolder );
	strcat( filePath, fragShaderFileName );
	fragShaderId = compileShader( shader, GL_FRAGMENT_SHADER, filePath );

	// Link the shaders inside the program
	GLCall(glAttachShader( shader->rendererId, vertShaderId ));
	GLCall(glAttachShader( shader->rendererId, fragShaderId ));
	GLCall(glLinkProgram( shader->rendererId ));
	GLCall(glValidateProgram( shader->rendererId ));

	// We can delete the shaders,
	// since they're already linked to our program
	GLCall(glDeleteShader( vertShaderId ));
	GLCall(glDeleteShader( fragShaderId ));

	free( filePath );
}

GLuint compileShader( Shader * shader, GLuint type, char* filePath ){

	char* shaderSource = readFile( filePath );
	//printf( "%s\n", shaderSource );

	GLuint shaderId = glCreateShader( type );
	GLCall(glShaderSource( shaderId, 1, &shaderSource, NULL )); // NULL seguro?
	GLCall(glCompileShader( shaderId ));

	// Error handling
	int result;
	GLCall(glGetShaderiv( shaderId, GL_COMPILE_STATUS, &result ));
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

void setUniform1i( Shader * shader, GLint location, GLint value ){

	bind( shader );
	GLCall(glUniform1i( location, value ));
}

void setUniform4f( Shader * shader, GLint location, GLfloat v0, GLfloat v1, GLfloat v2, GLfloat v3 ){

	bind( shader );
	GLCall(glUniform4f( location, v0, v1, v2, v3 ));
}

void setUniformMatrix4fv( Shader * shader, GLint location, mat4 matrix ){

	bind( shader );
	GLCall(glUniformMatrix4fv(
		location,
		1, // We're only passing 1 matrix
		GL_FALSE, // No need to transpose
		(GLfloat *) matrix
	));
}

GLint getUniformLocation( Shader * shader, char* name ){

	GLCall(GLint location = glGetUniformLocation( shader->rendererId, name ));

	if ( location == -1 )
		printf( "Uniform %s has not been found.\n", name );

	return location;
}

void bind( Shader * shader ){

	GLCall(glUseProgram( shader->rendererId ));
}

void unbind( Shader * shader ){

	GLCall(glUseProgram( 0 ));
}


void init( Renderer * renderer ){

}

void draw( Renderer * renderer, VertexArray * vertexArray, IndexBuffer * indexBuffer, Shader * shader ){

	GLCall(glDrawElements(
		GL_TRIANGLES,
		indexBuffer->size,
		GL_UNSIGNED_INT,
		NULL
	));
}





//
// Textures
//

void init( Texture * texture, const char* filePath ){

	// OpenGL expects the texture to start at the bottom left,
	// not the top left as it is saved in massive storage
	stbi_set_flip_vertically_on_load( true );

	unsigned char* localBuffer = stbi_load(
		filePath,
		&texture->width,
		&texture->height,
		&texture->bpp,
		// We want four channels for our picture (RGBA)
		4
	);

	GLCall(glGenTextures( 1, &texture->rendererId ));
	GLCall(glBindTexture( GL_TEXTURE_2D, texture->rendererId ));

	GLCall(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR ));
	GLCall(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR ));
	GLCall(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE ));
	GLCall(glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE ));

	GLCall(glTexImage2D(
		GL_TEXTURE_2D,
		0,
		GL_RGBA8,
		texture->width,
		texture->height,
		0,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		localBuffer
	));

	GLCall(glBindTexture( GL_TEXTURE_2D, 0 ));

	// Free the image once it's already been loaded to OpenGL
	if ( localBuffer )
		stbi_image_free( localBuffer );
}

void bind( Texture * texture, GLuint slot ){

	GLCall(glActiveTexture( GL_TEXTURE0 + slot ));
	GLCall(glBindTexture( GL_TEXTURE_2D, texture->rendererId ));
}

void unbind( Texture * texture ){

	GLCall(glBindTexture( GL_TEXTURE_2D, 0 ));
}





//
// Axes
//


void init( Axes * axes ){

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

void init( Triangle * triangle, Shader * shader ){


	triangle->shader = shader;

	GLfloat vertices[] = {
		// 3 coords for position,
		// 4 for base color,
		// 2 for texture mapping
		-0.5, -0.5, 0.0,	0.8, 0.5, 0.2, 1.0,		0.0, 0.0,
		0.5, -0.5, 0.0,		0.8, 0.5, 0.2, 1.0,		1.0, 0.0,
		0.0, 0.5, 0.0,		0.8, 0.5, 0.2, 1.0,		0.5, 0.8,
		0.1, 0.7, 0.0,		0.8, 0.5, 0.2, 1.0,		0.6, 1.0,
		-0.1, 0.7, 0.0,		0.8, 0.5, 0.2, 1.0,		0.4, 1.0
	};
	GLuint indices[] = {
		0, 1, 2,
		2, 3, 4
	};
	int positionDimensions = 3;
	int colorDimensions = 4;
	int textureDimensions = 2;
	int dimensions = positionDimensions + colorDimensions + textureDimensions;
	triangle->vertexCount =
		sizeof( vertices ) / sizeof( GLfloat ) / dimensions;
	triangle->indexCount =
		sizeof( indices ) / sizeof( GLuint );


	// Dealing with blending
	GLCall(glEnable( GL_BLEND ));
	GLCall(glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA ));


	// We first initialize the vertex array
	// to have it bound when dealing with the rest
	triangle->vertexArray =
		(VertexArray*) malloc( sizeof( VertexArray ) );
	init( triangle->vertexArray );
	bind( triangle->vertexArray );


	// Initialize index buffer

	triangle->indexBuffer =
		(IndexBuffer*) malloc( sizeof( IndexBuffer ) );
	init(
		triangle->indexBuffer,
		sizeof( indices ),
		indices
	);


	// Initialize vertex buffer and vertex array
	// and upload the vertex information
	// and how the information should be interpreted

	VertexBuffer * vertexBuffer =
		(VertexBuffer*) malloc( sizeof( VertexBuffer ) );
	init(
		vertexBuffer,
		sizeof( vertices ),
		vertices
	);

	VertexBufferLayout * vertexBufferLayout =
		(VertexBufferLayout*) malloc( sizeof( VertexBufferLayout ) );
	init( vertexBufferLayout );
	// We have <positionDimensions> floats per vertex for position
	push( vertexBufferLayout, positionDimensions, GL_FLOAT );
	// <colorDimensions> more floats per vertex for the base color
	push( vertexBufferLayout, colorDimensions, GL_FLOAT );
	// and <textureDimensions> more floats per vertex for texturing
	push( vertexBufferLayout, textureDimensions, GL_FLOAT );

	///////
	push(
		triangle->vertexArray,
		vertexBuffer,
		vertexBufferLayout
	);


	// Initialize and bind the texture

	triangle->texture =
		(Texture*) malloc( sizeof( Texture ) );
	init( triangle->texture, "img/texture.jpeg" );

	bind( triangle->texture, 0 ); // texture bound to slot 0

	triangle->u_Texture = getUniformLocation( triangle->shader, "u_Texture" );
	setUniform1i( triangle->shader, triangle->u_Texture, 0 ); // so 0 here too
}


void draw( Triangle * triangle, Renderer * renderer ){

	draw(
		renderer,
		triangle->vertexArray,
		triangle->indexBuffer,
		triangle->shader
	);
}




//
// Scene
//



void initScene( Scene * scene, int screenWidth, int screenHeight, Shader * shader ){

    scene->frontPlane = 10;
    scene->backPlane = 100;
    scene->observerDistance = 4 * scene->frontPlane;
    scene->observerAngleX = scene->observerAngleY = 0;

	scene->axes = (Axes*) malloc( sizeof( Axes ) );
	init( scene->axes );

	scene->triangle = (Triangle*) malloc( sizeof( Triangle ) );
	init( scene->triangle, shader );

	scene->cameraAngleX = 0;
	scene->cameraAngleY = 0;
	scene->cursorSpeed = 0.01;

    // Color to clear the scene in every frame
	glClearColor( 0.2, 0.3, 0.4, 1.0 );

    // Enable z-buffer
	//glEnable( GL_DEPTH_TEST );

	//changeProjection( scene );
	scene->width = screenWidth / 10;
	scene->height = screenHeight / 10;
	glViewport( 0, 0, screenWidth, screenHeight );
}



void drawScene( Scene * scene, Renderer * renderer ){

	glClear( GL_COLOR_BUFFER_BIT );//| GL_DEPTH_BUFFER_BIT );
	//changeObserver( scene );

	drawObjects( scene, renderer );
}

void drawObjects( Scene * scene, Renderer * renderer ){

	draw( scene->axes );
	draw( scene->triangle, renderer );
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
// Input
//

void update( Scene * scene, GLFWwindow * window ){


	static double currentMouseX, currentMouseY;

	glfwGetCursorPos( window, &currentMouseX, &currentMouseY );


	// Keyboard input

	if ( glfwGetKey( window, GLFW_KEY_LEFT ) == GLFW_PRESS ){
		scene->cameraAngleY += scene->cursorSpeed;
	}

	if ( glfwGetKey( window, GLFW_KEY_RIGHT ) == GLFW_PRESS ){
		scene->cameraAngleY -= scene->cursorSpeed;
	}

	if ( glfwGetKey( window, GLFW_KEY_UP ) == GLFW_PRESS ){
		scene->cameraAngleX += scene->cursorSpeed;
	}

	if ( glfwGetKey( window, GLFW_KEY_DOWN ) == GLFW_PRESS ){
		scene->cameraAngleX -= scene->cursorSpeed;
	}


	// Mouse input

	if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS ){
		scene->cameraAngleY +=
			( scene->prevCursorX - currentMouseX ) * scene->cursorSpeed;
		scene->cameraAngleX +=
			( scene->prevCursorY - currentMouseY ) * scene->cursorSpeed;
	}


	scene->prevCursorX = currentMouseX;
	scene->prevCursorY = currentMouseY;
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
		if ( key == GLFW_KEY_LEFT )
			;//cameraAngleY -= 0.01;
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

	if ( glfwGetMouseButton( window, GLFW_MOUSE_BUTTON_LEFT ) == GLFW_PRESS )
		printf( "\t%.0f %.0f\n", xPos, yPos );
}


void mouseButtonCallback( GLFWwindow*window, int button, int action, int modifierBits ){

	static double xPos, yPos;

	if ( button == GLFW_MOUSE_BUTTON_LEFT ){

		if ( action == GLFW_PRESS ){
			glfwGetCursorPos( window, &xPos, &yPos );
			printf( "%.0f %.0f\n", xPos, yPos );
		}
		else if ( action == GLFW_RELEASE ){

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
