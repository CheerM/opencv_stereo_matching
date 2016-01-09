#include "disparity.h"
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

#define FileNumber 21

int main(int argc, char** argv )
{


	//文件夹下的目录
	string name[FileNumber]= {"Aloe", "Baby1", "Baby2", "Baby3", "Bowling1",
	"Bowling2", "Cloth1", "Cloth2", "Cloth3", "Cloth4", "Flowerpots",
	"Lampshade1", "Lampshade2", "Midd1", "Midd2", "Monopoly",
	"Plastic", "Rocks1", "Rocks2", "Wood1", "Wood2"};
	
	//make destination dir
	mkdir("resultImages", S_IRWXU);

	const time_t start = time(NULL);

	for (int i = 0; i < FileNumber; i ++) {

		//在目的文件夹中创建相应的文件夹，以便存入图片
		string str = "resultImages/" + name[i];
		const char * dir = str.c_str();
		mkdir(dir, S_IRWXU);

		//读取images文件夹中的源图片
		Mat Left = imread("images/" + name[i] + "/view1.png" , -1 );
		Mat Right = imread("images/" + name[i] + "/view5.png" , -1 );
		if ( (!Left.data) || (!Right.data)) {
			printf("Please input right data~~\n");
			return -1;
		}

		//turn into gray image
		Mat disparityLeft = turnIntoGray(Left, 3);
		Mat disparityRight = turnIntoGray(Right, 3);
		
		//get the disparity map and save it
		//SSD
		Mat resultLeftSSD = SSD(disparityLeft, disparityRight, true, 5);
		imwrite( "resultImages/" + name[i] + "/" + name[i] + "_disp1_SSD.png", resultLeftSSD );
		Mat resultRightSSD = SSD(disparityLeft, disparityRight, false, 5);
		imwrite( "resultImages/" + name[i] + "/" + name[i] + "_disp5_SSD.png", resultRightSSD );

		//NCC
		Mat resultLeftNCC = NCC(disparityLeft, disparityRight, true, 5);
		imwrite( "resultImages/" + name[i] + "/" + name[i] + "_disp1_NCC.png", resultLeftNCC );
		Mat resultRightNCC = NCC(disparityLeft, disparityRight, false, 5);
		imwrite( "resultImages/" + name[i] + "/" + name[i] + "_disp5_NCC.png", resultRightNCC );

		//ASW
		Mat resultLeftASW = ASW(Left, Right, true, 11, 36, 7);
		imwrite( "resultImages/" + name[i] + "/" + name[i] + "_disp1_ASW.png", resultLeftASW );
		Mat resultRightASW = ASW(Left, Right, false, 11, 36, 7);
		imwrite( "resultImages/" + name[i] + "/" + name[i] + "_disp5_ASW.png", resultRightASW );

		//evaluate the quality of the disparity maps
		Mat standardLeft = imread("images/" + name[i] + "/disp1.png", -1);
		cout << name[i] << "_Left_ASW: ";
		Evaluate(standardLeft, resultLeftASW);
		cout << name[i] << "_Left_SSD: ";
		Evaluate(standardLeft, resultLeftSSD);
		cout << name[i] << "_Left_NCC: ";
		Evaluate(standardLeft, resultLeftNCC);

		Mat standardRight = imread("images/" + name[i] + "/disp5.png", -1);
		cout << name[i] << "_Right_ASW: ";
		Evaluate(standardRight, resultRightASW);
		cout << name[i] << "_Right_SSD: ";
		Evaluate(standardRight, resultRightSSD);
		cout << name[i] << "_Right_NCC: ";
		Evaluate(standardRight, resultRightNCC);
		

		//part4
		//add intensity
		/*
		Mat AddIntensityRight = addIntensity(disparityRight, 20);
		Mat resultRightSSD = SSD(disparityLeft, AddIntensityRight, false, 5);
		Mat resultRightNCC = NCC(disparityLeft, AddIntensityRight, false, 5);
		Mat standardRight = imread("images/" + name[i] + "/disp5.png", -1);
		cout << name[i] << "_Right_SSD: ";
		Evaluate(standardRight, resultRightSSD);
		cout << name[i] << "_Right_NCC: ";
		Evaluate(standardRight, resultRightNCC);
		*/
	}
	const time_t finish = time(NULL);
	cout << "duration = " << finish - start << " seconds" << endl;
    	
    	waitKey(0);
  	return 0;
}