#include "snoise.h"
#include <stdlib.h>
#include <math.h>
#include <float.h>
#include "../../third_party/stb/stb_image.h"

#define FASTFLOOR(x) (((int)(x) <= (x)) ? ((int)x) : (((int)x) - 1))

unsigned char perm[512] = {151, 160, 137, 91, 90, 15,
													 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
													 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
													 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
													 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
													 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
													 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
													 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
													 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
													 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
													 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
													 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
													 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180,
													 151, 160, 137, 91, 90, 15,
													 131, 13, 201, 95, 96, 53, 194, 233, 7, 225, 140, 36, 103, 30, 69, 142, 8, 99, 37, 240, 21, 10, 23,
													 190, 6, 148, 247, 120, 234, 75, 0, 26, 197, 62, 94, 252, 219, 203, 117, 35, 11, 32, 57, 177, 33,
													 88, 237, 149, 56, 87, 174, 20, 125, 136, 171, 168, 68, 175, 74, 165, 71, 134, 139, 48, 27, 166,
													 77, 146, 158, 231, 83, 111, 229, 122, 60, 211, 133, 230, 220, 105, 92, 41, 55, 46, 245, 40, 244,
													 102, 143, 54, 65, 25, 63, 161, 1, 216, 80, 73, 209, 76, 132, 187, 208, 89, 18, 169, 200, 196,
													 135, 130, 116, 188, 159, 86, 164, 100, 109, 198, 173, 186, 3, 64, 52, 217, 226, 250, 124, 123,
													 5, 202, 38, 147, 118, 126, 255, 82, 85, 212, 207, 206, 59, 227, 47, 16, 58, 17, 182, 189, 28, 42,
													 223, 183, 170, 213, 119, 248, 152, 2, 44, 154, 163, 70, 221, 153, 101, 155, 167, 43, 172, 9,
													 129, 22, 39, 253, 19, 98, 108, 110, 79, 113, 224, 232, 178, 185, 112, 104, 218, 246, 97, 228,
													 251, 34, 242, 193, 238, 210, 144, 12, 191, 179, 162, 241, 81, 51, 145, 235, 249, 14, 239, 107,
													 49, 192, 214, 31, 181, 199, 106, 157, 184, 84, 204, 176, 115, 121, 50, 45, 127, 4, 150, 254,
													 138, 236, 205, 93, 222, 114, 67, 29, 24, 72, 243, 141, 128, 195, 78, 66, 215, 61, 156, 180};

float grad2(int hash, float x, float y)
{
	int h = hash & 7;				 // Convert low 3 bits of hash code
	float u = h < 4 ? x : y; // into 8 simple gradient directions,
	float v = h < 4 ? y : x; // and compute the dot product with (x,y).
	return ((h & 1) ? -u : u) + ((h & 2) ? -2.0f * v : 2.0f * v);
}

float snoise2(float x, float y)
{

#define F2 0.366025403f // F2 = 0.5*(sqrt(3.0)-1.0)
#define G2 0.211324865f // G2 = (3.0-Math.sqrt(3.0))/6.0

	float n0, n1, n2; // Noise contributions from the three corners

	// Skew the input space to determine which simplex cell we're in
	float s = (x + y) * F2; // Hairy factor for 2D
	float xs = x + s;
	float ys = y + s;
	int i = FASTFLOOR(xs);
	int j = FASTFLOOR(ys);

	float t = (float)(i + j) * G2;
	float X0 = i - t; // Unskew the cell origin back to (x,y) space
	float Y0 = j - t;
	float x0 = x - X0; // The x,y distances from the cell origin
	float y0 = y - Y0;

	// For the 2D case, the simplex shape is an equilateral triangle.
	// Determine which simplex we are in.
	int i1, j1; // Offsets for second (middle) corner of simplex in (i,j) coords
	if (x0 > y0)
	{
		i1 = 1;
		j1 = 0;
	} // lower triangle, XY order: (0,0)->(1,0)->(1,1)
	else
	{
		i1 = 0;
		j1 = 1;
	} // upper triangle, YX order: (0,0)->(0,1)->(1,1)

	// A step of (1,0) in (i,j) means a step of (1-c,-c) in (x,y), and
	// a step of (0,1) in (i,j) means a step of (-c,1-c) in (x,y), where
	// c = (3-sqrt(3))/6

	float x1 = x0 - i1 + G2; // Offsets for middle corner in (x,y) unskewed coords
	float y1 = y0 - j1 + G2;
	float x2 = x0 - 1.0f + 2.0f * G2; // Offsets for last corner in (x,y) unskewed coords
	float y2 = y0 - 1.0f + 2.0f * G2;

	// Wrap the integer indices at 256, to avoid indexing perm[] out of bounds
	int ii = i & 0xff;
	int jj = j & 0xff;

	// Calculate the contribution from the three corners
	float t0 = 0.5f - x0 * x0 - y0 * y0;
	if (t0 < 0.0f)
		n0 = 0.0f;
	else
	{
		t0 *= t0;
		n0 = t0 * t0 * grad2(perm[ii + perm[jj]], x0, y0);
	}

	float t1 = 0.5f - x1 * x1 - y1 * y1;
	if (t1 < 0.0f)
		n1 = 0.0f;
	else
	{
		t1 *= t1;
		n1 = t1 * t1 * grad2(perm[ii + i1 + perm[jj + j1]], x1, y1);
	}

	float t2 = 0.5f - x2 * x2 - y2 * y2;
	if (t2 < 0.0f)
		n2 = 0.0f;
	else
	{
		t2 *= t2;
		n2 = t2 * t2 * grad2(perm[ii + 1 + perm[jj + 1]], x2, y2);
	}

	// Add contributions from each corner to get the final noise value.
	// The result is scaled to return values in the interval [-1,1].
	return 40.0f * (n0 + n1 + n2); // TODO: The scale factor is preliminary!
}

