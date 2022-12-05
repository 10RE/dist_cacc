using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class VehicleSim : MonoBehaviour
{
    public float throttle;
    public int throttleDelay = 20;
    public float drag = 0.05f;

    public float dragNoise = 0.03f;
    public float throttleNoise = 0.1f;

    public CommManager comm;

    private float curThrottle = 0;
    private float curSpeed = 0;

    private float maxThrottle = 30;

    private float prevTime = 0F;
    private float updatePeriod = 0.025F;

    private Queue<float> throttleQueue = new Queue<float>();

    public GameObject vehicleAhead;

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
        else if (t < -maxThrottle)
        {
            throttleQueue.Enqueue(-maxThrottle);
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

    // Update is called once per frame
    void Update()
    {
        throttle = comm.GetThrottle();
        if (Time.time > prevTime + updatePeriod)
        {
            curThrottle = updateThrottle(throttle);
        }
        float drag_affection = curSpeed > 0 ? (drag + Random.Range(0, dragNoise)) * curSpeed * Time.deltaTime : 0;
        float throttle_val = curThrottle + Random.Range(-throttleNoise, throttleNoise);
        throttle_val = curThrottle > 0 ?
            (throttle_val > 0 ? throttle_val : 0) :
            (throttle_val > 0 ? 0 : throttle_val);
        float throttle_affection = throttle_val * Time.deltaTime;
        curSpeed = curSpeed + throttle_affection - drag_affection;
        curSpeed = curSpeed > 0 ? curSpeed : 0;
        transform.Translate(Vector3.forward * Time.deltaTime * curSpeed);
        if (Time.time > prevTime + updatePeriod)
        {
            float curDist = vehicleAhead.transform.position.z - transform.position.z;
            comm.SetCurState(curSpeed, curDist);
        }
    }
}
