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

// Scanline algorithm. Given by Pablo Luis. Review and understand it.

void Image::ScanLineDDA(int x0, int y0, int x1, int y1, std::vector<Cell>& table) {

	float dx = x1 - x0;
	float dy = y1 - y0;

	float d = std::max(abs(dx), abs(dy));
	Vector2 v = Vector2(dx / d, dy / d);
	float x = x0, y = y0;

	for (float i = 0; i <= d; i++) {
		//Update the table only if the calculated y coordinates are within the range of the image
		if (y >= 0 && y < table.size()) {
			table[floor(y)].xMin = std::min((int)floor(x), table[floor(y)].xMin);
			table[floor(y)].xMax = std::max((int)floor(x), table[floor(y)].xMax);
		}
		x += v.x;
		y += v.y;
	}
}

void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor) {

	if (isFilled) {
		//Create table
		std::vector<Cell> table(height);
		//Update table with the min and max x values of the triangle
		ScanLineDDA(p0.x, p0.y, p1.x, p1.y, table);
		ScanLineDDA(p1.x, p1.y, p2.x, p2.y, table);
		ScanLineDDA(p0.x, p0.y, p2.x, p2.y, table);
		//Paint the triangle
		for (int i = 0; i < table.size(); i++) {
			//Paint each row of the triangle from minx to maxx (included)
			for (int j = table[i].xMin; j <= table[i].xMax; j++) {
				SetPixelSafe(j, i, fillColor);
			}
		}
	}

	DrawLineDDA(p0.x, p0.y, p1.x, p1.y, borderColor);
	DrawLineDDA(p0.x, p0.y, p2.x, p2.y, borderColor);
	DrawLineDDA(p1.x, p1.y, p2.x, p2.y, borderColor);
}

/*The objective now is to draw the mesh interpolating 3 colors; assigned for each vertex of the triangle. 
To do that, create another method in the Image class that draws a filled triangle but using barycentric interpolation between the colors of each vertex 
(from now we are passing a Vector3 for each vertex, since we will use the Z value of the projected vertex soon)

void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2);

	Z Buffer or Depth Buffer method in C++
		-
	https://www.tutorialspoint.com/z-buffer-or-depth-buffer-method-in-cplusplus
*/

void Image::DrawTriangleInterpolated(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Color& c0, const Color& c1, const Color& c2, FloatImage* zbuffer) {
	//Create table
	std::vector<Cell> table(height);
	//Update table with the min and max x values of the triangle
	ScanLineDDA(p0.x, p0.y, p1.x, p1.y, table);
	ScanLineDDA(p1.x, p1.y, p2.x, p2.y, table);
	ScanLineDDA(p0.x, p0.y, p2.x, p2.y, table);

	//m transforms the barycentric coordinates to the screen space
	Matrix44 m;
	// USE ROW MAJOR, NOT COLUMN MAJOR!!!!
	m.M[0][0] = p0.x; m.M[1][0] = p1.x; m.M[2][0] = p2.x;
	m.M[0][1] = p0.y; m.M[1][1] = p1.y; m.M[2][1] = p2.y;
	m.M[0][2] = 1; m.M[1][2] = 1; m.M[2][2] = 1;

	// Inverse gives the barycentric coordinates
	m.Inverse();

	Vector3 bCoords;
	Color c;

	//Paint the triangle
	for (int i = 0; i < table.size(); i++) {
		if (table[i].xMin <= table[i].xMax) {
			//Paint each row of the triangle from xMIn to xMax (included)
			for (int j = table[i].xMin; j <= table[i].xMax; j++) {
				//Calculate the barycentric coordinates
				// REMEMBER i is the y coordinate and j is the x coordinate
				bCoords = m * Vector3(j, i, 1);

				//Clamp the barycentric coordinates to avoid errors
				bCoords.Clamp(0.0f, 1.0f);
				
				// set z of the barycentric coordinates to 1-sum of others, so they add to 1
				bCoords.z = 1 - bCoords.x - bCoords.y;

				//Should be 1
				float sum = bCoords.x + bCoords.y + bCoords.z;
				//Normalize the barycentric coordinates
				bCoords = bCoords / sum;

				// Interpolate the z value using the barycentric coordinates
				float z = bCoords.x * p0.z + bCoords.y * p1.z + bCoords.z * p2.z;

				// Compare the z value of the pixel with the z value of the zbuffer
				if (z < zbuffer->GetPixel(j, i)) {
					//Update the zbuffer
					zbuffer->SetPixel(j, i, z);
					// Interpolate the color
					c = c0 * bCoords.x + c1 * bCoords.y + c2 * bCoords.z;

					//Set the pixel to the calculated color
					SetPixelSafe(j, i, c);
				}
				else {
					//If the z value of the pixel is greater than the z value of the zbuffer, skip the pixel
					continue;
				}
			}
		}
	}
}

/*		SCRAP THIS ~~ DOESN'T WORK
//create BarycentricCoordinates function here

Vector3 Image::BarycentricCoordinates(const Vector2& p, const Vector3& p0, const Vector3& p1, const Vector3& p2) {
	//Calculate the area of the triangle, using Shoelace formula
	float area = 0.5 * ((p1.y - p2.y) * (p0.x - p2.x) + (p2.x - p1.x) * (p0.y - p2.y));

	//Make area positive
	if (area < 0) area = -area;

	//Calculate the barycentric coordinates
	float u = 0.5 * ((p1.y - p2.y) * (p.x - p2.x) + (p2.x - p1.x) * (p.y - p2.y)) / area;
	u = clamp(u, 0.0f, 1.0f);

	float v = 0.5 * ((p2.y - p0.y) * (p.x - p2.x) + (p0.x - p2.x) * (p.y - p2.y)) / area;
	v = clamp(v, 0.0f, 1.0f);

	float w = 1 - u - v;
	w = clamp(w, 0.0f, 1.0f);

	//clamp coordinates to 0 and 1 to avoid errors, using clamp function
	
	
	

	return Vector3(u, v, w);
}

*/

/*

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


void Image::DrawTriangle(const Vector2& p0, const Vector2& p1, const Vector2& p2, const Color& borderColor, bool isFilled, const Color& fillColor) {
	DrawLineDDA(p0.x, p0.y, p1.x, p1.y, borderColor);
	DrawLineDDA(p1.x, p1.y, p2.x, p2.y, borderColor);
	DrawLineDDA(p2.x, p2.y, p0.x, p0.y, borderColor);
	if (isFilled) {
		//Vector made with floats, cast to ints and compare
		int h_max = std::max(static_cast<int>(p0.y), static_cast<int>(p1.y));
		h_max = std::max(h_max, static_cast<int>(p2.y));

		int h_min = std::min(static_cast<int>(p0.y), static_cast<int>(p1.y));
		h_min = std::min(h_min, static_cast<int>(p2.y));

		//Height of raster box
		int h = h_max - h_min;

		//Create vector of Cells of size h
		std::vector<Cell> table(h);

		//Scans the three vectors
		ScanLineDDA(p0.x, p0.y, p1.x, p1.y, table);
		ScanLineDDA(p1.x, p1.y, p2.x, p2.y, table);
		ScanLineDDA(p2.x, p2.y, p0.x, p0.y, table);

		// From 0 to height
		for (int y = 0; y < table.size(); y++) {
			// From min to max, of x at height y
			for (int x = table[y].xMin; x < table[y].xMax; x++) {
				// Set the pixel to the fill colour
 				SetPixelSafe(x, y + h_min, fillColor);
			}
		}
	}
}
*/

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