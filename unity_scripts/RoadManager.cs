using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class RoadManager : MonoBehaviour
{
    public int curLeadingPos = 0;
    public GameObject followingObject;
    public int roadSizeLimit = 20;
    public GameObject roadPrefab;
    public GameObject roadContainer;
    private bool spawnFlag = false;
    private Queue<GameObject> roadQueue = new Queue<GameObject>();

    public void spawnNewRoad(float pos)
    {
        if (roadQueue.Count > 20)
        {
            GameObject old_road = roadQueue.Dequeue();
            Destroy(old_road);
        }
        Vector3 newPos = roadContainer.transform.position;
        newPos.z = newPos.z + pos * 25;
        GameObject road = Instantiate(roadPrefab, newPos, new Quaternion(0, 0, 0, 0), roadContainer.transform);
        roadQueue.Enqueue(road);
    }

    public void readLeadingPos()
    {
        int localLeadingPos = (int) (followingObject.GetComponent<Transform>().position.z / 25.0) + roadSizeLimit / 2;
        if (localLeadingPos != curLeadingPos)
        {
            spawnFlag = true;
            curLeadingPos = localLeadingPos;
        }
    }

    // Start is called before the first frame update
    void Start()
    {
        for (int i = 0; i < roadSizeLimit; i ++)
        {
            spawnNewRoad(i - roadSizeLimit / 2);
        }
    }

    // Update is called once per frame
    void Update()
    {
        if (spawnFlag)
        {
            spawnNewRoad(curLeadingPos);
            spawnFlag = false;
        }
        readLeadingPos();
    }
}
