#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <QList>

#include "hw2.h"

#define MASK_WIDTH 3
#define MASK_HEIGHT 3

#define MASK_ANCHOR_X 1
#define MASK_ANCHOR_Y 1

typedef uint64_t Uint64;
typedef int64_t Sint64;
typedef uint32_t Uint32;
typedef int32_t Sint32;
typedef uint16_t Uint16;
typedef int16_t Sint16;
typedef uint8_t Uint8;
typedef int8_t Sint8;

int* imglob_dil;
int* imglob_er;
int* imglob_src;

int glob_width;
int glob_height;

const int glob_mask[MASK_WIDTH][MASK_HEIGHT] = {
{ 0, 1, 0 },
{ 1, 1, 1 }, //опорный элемент в центре
{ 0, 1, 0 }};

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	ImageProcess* imgproc = new ImageProcess();

	imgproc->loadImgFromFile("test.txt", 1);

	int mask[MASK_WIDTH * MASK_HEIGHT];

	for(int i = 0; i < MASK_HEIGHT; ++i)
	{
		for(int j = 0; j < MASK_WIDTH; ++j)
		{
			mask[i * MASK_WIDTH + j] = glob_mask[i][j];
		}
	}

	Img* massk = new Img(mask, MASK_WIDTH, MASK_HEIGHT);

	massk->c_x = 1;
	massk->c_y = 1;

	imgproc->updateMask(*(massk));

	delete massk;

	//std::cout << "glob " << glob_width << " " << glob_height;

	imglob_dil = new int[glob_width * glob_height];
	imglob_er = new int[glob_width * glob_height];
	imglob_src = new int[glob_width * glob_height];

	imgproc->dilotation();
	imgproc->erosion();

	for(int i = 0; i < glob_height; ++i)
	{
		for(int j = 0; j < glob_width; ++j)
		{
			std::cout << imglob_dil[glob_width * i + j];
		}

		std::cout << "\n";
	}

	std::cout << "dilotated\n\n";

	for(int i = 0; i < glob_height; ++i)
	{
		for(int j = 0; j < glob_width; ++j)
		{
			std::cout << imglob_er[glob_width * i + j];
		}

		std::cout << "\n";
	}

	std::cout << "eroded\n\n";

	for(int i = 0; i < glob_height; ++i)
	{
		for(int j = 0; j < glob_width; ++j)
		{
			std::cout << (imglob_er[glob_width * i + j] == imglob_dil[glob_width * i + j] ? "0" : "1");
		}

		std::cout << "\n";
	}

	std::cout << "contour";

	delete[] imglob_dil;
	delete[] imglob_er;
	delete imgproc;
    return a.exec();
}
