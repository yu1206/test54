
#include "MSRCR.h"
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream> 
#include <io.h>
#include <Windows.h>
#include "opencv\cv.h"
#include "opencv\highgui.h"
#include <direct.h>

using namespace cv;
using namespace std;

extern int readDir(char *dirName, vector<string> &filesName);
#define pi 3.1415926
unsigned int random_gen()
{
	//srand((unsigned)time(NULL));
	//srand((int)time(0));
	unsigned int rnd = 0;

	rnd = rand();

	printf("rnd=%d\n",rnd);

	return rnd;
}

float random_float()
{

	

	return ((float)random_gen() / (float)RAND_MAX);

}
float rand_uniform_strong(float min, float max)
{
	if (max < min)
	{
		float swap = min;
		min = max;
		max = swap;
	}
	return (random_float() * (max - min)) + min;
}

float rand_scale(float s)
{
	float scale = rand_uniform_strong(1, s);
	if (random_gen() % 2) 
		return scale;
	return 1. / scale;
}
void image_data_augmentation(Mat img, int w, int h,
	int pleft, int ptop, int swidth, int sheight, int flip,
	float jitter, float dhue, float dsat, float dexp, Mat & sized)
{
	

	// crop
	Rect src_rect(pleft, ptop, swidth, sheight);
	Rect img_rect(cv::Point2i(0, 0), img.size());
	Rect new_src_rect = src_rect & img_rect;

	Rect dst_rect(cv::Point2i(std::max<int>(0, -pleft), std::max<int>(0, -ptop)), new_src_rect.size());

	Mat cropped(cv::Size(src_rect.width, src_rect.height), img.type());
	cropped.setTo(cv::Scalar::all(0));

	img(new_src_rect).copyTo(cropped(dst_rect));

	// resize
	//Mat sized;
	resize(cropped, sized, cv::Size(w, h), 0, 0, INTER_LINEAR);

	// flip
	if (flip) 
	{
		cv::flip(sized, cropped, 1);    // 0 - x-axis, 1 - y-axis, -1 - both axes (x & y)
		sized = cropped.clone();
	}

	// HSV augmentation
	// CV_BGR2HSV, CV_RGB2HSV, CV_HSV2BGR, CV_HSV2RGB
	//if (ipl->nChannels >= 3)
	
		Mat hsv_src;
		cvtColor(sized, hsv_src, CV_BGR2HSV);    // also BGR -> RGB

		//vector<Mat> hsv;
		Mat hsv[3];
		split(hsv_src, hsv);

		hsv[1] *= dsat;
		hsv[2] *= dexp;
		hsv[0] += 179 * dhue;

		//vector<Mat> hsv1(&hsv[0],&hsv[2]);
		vector<Mat> hsv1;
		for (int k = 0; k < 3;k++)
		{
			hsv1.push_back(hsv[k]);
		}
		cv::merge(hsv1, hsv_src);

		//cv::merge(hsv)
		//cvtColor(hsv_src, sized, CV_HSV2RGB);    // now RGB instead of BGR
		cvtColor(hsv_src, sized, CV_HSV2BGR);
	
	int jjjj = 0;
	//std::stringstream window_name;
	//window_name << "augmentation - " << ipl;
	//cv::imshow(window_name.str(), sized);
	//cv::waitKey(0);

	// Mat -> IplImage -> image
	//IplImage src = sized;
	//image out = ipl_to_image(&src);

	//return out;
}

