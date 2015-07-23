#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <iostream>
#include <fstream>
#include "readimage.h"
using namespace std;
using namespace cv;

/** Function Headers */
void detectFaces(Mat frame);

/** Global variables */
String face_cascade_name = "/usr/demos/facedetect/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;
int counter = 0;


int main(int argc, const char* argv[]) {

	  const string command      = argv[1];           // the source file name
cout << command;

	  if(command == "video")
	  {
	    VideoCapture videorecord(0);              // Open input
	    if (!videorecord.isOpened())
	    {
	        printf("Could not open the input video");
	        return -1;
	    }


	    VideoWriter outputVideo;                                        // Open the output

		system("exec rm -r /usr/demos/facedetect/videos/*");

	    int frame_width=   videorecord.get(CV_CAP_PROP_FRAME_WIDTH);
	    int frame_height=   videorecord.get(CV_CAP_PROP_FRAME_HEIGHT);

//	    VideoWriter video("/usr/demos/facedetect/videos/out.mp4",CV_FOURCC('M','P','4','2'),10, Size(frame_width,frame_height),true);

//	    VideoWriter video("/usr/demos/facedetect/videos/out.ogv",CV_FOURCC('t', 'h', 'e', 'o'),10, Size(frame_width,frame_height),true);

	    VideoWriter video("/usr/demos/facedetect/videos/out.avi",CV_FOURCC('M', 'J', 'P', 'G'),10, Size(frame_width,frame_height),true);

	    for(;;){

	        Mat frame;
	        videorecord >> frame;
	        video.write(frame);
	        imwrite( "Frame.jpg", frame );
	        char c = (char)waitKey(33);
	        if( c == 27 )
	        	{
	        	cout << "";
	        	video.release();
	        	break;
	        	}
	     }
	     cout << "Finished writing" << endl;

	    return 0;
	  }
	//-- 1. Load the cascades
	if (!face_cascade.load(face_cascade_name)) {
		printf("You need to have haarcascade_frontalface_alt.xml at /usr/demos/facedetect folder");
		return -1;
	};
	Mat frame;
	system("exec rm -r /usr/demos/facedetect/images/*");
	for (counter = 0; counter < 10; counter++)
	//	while(1)
			{
		//	counter ++;
		printf("Take #: %d \n", counter);
		VideoCapture capture(0); //0=default, -1=any camera, 1..99=your camera
		capture.set(CV_CAP_PROP_FRAME_WIDTH, 320);
		capture.set(CV_CAP_PROP_FRAME_HEIGHT, 240);
		if (!capture.isOpened()) {
			puts("No camera found");
			return -1;
		}
		capture >> frame;

		if (!frame.empty()) {
			try
			{
			detectFaces(frame);
			}catch(cv::Exception ex)
			{
				cout << ex.msg;
			}
		} else {
			printf(" --(!) No captured frame -- Break!");
			break;
		}
		int c = waitKey(10);
		capture.release();
		// sleep(2);
		if ((char) c == 'c') {
			capture.release();
			break;
		}
	}
	return 0;
}

/**
 * @function detectFaces
 */
void detectFaces(Mat frame) {
	std::vector<Rect> faces;
	ofstream faceInfo;
	Mat frame_gray;

	std::stringstream temp;
	std::stringstream ss1;

	temp.str("");
	temp << "images/picture" << counter << ".png";

	imwrite(temp.str(), frame);

	ss1 << "{\"id\":" << counter << ",\"type\": \"original\",\"path\":"
			<< "\"" << temp.str() << "\"" << "}";

	//Update file for reading from NodeJS
	faceInfo.open("original.json");
	faceInfo << ss1.str();
	// close the opened file.
	faceInfo.close();

	cvtColor(frame, frame_gray, CV_BGR2GRAY);

	equalizeHist(frame_gray, frame_gray);
	//-- Detect faces
	face_cascade.detectMultiScale(frame_gray, faces, 1.1, 2,
			0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

	ss1.str("");
	ss1 << "{\"id\":" << counter << ",\"type\": \"facefound\", [";

	for (int i = 0; i < (int) faces.size(); i++) {
		Rect roi(faces[i].x - 10, faces[i].y - 10, faces[i].width + 10,
				faces[i].height + 10);
		Mat image_roi = frame(roi);
		temp.str("");
		temp << "images/facefound" << counter << "_" << i << ".png";
		imwrite(temp.str(), image_roi);

		ss1 << "{\"path\":"
				<< "\"" << temp.str() << "\"}" << ",";



		temp.str("");
		cout << temp.str();
		temp << "#" << i;

		putText(frame, temp.str(),
				cvPoint(faces[i].x + faces[i].width * 0.5,
						faces[i].y + faces[i].height * 0.5),
				FONT_HERSHEY_COMPLEX_SMALL, 0.5, cvScalar(100, 100, 250), 1,
				CV_AA);
		Point center(faces[i].x + faces[i].width * 0.5,
				faces[i].y + faces[i].height * 0.5);
		ellipse(frame, center,
				Size(faces[i].width * 0.5, faces[i].height * 0.5), 0, 0, 360,
				Scalar(255, 0, 255), 2, 8, 0);
		//Mat faceROI = frame_gray( faces[i] );

	}
	ss1 << "{\"path\":\"\"}]}";

	if((int) faces.size() > 0)
	{
		//Update file for reading from NodeJS
		faceInfo.open("facefound.json");
		faceInfo << ss1.str();
		// close the opened file.
		faceInfo.close();
	}

	//Picture with eclipse marking the face
	temp.str("");
	temp << "images/allfaces" << counter << ".png";
	imwrite(temp.str(), frame);

	ss1.str("");
	ss1 << "{\"id\":" << counter << ",\"type\": \"allfaces\", \"path\":"
			<< "\"" << temp.str() << "\"" << "}";

	//Update file for reading from NodeJS
	faceInfo.open("allface.json");
	faceInfo << ss1.str();
	// close the opened file.
	faceInfo.close();

}
