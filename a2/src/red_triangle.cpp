// First new GL program
// Just makes a red triangle

#include "Angel.h"
#include <iostream>
#include <vector>

const int NumPoints = 6283;
const double PI = 3.1415;


//--------------------------------------------------------------------------

void
create_circle(std::vector<vec3> &points, std::vector<vec3> &colors, vec3 center, double radius)
{
    int first = points.size();
    double x, y;

    for (float angle = 0; angle < (2.0f * PI); angle+=0.001)
    {
        x = radius * cos(angle);
        y = radius * sin(angle);
        x += center.x;
        y += center.y;

        points.push_back(vec3(x, y, 0));
        colors.push_back(vec3( 1.0 * atan(x/y), 0.0, 0.0));
    }
    points.push_back(points.at(first));
    colors.push_back(colors.at(first));

}

void create_elipse(std::vector<vec3> &points, std::vector<vec3> &colors, vec3 center, double radius, double xscale, double yscale)
{
    int first = points.size();

    for (float angle = 0; angle < (2.0f * PI); angle+=0.1)
    {
        points.push_back(vec3(center.x + (radius * cos(angle)) * xscale, center.y + (radius * sin(angle)) * yscale, 0));
        colors.push_back(vec3( 1.0, 0.0, 0.0));
    }
    points.push_back(points.at(first));
    colors.push_back(colors.at(first));
}

void
create_triangle(std::vector<vec3> &points, std::vector<vec3> &colors)
{
    points.push_back(vec3(-0.2, 0.4, 0));
    points.push_back(vec3(0.0, 1, 0));
    points.push_back(vec3(0.2, 0.4, 0));
    colors.push_back(vec3(0.0, 1.0, 0.0));
    colors.push_back(vec3(1.0, 0.0, 0.0));
    colors.push_back(vec3(0.0, 0.0, 1.0));
}

void
create_square(std::vector<vec3> &points, std::vector<vec3> &colors)
{
    vec3 color;
    vec3 bottom_left = vec3(-0.35, -0.7, 0);
    vec3 top_right = vec3(0.35, 0, 0);
    double factor = 0.05;

    for(int i = 0; i < 6; ++i)
    {
        color = vec3(1, 1, 1);
        if( i % 2 == 1)
        {
            color = vec3(0, 0, 0);
        }
        points.push_back(vec3(bottom_left.x, top_right.y, 0));
        points.push_back(bottom_left);
        points.push_back(top_right);
        points.push_back(vec3(top_right.x, bottom_left.y, 0));
        bottom_left = vec3(bottom_left.x + factor, bottom_left.y + factor, 0);
        top_right = vec3(top_right.x - factor, top_right.y - factor, 0);
        colors.push_back(color);
        colors.push_back(color);
        colors.push_back(color);
        colors.push_back(color);
    }
}

void
print_vector(std::vector<vec3> v)
{
    for(std::vector<vec3>::const_iterator i = v.begin(); i != v.end(); ++i)
       std::cout << *i << ' ' << std::endl;
    std::cout << "SIZE: " << v.size() << std::endl;
}


void
init( void )
{
    std::vector<vec3> vertices;
    std::vector<vec3> colors;


    // Functions add to the vertices/colors vectors
    create_circle(vertices, colors, vec3(0.5, 0.6, 0), 0.2);
    create_elipse(vertices, colors, vec3( -0.5, 0.6, 0), 0.1, 1, .6);
    create_triangle(vertices, colors);
    create_square(vertices, colors);

    //print_vector(vertices);
    //print_vector(colors);

    int v_size = vertices.size() * sizeof(vec3);
    int c_size = colors.size() * sizeof(vec3);

    // Create a vertex array object
    GLuint vao[1];
    glGenVertexArrays( 1, vao );
    glBindVertexArray( vao[0] );

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
    glDrawArrays( GL_TRIANGLE_FAN, 0, 6285);
    glDrawArrays( GL_TRIANGLE_FAN, 6285, 64);
    glDrawArrays( GL_TRIANGLES, 6349, 3);
    glDrawArrays( GL_TRIANGLE_STRIP, 6352, 4);
    glDrawArrays( GL_TRIANGLE_STRIP, 6356, 4);
    glDrawArrays( GL_TRIANGLE_STRIP, 6360, 4);
    glDrawArrays( GL_TRIANGLE_STRIP, 6364, 4);
    glDrawArrays( GL_TRIANGLE_STRIP, 6368, 4);
    glDrawArrays( GL_TRIANGLE_STRIP, 6372, 4);
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