int test54(int argc, char *argv[])
{



	srand((unsigned)time(NULL));

	string imgpath1 = "E:\\wusuizhedangqufen\\pachong\\biaoji\\patupian1";
	string savepath = "E:\\wusuizhedangqufen\\pachong\\biaojidata\\patupian1";
	mkdir(savepath.c_str());
	string drawpath = "E:\\wusuizhedangqufen\\pachong\\biaojidata\\patupian1draw";
	mkdir(drawpath.c_str());
	int nrotate = 10;//旋转个数
	int nmove = 10;//平移个数
	int ncrop = 10;//裁剪个数
	int nmsrcr = 10;//msrcr个数
	int ncolor = 10;//色彩抖动
	vector<string> v1_img_;
	readDir((char*)imgpath1.c_str(), v1_img_);

	for (int i = 0; i < v1_img_.size(); i++)
	{
		//string imagename = v1_img_[i];

		string imagename = "E:\\wusuizhedangqufen\\pachong\\biaoji\\patupian1\\2.jpg";

		int npos = imagename.find_last_of('\\');
		int npos2 = imagename.find_last_of('.');

		string name1 = imagename.substr(npos + 1, npos2 - npos - 1);
		Mat img = imread(imagename.c_str());

		cout << imagename.c_str() << endl;

		if (img.data == NULL)
		{
			printf("img.data = NULL！\n");
			system("pause");
			continue;
		}

		

		int use_flip = 1;
		float hue = 0.1;
		float saturation = 1.5;
		float	exposure = 1.5;

		int oh = img.rows;
		int ow = img.cols;

		
		float jitter = 0.3;
		int dw = (ow*jitter);
		int dh = (oh*jitter);

		for (int j = 0; j < 10;j++)
		{
			printf("-------%d-------------\n",j);

			int pleft = rand_uniform_strong(-dw, dw);
			int pright = rand_uniform_strong(-dw, dw);
			int ptop = rand_uniform_strong(-dh, dh);
			int pbot = rand_uniform_strong(-dh, dh);

			int swidth = ow - pleft - pright;
			int sheight = oh - ptop - pbot;

			float sx = (float)swidth / ow;
			float sy = (float)sheight / oh;

			int flip = 0; //= use_flip ? random_gen() % 2 : 0;

			float dx = ((float)pleft / ow) / sx;
			float dy = ((float)ptop / oh) / sy;

			float dhue = rand_uniform_strong(-hue, hue);
			float dsat = rand_scale(saturation);
			float dexp = rand_scale(exposure);



			int pleft1 = rand() % (2 * dw) - dw;
			int pright1 = rand() % (2 * dw) - dw;
			int ptop1 = rand() % (2 * dh) - dh;
			int pbot1 = rand() % (2 * dh) - dh;

			printf("pleft1=%d,dw=%d,rand()=%d\n", pleft1, dw, rand());
			printf("pright1=%d,dw=%d,rand()=%d\n", pright1, dw, rand());
			printf("ptop1=%d,dw=%d,rand()=%d\n", ptop1, dh, rand());
			printf("pbot1=%d,dw=%d,rand()=%d\n", pbot1, dh, rand());

			int swidth1 = ow - pleft1 - pright1;
			int sheight1 = oh - ptop1 - pbot1;

			float sx1 = (float)swidth1 / ow;
			float sy1 = (float)sheight1 / oh;
			float dx1 = ((float)pleft1 / ow) / sx1;
			float dy1 = ((float)ptop1 / oh) / sy1;


			float fhue = rand() / double(RAND_MAX);
			float dhue1 = fhue * (2 * hue) - hue;

			float fsat = rand() / double(RAND_MAX);
			float dsat1 = fsat*(saturation-1)+1;

			float fexp = rand() / double(RAND_MAX);
			float dexp1 = fexp*(exposure-1)+1;

			printf("dhue1=%0.2f,fhue=%0.2f,rand()=%d\n", dhue1, fhue, rand());
			printf("dsat1=%0.2f,fsat=%0.2f,rand()=%d\n", dsat1, fsat, rand());
			printf("dexp1=%0.2f,fexp=%0.2f,rand()=%d\n", dexp1, fexp, rand());

			Mat dst;
			image_data_augmentation(img, 608, 608, pleft, ptop, swidth, sheight, flip, jitter, dhue, dsat, dexp, dst);
			

			/*char file[1024];
			sprintf(file, "%s\\%s-%d-%d-%d-%d-%0.2f-%0.2f-%0.2f.jpg", savepath.c_str(), name1.c_str(), pleft, pright, ptop, pbot,
				 dhue, dsat, dexp);
			imwrite(file, dst);*/

			Mat dst1;
			char file1[1024];
			image_data_augmentation(img, 608, 608, pleft1, ptop1, swidth1, sheight1, flip, jitter, dhue1, dsat1, dexp1, dst1);
			sprintf(file1, "%s\\-%s-%d-%d-%d-%d-%0.2f-%0.2f-%0.2f.jpg", savepath.c_str(), name1.c_str(), pleft1, pright1, ptop1, pbot1,
				dhue1, dsat1, dexp1);
			imwrite(file1, dst1);
		}
		

		
		


		int jjjj = 0;
	}
	return 0;
}









