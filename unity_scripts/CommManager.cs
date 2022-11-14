using System.Collections;
using UnityEngine;
using System.IO.Ports;
using System.Threading;
using System;

public class CommManager : MonoBehaviour
{
    public String commPort;

    private SerialPort serial;

    private Queue outputQueue;
    private Queue inputQueue;

    private float curSpeed = 0;
    private float curDist = 0;
    private float curThrottle = 0;

    private float msg = 0;

    private float prevTime = 0F;
    private float sendPeriod = 0.5F;

    private bool valChecked = false;

    public int test_send = 0;

    // Start is called before the first frame update
    // Start is called before the first frame update
    void Start()
    {
        Debug.Log("Start");
        serial = new SerialPort(commPort, 115200);
        serial.Open();
        serial.Encoding = System.Text.Encoding.UTF8;
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
        Debug.Log("Send: " + (int)message[6] + " " + (int)message[7] + " " + (int)message[8] + " " + (int)message[9] + " ");
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
        byte[] b_array = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        //System.Array.Reverse(b_array);
        //byte[] correct_b_array = new[] { b_array[1], b_array[0], b_array[3], b_array[2]};
        //b_array = System.Text.Encoding.UTF8.GetBytes(data);
        for (int i = 0; i < 11; i++)
        {
            //Debug.Log((int)message[i]);
            b_array[i] = System.Text.Encoding.UTF32.GetBytes("" + message[i])[0];
            //Debug.Log((int)b_array[i]);
        }
        serial.Write(b_array, 0, 11);
        serial.BaseStream.Flush();
    }

    public string Retrieve(int timeout = 50)
    {
        string ret = "";
        char data = ' ';
        try
        {
            data = (char)serial.ReadByte();
            Debug.Log(data);
            if (data != '$')
            {
                return null;
            }
            ret += data;
            data = (char)serial.ReadByte();
            Debug.Log(data);
            ret += data;
            int length = (int)data;
            for (int i = 0; i < length + 1; i++)
            {
                data = (char)serial.ReadByte();
                //Debug.Log(System.Text.Encoding.UTF8.GetBytes("" + data)[0]);
                ret += data;
                //Debug.Log("");
            }
            //Debug.Log("exit");
            if (ret[2 + length] != '^')
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
        //Debug.Log(FromFloat2Byte(1.1f));
    }

    public float GetThrottle()
    {
        return curThrottle;
    }

    void DecodeMessage(string msg)
    {
        string print = "Recv: ";
        byte[] b_array = { 0, 0, 0, 0};
        for (int i = 0; i < 4; i++)
        {
            b_array[i] = System.BitConverter.GetBytes(msg[i + 2])[0];
        }
        print += BitConverter.ToString(b_array);
        Debug.Log(print);
        curThrottle = System.BitConverter.ToSingle(b_array, 0);
        //curThrottle = FromByte2Float(msg.Substring(2, 4));  
        //Debug.Log("Throttle: " + curThrottle);
    }

    string FromFloat2Byte(float num)
    {
        string ret = "";
        byte[] bytes = System.BitConverter.GetBytes(num);
        Debug.Log("Send: " + BitConverter.ToString(bytes));
        ret += (char) bytes[0];
        ret += (char) bytes[1];
        ret += (char) bytes[2];
        ret += (char) bytes[3];
        //Debug.Log("Send: " + (int)bytes[0] + " " + (int)bytes[1] + " " + (int)bytes[2] + " " + (int)bytes[3] + " ");
        return ret;
    }

    float FromByte2Float(string data)
    {
        byte[] b_array = { 0, 0, 0, 0};
        //System.Array.Reverse(b_array);
        //byte[] correct_b_array = new[] { b_array[1], b_array[0], b_array[3], b_array[2]};
        //b_array = System.Text.Encoding.UTF8.GetBytes(data);
        for (int i = 0; i < 4; i ++)
        {
            b_array[i] = System.Text.Encoding.UTF8.GetBytes("" + data[i])[0];
            //Debug.Log((int)b_array[i]);
        }
        float ret = System.BitConverter.ToSingle(b_array, 0);
        //Debug.Log(ret);
        return ret;
    }

    void SendCurState()
    {
        //Debug.Log("#");
        //Debug.Log(curSpeed);
        //Debug.Log("Dist: " + curDist);
        string msg = "$";
        msg += (char)8;
        //msg += (byte)((test_send) & 0xFF);
        msg += FromFloat2Byte(curSpeed);
        msg += FromFloat2Byte(curDist);
        msg += '^';
        Send(msg);
    }

    public void SetCurState(float speed, float distance)
    {
        curSpeed = speed;
        curDist = distance;
    }
}