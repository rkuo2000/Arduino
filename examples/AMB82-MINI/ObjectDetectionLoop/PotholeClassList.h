#ifndef __POTHOLECLASSLIST_H__
#define __POTHOLECLASSLIST_H__

struct PotholeDetectionItem {
    uint8_t index;
    const char* potholeName;
    uint8_t filter;
};

// List of objects the pre-trained model is capable of recognizing
// Index number is fixed and hard-coded from training
// Set the filter value to 0 to ignore any recognized objects
ObjectDetectionItem itemList[1] = {
{0,  "pothole",          1},
};

#endif
