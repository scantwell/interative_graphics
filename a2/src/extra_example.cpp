// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <vector>

const double PI = 3.1415;
const int NumPoints = 18;

//--------------------------------------------------------------------------

void
create_circle(std::vector<vec3> &points, std::vector<vec3> &colors, vec3 center, double radius, vec3 color)
{
    int first = points.size();
    double x, y;

    for (float angle = 0; angle < (2.0f * PI); angle+=0.1)
    {
        x = radius * cos(angle);
        y = radius * sin(angle);
        x += center.x;
        y += center.y;

        points.push_back(vec3(x, y, 0));
        colors.push_back(color);
    }
    points.push_back(points.at(first));
    colors.push_back(colors.at(first));

}

void
init( void )
{
    std::vector<vec3> vertices;
    std::vector<vec3> colors;

    //body
    create_circle(vertices, colors, vec3(0, -.5, 0), .35, vec3( 1, 1, 1));
    create_circle(vertices, colors, vec3(0, 0, 0), .25, vec3( 1, 1, 1));
    create_circle(vertices, colors, vec3(0, .4, 0), .2, vec3( 1, 1, 1));
    // eyes
    create_circle(vertices, colors, vec3(-0.075, 0.45, 0), .025, vec3( 0, 0, 0));
    create_circle(vertices, colors, vec3(0.075, 0.45, 0), .025, vec3( 0, 0, 0));
    // nose
    create_circle(vertices, colors, vec3(0, 0.4, 0), .015, vec3( 0, 0, 0));

    // smile
    create_circle(vertices, colors, vec3(-0.08, 0.325, 0), .015, vec3( 0, 0, 0));
    create_circle(vertices, colors, vec3(-0.025, 0.3, 0), .015, vec3( 0, 0, 0));
    create_circle(vertices, colors, vec3(0.025, 0.3, 0), .015, vec3( 0, 0, 0));
    create_circle(vertices, colors, vec3(0.08, 0.325, 0), .015, vec3( 0, 0, 0));
    // arms
    vertices.push_back(vec3(-0.65, .2, 0));
    vertices.push_back(vec3(0, 0, 0));
    vertices.push_back(vec3(0.55, .2, 0));
    colors.push_back(vec3(1,1,1));
    colors.push_back(vec3(1,1,1));
    colors.push_back(vec3(1,1,1));

    int v_size = vertices.size() * sizeof(vec3);
    int c_size = colors.size() * sizeof(vec3);

    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );

    glEnable(GL_LINE_SMOOTH);
    glLineWidth(10);

    // Create and initialize a buffer object
    GLuint buffer;
    glGenBuffers( 1, &buffer );
    glBindBuffer( GL_ARRAY_BUFFER, buffer );
    glBufferData( GL_ARRAY_BUFFER, v_size + c_size, NULL, GL_STATIC_DRAW );
    glBufferSubData( GL_ARRAY_BUFFER, 0, v_size, &vertices.front());
    glBufferSubData( GL_ARRAY_BUFFER, v_size, c_size, &colors.front());

    // Load shaders and use the resulting shader program
    GLuint program = InitShader( "vshader21.glsl", "fshader21.glsl" );
    glUseProgram( program );

    // Initialize the vertex position attribute from the vertex shader
    GLuint loc = glGetAttribLocation( program, "vPosition" );
    glEnableVertexAttribArray( loc );
    glVertexAttribPointer( loc, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

    GLuint loc_colors = glGetAttribLocation( program, "vColor");
    glEnableVertexAttribArray( loc_colors);
    glVertexAttribPointer( loc_colors, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(v_size));

    glClearColor( 0, 0, 0, 1.0 );
}

//----------------------------------------------------------------------------

void
display( void )
{
    glClear( GL_COLOR_BUFFER_BIT );     // clear the window
    int offset = 0;

    for( int i = 0; i < 10; ++i)
    {
        offset = 64 * i;
        glDrawArrays( GL_TRIANGLE_FAN, offset, 64);
    }
    offset += 64;
    glDrawArrays( GL_LINE_STRIP, offset, 2);
    glDrawArrays( GL_LINE_STRIP, offset + 1, 2);
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
