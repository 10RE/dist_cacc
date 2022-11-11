using System.Collections;
using UnityEngine;
using System.IO.Ports;
using System.Threading;
using System;



public class CommManager : MonoBehaviour
{
    public String commPort = "COM3";

    private SerialPort serial;

    private Queue outputQueue;
    private Queue inputQueue;

    private float curSpeed = 0;
    private float curDist = 0;

    private float curThrottle = 0;

    private float prevTime = 0F;
    private float sendPeriod = 0.05F;

    private bool valChecked = false;

    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("Start");
        serial = new SerialPort(commPort, 115200);
        serial.Open();
        Debug.Log(serial.IsOpen);
        serial.ReadTimeout = 50;

        outputQueue = Queue.Synchronized(new Queue());
        inputQueue = Queue.Synchronized(new Queue());
        Thread thread = new Thread(ThreadLoop);
        thread.Start();
    }

    private void OnApplicationQuit()
    {
        serial.Close();
    }

    public void Send(string message)
    {
        outputQueue.Enqueue(message);
    }

    public string Read()
    {
        if (inputQueue.Count == 0)
            return null;
        return (string)inputQueue.Dequeue();
    }

    public void ClearInput()
    {
        inputQueue.Clear();
    }

    public void Write(string message)
    {
        serial.Write(message);
        serial.BaseStream.Flush();
    }

    public string Retrieve(int timeout = 50)
    {
        string ret = "";
        char data = ' ';
        try
        {
            data = (char)serial.ReadByte();
            //Debug.Log(data);
            if (data != '$')
            {
                return null;
            }
            ret += data;
            for (int i = 0; i < 5; i++)
            {
                data = (char)serial.ReadByte();
                //Debug.Log(System.Text.Encoding.UTF8.GetBytes("" + data)[0]);
                ret += data;
                //Debug.Log(i);
            }
            //Debug.Log("exit");
            if (ret[5] != '^')
            {
                Debug.LogWarning("End of package check failed");
                return null;
            }
            return ret;
        }
        catch (TimeoutException)
        {
            return null;
        }
    }

    public void ThreadLoop()
    {
        while (true)
        {
            // Send
            if (outputQueue.Count != 0)
            {
                string command = outputQueue.Dequeue().ToString();
                Write(command);
            }
            // Read
            string result = Retrieve();
            if (result != null)
                inputQueue.Enqueue(result);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (Time.time > prevTime + sendPeriod)
        {
            valChecked = false;
            prevTime = Time.time;
            SendCurState();
        }
        string message = Read();
        if (message != null)
        {
            DecodeMessage(message);
        }

    }

    public float GetThrottle()
    {
        return curThrottle;
    }

    void DecodeMessage(string msg)
    {
        curThrottle = FromByte2Float(msg.Substring(1, 4));
    }

    string FromFloat2Byte(float num)
    {
        string ret = "";
        ret += (char)(((byte)num & 0xFF000000) >> 24);
        ret += (char)(((byte)num & 0x00FF0000) >> 16);
        ret += (char)(((byte)num & 0x0000FF00) >> 8);
        ret += (char)((byte)num & 0x000000FF);
        return ret;
    }

    float FromByte2Float(string data)
    {
        byte[] b_array = System.Text.Encoding.UTF8.GetBytes(data); ;
        return System.BitConverter.ToSingle(b_array, 0);
    }

    void SendCurState()
    {
        string msg = "$";
        msg += FromFloat2Byte(curSpeed);
        msg += FromFloat2Byte(curDist);
        msg += '^';
        //Debug.Log(msg);
        Send(msg);
    }

    public void SetCurState(float speed, float distance)
    {
        curSpeed = speed;
        curDist = distance;
    }
}