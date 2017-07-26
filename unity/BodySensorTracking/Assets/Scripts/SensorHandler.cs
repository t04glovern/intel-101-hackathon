using UnityEngine;
using UnityEngine.UI;
using TMPro;

public class SensorHandler : MonoBehaviour {

	private Color colorEnd = Color.red;
	private Color colorStart = Color.green;
	private Renderer rend;
    private TextMeshPro sensorInfo;
    public SensorData sensorData;
    private SensorDataHandler sensorDataHandler;

    public string sensor_name = "demo";

    public Text panelHeadingText;
    public Text timestampText;
    public Text averageRatingText;

	// Use this for initialization
	void Start () {
        sensorDataHandler = SensorDataHandler.instance;
		rend = GetComponent<Renderer>();
        sensorInfo = GetComponentInChildren<TextMeshPro>();
        sensorInfo.gameObject.SetActive(false);
	}
	
	// Update is called once per frame
	void Update () {
        if (sensorData != null) {
            rend.material.color = Color.Lerp(colorStart, colorEnd, sensorData.currentMag);
            UpdateTextFields();
            sensorData = null;
        } else {
            sensorData = sensorDataHandler.GetSensorItem(sensor_name);
        }
	}

    // Handles the clicking of the sensor
	void OnMouseDown() {
        sensorInfo.gameObject.SetActive(!sensorInfo.gameObject.activeInHierarchy);
        if (sensorData != null) {
            UpdateTextFields();
        }
	}

    void UpdateTextFields() {
        if (sensorInfo.gameObject.activeInHierarchy) {
            sensorInfo.text = sensorData.sensorName + "\nRating: " + sensorData.currentMag.ToString();
            if (sensorDataHandler.selected == null) {
                sensorDataHandler.selected = this;
            }
            if (sensorDataHandler.selected == this)
            {
				panelHeadingText.text = sensorData.sensorName + " Sensor";
				timestampText.text = sensorData.timestamp;
				averageRatingText.text = sensorData.currentMag.ToString();
            }
        } else {
            if (sensorDataHandler.selected == null) {
				panelHeadingText.text = "";
				timestampText.text = "-";
				averageRatingText.text = "-";
				sensorInfo.text = "";
            }
        }
    }
}