/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#ifndef BLOBSFINDER_H
#define BLOBSFINDER_H

#include <map>
#include <iostream>
#include "Blob.h"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;


class BlobsFinder
{
    public:

        /**
            Return a list of Blobs. In order to find one or more Blob it uses:
                - non linear stretching
                - gaussian filtering
                - find contours procedure
         */
        static vector<Blob*> findBlobs(string filePath, int ** image, int rows, int cols, double CDELT1, double CDELT2);

    private:
        BlobsFinder();
	static void nonLinearStretch(Mat* inputImage, float r);
	static void gaussianBlur(Mat* inputImg, Size kernelSize, double sigma);
	static vector<pair<CustomPoint,int>> computePixelsOfBlob(vector<Point>& c, Mat& image);

	static void printImageBlobs(int rows,int cols, vector<Blob>& blobs, string windowName);
	static void printImageBlob(Mat& inputImage, Blob& b, string windowName);
	static void printImage(Mat& image,string windowName);
	static void printImageInConsole(Mat& image);
	static void printImageInConsole(int ** image, int rows, int cols);
};

#endif // BLOBSFINDER_H
