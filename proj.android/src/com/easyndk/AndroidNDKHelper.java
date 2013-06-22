package com.easyndk;

import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

import android.util.Log;
import org.json.JSONException;
import org.json.JSONObject;

public class AndroidNDKHelper
{
	private static Object callHandler = null;

	private static native void cppNativeCallHandler(String json);

	private static String __CALLED_METHOD__ = "calling_method_name";
	private static String __CALLED_METHOD_PARAMS__ = "calling_method_params";

    public static void setNDKReceiver(Object callReceiver) {
		callHandler = callReceiver;
	}
	
	public static void sendMessageWithParameters(String methodToCall, JSONObject paramList)
	{
		JSONObject obj = new JSONObject();
		try {
			obj.put(__CALLED_METHOD__, methodToCall);
			obj.put(__CALLED_METHOD_PARAMS__, paramList);
			cppNativeCallHandler(obj.toString());
		} catch (Exception e) {
			throw new IllegalStateException(e);
		}
	}
	
	public static String receiveCppMessage(String json) {
		if (json != null)
		{
			try
			{
				JSONObject obj = new JSONObject(json);
				if (obj.has(__CALLED_METHOD__))
				{
					String methodName = obj.getString(__CALLED_METHOD__);
					JSONObject methodParams = null;
					
					try
					{
						methodParams = obj.getJSONObject(__CALLED_METHOD_PARAMS__);
					}
					catch (JSONException e)
					{
						// if we are finding trouble in looking for params, params might be null
					}
					
					try {
						Method m = AndroidNDKHelper.callHandler.getClass().getMethod(methodName, JSONObject.class);

                        JSONObject retParamsJson =
                                (JSONObject) m.invoke(AndroidNDKHelper.callHandler, methodParams);
                        if (retParamsJson == null) {
                            retParamsJson = new JSONObject();
                        }
                        Log.v("EasyNDK", "retParamsJson: " + retParamsJson.toString());
                        return retParamsJson.toString();
					}
					catch (NoSuchMethodException e) {
                        throw new IllegalStateException(e);
					}
					catch (IllegalArgumentException e) {
                        Log.e("EasyNDK", "receiveCppMessage raised exception", e);
					} catch (InvocationTargetException e) {
                        Log.e("EasyNDK", "receiveCppMessage raised exception", e);
                    } catch (IllegalAccessException e) {
                        Log.e("EasyNDK", "receiveCppMessage raised exception", e);
                    }
                } else {
                    Log.e("EasyNDK", "receiveCppMessage raised exception: Wrong JSON structure");
                }
			}
			catch (JSONException e) {
                Log.e("EasyNDK", "receiveCppMessage raised exception", e);
			}
        }
        return null;
	}
}
