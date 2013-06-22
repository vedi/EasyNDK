//
//  NDKHelper.h
//  EasyNDK-for-cocos2dx
//
//  Created by Amir Ali Jiwani on 23/02/2013.
//
//

#ifndef __EasyNDK_for_cocos2dx__NDKHelper__
#define __EasyNDK_for_cocos2dx__NDKHelper__

#include <iostream>
#include "cocos2d.h"
#include <string>
#include <vector>
#include "jansson.h"
#include "NDKCallbackData.h"
USING_NS_CC;
using namespace std;

class NDKHelper {
public :
    static CCObject *sendMessageWithParams(string methodName, CCObject *methodParams);
    static void addSelector(char const *groupName, char const *name, CCObject *target, SEL_CallFuncO selector);
    static void removeSelectorsInGroup(char const *groupName);
    static void printSelectorList();
    static CCObject *getCCObjectFromJson(json_t *obj);
    static json_t *getJsonFromCCObject(CCObject* obj);
    static void handleMessage(json_t *methodName, json_t* methodParams);
private :
    static vector<NDKCallbackData> selectorList;
    static void removeAtIndex(int index);
};

#endif /* defined(__EasyNDK_for_cocos2dx__NDKHelper__) */
