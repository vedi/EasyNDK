//
// Created by Fedor Shubin on 6/22/13.
//



#ifndef __JsonHelper_H_
#define __JsonHelper_H_

#include <iostream>
#include "cocos2d.h"
#include "jansson.h"


class JsonHelper {

public:
    static CCObject *getCCObjectFromJson(json_t *obj);

    static json_t *getJsonFromCCObject(CCObject* obj);
};


#endif //__JsonHelper_H_
