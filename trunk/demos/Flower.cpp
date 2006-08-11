// Flower Demo
// Copyright © Glenn Fiedler
// http://www.pixeltoaster.com

#include <vector>
#include <math.h>
#include <assert.h>
#include <PixelToaster.h>

using namespace PixelToaster;


// flower parameters

const int width = 300;
const int height = 300;


// a simple vector class

struct Vector
{
	float x,y,z;

	Vector() {}

	Vector( float x, float y, float z )
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}
};

// and a simple matrix class

struct Matrix
{
	Matrix() {}

	static Matrix rotate_x( float radians )
	{
        const float c = cos( radians );
        const float s = sin( radians );
        
		Matrix matrix;

		matrix.column[0].x = 1;
        matrix.column[0].y = 0;
        matrix.column[0].z = 0;
        
        matrix.column[1].x = 0;
        matrix.column[1].y = c;
        matrix.column[1].z = -s;
        
        matrix.column[2].x = 0;
        matrix.column[2].y = s;
        matrix.column[2].z = c;

		return matrix;
	}

	static Matrix rotate_y( float radians )
	{
        const float c = cos( radians );
        const float s = sin( radians );
        
		Matrix matrix;
	
		matrix.column[0].x = c;
        matrix.column[0].y = 0;
        matrix.column[0].z = s;
        
        matrix.column[1].x = 0;
        matrix.column[1].y = 1;
        matrix.column[1].z = 0;
        
        matrix.column[2].x = -s;
        matrix.column[2].y = 0;
        matrix.column[2].z = c;

		return matrix;
	}

	static Matrix rotate_z( float radians )
	{
        const float c = cos( radians );
        const float s = sin( radians );
        
		Matrix matrix;
	
		matrix.column[0].x = c;
        matrix.column[0].y = -s;
        matrix.column[0].z = 0;
        
        matrix.column[1].x = s;
        matrix.column[1].y = c;
        matrix.column[1].z = 0;
        
        matrix.column[2].x = 0;
        matrix.column[2].y = 0;
        matrix.column[2].z = 1;

		return matrix;
	}

	static Matrix scale( float s )
	{
		Matrix matrix;
	
		matrix.column[0].x = s;
        matrix.column[0].y = 0;
        matrix.column[0].z = 0;
        
        matrix.column[1].x = 0;
        matrix.column[1].y = s;
        matrix.column[1].z = 0;
        
        matrix.column[2].x = 0;
        matrix.column[2].y = 0;
        matrix.column[2].z = s;

		return matrix;
	}

	Matrix operator * ( const Matrix & other )
	{
		Matrix matrix;

        matrix.column[0].x = column[0].x*other.column[0].x + column[1].x*other.column[0].y + column[2].x*other.column[0].z;
        matrix.column[1].x = column[0].x*other.column[1].x + column[1].x*other.column[1].y + column[2].x*other.column[1].z;
        matrix.column[2].x = column[0].x*other.column[2].x + column[1].x*other.column[2].y + column[2].x*other.column[2].z;

		matrix.column[0].y = column[0].y*other.column[0].x + column[1].y*other.column[0].y + column[2].y*other.column[0].z;
        matrix.column[1].y = column[0].y*other.column[1].x + column[1].y*other.column[1].y + column[2].y*other.column[1].z;
		matrix.column[2].y = column[0].y*other.column[2].x + column[1].y*other.column[2].y + column[2].y*other.column[2].z;

		matrix.column[0].z = column[0].z*other.column[0].x + column[1].z*other.column[0].y + column[2].z*other.column[0].z;
        matrix.column[1].z = column[0].z*other.column[1].x + column[1].z*other.column[1].y + column[2].z*other.column[1].z;
        matrix.column[2].z = column[0].z*other.column[2].x + column[1].z*other.column[2].y + column[2].z*other.column[2].z;

		return matrix;
	}

	void transform( Vector & vector ) const
	{
		const float x = column[0].x * vector.x + column[0].y * vector.y + column[0].z * vector.z;
		const float y = column[1].x * vector.x + column[1].y * vector.y + column[1].z * vector.z;
		const float z = column[2].x * vector.x + column[2].y * vector.y + column[2].z * vector.z;
		vector.x = x;
		vector.y = y;
		vector.z = z;
	}

	Vector column[3];
};

// rendering routines

void project( Vector & point )
{
	const float sx = width * 0.25f;
	const float sy = height * 0.25f;

	const float cx = width * 0.5f;
	const float cy = height * 0.5f;

	float d = ( point.z + 2.0f ) * 1.0f;
	
	if ( d < 1.0f )
		d = 1.0f;

	const float ood = 1.0f / d;

	point.x = point.x * sx * ood + cx;
	point.y = point.y * sy * ood + cy;
}

