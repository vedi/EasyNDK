//
//  NDKCallbackData.cpp
//  EasyNDK-for-cocos2dx
//
//  Created by Amir Ali Jiwani on 23/02/2013.
//
//

#include "NDKCallbackData.h"

NDKCallbackData::NDKCallbackData(char const *groupName, char const *name, CCObject *target, SEL_CallFuncO sel) {
    this->groupName = groupName;
    this->name = name;
    this->target = target;
    this->sel = sel;
}

string NDKCallbackData::getName() {
    return this->name;
}

string NDKCallbackData::getGroup() {
    return this->groupName;
}

CCObject *NDKCallbackData::getTarget() {
    return this->target;
}

SEL_CallFuncO NDKCallbackData::getSelector() {
    return this->sel;
}
