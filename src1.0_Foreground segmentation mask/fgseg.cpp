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
bgs::bgs(double threshold,bool rgb)
{
	_rgb=rgb;

	_threshold=threshold;

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
	}
	else{
		_bkg = cv::Mat::zeros(Frame.size(), Frame.type());
		_bkg = Frame.clone();

	}

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
	// init Shadow Mask (currently Shadow Detection not implemented)
	_bgsmask.copyTo(_shadowmask); // creates the mask (currently with bgs)

	//ADD YOUR CODE HERE
	//...
	absdiff(_bgsmask, _bgsmask, _shadowmask);// currently void function mask=0 (should create shadow mask)
	//...

	absdiff(_bgsmask, _shadowmask, _fgmask); // eliminates shadows from bgsmask
}

//ADD ADDITIONAL FUNCTIONS HERE




