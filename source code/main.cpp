#include "opencv2/highgui.hpp"

#include "GetFile.h"

using namespace cv;
using namespace std;

Vec2d chromacity(Vec3i color) {
	Vec2d chrom;
	chrom[0] = color[0] * 1.0 / (color[0] + color[1] + color[2]);
	chrom[1] = color[1] * 1.0 / (color[0] + color[1] + color[2]);
	return chrom;
}

int main(int argc, char* argv[])
{
    char filename[1024];
    GetFile(filename, 1024);
    if (filename[0] == '\0')
    {
        fprintf(stderr, "Please select a file\n");
        return EXIT_FAILURE;
    }
    
    VideoCapture vc = VideoCapture(filename);
	int frame = vc.get(CAP_PROP_FRAME_COUNT);
	double frameRate = vc.get(CAP_PROP_FPS);
	int width = vc.get(CAP_PROP_FRAME_WIDTH);
	int height = vc.get(CAP_PROP_FRAME_HEIGHT);
	Mat Col_C_STI(height,frame-1,CV_8UC3);
	Mat Row_C_STI(width, frame-1, CV_8UC3);
	Mat Col_H_STI(width,frame-1,CV_64FC1);
	Mat Row_H_STI(height,frame-1, CV_64FC1);
	Mat Col_threshold(width, frame - 1, CV_64FC1);
	Mat Row_threshold(height,frame - 1, CV_64FC1);
	Mat image1, image2;

	int N = round(1+log2(height));
	double cut = 1.0 / N;
	int **hist_p = new int*[N];
	int **hist_t=new int*[N];
	for (int i = 0;i < N;i++) {
		hist_t[i] = new int[N];
		hist_p[i] = new int[N];
	}

   // namedWindow("Display", WINDOW_NORMAL);
	int i = 0;
	vc.read(image1);
    for(i=0;i<frame;i++)
    {
		//1.copy pixel using center column		
		image1.col(width / 2).copyTo(Col_C_STI.col(i));
		//2.copy pixel using center row	
		for (int j = 0;j < width;j++) {
			Row_C_STI.at<Vec3b>(j, i) = image1.at<Vec3b>(height / 2, j);
		}

        // Get the image from the video file
       
		vc.read(image2);
		if (image2.empty()) break;
        // Stop if there are no more frames in the video
       
	   
		// generate histogram using column
		for (int col = 0; col < width;col++) {

			//clear the histograms
			for (int rrow = 0;rrow < N;rrow++) {
				for (int ccol = 0;ccol < N;ccol++) {
					hist_t[rrow][ccol] = 0;
					hist_p[rrow][ccol] = 0;
				}
			}


			for (int row = 0;row < height;row++) {
				//calculate H(t-1)
				Vec3i color_value;
				color_value[0] = (int)(image1.at<Vec3b>(row, col)[0]);
				color_value[1] = (int)(image1.at<Vec3b>(row, col)[1]);
				color_value[2] = (int)(image1.at<Vec3b>(row, col)[2]);
				Vec2d chrom = chromacity(color_value);
				int r = (int)(chrom[0] / cut);
				if (r == 9) r--;
				int g = (int)(chrom[1] / cut);
				if (g == 9)g--;
				hist_p[r][g]++;

				//calculate H(t)
				color_value[0] = (int)(image2.at<Vec3b>(row, col)[0]);
				color_value[1] = (int)(image2.at<Vec3b>(row, col)[1]);
				color_value[2] = (int)(image2.at<Vec3b>(row, col)[2]);
				chrom = chromacity(color_value);
				r = (int)(chrom[0] / cut);
				if (r == 9) r--;
				g = (int)(chrom[1] / cut);
				if (g == 9)g--;
				hist_t[r][g]++;
			}
			//calculate intersection
			double intersection = 0;
			for (int rr = 0;rr < N;rr++) {
				for (int gg = 0;gg < N;gg++) {
					intersection = intersection + min(hist_p[rr][gg], hist_t[rr][gg]);
				}
			}
			intersection = intersection*1.0 / height;

		//	Col_H_STI.at<double>(col,i) = intersection;

			if (intersection < 0.65)
				Col_threshold.at<double>(col, i) = 0;
			else
				Col_threshold.at<double>(col, i) = 1;
		}

		//generate histogram using row 
		for (int row = 0;row < height;row++) {
			//clear the histograms
			for (int rrow = 0;rrow < N;rrow++) {
				for (int ccol = 0;ccol < N;ccol++) {
					hist_t[rrow][ccol] = 0;
					hist_p[rrow][ccol] = 0;
				}
			}

			for (int col = 0;col < width;col++) {
				//calculate H(t-1)
				Vec3i color_value;
				color_value[0] = (int)(image1.at<Vec3b>(row, col)[0]);
				color_value[1] = (int)(image1.at<Vec3b>(row, col)[1]);
				color_value[2] = (int)(image1.at<Vec3b>(row, col)[2]);
				Vec2d chrom = chromacity(color_value);
				int r = (int)(chrom[0] / cut);
				if (r == 9) r--;
				int g = (int)(chrom[1] / cut);
				if (g == 9)g--;
				hist_p[r][g]++;

				//calculate H(t)
				color_value[0] = (int)(image2.at<Vec3b>(row, col)[0]);
				color_value[1] = (int)(image2.at<Vec3b>(row, col)[1]);
				color_value[2] = (int)(image2.at<Vec3b>(row, col)[2]);
				chrom = chromacity(color_value);
				r = (int)(chrom[0] / cut);
				if (r == 9) r--;
				g = (int)(chrom[1] / cut);
				if (g == 9)g--;
				hist_t[r][g]++;
			}

			//calculate intersection
			double intersection = 0;
			for (int rr = 0;rr < N;rr++) {
				for (int gg = 0;gg < N;gg++) {
					intersection = intersection + min(hist_p[rr][gg], hist_t[rr][gg]);
				}
			}
			intersection = intersection*1.0 / height;

			Row_H_STI.at<double>(row, i) = intersection;
			if (intersection < 0.5)
				Row_threshold.at<double>(row, i) = 0;
			else
				Row_threshold.at<double>(row, i) = 1;
		}
		
		image2.copyTo(image1);

    }

	imshow("center column STI", Col_C_STI);
	imshow("center row STI", Row_C_STI);
	imshow("histogram column STI", Col_H_STI);
	imshow("threshold of STI using column", Col_threshold);
	imshow("histogram row STI", Row_H_STI);
	imshow("threshold of STI using row", Row_threshold);
	waitKey(-1);
}