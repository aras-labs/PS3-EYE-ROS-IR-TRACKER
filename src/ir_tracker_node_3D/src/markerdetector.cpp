#include "../include/markerdetector.h"


markerDetector::~markerDetector()
{

}

markerDetector::markerDetector(uint16_t col, uint16_t rows)
{

}

markerDetector::markerDetector()
{
    this->cols = 640;
    this->rows =480;

    IdxLUT(lut,cols,rows);

}

void markerDetector::getPixelPos(Mat &frame, double *x, double *y)
{

    //cvtColor(frame,frameGray,CV_BGR2GRAY);

    threshold(frame,frame_Thr,150,255,CV_THRESH_BINARY);
    //imshow("Captured  thershold",frame_Thr);

    extractPixelLocs(frame_Thr,index);

    CM_CALC(index , CM);
    index.clear();
    //qDebug() << CM.x << "," << CM.y;

    *x=CM.x;
    *y=CM.y;

}

void markerDetector::CM_CALC(std::vector<uint32_t> index, Point2f &CM){
    if(index.size()){
        CM.x=0;
        CM.y=0;

        for(uint32_t i =0 ; i<index.size() ; i++){
            CM.x = CM.x+this->lut[index[i]].x;
            CM.y = CM.y+this->lut[index[i]].y;
        }
        CM.x = CM.x/index.size();
        CM.y = CM.y/index.size();
    }else{
        CM.x = -1;
        CM.y = -1;
    }
}

void markerDetector::IdxLUT(std::vector<Point2f> &p, uint cols , uint rows){

    for(uint32_t i =0 ; i < cols*rows ; i++){
        p.push_back( Point(i%cols,(uint16_t)(i/cols)) );
    }

}

void markerDetector::extractPixelLocs(Mat &input ,std::vector<uint32_t> &Index){


    uint rows=input.rows;
    uint cols=input.cols;


    if(input.isContinuous()){

        uint32_t index = cols*rows;
        uchar *p = input.ptr(0);

        for(uint32_t i=0 ; i<index ; i++)
            if(p[i])
                Index.push_back(i);                           //Save all the white pixels indexes
    }else{

        for(uint32_t i=0 ; i<rows ; i++){

            uchar *p = input.ptr(i);

            for(uint32_t j=0 ; j<cols ; j++)
                if(p[i])
                    Index.push_back(i);
        }
    }

    //qDebug() << "Index Size is:" << Index.size();
}
