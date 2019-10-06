#ifndef MARKERDETECTOR_H
#define MARKERDETECTOR_H

#include <opencv/cv.hpp>
using namespace cv;

class markerDetector
{
public:
   ~markerDetector();
    markerDetector(uint16_t col , uint16_t rows);
    markerDetector();

    void isVisable();
    void getPixelPos(Mat &frame , double *x , double *y);

private:

    std::vector<Point2f>  lut;
    std::vector<uint32_t> index;
    Point2f CM;
    Mat frame_Thr;
    Mat frameGray;

    int cols;
    int rows;

    void CM_CALC(std::vector<uint32_t> index, Point2f &CM);
    void IdxLUT(std::vector<Point2f> &p, uint cols , uint rows);
    void extractPixelLocs(Mat &input ,std::vector<uint32_t> &Index);

};

#endif // MARKERDETECTOR_H
