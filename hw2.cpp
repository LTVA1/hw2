#include "hw2.h"
#include <fstream>
#include <iostream>

#define MASK_ANCHOR_X 1
#define MASK_ANCHOR_Y 1

extern int glob_width;
extern int glob_height;

extern int* imglob_dil;
extern int* imglob_er;
extern int* imglob_src;

ImageProcess::ImageProcess()
{
	srcImg = processedImg = mask = nullptr;
}

ImageProcess::ImageProcess(int w, int h)
{
	glob_width = w;
	glob_height = h;

	mask = nullptr;
	srcImg = new Img(w, h);
	processedImg = new Img(w, h);
}

ImageProcess::ImageProcess(const Img* img)
{
	srcImg = new Img(img->srcImg, img->width, img->height);
	processedImg = new Img(img->width, img->height);

	mask = nullptr;
	glob_width = img->width;
	glob_height = img->height;
}

ImageProcess::ImageProcess(const char* fileName)
{
	loadImgFromFile(fileName, 1);
}

ImageProcess::~ImageProcess()
{
	delete[] srcImg;
	delete[] processedImg;
	delete[] mask;
}

int ImageProcess::updateMask(const Img& mask)
{
	this->mask = new Img(mask.srcImg, mask.width, mask.height);

	mask_anchor_x = MASK_ANCHOR_X;
	mask_anchor_y = MASK_ANCHOR_Y;

	if(this->mask == nullptr)
	{
		return -1;
	}

	else
	{
		return 0;
	}
}

int ImageProcess::updateSrcImg()
{
	for(int i = 0; i < this->srcImg->height; ++i)
	{
		for(int j = 0; j < this->srcImg->width; ++j)
		{
			this->srcImg->srcImg[this->srcImg->width * i + j] = this->processedImg->srcImg[this->srcImg->width * i + j];
		}
	}

	if(this->processedImg->srcImg[this->srcImg->width] != this->srcImg->srcImg[this->srcImg->width])
	{
		return -1;
	}

	else
	{
		return 0;
	}
}

int ImageProcess::loadImgFromFile(const char* fileName, int format)
{
	FILE* in;

	in = fopen(fileName, "r");

	int fwidth = 0, fheight = 0;

	fscanf(in, "%d\t%d\n", &fwidth, &fheight);

	this->srcImg = new Img(fwidth, fheight);
	this->processedImg = new Img(fwidth, fheight);

	glob_width = fwidth;
	glob_height = fheight;

	int counter = 0;
	char c;

	while(fscanf(in, "%c", &c) != EOF)
	{
		if(c != '\n' && counter < fwidth * fheight) //поддерживает оба формата
		{
			srcImg->srcImg[counter] = c == '0' ? 0 : 1;

			counter++;
		}
	}

	fclose(in);

	return 0;
}

int ImageProcess::saveImgToFile(const char* fileName, int format)
{
	FILE* out;

	out = fopen(fileName, "w");

	fprintf(out, "%d\t%d\n", srcImg->width, srcImg->height);

	switch(format)
	{
		case 0:
		{
			for(int i = 0; i < srcImg->height * srcImg->width; ++i)
			{
				fprintf(out, "%d", srcImg->srcImg[i]);
			}

			fprintf(out, "\n");

			break;
		}

		case 1:
		{
			for(int i = 0; i < srcImg->height; ++i)
			{
				for(int j = 0; j < srcImg->width; ++j)
				{
					fprintf(out, "%d", srcImg->srcImg[srcImg->width * i + j]);
				}

				fprintf(out, "\n");
			}

			break;
		}

		default: break;
	}

	fclose(out);
	return 0;
}

int ImageProcess::dilotation()
{
	for(int i = 0; i < srcImg->height; ++i)
	{
		for(int j = 0; j < srcImg->width; ++j)
		{
			std::cout << srcImg->srcImg[srcImg->width * i + j];
		}

		std::cout << "\n";
	}

	std::cout << "source\n\n";

	for(int i = 0; i < srcImg->height; ++i)
	{
		for(int j = 0; j < srcImg->width; ++j)
		{
			int counter = 0;

			for(int k = 0; k < mask->height; ++k)
			{
				for(int l = 0; l < mask->width; ++l)
				{
					if(srcImg->width * i + j - (srcImg->width * (mask_anchor_y - k) + (mask_anchor_x - l)) >= 0 && srcImg->width * i + j - (srcImg->width * (mask_anchor_y - k) + (mask_anchor_x - l)) < srcImg->width * srcImg->height)
					{
						if(srcImg->srcImg[srcImg->width * i + j - (srcImg->width * (mask_anchor_y - k) + (mask_anchor_x - l))] == mask->srcImg[k * mask->width + l] && mask->srcImg[k * mask->width + l] == 1)
						{
							counter++;
						}
					}
				}
			}

			if(counter > 0)
			{
				processedImg->srcImg[srcImg->width * i + j] = 1;
			}

			else
			{
				processedImg->srcImg[srcImg->width * i + j] = 0;
			}

			imglob_dil[srcImg->width * i + j] = processedImg->srcImg[srcImg->width * i + j];
			imglob_src[srcImg->width * i + j] = srcImg->srcImg[srcImg->width * i + j];
		}
	}

	return 0;
}

int ImageProcess::erosion()
{
	int amount_of_mask_ones = 0;

	for(int i = 0; i < mask->height; ++i)
	{
		for(int j = 0; j < mask->width; ++j)
		{
			if(mask->srcImg[mask->width * i + j] == 1)
			{
				amount_of_mask_ones++;
			}
		}
	}

	for(int i = 0; i < srcImg->height; ++i)
	{
		for(int j = 0; j < srcImg->width; ++j)
		{
			int counter = 0;

			for(int k = 0; k < mask->height; ++k)
			{
				for(int l = 0; l < mask->width; ++l)
				{
					if(srcImg->width * i + j - (srcImg->width * (mask_anchor_y - k) + (mask_anchor_x - l)) >= 0 && srcImg->width * i + j - (srcImg->width * (mask_anchor_y - k) + (mask_anchor_x - l)) < srcImg->width * srcImg->height)
					{
						if(srcImg->srcImg[srcImg->width * i + j - (srcImg->width * (mask_anchor_y - k) + (mask_anchor_x - l))] == mask->srcImg[k * mask->width + l] && mask->srcImg[k * mask->width + l] == 1)
						{
							counter++;
						}
					}
				}
			}

			if(counter == amount_of_mask_ones)
			{
				processedImg->srcImg[srcImg->width * i + j] = srcImg->srcImg[srcImg->width * i + j];
			}

			else
			{
				processedImg->srcImg[srcImg->width * i + j] = 0;
			}

			imglob_er[srcImg->width * i + j] = processedImg->srcImg[srcImg->width * i + j];
		}
	}

	return 0;
}