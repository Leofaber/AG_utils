/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
*/


#include "Blob.h"

Blob::Blob(string _filePath, vector<CustomPoint>& _contourPixels, vector<pair<CustomPoint,int>>& _blobPixels, double ** image, int ** photonImage, double CDELT1, double CDELT2) : agileMapTool(_filePath.c_str())
{
	filePath = _filePath;
	
	pixelArea = CDELT1*CDELT2;

	contour = _contourPixels;

	centroid = computeCentroid();

	galacticCentroid = computeGalacticCentroid();

	blobPixels = _blobPixels;

	numberOfPixels = blobPixels.size();

	blobArea = numberOfPixels*pixelArea;

	pixelMean = computePixelMean();

	photonsInBlob = computePhotonsBlob(photonImage);

	photonsCloseness = computePhotonsCloseness();
	
	//cout << 50-centroid.y <<","<<centroid.x << endl;
 	//cout << "photonsCloseness" << photonsCloseness << endl;
 
}



/// GETTERS
string Blob::getFilePath(){
	return filePath;
}
vector<CustomPoint> Blob::getContour(){
	return contour;
}
CustomPoint Blob::getCentroid(){
	return centroid;
}
CustomPoint Blob::getGalacticCentroid(){
	return galacticCentroid;
}
double Blob::getGalacticCentroidL(){
	return galacticCentroid.y;
}
double Blob::getGalacticCentroidB(){
	return galacticCentroid.x;
}
int Blob:: getNumberOfPixels() {
	return numberOfPixels;
}
int Blob::getNumberOfPhotonsInBlob() {
	return photonsInBlob.size();
}
double Blob::getPixelsMean(){
	return pixelMean;
}
double Blob::getPhotonsCloseness(){
	return photonsCloseness;
}
double Blob::getArea(){
	return blobArea;
}

CustomPoint Blob::computeCentroid(){
	int sumX=0;
	int sumY=0;
	for(vector<CustomPoint>::iterator l = contour.begin(); l < contour.end(); l++){
		CustomPoint p = *l;
		sumX+=p.x;
		sumY+=p.y;
	}

	CustomPoint c(sumY/contour.size(),sumX/contour.size());
	return c;
}

CustomPoint Blob::computeGalacticCentroid(){
	// Changing the reference system
	int y_euclidean_ref_sys = agileMapTool.Rows() - getCentroid().y;
	 

	double l  = agileMapTool.l(centroid.x, y_euclidean_ref_sys );
	double b  = agileMapTool.b(centroid.x, y_euclidean_ref_sys );

	return CustomPoint(l,b);
	
}
vector<CustomPoint> Blob::computePhotonsBlob(int ** photonImage){
	vector<CustomPoint> photonPixels;

	for(vector<pair<CustomPoint,int>>::iterator i = blobPixels.begin(); i != blobPixels.end(); i++){
		pair<CustomPoint,int> p = *i;

		int greyLevel = photonImage[p.first.y][p.first.x];

		for(int j = 0; j < greyLevel; j++){

			photonPixels.push_back(p.first);
		}        

	}
	if(photonPixels.size()==0){
		// debug
		cout << "[Blob "<<centroid.y<<","<<centroid.x<<"]computePhotonsBlob()-> Error: 0 fotoni nel blob! File: "<< filePath << endl;
		cout << "Number of pixels: " << getNumberOfPixels() << endl;
		cout << "Number of contour pixels: " << contour.size() << endl;

		for(vector<pair<CustomPoint,int>>::iterator i = blobPixels.begin(); i != blobPixels.end(); i++){
			pair<CustomPoint,int> p = *i;
			cout << "(" << p.first.y << "," << p.first.x <<")"<<" gl: "<<p.second<<" photon:" <<photonImage[p.first.y][p.first.x]<<endl;
		}
		cout <<"\nContorno:" << endl;
		for(vector<CustomPoint>::iterator i = contour.begin(); i != contour.end(); i++){
			CustomPoint p = *i;
			cout << "(" << p.y << "," << p.x <<")"<<" photon:" <<photonImage[p.y][p.x]<<endl;
		}

		exit(EXIT_FAILURE);
	}

	return photonPixels;
}



double Blob::computePixelMean(){
    double numberOfBlobPixels = (double)blobPixels.size();
    double greyLevelCount = 0;

    for (vector<pair<CustomPoint,int>>::iterator it = blobPixels.begin(); it != blobPixels.end(); ++it){
        pair<CustomPoint,int> p= *it;
        greyLevelCount+=p.second;
    }

	//cout << "["<<centroid.y<<","<<centroid.x<<"]" << greyLevelCount << " / " << numberOfBlobPixels << " = "<< greyLevelCount/numberOfBlobPixels << endl;
    return greyLevelCount/numberOfBlobPixels;
}

double Blob::computePhotonsCloseness(){
    double photonsCloseness = 0;
    double countDistances = 0;
    double countPhotons = photonsInBlob.size();        
	
    for(vector<CustomPoint>::iterator i = photonsInBlob.begin(); i != photonsInBlob.end(); i++){
        CustomPoint photon = *i;
        countDistances += getDistanceFromCentroid(photon);
	//countDistances += getSphericalDistanceFromCentroid(photon);
        countPhotons++;
    }
    // cout << "countDistances: " << countDistances << endl;
    // cout << "countPhotons: " << countPhotons << endl;
    
    photonsCloseness = countDistances/countPhotons;
    return photonsCloseness;
}

double Blob::getDistanceFromCentroid(CustomPoint photon) {
	double distance =  0;
	CustomPoint centroid = getCentroid();
	double arg =  pow(photon.x - centroid.x,2) +pow (photon.y - centroid.y,2) ;
	distance = pow(arg , 0.5);
	return distance;
}
double Blob::getSphericalDistanceFromCentroid(CustomPoint photon){
	double distance =  0;
	CustomPoint centroid = getCentroid();

	/*
	cout <<"\nfile: "<<agileMapTool.GetFileName()<<endl;
	cout <<"Photon: "<<50-photon.y<<","<< photon.x<<endl;
	cout <<"Photon Gal: "<<agileMapTool.l(photon.x,50-photon.y)<<","<<agileMapTool.b(photon.x,50-photon.y)<<endl;
	cout <<"Centroid: "<<50-centroid.y <<","<<centroid.x <<endl;
	cout <<"Centroid Gal: "<<getGalacticCentroidL()<<","<<getGalacticCentroidB()<<endl;
	*/
	distance = agileMapTool.SrcDist(photon.x,photon.y,getGalacticCentroidL(),getGalacticCentroidB());
	//cout << "distance: " << distance << ""<<endl;
	return distance;
}
bool Blob::isCentered(){
    	int centerY = agileMapTool.Rows()/2;
	int centerX = agileMapTool.Cols()/2;
	int offset = 15;

	if( centroid.x <= centerX+offset && centroid.x >= centerX-offset && centroid.y <= centerY+offset && centroid.y >= centerY-offset ){
	 	return true;
	}else{
		return false;
	}


}
