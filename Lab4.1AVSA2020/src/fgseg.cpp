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

		_bkg.copyTo(_mean);
		_bkg.copyTo(_sigma);
		_sigma=INIT_SIGMA_SQUARED;
	}
	else{
		cvtColor(Frame, Frame, CV_BGR2HSV);

		cout << "Colour currently not supported" << endl;
		exit(1);
	}
	counter = Mat::zeros(Size(Frame.cols,Frame.rows), CV_8UC1);
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

		//absdiff(_frame,_bkg,_diff);
		//threshold(_diff, _bgsmask, _threshold, 255,THRESH_BINARY);
		//progressiveupdate();

		new_fgmask(_frame);
		absdiff(_frame,_bkg,_diff);
		threshold(_diff, _bgsmask, _threshold, 255,THRESH_BINARY);
		//_bgsmask = _fgmask<=0;

		//threshold(_diff, _bgsmask, _threshold, 255,THRESH_BINARY);
		progressiveupdate();
		//...
	}
	else{


		 cout << "Colour currently not supported" << endl;
		 exit(1);
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


void bgs::progressiveupdate()
{
	if(!_selective_bkg_update){
		_bkg =_alpha*_frame+(1-_alpha)*_bkg;
	}
	else{
		cv::Mat fgLogicalMask = _bgsmask/ 255;
	    cv::Mat bgLogicalMask =(255-_bgsmask)/ 255;

	    counter = Mat::zeros(Size(_frame.cols,_frame.rows), CV_8UC1);

	    // Counter for pixels belong to bg
	    counter = counter.mul(fgLogicalMask);
	    // Counter for pixels belong to fg
	    counter = counter + fgLogicalMask;

	    //  logical mask if pixels exceeds the threshold value
	    cv::Mat counter_lg_mask = (counter > _threshold_ghosts2)/ 255;
	    // update frame if pixels exceeds the threshold value
	    cv::Mat update = counter_lg_mask.mul(_frame);
	    // inverse of logical mask
	    cv::Mat inv_counter_lg_mask = (1-counter_lg_mask);

	    cv::Mat upd_bkg = _bkg.mul(inv_counter_lg_mask);

	    _bkg = update + upd_bkg;

		cv::Mat _updatebkg = _alpha*(_frame)+(1-_alpha)*_bkg;
		_bkg = bgLogicalMask.mul(_updatebkg) + fgLogicalMask.mul(_bkg);
	}
}

void bgs::new_fgmask(cv::Mat frame)
{
	    cv::Mat mask(frame.rows,frame.cols, CV_8UC1);
		double mean;
		double sigma;
		int pvalue=0;
		int maskvalue = 0;
		int std = 0.5;
        double alpha_gaussian = 0.05;

		for(int row = 0;row<frame.rows;row++)
		{
			for(int column = 0;column<frame.cols;column++)
			{
				pvalue = frame.at<uchar>(row, column);
				mean = _mean.at<uchar>(row, column);
				sigma = sqrt(_sigma.at<uchar>(row, column));

				if(abs(pvalue-mean) > std*sigma)
				{
					maskvalue = 1;
				}
				else
				{
					maskvalue = 0;
				}
				mask.at<uchar>(row,column)=maskvalue;


				//Update sigma_2 and mean
				if(0 == maskvalue)
				{
					_mean.at<uchar>(row, column)=alpha_gaussian*pvalue+(1-alpha_gaussian)*mean;
				}
					_sigma.at<uchar>(row, column)=alpha_gaussian*pow((pvalue-mean),2)+(1-alpha_gaussian)*_sigma.at<uchar>(row, column);

			}
		}
		absdiff(_frame,_mean,_diff);
		_bkg = (_diff/_sigma)<=0;
		//_fgmask = (_diff/_sigma)>0;
}
