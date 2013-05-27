//
//  NDKHelper.cpp
//  EasyNDK-for-cocos2dx
//
//  Created by Amir Ali Jiwani on 23/02/2013.
//
//

#include "NDKHelper.h"

#define __CALLED_METHOD__           "calling_method_name"
#define __CALLED_METHOD_PARAMS__    "calling_method_params"

vector<NDKCallbackData> NDKHelper::selectorList;

void NDKHelper::addSelector(char const *groupName, char const *name, CCObject *target, SEL_CallFuncO selector) {
    NDKHelper::selectorList.push_back(NDKCallbackData(groupName, name, target, selector));
}

void NDKHelper::removeAtIndex(int index) {
    NDKHelper::selectorList[index] = NDKHelper::selectorList.back();
    NDKHelper::selectorList.pop_back();
}

void NDKHelper::removeSelectorsInGroup(char const *groupName) {
    std::vector<int> markedIndices;
    
    for (unsigned int i = 0; i < NDKHelper::selectorList.size(); ++i)
    {
        if (NDKHelper::selectorList[i].getGroup().compare(groupName) == 0)
        {
            markedIndices.push_back(i);
        }
    }
    
    for (unsigned int i = 0; i < markedIndices.size(); ++i)
    {
        NDKHelper::removeAtIndex(markedIndices[i]);
    }
}

CCObject *NDKHelper::getCCObjectFromJson(json_t *obj) {
    if (obj == NULL) {
        return NULL;
    }

    if (json_is_object(obj)) {
        CCDictionary *dictionary = new CCDictionary();
//        CCDictionary::create();

        const char *key;
        json_t *value;

        void *iter = json_object_iter(obj);
        while (iter) {
            key = json_object_iter_key(iter);
            value = json_object_iter_value(iter);

            dictionary->setObject(NDKHelper::getCCObjectFromJson(value)->autorelease(), string(key));

            iter = json_object_iter_next(obj, iter);
        }

        return dictionary;
    }
    else if (json_is_array(obj)) {
        size_t sizeArray = json_array_size(obj);
        CCArray *array = new CCArray();
        //CCArray::createWithCapacity(sizeArray);

        for (unsigned int i = 0; i < sizeArray; i++) {
            array->addObject(NDKHelper::getCCObjectFromJson(json_array_get(obj, i))->autorelease());
        }

        return array;
    }
    else if (json_is_boolean(obj)) {
        CCBool *ccBool = new CCBool(json_boolean(obj));
        //CCString::create(str.str());
        return ccBool;
    }
    else if (json_is_integer(obj)) {
        json_int_t intVal = json_integer_value(obj);

        CCInteger *ccInteger = new CCInteger(intVal);
        //CCInteger::create(str.str());
        return ccInteger;
    }
    else if (json_is_real(obj)) {
        double realVal = json_real_value(obj);

        CCDouble *ccDouble = new CCDouble(realVal);
        //CCDouble::create(str.str());
        return ccDouble;
    }
    else if (json_is_string(obj)) {
        stringstream str;
        str << json_string_value(obj);

        CCString *ccString = new CCString(str.str());
        //CCString::create(str.str());
        return ccString;
    }
    else {
        CC_ASSERT(false);
        return NULL;
    }
}

json_t* NDKHelper::getJsonFromCCObject(CCObject* obj) {
    if (dynamic_cast<CCDictionary *>(obj)) {
        CCDictionary *mainDict = (CCDictionary *) obj;
        CCArray *allKeys = mainDict->allKeys();
        json_t *jsonDict = json_object();

        if (allKeys == NULL ) return jsonDict;
        for (unsigned int i = 0; i < allKeys->count(); i++) {
            const char *key = ((CCString *) allKeys->objectAtIndex(i))->getCString();
            json_object_set_new(jsonDict,
                    key,
                    NDKHelper::getJsonFromCCObject(mainDict->objectForKey(key)));
        }

        return jsonDict;
    }
    else if (dynamic_cast<CCArray *>(obj)) {
        CCArray *mainArray = (CCArray *) obj;
        json_t *jsonArray = json_array();

        for (unsigned int i = 0; i < mainArray->count(); i++) {
            json_array_append_new(jsonArray,
                    NDKHelper::getJsonFromCCObject(mainArray->objectAtIndex(i)));
        }

        return jsonArray;
    }
    else if (dynamic_cast<CCString *>(obj)) {
        CCString *mainString = (CCString *) obj;
        json_t *jsonString = json_string(mainString->getCString());

        return jsonString;
    }
    else if (dynamic_cast<CCInteger *>(obj)) {
        CCInteger *mainInteger = (CCInteger *) obj;
        json_t *jsonInt = json_integer(mainInteger->getValue());

        return jsonInt;
    }
    else if (dynamic_cast<CCDouble *>(obj)) {
        CCDouble *mainDouble = (CCDouble *) obj;
        json_t *jsonReal = json_real(mainDouble->getValue());

        return jsonReal;
    }
    else if (dynamic_cast<CCFloat *>(obj)) {
        CCFloat *mainFloat = (CCFloat *) obj;
        json_t *jsonString = json_real(mainFloat->getValue());

        return jsonString;
    }
    else if (dynamic_cast<CCBool *>(obj)) {
        CCBool *mainBool = (CCBool *) obj;
        json_t *jsonBoolean = json_boolean(mainBool->getValue());

        return jsonBoolean;
    }
    else {
        CC_ASSERT(false);
        return NULL;
    }
}