float clamp(float d, float min, float max)
{
	const float t = d < min ? min : d;
	return t > max ? max : t;
}

int **create_heightmap(int dimensionx, int dimensionz, int seedx, int seedz, float precision, int border_add,
											 DA *simplex_points, DA *corresponding_heights, int octaves, float amplitude,
											 float lacunarity, float persistence, int maxheightmult)
{
	int **hm = malloc(sizeof(int *) * dimensionx);
	float *points = 0;
	int *heights = 0;
	if (simplex_points != 0 && corresponding_heights != 0)
	{
		points = get_data_DA(simplex_points);
		heights = get_data_DA(corresponding_heights);
	}
	float simple_res = 0;
	float fractal_res = 0;
	float param1 = 0, param2 = 0;
	float amplitude2 = amplitude;
	float slopex = 1, slopez = 1;
	for (int i = 0; i < dimensionx; i++)
	{
		hm[i] = malloc(sizeof(int) * dimensionz);
		for (int i2 = 0; i2 < dimensionz; i2++)
		{
			fractal_res = 0;
			param1 = (float)(seedx + i) / precision;
			param2 = (float)(seedz + i2) / precision;
			amplitude2 = amplitude;
			for (int i3 = 0; i3 < octaves; i3++)
			{
				simple_res = (snoise2(param1, param2) + 1) / 2.0f;
				if (i3 == 0)
				{
					slopex = (snoise2(param1 + 1, param2) + 1) / 2.0f - simple_res;
					slopez = (snoise2(param1, param2 + 1) + 1) / 2.0f - simple_res;
					fractal_res += simple_res * amplitude2;
				}
				else
				{
					fractal_res += simple_res * amplitude2 * (1 - ((slopex + slopez) / 2));
				}
				param1 *= lacunarity;
				param2 *= lacunarity;
				amplitude2 *= persistence;
			}
			if (simplex_points != 0 && corresponding_heights != 0)
			{
				fractal_res /= octaves;
			}
			fractal_res *= fractal_res;
			if (simplex_points != 0 && corresponding_heights != 0)
			{
				fractal_res = clamp(fractal_res, 0, 1);
			}
			if (simplex_points != 0 && corresponding_heights != 0)
			{
				for (unsigned int i3 = 0; i3 < get_size_DA(simplex_points) - 1; i3++)
				{
					if (fractal_res >= points[i3] && fractal_res <= points[i3 + 1])
					{
						hm[i][i2] = (int)(heights[i3] + ((fractal_res - points[i3]) / (points[i3 + 1] - points[i3])) * (heights[i3 + 1] - heights[i3]));
					}
				}
			}
			else
			{
				hm[i][i2] = (int)(fractal_res * maxheightmult);
			}
			if (i == 0 || i == dimensionx - 1 || i2 == 0 || i2 == dimensionz - 1)
			{
				hm[i][i2] += border_add;
			}
		}
	}

	return hm;
}

