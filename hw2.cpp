#include "hw2.h"
#include <fstream>
#include <iostream>
#include <algorithm>

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
	int* temp = processedImg->srcImg;
	processedImg->srcImg = srcImg->srcImg;
	srcImg->srcImg = temp;

	return 0;
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

std::vector<int>* ImageProcess::is_pixel_in_list(int index)
{
	for (int i = 0; i < contours.size(); ++i)
	{
		std::vector<int> contour = contours[i];

		if ((std::find(contour.begin(), contour.end(), index) != contour.end()))
		{
			return &contours[i];
		}
	}

	return nullptr;
}

//                             x,   y
std::list<std::list<std::pair<int, int>>> ImageProcess::getListContours()
{
	contours.clear(); //очистка, в буфере могли остаться пиксели с предыдущего вызова

	dilotation(); //на всякий случай, если до этого не вызывалась дилатация

	for (int i = 0; i < srcImg->width * srcImg->height; i++)
	{
		if ((processedImg->srcImg[i] == srcImg->srcImg[i] ? 0 : 1) == 1)
		{
			std::vector<int>* contour = is_pixel_in_list(i);

			if (contour != nullptr)
			{
				continue;
			}

			bool is_added = false;

			std::vector<std::vector<int>*> contours_connect;

			std::pair<int, int> coords = std::make_pair(i - i / srcImg->width * srcImg->width, i / srcImg->width);

			for (int y = -1; y <= 1; y++) //все 8 соседних пикселей
			{
				for (int x = -1; x <= 1; x++)
				{
					int pix_coord_in_arr = (y + coords.second) * srcImg->width + x + coords.first;

					if ((x == 0) && (y == 0)) //не смотрим на центральный пиксель, нужны только соседние
					{
						continue;
					}

					std::vector<int>* cont = is_pixel_in_list(pix_coord_in_arr);

					if (cont != nullptr)
					{
						if (!is_added)
						{
							cont->push_back(i);

							is_added = true;
						}

						if (!(std::find(contours_connect.begin(), contours_connect.end(), cont) != contours_connect.end()))
						{
							contours_connect.push_back(cont);
						}
					}
				}
			}

			if (!is_added)
			{
				std::vector<int> cntr;

				cntr.push_back(i);

				contours.push_back(cntr);
			}

			else
			{
				if (contours_connect.size() > 1)
				{
					for (int i = 1; i < contours_connect.size(); i++)
					{
						for (int j = 0; j < contours_connect[i]->size(); j++)
						{
							contours_connect[0]->push_back((*contours_connect[i])[j]);
						}

						contours.erase(std::remove(contours.begin(), contours.end(), *contours_connect[i]), contours.end());
					}
				}
			}
		}
	}

	std::list<std::list<std::pair<int, int>>> countors_final;

	for (int i = 0; i < contours.size(); i++)
	{
		std::list<std::pair<int, int>> countor_final;

		std::vector<int> contour = contours[i];

		for (int j = 0; j < contour.size(); j++)
		{
			countor_final.push_back(std::make_pair(contour[j] - contour[j] / srcImg->width * srcImg->width, contour[j] / srcImg->width));
		}

		countors_final.push_back(countor_final);
	}

	return countors_final;
}
