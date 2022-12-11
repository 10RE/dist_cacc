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

    //private float msg = 0;

    private float prevTime = 0F;
    private float sendPeriod = 0.05F;

    private bool valChecked = false;

    public bool sendClear = false;

    public int test_send = 0;

    public float kp = 0f;
    public float ki = 0f;
    public float kd = 0f;
    
    public bool sendP = false;
    public bool sendI = false;
    public bool sendD = false;
    public bool sendPID = false;



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
        SendClear();
        SendClear();
        SendClear();
    }

    private void OnApplicationQuit()
    {
        serial.Close();
    }

    public void Send(byte[] message)
    {
        //Debug.Log("Send: " + (int)message[6] + " " + (int)message[7] + " " + (int)message[8] + " " + (int)message[9] + " ");
        outputQueue.Enqueue(message);
    }

    public byte[] Read()
    {
        if (inputQueue.Count == 0)
            return null;
        return (byte[])inputQueue.Dequeue();
    }

    public void ClearInput()
    {
        inputQueue.Clear();
    }

    public void Write(byte[] message)
    {
        //byte[] b_array = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
        //System.Array.Reverse(b_array);
        //byte[] correct_b_array = new[] { b_array[1], b_array[0], b_array[3], b_array[2]};
        //b_array = System.Text.Encoding.UTF8.GetBytes(data);
        //for (int i = 0; i < 11; i++)
        //{
            //Debug.Log((int)message[i]);
        //    b_array[i] = System.Text.Encoding.UTF32.GetBytes("" + message[i])[0];
            //Debug.Log((int)b_array[i]);
        //}
        serial.Write(message, 0, 11);
        serial.BaseStream.Flush();
    }

    public byte[] Retrieve(int timeout = 50)
    {
        byte[] ret = new byte[7];
        byte data;
        try
        {
            data = (byte)serial.ReadByte();
            //Debug.Log(data);
            if (data != '$')
            {
                return null;
            }
            ret[0] = data;
            data = (byte)serial.ReadByte();
            //Debug.Log(data);
            ret[1] = data;
            int length = (int)data;
            for (int i = 0; i < length + 1; i++)
            {
                data = (byte)serial.ReadByte();
                //Debug.Log(System.Text.Encoding.UTF8.GetBytes("" + data)[0]);
                ret[2 + i] = data;
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
                byte[] command = (byte[])outputQueue.Dequeue();
                Write(command);
            }
            // Read
            byte[] result = Retrieve();
            if (result != null && result.Length != 0)
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
        byte[] message = Read();
        if (message != null)
        {
            DecodeMessage(message);
        }
        if (sendClear)
        {
            SendClear();
            sendClear = false;
        }
        if (sendP)
        {
            SendP();
            sendP = false;
        }
        if (sendI)
        {
            SendI();
            sendI = false;
        }
        if (sendD)
        {
            SendD();
            sendD = false;
        }
        if (sendPID)
        {
            SendPID();
            sendPID = false;
        }
        //Debug.Log(FromFloat2Byte(1.1f));
    }

    public float GetThrottle()
    {
        return curThrottle;
    }

    void DecodeMessage(byte[] msg)
    {
        string print = " Recv: ";
        byte[] b_array = { 0, 0, 0, 0};
        for (int i = 0; i < 4; i++)
        {
            b_array[i] = msg[2 + i];
        }
        print += BitConverter.ToString(b_array);
        curThrottle = System.BitConverter.ToSingle(b_array, 0);
        Debug.Log(commPort + print + " " + curThrottle);
        //curThrottle = FromByte2Float(msg.Substring(2, 4));  
        //Debug.Log("Throttle: " + curThrottle);
    }

    byte[] FromFloat2Byte(float num)
    {
        string ret = "";
        byte[] bytes = System.BitConverter.GetBytes(num);
        //Debug.Log("Send: " + num);
        //Debug.Log("Send: " + BitConverter.ToString(bytes));
        ret += (char) bytes[0];
        ret += (char) bytes[1];
        ret += (char) bytes[2];
        ret += (char) bytes[3];
        //Debug.Log("Send: " + (int)bytes[0] + " " + (int)bytes[1] + " " + (int)bytes[2] + " " + (int)bytes[3] + " ");
        return bytes;
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
        byte[] msg = new byte[11];
        msg[0] = (byte)'$';
        msg[1] = (byte)0;
        //msg += (byte)((test_send) & 0xFF);
        byte[] float_byte = FromFloat2Byte(curSpeed);
        for (int i = 0; i < 4; i ++)
        {
            msg[2 + i] = float_byte[i];
        }
        float_byte = FromFloat2Byte(curDist);
        for (int i = 0; i < 4; i++)
        {
            msg[6 + i] = float_byte[i];
        }
        msg[10] = (byte)'^';
        //Debug.Log("Send: " + BitConverter.ToString(msg));
        Send(msg);
    }

    public void SendClear()
    {
        byte[] msg = new byte[11];
        msg[0] = (byte)'$';
        msg[1] = (byte)1;
        //msg += (byte)((test_send) & 0xFF);
        byte[] float_byte = FromFloat2Byte(curSpeed);
        for (int i = 0; i < 8; i++)
        {
            msg[2 + i] = 0;
        }
        msg[10] = (byte)'^';
        Send(msg);
    }

    public void SendP() {
        byte[] msg = new byte[11];
        msg[0] = (byte)'$';
        msg[1] = (byte)2;
        //msg += (byte)((test_send) & 0xFF);
        byte[] float_byte = FromFloat2Byte(kp);
        for (int i = 0; i < 4; i ++)
        {
            msg[2 + i] = float_byte[i];
        }
        for (int i = 0; i < 4; i++)
        {
            msg[6 + i] = 0;
        }
        msg[10] = (byte)'^';
        Send(msg);
    }

    public void SendI() {
        byte[] msg = new byte[11];
        msg[0] = (byte)'$';
        msg[1] = (byte)3;
        //msg += (byte)((test_send) & 0xFF);
        byte[] float_byte = FromFloat2Byte(ki);
        for (int i = 0; i < 4; i ++)
        {
            msg[2 + i] = float_byte[i];
        }
        for (int i = 0; i < 4; i++)
        {
            msg[6 + i] = 0;
        }
        msg[10] = (byte)'^';
        Send(msg);
    }

    public void SendD() {
        byte[] msg = new byte[11];
        msg[0] = (byte)'$';
        msg[1] = (byte)4;
        //msg += (byte)((test_send) & 0xFF);
        byte[] float_byte = FromFloat2Byte(kd);
        for (int i = 0; i < 4; i ++)
        {
            msg[2 + i] = float_byte[i];
        }
        for (int i = 0; i < 4; i++)
        {
            msg[6 + i] = 0;
        }
        msg[10] = (byte)'^';
        Send(msg);
    }

    public void SendKID() {
        SendClear();
        SendP();
        SendI();
        SendD();
    }

    public void SetCurState(float speed, float distance)
    {
        curSpeed = speed;
        curDist = distance;
    }
}