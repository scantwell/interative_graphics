// First new GL program
// Just makes a red triangle

#include "Angel.h"

const int NumPoints = 18;

//--------------------------------------------------------------------------

void
init( void )
{
    // Specifiy the vertices for a triangle
    vec2 vertices[18] = {
        vec2( -0.625, 0.375 ), vec2( -0.500, 0.625 ), vec2( -0.375, 0.375 ),
        vec2( 0.25, 0.75 ), vec2( 0.5, 0.25 ), vec2( 0.5, 0.75 ),
        vec2( 0.5, 0.75 ), vec2( 0.5, 0.25 ), vec2( 0.75, 0.25 ),
        vec2( 0.15, -0.45 ), vec2( 0.375, -0.9 ), vec2( 0.525, -0.15 ),
        vec2( 0.375, -0.9 ), vec2( 0.675, -0.9 ), vec2( 0.525, -0.15 ),
        vec2( 0.525, -0.15), vec2( 0.675, -0.9 ), vec2( 0.9, -0.45 )
    };

    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW );

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 2, GL_FLOAT, GL_FALSE, 0,
                           BUFFER_OFFSET(0) );

    glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    glDrawArrays( GL_TRIANGLES, 0, 3 );    // draw the points
    glDrawArrays( GL_TRIANGLES, 3, 6);
    glDrawArrays( GL_TRIANGLES, 9, 9);
    glFlush();
}

//----------------------------------------------------------------------------

void
keyboard( unsigned char key, int x, int y )
{
    switch ( key ) {
    case 033:
        exit( EXIT_SUCCESS );
        break;
    }
}

//----------------------------------------------------------------------------

int
main( int argc, char **argv )
{
    glutInit( &argc, argv );
    glutInitDisplayMode( GLUT_RGBA );
    glutInitWindowSize( 512, 512 );

    glutCreateWindow( "Red Triangle" );
    glewExperimental=GL_TRUE; 
    glewInit();    
    init();

    glutDisplayFunc( display );
    glutKeyboardFunc( keyboard );

    glutMainLoop();
    return 0;
}