void line( std::vector<Pixel> & pixels, const Vector & s, const Vector & t )
{
	float x1 = s.x;
	float y1 = s.y;

	float x2 = t.x;
	float y2 = t.y;

	assert( x1 >= 0.0f && x1 <= width - 1 );
	assert( x2 >= 0.0f && x2 <= width - 1 );
	assert( y1 >= 0.0f && y1 <= height - 1 );
	assert( y1 >= 0.0f && y1 <= height - 1 );

	const float i = 1.0f;
	const float r = i * 0.002f;
	const float g = i * 0.015f;
	const float b = i * 0.1f;

    const float abs_dx = fabs( x2 - x1 );
    const float abs_dy = fabs( y2 - y1 );

    if ( abs_dx==0 && abs_dy==0 ) return;

    if ( abs_dx > abs_dy )
    {
		if ( x1 > x2 )
		{
			float tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}

        int x = (int) x1;
        int y = (int) (y1 * 65535.0f);

		const int end = (int) x2;

		const float dx = x2 - x1;
		const float dy = y2 - y1;

        const int dydx = (int) (dy / dx * 65535.0f);

        while ( x < end )
        {
            const int offset = (y>>16) * width + x;

            pixels[offset].r -= r;
            pixels[offset].g -= g;
            pixels[offset].b -= b;

            y += dydx;
            x ++;
        }
    }
	else
	{
		if (y1>y2)
		{
			float tmp = x1;
			x1 = x2;
			x2 = tmp;
			tmp = y1;
			y1 = y2;
			y2 = tmp;
		}

        int x = (int) (x1 * 65535.0f);
        int y = (int) y1;

		const int end = (int) y2;

		const float dx = x2 - x1;
		const float dy = y2 - y1;

        const int dxdy = (int) (dx / dy * 65535.0f);

		int base = y * width;

        while ( y < end )
        {
            const int offset = base + (x>>16);

            pixels[offset].r -= r;
            pixels[offset].g -= g;
            pixels[offset].b -= b;

            x += dxdy;
            y ++;

			base += width;
        }
	}
}

void cube( std::vector<Pixel> & pixels, const Matrix & matrix )
{
	// setup cube vertices in local space

    Vector vertex[8];
    
	vertex[0] = Vector( -1.0f, -1.0f, -1.0f );
    vertex[1] = Vector( +1.0f, -1.0f, -1.0f );
    vertex[2] = Vector( +1.0f, +1.0f, -1.0f );
    vertex[3] = Vector( -1.0f, +1.0f, -1.0f );
    vertex[4] = Vector( -1.0f, -1.0f, +1.0f );
    vertex[5] = Vector( +1.0f, -1.0f, +1.0f );
    vertex[6] = Vector( +1.0f, +1.0f, +1.0f );
    vertex[7] = Vector( -1.0f, +1.0f, +1.0f );

	// transform vertices by matrix

	matrix.transform( vertex[0] );
	matrix.transform( vertex[1] );
	matrix.transform( vertex[2] );
	matrix.transform( vertex[3] );
	matrix.transform( vertex[4] );
	matrix.transform( vertex[5] );
	matrix.transform( vertex[6] );
	matrix.transform( vertex[7] );

	// project vertices to screen space

	project( vertex[0] );
	project( vertex[1] );
	project( vertex[2] );
	project( vertex[3] );
	project( vertex[4] );
	project( vertex[5] );
	project( vertex[6] );
	project( vertex[7] );

	// render cube

	line( pixels, vertex[0], vertex[1] );
	line( pixels, vertex[1], vertex[2] );
	line( pixels, vertex[2], vertex[3] );
	line( pixels, vertex[3], vertex[0] );

	line( pixels, vertex[4], vertex[5] );
	line( pixels, vertex[5], vertex[6] );
	line( pixels, vertex[6], vertex[7] );
	line( pixels, vertex[7], vertex[4] );

	line( pixels, vertex[0], vertex[4] );
	line( pixels, vertex[1], vertex[5] );
	line( pixels, vertex[2], vertex[6] );
	line( pixels, vertex[3], vertex[7] );
}

void clear( std::vector<Pixel> & pixels )
{
	for ( unsigned int index = 0; index < pixels.size(); ++index )
	{
		pixels[index].r = 1.0f;
		pixels[index].g = 1.0f;
		pixels[index].b = 1.0f;
	}
}


class Application : public Listener
{
public:

	Application()
	{
		quit = false;
		x_rotation = 0.0f;
		y_rotation = 0.0f;
		x_rotation_target = 0.0f;
		y_rotation_target = 0.0f;
		dragging = false;
	}

    void run()
    {
        Display display( "Flower", width, height );

		if ( !display.open() )
			return;

		display.listener( this );

        std::vector<Pixel> pixels( width * height );

        const int shells = 300;
        const float inside = 0.5f;
        const float outside = 1.0f;
        const float step = (outside - inside) / shells;
        const float lag = 0.02f;

		Timer timer;

		while ( !quit )
        {
            const float time = (float) timer.time() * 0.35f;

			x_rotation += ( x_rotation_target - x_rotation ) * 0.1f;
			y_rotation += ( y_rotation_target - y_rotation ) * 0.1f;

			float t = time;
            float s = inside;

			clear( pixels );

            for ( int i = 0; i < shells; ++i )
            {
                const float rx = t * 0.9f;
                const float ry = t * 1.1f;
                const float rz = t * 1.2f;

				const float scale = 1.0f;

				Matrix matrix = Matrix::rotate_z(rz) * Matrix::rotate_y(ry-x_rotation) * Matrix::rotate_x(rx+y_rotation) * Matrix::scale(s*scale);
		
                cube( pixels, matrix );

			    t -= lag;
                s += step;
            }

            display.update( pixels );
        }
    }

protected:

    void onMouseButtonDown( Mouse mouse )
	{
		if ( !dragging )
		{
			old = mouse;
			dragging = true;
		}
	}

    void onMouseMove( Mouse mouse )
	{
		if ( dragging )
		{
			const float dx = mouse.x - old.x;
			const float dy = mouse.y - old.y;

			x_rotation_target += dx * 0.005f;
			y_rotation_target += dy * 0.005f;

			old = mouse;
		}
	}

    void onMouseButtonUp( Mouse mouse )
	{
		if ( dragging )
			dragging = false;
	}

	void onKeyDown( Key key )
	{
		if ( key == Key::Escape )
			quit = true;
	}

	void onClose()
	{
		quit = true;
	}

private:

	bool quit;
	float x_rotation;
	float y_rotation;
	float x_rotation_target;
	float y_rotation_target;
	bool dragging;
	Mouse old;
};


int main()
{
    Application application;
    application.run();
}
