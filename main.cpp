#include "mainwindow.h"

#include <QApplication>
#include <QImage>
#include <QLabel>

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
    QApplication a(argc, argv);

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

	imgproc->updateMask(*(massk));

	delete massk;

	imglob_dil = new int[glob_width * glob_height];
	imglob_er = new int[glob_width * glob_height];
	imglob_src = new int[glob_width * glob_height];

	//======================================================================================
	//зачёт без зачёта

	imgproc->dilotation();

	//std::list<std::list<std::pair<int /*x*/,int /*y*/>>> test = imgproc->getListContours();

	QImage myImage;
	QImage *temp = new QImage(std::max(glob_width, 50), std::max(glob_height, 50), QImage::Format_Indexed8);
	myImage = *temp;

	myImage.setColor(0, qRgb(255, 255, 255)); //белый фон
	myImage.setColor(1, qRgb(0, 0, 0)); //чёрное изображение
	myImage.setColor(2, qRgb(255, 0, 0)); //красный контур

	for(int i = 0; i < myImage.width(); ++i)
	{
		for(int j = 0; j < myImage.height(); ++j)
		{
			myImage.scanLine(i)[j] = 0;
		}
	}

	for(int i = 0; i < glob_height; ++i)
	{
		for(int j = 0; j < glob_width; ++j)
		{
			std::cout << (imglob_src[i * glob_width + j] != imglob_dil[i * glob_width + j] ? 1 : 0);
		}

		std::cout << "\n";
	}

	for(int i = 0; i < glob_height; ++i)
	{
		for(int j = 0; j < glob_width; ++j)
		{
			if(imglob_src[i * glob_width + j] == 1)
			{
				myImage.scanLine(i)[j] = 1;
			}

			if(imglob_src[i * glob_width + j] != imglob_dil[i * glob_width + j])
			{
				myImage.scanLine(i)[j] = 2;
			}
		}
	}

	QLabel myLabel;

	QPixmap p = QPixmap::fromImage(myImage);

	myLabel.setPixmap(p.scaled(myImage.width() * 3, myImage.height() * 3, Qt::KeepAspectRatio));

	myLabel.show();

    return a.exec();
}
