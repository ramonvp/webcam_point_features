
//OpenCV
#include "opencv2/opencv.hpp"
#include "opencv2/core.hpp"
#include "opencv2/features2d.hpp"

//std
#include <iostream>
#include <cstdlib>
#include <vector>

//consts
const unsigned int MIN_NUM_FEATURES = 300; //minimum number of point features

void compare_orb_masks(const char * filename, cv::Ptr<cv::ORB> & orb_detector, double factor)
{
    cv::Mat img1 = cv::imread( filename );
    cv::Mat img2 = cv::imread( filename );

    std::vector<cv::KeyPoint> point_set1; //set of point features
    cv::Mat descriptor_set1; //set of descriptors, for each feature there is an associated descriptor

    std::vector<cv::KeyPoint> point_set2; //set of point features
    cv::Mat descriptor_set2; //set of descriptors, for each feature there is an associated descriptor

    cv::Mat mask(cv::Mat::zeros(img2.size(),CV_8U));

    mask(cv::Rect(0,0,img2.cols*factor,img2.rows*factor))=1;

    orb_detector->detectAndCompute(img1, cv::noArray(), point_set1, descriptor_set1);
    orb_detector->detectAndCompute(img2, mask, point_set2, descriptor_set2);

    //draw points on the image
    cv::drawKeypoints( img1, point_set1, img1, 255, cv::DrawMatchesFlags::DEFAULT );
    cv::drawKeypoints( img2, point_set2, img2, 255, cv::DrawMatchesFlags::DEFAULT );


    std::cout << "Factor: " << factor << std::endl;
    std::cout << "Num points no mask: " << point_set1.size() << std::endl;
    std::cout << "Num points with mask: " << point_set2.size() << std::endl;
    //********************************************************************

    //show image
    cv::imshow("No Mask", img1);
    cv::imshow("With Mask", img2);

    cv::waitKey();
}

int main(int argc, char *argv[])
{

    cv::Ptr<cv::ORB> orb_detector = cv::ORB::create(); //ORB point feature detector
    orb_detector->setMaxFeatures(MIN_NUM_FEATURES);

    std::string cmd(argv[0]);
    if( cmd.find("compare_orb") != cmd.npos )
    {
      if(argc<2)
      {
          std::cerr << "Missing input image file\n";
          return -1;
      }
      double factor = 1.0;
      if(argc>2) factor = atof(argv[2]);
      compare_orb_masks( argv[1], orb_detector, factor );

      return 0;
    }


    cv::VideoCapture camera; //OpenCV video capture object
    cv::Mat image; //OpenCV image object
    int cam_id; //camera id . Associated to device number in /dev/videoX

    //check user args
    switch(argc)
    {
      case 1: //no argument provided, so try /dev/video0
        cam_id = 0;
        break;
      case 2: //an argument is provided. Get it and set cam_id
        cam_id = atoi(argv[1]);
        break;
      default:
        std::cout << "Invalid number of arguments. Call program as: webcam_capture [video_device_id]. " << std::endl;
        std::cout << "EXIT program." << std::endl;
        break;
    }

    //advertising to the user
    std::cout << "Opening video device " << cam_id << std::endl;

    //open the video stream and make sure it's opened
    if( !camera.open(cam_id) )
    {
        std::cout << "Error opening the camera. May be invalid device id. EXIT program." << std::endl;
        return -1;
    }

    //Process loop. Capture and point feature extraction. User can quit pressing a key
    while(1)
    {
        //Read image and check it. Blocking call up to a new image arrives from camera.
        if(!camera.read(image))
        {
            std::cout << "No image" << std::endl;
            cv::waitKey();
        }

        //**************** Find ORB point fetaures and descriptors ****************************

        std::vector<cv::KeyPoint> point_set; //set of point features
        cv::Mat descriptor_set; //set of descriptors, for each feature there is an associated descriptor

        //clear previous points
        //point_set.clear();

        /* Detects keypoints and computes the descriptors
        virtual void cv::Feature2D::detectAndCompute 	(
            InputArray  	image,
            InputArray  	mask,
            std::vector< KeyPoint > &  	keypoints,
            OutputArray  	descriptors,
            bool  	useProvidedKeypoints = false
          )
        */
        //detect and compute(extract) features
        cv::Mat mask(cv::Mat::ones(image.size(),CV_8U));

        orb_detector->detectAndCompute(image, cv::noArray(), point_set, descriptor_set);
        //orb_detector->detectAndCompute(image, mask, point_set, descriptor_set);

        //draw points on the image
        cv::drawKeypoints( image, point_set, image, 255, cv::DrawMatchesFlags::DEFAULT );

        //********************************************************************

        //show image
        cv::imshow("Output Window", image);

        //Waits 30 millisecond to check if 'q' key has been pressed. If so, breaks the loop. Otherwise continues.
        if( (unsigned char)(cv::waitKey(30) & 0xff) == 'q' ) break;
    }   
}