void NDKHelper::printSelectorList()
{
    for (unsigned int i = 0; i < NDKHelper::selectorList.size(); ++i) {
        string s = NDKHelper::selectorList[i].getGroup();
        s.append(NDKHelper::selectorList[i].getName());
        CCLog(s.c_str());
    }
}

void NDKHelper::handleMessage(json_t *methodName, json_t* methodParams)
{
    if (methodName == NULL)
        return;
    
    const char *methodNameStr = json_string_value(methodName);
    
    for (unsigned int i = 0; i < NDKHelper::selectorList.size(); ++i)
    {
        if (NDKHelper::selectorList[i].getName().compare(methodNameStr) == 0)
        {
            CCObject *dataToPass = NDKHelper::getCCObjectFromJson(methodParams);
            SEL_CallFuncO sel = NDKHelper::selectorList[i].getSelector();
            CCObject *target = NDKHelper::selectorList[i].getTarget();

            (target->*sel)(dataToPass);

            if (dataToPass != NULL) {
                dataToPass->autorelease();
                dataToPass->retain();
            }
            break;
        }
    }
}

#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    #include "../cocos2dx/platform/android/jni/JniHelper.h"
    #include <android/log.h>
    #include <jni.h>
    #define  LOG_TAG    "EasyNDK-for-cocos2dx"

    #define CLASS_NAME "com/easyndk/classes/AndroidNDKHelper"
#endif

#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    #import "IOSNDKHelper-C-Interface.h"
#endif

extern "C"
{
    #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // Method for recieving NDK messages from Java, Android
    void Java_com_easyndk_classes_AndroidNDKHelper_cppNativeCallHandler(JNIEnv* env, jobject thiz, jstring json)
    {
        string jsonString = JniHelper::jstring2string(json);
        const char *jsonCharArray = jsonString.c_str();
        
        json_error_t error;
        json_t *root;
        root = json_loads(jsonCharArray, 0, &error);
        
        if (!root)
        {
            fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
            return;
        }
        
        json_t *jsonMethodName, *jsonMethodParams;
        jsonMethodName = json_object_get(root, __CALLED_METHOD__);
        jsonMethodParams = json_object_get(root, __CALLED_METHOD_PARAMS__);
        
        // Just to see on the log screen if messages are propogating properly
        __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, jsonCharArray);
        
        NDKHelper::handleMessage(jsonMethodName, jsonMethodParams);
        json_decref(root);
    }
    #endif
    
    // Method for sending message from CPP to the targetted platform
    CCObject* sendMessageWithParams(string methodName, CCObject* methodParams, bool async) {
        CCDictionary *retParams = CCDictionary::create();

        if (0 == strcmp(methodName.c_str(), "")) {
            return retParams;
        }

        json_t *toBeSentJson = json_object();
        json_object_set_new(toBeSentJson, __CALLED_METHOD__, json_string(methodName.c_str()));
        
        if (methodParams != NULL)
        {
            json_t* paramsJson = NDKHelper::getJsonFromCCObject(methodParams);
            json_object_set_new(toBeSentJson, __CALLED_METHOD_PARAMS__, paramsJson);
        }

        json_t *retJsonParams = NULL;
        #if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        JniMethodInfo t;
        
		if (JniHelper::getStaticMethodInfo(t,
                                           CLASS_NAME,
                                           "receiveCppMessage",
                                           "(Ljava/lang/String;Z)Ljava/lang/String;"))
		{
            char* jsonStrLocal = json_dumps(toBeSentJson, JSON_COMPACT | JSON_ENSURE_ASCII);
            string jsonStr(jsonStrLocal);
            free(jsonStrLocal);
            
            jstring stringArg1 = t.env->NewStringUTF(jsonStr.c_str());
            jstring retString = (jstring) t.env->CallStaticObjectMethod(t.classID, t.methodID, stringArg1, (jboolean)async);

            t.env->DeleteLocalRef(stringArg1);
			t.env->DeleteLocalRef(t.classID);

		    const char *nativeString = t.env->GetStringUTFChars(retString, 0);
		    string retParamsStr(nativeString);
		    t.env->ReleaseStringUTFChars(retString, nativeString);


            const char *jsonCharArray = retParamsStr.c_str();

            json_error_t error;
            retJsonParams = json_loads(jsonCharArray, 0, &error);

            if (!retJsonParams) {
                fprintf(stderr, "error: on line %d: %s\n", error.line, error.text);
                return retParams;
            }
		}
        #elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        json_t *jsonMessageName = json_string(methodName.c_str());

        if (methodParams != NULL)
        {
            json_t *jsonParams = NDKHelper::getJsonFromCCObject(methodParams);
            retJsonParams = IOSNDKHelperImpl::receiveCPPMessage(jsonMessageName, jsonParams);
            json_decref(jsonParams);
        } else {
            retJsonParams = IOSNDKHelperImpl::receiveCPPMessage(jsonMessageName, NULL);
        }

        if (!retJsonParams) {
            return retParams;
        }

        json_decref(jsonMessageName);
        #endif
        
        json_decref(toBeSentJson);
        return NDKHelper::getCCObjectFromJson(retJsonParams);
    }
}