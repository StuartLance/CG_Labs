#include <string>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "GL/glew.h"
#include "../extra/picopng.h"
#include "image.h"
#include "utils.h"
#include "camera.h"
#include "mesh.h"
#include "entity.h"

Image::Image() {
	width = 0; height = 0;
	pixels = NULL;
}

Image::Image(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new Color[width*height];
	memset(pixels, 0, width * height * sizeof(Color));
}

// Copy constructor
Image::Image(const Image& c)
{
	pixels = NULL;
	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;
	if(c.pixels)
	{
		pixels = new Color[width*height];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
}

// Assign operator
Image& Image::operator = (const Image& c)
{
	if(pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	bytes_per_pixel = c.bytes_per_pixel;

	if(c.pixels)
	{
		pixels = new Color[width*height*bytes_per_pixel];
		memcpy(pixels, c.pixels, width*height*bytes_per_pixel);
	}
	return *this;
}

Image::~Image()
{
	if(pixels) 
		delete pixels;
}

void Image::Render()
{
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
	glDrawPixels(width, height, bytes_per_pixel == 3 ? GL_RGB : GL_RGBA, GL_UNSIGNED_BYTE, pixels);
}

// Change image size (the old one will remain in the top-left corner)
void Image::Resize(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for(unsigned int x = 0; x < min_width; ++x)
		for(unsigned int y = 0; y < min_height; ++y)
			new_pixels[ y * width + x ] = GetPixel(x,y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

// Change image size and scale the content
void Image::Scale(unsigned int width, unsigned int height)
{
	Color* new_pixels = new Color[width*height];

	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++y)
			new_pixels[ y * width + x ] = GetPixel((unsigned int)(this->width * (x / (float)width)), (unsigned int)(this->height * (y / (float)height)) );

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}

Image Image::GetArea(unsigned int start_x, unsigned int start_y, unsigned int width, unsigned int height)
{
	Image result(width, height);
	for(unsigned int x = 0; x < width; ++x)
		for(unsigned int y = 0; y < height; ++x)
		{
			if( (x + start_x) < this->width && (y + start_y) < this->height) 
				result.SetPixel( x, y, GetPixel(x + start_x,y + start_y) );
		}
	return result;
}

void Image::FlipY()
{
	int row_size = bytes_per_pixel * width;
	Uint8* temp_row = new Uint8[row_size];
#pragma omp simd
	for (int y = 0; y < height * 0.5; y += 1)
	{
		Uint8* pos = (Uint8*)pixels + y * row_size;
		memcpy(temp_row, pos, row_size);
		Uint8* pos2 = (Uint8*)pixels + (height - y - 1) * row_size;
		memcpy(pos, pos2, row_size);
		memcpy(pos2, temp_row, row_size);
	}
	delete[] temp_row;
}

bool Image::LoadPNG(const char* filename, bool flip_y)
{
	std::string sfullPath = absResPath(filename);
	std::ifstream file(sfullPath, std::ios::in | std::ios::binary | std::ios::ate);

	// Get filesize
	std::streamsize size = 0;
	if (file.seekg(0, std::ios::end).good()) size = file.tellg();
	if (file.seekg(0, std::ios::beg).good()) size -= file.tellg();

	if (!size)
		return false;

	std::vector<unsigned char> buffer;

	// Read contents of the file into the vector
	if (size > 0)
	{
		buffer.resize((size_t)size);
		file.read((char*)(&buffer[0]), size);
	}
	else
		buffer.clear();

	std::vector<unsigned char> out_image;

	if (decodePNG(out_image, width, height, buffer.empty() ? 0 : &buffer[0], (unsigned long)buffer.size(), true) != 0)
		return false;

	size_t bufferSize = out_image.size();
	unsigned int originalBytesPerPixel = (unsigned int)bufferSize / (width * height);
	
	// Force 3 channels
	bytes_per_pixel = 3;

	if (originalBytesPerPixel == 3) {
		pixels = new Color[bufferSize];
		memcpy(pixels, &out_image[0], bufferSize);
	}
	else if (originalBytesPerPixel == 4) {

		unsigned int newBufferSize = width * height * bytes_per_pixel;
		pixels = new Color[newBufferSize];

		unsigned int k = 0;
		for (unsigned int i = 0; i < bufferSize; i += originalBytesPerPixel) {
			pixels[k] = Color(out_image[i], out_image[i + 1], out_image[i + 2]);
			k++;
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	return true;
}

// Loads an image from a TGA file
bool Image::LoadTGA(const char* filename, bool flip_y)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	unsigned char TGAcompare[12];
	unsigned char header[6];
	unsigned int imageSize;
	unsigned int bytesPerPixel;

    std::string sfullPath = absResPath( filename );

	FILE * file = fopen( sfullPath.c_str(), "rb");
   	if ( file == NULL || fread(TGAcompare, 1, sizeof(TGAcompare), file) != sizeof(TGAcompare) ||
		memcmp(TGAheader, TGAcompare, sizeof(TGAheader)) != 0 ||
		fread(header, 1, sizeof(header), file) != sizeof(header))
	{
		std::cerr << "File not found: " << sfullPath.c_str() << std::endl;
		if (file == NULL)
			return NULL;
		else
		{
			fclose(file);
			return NULL;
		}
	}

	TGAInfo* tgainfo = new TGAInfo;
    
	tgainfo->width = header[1] * 256 + header[0];
	tgainfo->height = header[3] * 256 + header[2];
    
	if (tgainfo->width <= 0 || tgainfo->height <= 0 || (header[4] != 24 && header[4] != 32))
	{
		fclose(file);
		delete tgainfo;
		return NULL;
	}
    
	tgainfo->bpp = header[4];
	bytesPerPixel = tgainfo->bpp / 8;
	imageSize = tgainfo->width * tgainfo->height * bytesPerPixel;
    
	tgainfo->data = new unsigned char[imageSize];
    
	if (tgainfo->data == NULL || fread(tgainfo->data, 1, imageSize, file) != imageSize)
	{
		if (tgainfo->data != NULL)
			delete tgainfo->data;
            
		fclose(file);
		delete tgainfo;
		return false;
	}

	fclose(file);

	// Save info in image
	if(pixels)
		delete pixels;

	width = tgainfo->width;
	height = tgainfo->height;
	pixels = new Color[width*height];

	// Convert to float all pixels
	for (unsigned int y = 0; y < height; ++y) {
		for (unsigned int x = 0; x < width; ++x) {
			unsigned int pos = y * width * bytesPerPixel + x * bytesPerPixel;
			// Make sure we don't access out of memory
			if( (pos < imageSize) && (pos + 1 < imageSize) && (pos + 2 < imageSize))
				SetPixel(x, height - y - 1, Color(tgainfo->data[pos + 2], tgainfo->data[pos + 1], tgainfo->data[pos]));
		}
	}

	// Flip pixels in Y
	if (flip_y)
		FlipY();

	delete tgainfo->data;
	delete tgainfo;

	return true;
}

// Saves the image to a TGA file
bool Image::SaveTGA(const char* filename)
{
	unsigned char TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};

	std::string fullPath = absResPath(filename);
	FILE *file = fopen(fullPath.c_str(), "wb");
	if ( file == NULL )
	{
		perror("Failed to open file: ");
		return false;
	}

	unsigned short header_short[3];
	header_short[0] = width;
	header_short[1] = height;
	unsigned char* header = (unsigned char*)header_short;
	header[4] = 24;
	header[5] = 0;

	fwrite(TGAheader, 1, sizeof(TGAheader), file);
	fwrite(header, 1, 6, file);

	// Convert pixels to unsigned char
	unsigned char* bytes = new unsigned char[width*height*3];
	for(unsigned int y = 0; y < height; ++y)
		for(unsigned int x = 0; x < width; ++x)
		{
			Color c = pixels[y*width+x];
			unsigned int pos = (y*width+x)*3;
			bytes[pos+2] = c.r;
			bytes[pos+1] = c.g;
			bytes[pos] = c.b;
		}

	fwrite(bytes, 1, width*height*3, file);
	fclose(file);

	return true;
}

// DRAW FUNCTIONS


void Image::DrawLineDDA(int x0, int y0, int x1, int y1, const Color& c)
{
	float x, y, dx, dy, step;

	x  = x0;	   y = y0;
	dx = x1 - x0; dy = y1 - y0;

	if (abs(dx) >= abs(dy)) step = abs(dx);
	else step = abs(dy);

	dx = dx / step;
	dy = dy / step;
		
	int i;
	for (i = 0; i < step; i++) {
		SetPixelSafe(x, y, c);
		x += dx;
		y += dy;
	}
}

//Fill function below with updated Rectangle drawing with specified border widths
//DON'T USE DDA AS FOR LINES


void Image::DrawRect(int x, int y, int w, int h, const Color& c)
{
	for (int i = 0; i < w; ++i) {
		SetPixel(x + i, y, c);
		SetPixel(x + i, y + h - 1, c);
	}

	for (int j = 0; j < h; ++j) {
		SetPixel(x, y + j, c);
		SetPixel(x + w - 1, y + j, c);
	}
}
void Image::DrawRect(int x, int y, int w, int h, const Color& borderColor,
	int borderWidth, bool isFilled, const Color& fillColor) {
	int i, j, b;
	for (i = x; i < x + w; i++) {
		for (b = 0; b < borderWidth; b++) {
			SetPixel(i, y + b, borderColor);
			SetPixel(i, y + (h-1) - b, borderColor);
		}
	}

	for (j = y + borderWidth; j < y + h - borderWidth; j++) {
		for (b = 0; b < borderWidth; b++) {
			SetPixel(x + b, j, borderColor);
			SetPixel(x + (w-1) - b, j, borderColor);
		}
	}
	if (isFilled == false) return;
	else {
		for (i = x + borderWidth; i < x + w - borderWidth; i++) {
			for (j = y + borderWidth; j < y + h - borderWidth; j++) {
				SetPixel(i, j, fillColor);
			}
		}
	}

}




void Image::ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<Cell>& table) {
	int step;
	// Orients so x0 is smallest
	if (x0 > x1) {
		std::swap(x0, x1);
		std::swap(y0, y1);
	}

	// Difference in x and y
	float dx = x1 - x0;
	float dy = y1 - y0;

	// take largestt dif and that is how many steps needed
	if (abs(dx) >= abs(dy)) step = abs(dx);
	else step = abs(dy);

	// Increment per step
	float xInc = dx / step;
	float yInc = dy / step;

	// Initialise x and y to the starting point
	float x = x0;
	float y = y0;

	// For every step
	for (int i = 0; i <= step; i++) {

		// 
		if (x >= 0 && x < table.size() && y >= 0 && y < table.size()) {
			if (x < table[static_cast<int>(y)].xMin) {
				table[static_cast<int>(y)].xMin = static_cast<int>(x);
			}
			if (x > table[static_cast<int>(y)].xMax) {
				table[static_cast<int>(y)].xMax = static_cast<int>(x);
			}
		}
		x += xInc;
		y += yInc;
	}
	
}


void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor)
{
    // Create the Active Edges Table (AET)
    std::vector<Cell> aet(width);

    // Sort the vertices by y-coordinate
    Vector2 vertices[3] = { p0, p1, p2 };
    std::sort(vertices, vertices + 3, [](const Vector2& a, const Vector2& b) { return a.y < b.y; });

    // Initialize the AET with the edges of the triangle
    for (int i = 0; i < 3; ++i)
    {
        int nextIndex = (i + 1) % 3;
        int y0 = static_cast<int>(vertices[i].y);
        int y1 = static_cast<int>(vertices[nextIndex].y);

        if (y0 != y1) // Avoid horizontal edges
        {
            int x0 = static_cast<int>(vertices[i].x);
            int x1 = static_cast<int>(vertices[nextIndex].x);

            // Use DDA to update the AET for each edge
            ScanLineDDA(x0, y0, x1, y1, aet);
        }
    }

    // Fill the triangle
    if (isFilled)
    {
        for (int y = static_cast<int>(vertices[0].y); y <= static_cast<int>(vertices[2].y); ++y)
        {
            for (int x = aet[y].minx; x <= aet[y].maxx; ++x)
            {
                SetPixel(x, y, fillColor);
            }
        }
    }

    // Draw the border of the triangle
    DrawLineDDA(p0.x, p0.y, p1.x, p1.y, borderColor);
	DrawLineDDA(p1.x, p1.y, p2.x, p2.y, borderColor);
	DrawLineDDA(p2.x, p2.y, p0.x, p0.y, borderColor);
}


void Image::FillCircle(int x, int y, int rOuter, int rInner, const Color& fillColor) {

	int dx, dy;
	// Iterates over whole circle range
	for (int pixelY = y - rOuter + 1; pixelY < y + rOuter; ++pixelY) {
		for (int pixelX = x - rOuter + 1; pixelX < x + rOuter; ++pixelX) {


			dx = pixelX - x;
			dy = pixelY - y;
			// Check if the pixel is inside the circle, using equation of a circle
			// also check if pixel is outside the smaller circle
			if ((dx * dx + dy * dy <= rOuter * rOuter)&&(dx * dx + dy * dy >= rInner * rInner)){
				// Fill the pixel with the specified color
				SetPixelSafe(pixelX, pixelY, fillColor);
			}
		}
	}
}


void Image::DrawCircle(int x, int y, int r, const Color& borderColor,
	int borderWidth, bool isFilled, const Color& fillColor) {

	float p;

	int a = 0;
	int b = r;

	// Inital 4 confining points
	SetPixelSafe(x, y + r, borderColor);
	SetPixelSafe(x, y - r, borderColor);
	SetPixelSafe(x - r, y, borderColor);
	SetPixelSafe(x + r, y, borderColor);

	// Initial value of midpoint algo
	p = (5 / 4) - r;

	// Keep the loop short by only doing 1/8th of the circle i.e. a<=b implies below line a = b
	while (a <= b){

		if (p < 0) {
			p += (2 * a) + 3;
		}
		else {
			p += (2 * (a - b)) + 5;
			b--;
		}
		a++;

		// Split into octants

		//Paired by flipping in y axis
		SetPixelSafe(x + a, y + b, borderColor);
		SetPixelSafe(x + a, y - b, borderColor);
		
		SetPixelSafe(x - a, y + b, borderColor);
		SetPixelSafe(x - a, y - b, borderColor);

		SetPixelSafe(x + b, y - a, borderColor);
		SetPixelSafe(x + b, y + a, borderColor);

		SetPixelSafe(x - b, y - a, borderColor);
		SetPixelSafe(x - b, y + a, borderColor);		
	}

	//Fills circle from radius to border with border colour
	FillCircle(x, y, r, r - borderWidth, borderColor);
	

	//If filled, then fill circle from border to center with fill colour
	if (isFilled) {
		FillCircle(x, y, r - borderWidth, 0, fillColor);
	}
	
	
}


/*
void Image::InitializeParticles() {
	for (int i = 0; i < MAX_PARTICLES; ++i) {
		particles[i].position = Vector2{ static_cast<float>(rand() % width), static_cast<float>(rand() % height) };
		particles[i].velocity = Vector2{ 0.0f, static_cast<float>(rand() % 50) / 100.0f + 0.5f };
		particles[i].color = Color{ rand() % 256, rand() % 256, rand() % 256 };
		particles[i].acceleration = 0.0f;
		particles[i].ttl = static_cast<float>(rand() % 100) / 50.0f;
		particles[i].inactive = false;
	}
}

void Image::RenderParticles(Image* framebuffer) {
	for (int i = 0; i < MAX_PARTICLES; ++i) {
		if (!particles[i].inactive) {
			RenderParticle(framebuffer, particles[i]);
		}
	}
}
void Image::UpdateParticles(float dt) {
	for (int i = 0; i < MAX_PARTICLES; ++i) {
		if (!particles[i].inactive) {
			particles[i].position.y -= particles[i].velocity.y * dt * 100;
			particles[i].ttl -= dt;

			if (particles[i].position.y < 0.0f) {
				ResetParticle(particles[i]);
			}
		}
	}
}

void Image::RenderParticle(Image* framebuffer, const Particle& particle) {
	int x = static_cast<int>(particle.position.x);
	int y = static_cast<int>(particle.position.y);

	for (int dx = -1; dx <= 1; ++dx) {
		for (int dy = -1; dy <= 1; ++dy) {
			int px = x + dx;
			int py = y + dy;

			if (IsWithinBounds(px, py)) {
				framebuffer->SetPixel(px, py, particle.color);
			}
		}
	}
}

void Image::ResetParticle(Particle& particle) {
	particle.position.y = height;
	particle.ttl = static_cast<float>(rand() % 100) / 50.0f;
}

bool Image::IsWithinBounds(int x, int y) const {
	return x >= 0 && x < width && y >= 0 && y < height;
}
*/

#ifndef IGNORE_LAMBDAS

// You can apply and algorithm for two images and store the result in the first one
// ForEachPixel( img, img2, [](Color a, Color b) { return a + b; } );
template <typename F>
void ForEachPixel(Image& img, const Image& img2, F f) {
	for(unsigned int pos = 0; pos < img.width * img.height; ++pos)
		img.pixels[pos] = f( img.pixels[pos], img2.pixels[pos] );
}

#endif

FloatImage::FloatImage(unsigned int width, unsigned int height)
{
	this->width = width;
	this->height = height;
	pixels = new float[width * height];
	memset(pixels, 0, width * height * sizeof(float));
}

// Copy constructor
FloatImage::FloatImage(const FloatImage& c) {
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
}

// Assign operator
FloatImage& FloatImage::operator = (const FloatImage& c)
{
	if (pixels) delete pixels;
	pixels = NULL;

	width = c.width;
	height = c.height;
	if (c.pixels)
	{
		pixels = new float[width * height * sizeof(float)];
		memcpy(pixels, c.pixels, width * height * sizeof(float));
	}
	return *this;
}

FloatImage::~FloatImage()
{
	if (pixels)
		delete pixels;
}

// Change image size (the old one will remain in the top-left corner)
void FloatImage::Resize(unsigned int width, unsigned int height)
{
	float* new_pixels = new float[width * height];
	unsigned int min_width = this->width > width ? width : this->width;
	unsigned int min_height = this->height > height ? height : this->height;

	for (unsigned int x = 0; x < min_width; ++x)
		for (unsigned int y = 0; y < min_height; ++y)
			new_pixels[y * width + x] = GetPixel(x, y);

	delete pixels;
	this->width = width;
	this->height = height;
	pixels = new_pixels;
}
