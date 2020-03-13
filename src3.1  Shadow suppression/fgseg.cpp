/* Applied Video Sequence Analysis (AVSA)
 *
 *	LAB1.0: Background Subtraction - Unix version
 *	fgesg.cpp
 *
 * 	Authors: José M. Martínez (josem.martinez@uam.es), Paula Moral (paula.moral@uam.es) & Juan Carlos San Miguel (juancarlos.sanmiguel@uam.es)
 *	VPULab-UAM 2020
 */

#include <opencv2/opencv.hpp>
#include "fgseg.hpp"

using namespace fgseg;

//default constructor
bgs::bgs(double threshold, double alpha, bool selective_bkg_update, int threshold_ghosts2, bool rgb)
{
	_rgb=rgb;

	_threshold=threshold;

	_alpha = alpha;

	_selective_bkg_update = selective_bkg_update;

	_threshold_ghosts2 = threshold_ghosts2;

}

//default destructor
bgs::~bgs(void)
{
}



//method to initialize bkg (first frame - hot start)
void bgs::init_bkg(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color

		_bkg = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		//ADD YOUR CODE HERE
		//...
		_bkg = Frame.clone();
		//...
		counter = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
	}
	else{
		_bkg = cv::Mat::zeros(Frame.size(), Frame.type());
		counter = cv::Mat::zeros(Frame.size(), Frame.type());
		_bkg = Frame.clone();
	}
	//counter = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
}

//method to perform BackGroundSubtraction
void bgs::bkgSubtraction(cv::Mat Frame)
{

	if (!_rgb){
		cvtColor(Frame, Frame, COLOR_BGR2GRAY); // to work with gray even if input is color
		Frame.copyTo(_frame);

		_diff = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		_bgsmask = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		//ADD YOUR CODE HERE
		//...

		absdiff(_frame,_bkg,_diff);
		threshold(_diff, _bgsmask, _threshold, 255,THRESH_BINARY);
		progressiveupdate(); //call a function for a progressive update

		//...
	}
	else{
		Frame.copyTo(_frame);

		_diff = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		_bgsmask = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); // void function for Lab1.0 - returns zero matrix
		_tempbgmask = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1); //temp bgmask for calculations per channel
		int num_channels = 3; //number of channels in RGB
		Mat bgr_frame[num_channels]; //mat to split frame into 3 channels
		Mat bgr_bkg[num_channels]; //mat to split background into 3 channels
		Mat diff_channel[num_channels]; //mat to calculate difference per channel
		Mat bgsmask_channel[num_channels]; //mat to bgsmask per channel

		split(_frame,bgr_frame); // split frame into RGB channels
		split(_bkg,bgr_bkg); // split background into RGB channels

		for(int i=0;i<num_channels;i++)
		{
			absdiff(bgr_frame[i],bgr_bkg[i],diff_channel[i]);
			threshold(diff_channel[i], _tempbgmask, _threshold, 255,THRESH_BINARY);
			bgsmask_channel[i]=_tempbgmask;
		}

		merge(bgsmask_channel,3,_bgsmask);
		merge(diff_channel,3,_diff);
	    }

}

//method to detect and remove shadows in the BGS mask to create FG mask
void bgs::removeShadows()
{
	cv::Mat _framehsv;
	cv::Mat _bkghsv;
	_shadowmask = Mat::zeros(Size(_frame.cols,_frame.rows), CV_8UC1);
    _fgmask = Mat::zeros(Size(_frame.cols,_frame.rows), CV_8UC1);
	cvtColor(_frame, _framehsv, COLOR_BGR2HSV);
	cvtColor(_bkg, _bkghsv, COLOR_BGR2HSV);

	int rows = _frame.rows;
	int cols = _frame.cols;

	Mat hsv_img[3]; //to save frame per HSV channels
	Mat hsv_background[3]; //to save background per HSV channels
	split(_framehsv,hsv_img);
	split(_bkghsv,hsv_background);

	_bgsmask.copyTo(_fgmask);

	// parameters of the algorithm
	const double ShadowDetetionAlpha_ = 0.4; //alpha value
	const double ShadowDetetionBeta_ = 0.9; //beta value
	const int ShadowDetection_S_ = 100; // threshold for saturation
	const int ShadowDetection_H_ = 100; // threshold for hue

	double V_ratio = 0.0; // the following set of var needed for the loop
	int S_diff = 0;
	int H_distance1 = 0;
	int H_distance2 = 0;
	int H_min_distance = 0;

			for(int i = 0; i < rows; ++i)
			{
				for(int j = 0; j < cols; ++j)
				{
					V_ratio = (double)hsv_img[2].at<uchar>(i,j) / (double)hsv_background[2].at<uchar>(i,j);
					S_diff = abs((int)hsv_img[1].at<uchar>(i,j) - (int)hsv_background[1].at<uchar>(i,j));
					H_distance1 = abs((int)hsv_img[0].at<uchar>(i,j) - (int)hsv_background[0].at<uchar>(i,j));
					H_distance2 = 360 - H_distance1;
					H_min_distance = H_distance1 < H_distance2 ? H_distance1 : H_distance2;

					if(V_ratio >= ShadowDetetionAlpha_ && V_ratio <= ShadowDetetionBeta_
						&& S_diff <= ShadowDetection_S_
						&& H_min_distance <= ShadowDetection_H_)
					{
						_shadowmask.at<uchar>(i, j) = 255;
						_fgmask.at<uchar>(i, j) =abs((double)_bgsmask.at<uchar>(i, j)*((255-(double)_shadowmask.at<uchar>(i, j))/255));
					}
				}
			}
}

//ADD ADDITIONAL FUNCTIONS HERE

// method for progressive update
void bgs::progressiveupdate()
{
	if(!_selective_bkg_update){
		_bkg =_alpha*_frame+(1-_alpha)*_bkg;
	}
	else{
		cv::Mat fgLogicalMask = Mat::zeros(Size(_frame.cols,_frame.rows), CV_8UC1);
		fgLogicalMask = _bgsmask/ 255;
		cv::Mat bgLogicalMask =(255-_bgsmask)/ 255;

		// Counter for pixels belong to bg
		// Counter for pixels belong to fg
		counter = (counter + fgLogicalMask).mul(fgLogicalMask);
		//  logical mask if pixels exceeds the threshold value
		cv::Mat counter_lg_mask = (counter > _threshold_ghosts2)/ 255;
		// update frame if pixels exceeds the threshold value
		cv::Mat update = counter_lg_mask.mul(_frame);
		// inverse of logical mask
		cv::Mat inv_counter_lg_mask = (1-counter_lg_mask);

		cv::Mat upd_bkg = _bkg.mul(inv_counter_lg_mask);
		// update background
		_bkg = update + upd_bkg;

		cv::Mat _updatebkg = _alpha*(_frame)+(1-_alpha)*_bkg;
		_bkg = bgLogicalMask.mul(_updatebkg) + fgLogicalMask.mul(_bkg);
	}
}

