using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class FileWriter : MonoBehaviour
{

    public string path;
    private StreamWriter writer;

    // Start is called before the first frame update
    void Start()
    {
        writer = new StreamWriter(path, false);
    }

    public void Write(string s)
    {
        //Write some text to the test.txt file
        writer.WriteLine(s);
    }

    private void OnApplicationQuit()
    {
        writer.Close();
    }
}