/*
disaster wrong function
float *create_points_heightmap(int **hm, int dimensionx, int dimensionz, int startx, int startz, int widthx, int widthz)
{
	float *res = malloc(sizeof(float) * widthx * widthz * 4);
	for (int i2 = startz; i2 < startz + widthz; i2++)
	{
		for (int i = startx; i < startx + widthx; i++)
		{
			if (i < dimensionx && i2 < dimensionz)
			{
				res[4 * i2 * widthz + 2 * i] = (float)hm[i][i2];
				res[4 * i2 * widthz + 2 * i + 1] = (float)hm[i][i2];
				res[4 * i2 * widthz + 2 * i + 2 * widthz] = (float)hm[i][i2];
				res[4 * i2 * widthz + 2 * i + 2 * widthz + 1] = (float)hm[i][i2];
			}
			else
			{
				res[4 * i2 * widthz + 2 * i] = FLT_MAX;
				res[4 * i2 * widthz + 2 * i + 1] = FLT_MAX;
				res[4 * i2 * widthz + 2 * i + 2 * widthz] = FLT_MAX;
				res[4 * i2 * widthz + 2 * i + 2 * widthz + 1] = FLT_MAX;
			}
		}
	}
	return res;
}*/

void bilinear_interpolation(float **data, int input_x,
														int input_y, int output_x,
														int output_y, float **output)
{
	float x_ratio, y_ratio;

	if (output_x > 1)
	{
		x_ratio = ((float)input_x - 1.0f) / ((float)output_x - 1.0f);
	}
	else
	{
		x_ratio = 0;
	}

	if (output_y > 1)
	{
		y_ratio = ((float)input_y - 1.0f) / ((float)output_y - 1.0f);
	}
	else
	{
		y_ratio = 0;
	}

	for (int i = 0; i < output_y; i++)
	{
		for (int j = 0; j < output_x; j++)
		{
			float x_l = floorf(x_ratio * (float)j);
			float y_l = floorf(y_ratio * (float)i);
			float x_h = ceilf(x_ratio * (float)j);
			float y_h = ceilf(y_ratio * (float)i);

			float x_weight = (x_ratio * (float)j) - x_l;
			float y_weight = (y_ratio * (float)i) - y_l;

			float a = data[(int)x_l][(int)y_l];
			float b = data[(int)x_h][(int)y_l];
			float c = data[(int)x_l][(int)y_h];
			float d = data[(int)x_h][(int)y_h];

			float pixel = a * (1.0f - x_weight) * (1.0f - y_weight) +
										b * x_weight * (1.0f - y_weight) +
										c * y_weight * (1.0f - x_weight) +
										d * x_weight * y_weight;

			output[j][i] = pixel;
		}
	}
}

int **create_heightmap_texture(const char *path, int maxheightmult, int border_add, int result_dimensionx, int result_dimensionz)
{
	int widthImg, heightImg, numColCh;
	float *bytes = stbi_loadf(path, &widthImg, &heightImg, &numColCh, 1);

	float **hm = malloc(sizeof(float *) * heightImg);
	for (int i = 0; i < heightImg; i++)
	{
		hm[i] = malloc(sizeof(float) * widthImg);
		for (int i2 = 0; i2 < widthImg; i2++)
		{
			hm[i][i2] = bytes[i * widthImg + i2];
		}
	}

	stbi_image_free(bytes);

	float **interpolated_hm = malloc(sizeof(float *) * result_dimensionx);
	for (int i = 0; i < result_dimensionx; i++)
	{
		interpolated_hm[i] = malloc(sizeof(float) * result_dimensionz);
	}
	bilinear_interpolation(hm, heightImg, widthImg, result_dimensionx, result_dimensionz, interpolated_hm);

	int **res = malloc(sizeof(int *) * result_dimensionx);
	for (int i = 0; i < result_dimensionx; i++)
	{
		res[i] = malloc(sizeof(int) * result_dimensionz);
		for (int i2 = 0; i2 < result_dimensionz; i2++)
		{
			res[i][i2] = (int)(interpolated_hm[i][i2] * (float)maxheightmult);
			if (i == 0 || i == heightImg - 1 || i2 == 0 || i2 == widthImg - 1)
			{
				res[i][i2] += border_add;
			}
		}
	}

	for (int i = 0; i < heightImg; i++)
	{
		free(hm[i]);
	}
	free(hm);

	for (int i = 0; i < result_dimensionx; i++)
	{
		free(interpolated_hm[i]);
	}
	free(interpolated_hm);

	return res;
}