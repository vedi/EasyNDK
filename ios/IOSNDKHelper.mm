//
//  IOSNDKHelper.cpp
//  EasyNDK-for-cocos2dx
//
//  Created by Amir Ali Jiwani on 23/02/2013.
//
//

#import "IOSNDKHelper.h"
#import "NDKHelper.h"

#pragma mark -
#pragma mark Private To IOSNDKHelper.mm

static NSObject *helperInstance = nil;

void IOSNDKHelperImpl::setNDKReceiver(void *receiver)
{
    helperInstance = (NSObject*) receiver;
}

NSObject *getHelperInstance()
{
    return helperInstance;
}

#pragma mark -
#pragma mark IOSNDKHelperImpl Methods

json_t * IOSNDKHelperImpl::receiveCPPMessage(json_t *methodName, json_t *methodParams)
{
    NSObject *receiver = getHelperInstance();
    if (receiver == nil) {
        return NULL;
    }
    
    if (methodName == NULL) {
        return  NULL;
    }

    const char* methodCalled = json_string_value(methodName);
    NSString *methodCalledStr = [NSString stringWithFormat:@"%@:", [NSString stringWithUTF8String:methodCalled]];
    
    SEL selectorToBeCalled;
    
    @try
    {
        selectorToBeCalled = NSSelectorFromString(methodCalledStr);
        
        // Return from message if the selector won't respond to our receiver
        if (![receiver respondsToSelector:selectorToBeCalled]) {
            NSLog(@"Receiver won't respond to selector : %@", methodCalledStr);
            return NULL;
        }
    }
    @catch (NSException * e)
    {
        NSLog(@"Exception trying to find selector to be called : %@", methodCalledStr);
        return NULL;
    }
    
    if (methodParams != NULL) {
        // Convert the parameters into NSDictionary
        char* jsonStrLocal = json_dumps(methodParams, JSON_COMPACT | JSON_ENSURE_ASCII);
        NSString *methodParamsJson = [[NSString alloc] initWithUTF8String:jsonStrLocal];
        free(jsonStrLocal);
        
        NSData *jsonData = [methodParamsJson dataUsingEncoding:NSUTF8StringEncoding];

        //parse out the json data
        NSError* error = nil;
        NSDictionary* json = [NSJSONSerialization
                              JSONObjectWithData:jsonData
                              options:kNilOptions
                              error:&error];

        [methodParamsJson release];
        // If parameters are available call the respective selector with parameters
        if (error == nil)
        {
            NSObject* retParams = [receiver performSelector:selectorToBeCalled withObject:json];

            if (retParams == nil) {
                return nil;
            }

            NSError *error = nil;
            NSData *jsonData = [NSJSONSerialization
                    dataWithJSONObject:retParams
                               options:NSJSONWritingPrettyPrinted
                                 error:&error];

            if (error != nil) {
                return NULL;
            }

            NSString *jsonPrmsString = [[NSString alloc] initWithData:jsonData
                                                             encoding:NSUTF8StringEncoding];

            json_error_t jerror;
            json_t *retParamsJson = json_loads([jsonPrmsString UTF8String], 0, &jerror);

            if (!retParamsJson) {
                fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
                return NULL;
            }

            [jsonPrmsString release];

            return retParamsJson;
        }
        else
        {
            return NULL;
        }
    }
    else
    {
        [receiver performSelector:selectorToBeCalled withObject:[NSDictionary dictionary]];
    }
}

#pragma mark -
#pragma mark IOSNDKHelper Implementation

@implementation IOSNDKHelper

+(void)sendMessage:(NSString *)methodName withParameters:(NSDictionary *)params
{
    json_t* jsonMethod = json_string([methodName UTF8String]);
    json_t *jsonPrms = NULL;
    
    if (params != nil)
    {
        NSError *error = nil;
        NSData *jsonData = [NSJSONSerialization
                            dataWithJSONObject:params
                            options:NSJSONWritingPrettyPrinted
                            error:&error];
        
        if (error != nil)
            return;
        
        NSString *jsonPrmsString = [[NSString alloc] initWithData:jsonData
                                                         encoding:NSUTF8StringEncoding];
        
        json_error_t jerror;
        jsonPrms = json_loads([jsonPrmsString UTF8String], 0, &jerror);
        
        if (!jsonPrms) {
            fprintf(stderr, "error: on line %d: %s\n", jerror.line, jerror.text);
            return;
        }
        
        [jsonPrmsString release];
    }
    
    NDKHelper::handleMessage(jsonMethod, jsonPrms);
    json_decref(jsonMethod);
    
    if (jsonPrms)
        json_decref(jsonPrms);
}

+ (void)setNDKReceiver:(NSObject*)receiver
{
    IOSNDKHelperImpl::setNDKReceiver((void*) receiver);
}
@end