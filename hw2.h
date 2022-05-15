#pragma once

#include <stdlib.h>
#include <list>
#include <vector>

struct Img
{
	int* srcImg;
	int width;
	int height;

	Img()
	{
		srcImg = nullptr;
		width = height = 0;
	}

	Img(int w, int h)
	{
		srcImg = new int[w * h];

		for(int i = 0; i < h; ++i)
		{
			for(int j = 0; j < w; ++j)
			{
				srcImg[i * h + j] = 0;
			}
		}

		width = w;
		height = h;
	}

	Img(const int* src, int w, int h)
	{
		srcImg = new int[w * h];

		for(int i = 0; i < h; ++i)
		{
			for(int j = 0; j < w; ++j)
			{
				srcImg[i * h + j] = src[i * h + j];
			}
		}

		width = w;
		height = h;
	}

	~Img()
	{
		delete[] srcImg;
	}
};

class ImageProcess
{
	private:
		Img* srcImg;
		Img* processedImg;
		Img* mask;
		//здесь можно объявлять другие переменные и функции
		int mask_anchor_y;
		int mask_anchor_x;

		std::vector<std::vector<int>> contours; //промежуточное нормальное хранилище контуров

		std::vector<int>* is_pixel_in_list(int index);

	public:
		//здесь нельзя объявлять функции и переменные
		/**/
		ImageProcess();
		/* выделяется память для картинки размера w*h */
		ImageProcess(int w, int h);
		/* выделяется память для картинки размера w*h и копируется картинка*/
		ImageProcess(const Img* img);
		/* выделяется память для картинки копируется картинка из файла*/
		ImageProcess(const char* fileName);
		~ImageProcess();

		/*задание маски*/
		int updateMask(const Img& mask);
		/*перезаписать исходную картинку картинкой, которая была получена в результате дилотации и/или эрозии*/
		int updateSrcImg();
		/*дилотация картинки, результат записать в processedImg*/
		int dilotation();
		/*эрозия картинки, результат записать в processedImg*/
		int erosion();
		/*
		загрузить/сохранить из файла с имененм fileName картинку
		input		:	fileName - имя файла
						format - формат файла 	0 - последовательность 0 и 1, без перехода на новую строку
												1 - последовательность 0 и 1, с переходом на новую строку
	*/
		int loadImgFromFile(const char* fileName, int format = 1);
		int saveImgToFile(const char* fileName, int format = 1);

		/*на зачёт без зачёта*/
		/*вернуть список контуров (2 и более пикселей, включая диагональные)*/
		std::list<std::list<std::pair<int /*x*/,int /*y*/>>> getListContours();
};

