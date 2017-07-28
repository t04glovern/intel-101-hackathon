using System.Collections.Generic;
using System.Collections;
using Boomlagoon.JSON;
using UnityEngine;
using System;

public class SensorDataHandler : MonoBehaviour
{

    // Singleton static instance of SensorDataHandler
    public static SensorDataHandler instance;

    public SensorHandler selected;

    public float min = 1.0f;
    public float max = 1.8f;

    public Queue<SensorData> sensorDataQueue1;
    public Queue<SensorData> sensorDataQueue2;
    public Queue<SensorData> sensorDataQueue3;
    public Queue<SensorData> sensorDataQueue4;
    string Url;

    void Awake()
    {
        instance = this;
    }

    // Use this for initialization
    void Start()
    {
        Url = "http://nathanglover.com:3000/api/bledata";
        sensorDataQueue1 = new Queue<SensorData>();
        sensorDataQueue2 = new Queue<SensorData>();
        sensorDataQueue3 = new Queue<SensorData>();
        sensorDataQueue4 = new Queue<SensorData>();

        GetData();

        InvokeRepeating("GetData", 1.0f, 0.05f);
    }
   

    //Invoke this function where to want to make request.
    void GetData()
    {
        //sending the request to url
        WWW www = new WWW(Url);
        StartCoroutine(GetdataEnumerator(www));
    }

    IEnumerator GetdataEnumerator(WWW www)
    {
        //Wait for request to complete
        yield return www;
        if (www.error != null)
        {
            string serviceData = www.text;
            //Data is in json format, we need to parse the Json.
            //Debug.Log(serviceData);
            JSONObject json = JSONObject.Parse(serviceData);
            if (json["sensorName"] == null)
            {
                Debug.Log("No data converted");
            }
            else
            {
                SensorData sensorData = new SensorData();
                //now we can get the values from json of any attribute.
                sensorData.timestamp = json["timestamp"].Str;
                sensorData.sensorName = json["sensorName"].Str;
                sensorData.accX = Convert.ToSingle(json["accX"].Number);
                sensorData.accY = Convert.ToSingle(json["accY"].Number);
                sensorData.accZ = Convert.ToSingle(json["accZ"].Number);
                sensorData.accMag = Convert.ToSingle(json["accMag"].Number);

                // Scale
                sensorData.accMag = (sensorData.accMag - min) / (max - min);

				sensorData.gyroX = Convert.ToSingle(json["gyroX"].Number);
				sensorData.gyroY = Convert.ToSingle(json["gyroY"].Number);
				sensorData.gyroZ = Convert.ToSingle(json["gyroZ"].Number);

                // Adds data to queue
                SetSensorData(sensorData);
            }
        }
        else
        {
            Debug.Log(www.error);
        }
    }

    public SensorData GetSensorItem(string sensorName) {
        if (sensorName == "right-arm" && sensorDataQueue1.Count > 0)
		{
			return sensorDataQueue1.Dequeue();
		}
		else if (sensorName == "left-arm" && sensorDataQueue2.Count > 0)
		{
			return sensorDataQueue2.Dequeue();
		}
		else if (sensorName == "right-leg" && sensorDataQueue3.Count > 0)
		{
			return sensorDataQueue3.Dequeue();
		}
		else if (sensorName == "left-leg" && sensorDataQueue4.Count > 0)
		{
			return sensorDataQueue4.Dequeue();
		}
        else
        {
            return null;
        }
    }

    void SetSensorData(SensorData sensorData) {
		if (sensorData.sensorName == "right-arm")
		{
			sensorDataQueue1.Enqueue(sensorData);
		}
		else if (sensorData.sensorName == "left-arm")
		{
			sensorDataQueue2.Enqueue(sensorData);
		}
		else if (sensorData.sensorName == "right-leg")
		{
			sensorDataQueue3.Enqueue(sensorData);
		}
		else if (sensorData.sensorName == "left-leg")
		{
			sensorDataQueue4.Enqueue(sensorData);
		}
        else
        {
            Debug.Log("No sensor exists for " + sensorData.sensorName);
        }
    }
}