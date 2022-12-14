using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class VehicleSim : MonoBehaviour
{
    public int vid = 0;

    public float throttle;
    private int throttleDelay = 20;
    private float drag = 0.0053219f;
    //private float ground_drag = 1.76f;

    private float mass = 30000;

    private float dragNoise = 0.02f;
    private float throttleNoise = 0.1f;

    public CommManager comm;

    private float curThrottle = 0;
    private float curSpeed = 0;

    private float maxThrottle = 1.3f;
    private float maxBrake = 6f;

    private float prevTime = 0F;
    private float updatePeriod = 0.025F;

    private Queue<float> throttleQueue = new Queue<float>();

    private float prevWriteTime = 0f;
    private float writePeriod = 1f;

    public GameObject vehicleAhead;

    public Text text;

    public bool useComm = true;

    float updateThrottle(float t)
    {
        if (t < 0)
        {
            throttleQueue.Clear();
        }
        else
        {
            while (throttleQueue.Count < throttleDelay)
            {
                throttleQueue.Enqueue(0);
            }
        }
        if (t > maxThrottle)
        {
            throttleQueue.Enqueue(maxThrottle);
        }
        else if (t < -maxBrake)
        {
            throttleQueue.Enqueue(-maxBrake);
        }
        else
        {
            throttleQueue.Enqueue(t);
        }
        return throttleQueue.Dequeue();
    }

    // Start is called before the first frame update
    void Start()
    {
        
    }

    void UpdateText(float speed, float throttle, float dist)
    {
        text.text = gameObject.name + "\n";
        text.text += dist.ToString() + " m\n";
        text.text += computeIdealDistanceCACC(speed) + "m\n";
        text.text += speed.ToString() + " m/s\n";
        text.text += throttle.ToString() + " m/s2\n";
    }

    float computeIdealDistanceACC(float speed)
    {
        return 3 * (speed / 6 + 1) * (speed / 6 + 1) + 5;
    }

    float computeIdealDistanceCACC(float speed) {
        float vehicleLen = 15.5f;
        float commuLatency = 0.5f;
        float lower_safe_boundary_with_comm_latency = 3.65f * commuLatency * commuLatency + vehicleLen * 0.5f;
        float ret = 0;
        if (speed < 10f) {
            ret = 0.6f * vehicleLen + 0.6f * vehicleLen /(1f + Mathf.Exp(-((speed - 3.5f))));
        }
        else {
            ret = 1.2f * vehicleLen + 0.4f * vehicleLen /(1f + Mathf.Exp(-(speed - 13.5f)));
        }

        ret = ret < lower_safe_boundary_with_comm_latency ? lower_safe_boundary_with_comm_latency : ret;

        return ret;
    }

    // Update is called once per frame
    void Update()
    {

        if (useComm)
        {
            throttle = comm.GetThrottle();
        }

        // if (Time.time > prevTime + updatePeriod)
        // {
        //     curThrottle = updateThrottle(throttle);
        // }
        curThrottle = throttle;
        float drag_affection = curSpeed > 0 ? (((drag + Random.Range(0, dragNoise)) * curSpeed * curSpeed / mass + 0.05f * curSpeed ) * Time.deltaTime) : 0;
        float throttle_val = curThrottle + Random.Range(-throttleNoise, throttleNoise);
        throttle_val = curThrottle > 0 ?
            (throttle_val > 0 ? throttle_val : 0) :
            (throttle_val > 0 ? 0 : throttle_val);
        float throttle_affection = throttle_val * Time.deltaTime;
        curSpeed = curSpeed + throttle_affection - drag_affection;
        curSpeed = curSpeed > 0 ? curSpeed : 0;
        transform.Translate(Vector3.forward * Time.deltaTime * curSpeed);
        float curDist = vehicleAhead.transform.position.z - transform.position.z - transform.localScale.z;
        if (Time.time > prevTime + updatePeriod)
        {
            //float curDist = vehicleAhead.transform.position.z - transform.position.z - transform.localScale.z;
            UpdateText(curSpeed, curThrottle, curDist);
            if (useComm)
            {
                // if (vid == 1) {
                //     Debug.Log(curSpeed.ToString() + " " + curDist.ToString());
                // }
                comm.SetCurState(curSpeed, curDist);
            }
            prevTime = Time.time;
        }

        if (Time.time > prevWriteTime + writePeriod)
        {
            GetComponent<FileWriter>().Write(curSpeed.ToString() + "," + curDist.ToString() + "," + curThrottle.ToString());
            prevWriteTime = Time.time;
        }

        
    }
}
