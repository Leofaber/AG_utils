/*
 * Copyright (c) 2017
 *     Leonardo Baroncelli, Giancarlo Zollino,
 *
 * Any information contained in this software
 * is property of the AGILE TEAM and is strictly
 * private and confidential.
 * 
 * https://github.com/Leofaber/MapConverter
*/


#include "FitsUtils.h"
#include <iostream>
#include <string>

using namespace std;
/*
class CustomPoint{
	public:
		CustomPoint();
		CustomPoint(int _x, int _y){
			x = _x;
			y = _y;
		}
		void print(){
			cout << "[" << y << "," << x << "] [row,col]" << endl; 
		}
		int x; // colonne
		int y; // righe
};

*/
//template <class T>
class CustomMap {

	public:
  		//T image;
		int ** image;
		int rows;
		int cols;
		string imagePath;

		CustomMap(){ 
			rows = 0;
			cols = 0;
			imagePath = "";
		}

		~CustomMap(){
			for (  int i = 0; i < rows; i++){
				delete [] image[i];
			}
			delete [] image;
			image = 0;
		}
	
		 
		void initializeImage(string imgPath, int r, int c){
			imagePath = imgPath;			
			rows = r;
			cols = c;
			image = new int*[rows];
			for (int y = 0; y < rows; ++y){
				image[y] = new int[cols];
			}
		}
		
};

class MapConverter
{
	public:

		
		// Convert an image in a **double matrix
		static double ** mapPathToDoublePtr(const char * imagePath);
		
		// Convert an image in a **double matrix
		static /*Map<int **>*/ CustomMap mapPathToIntPtr(const char * imagePath);
	


	private:
		// Constructor
		MapConverter();
};
