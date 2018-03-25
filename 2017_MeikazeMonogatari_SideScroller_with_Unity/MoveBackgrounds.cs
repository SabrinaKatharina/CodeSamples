using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MoveBackgrounds : MonoBehaviour {

    public List<GameObject> backgrounds = new List<GameObject>();

    public float backgroundWidth;
    public GameObject player;
	
	// Update is called once per frame
	void Update ()
    {
		if (backgrounds.Count > 0 && player.transform.position.x > backgrounds[0].transform.position.x + backgroundWidth / 2)
        {
            GameObject background = backgrounds[0];
            background.transform.position = new Vector3(backgrounds[backgrounds.Count-1].transform.position.x + 21f, background.transform.position.y, background.transform.position.z);
            backgrounds.RemoveAt(0);
            backgrounds.Add(background);
        }
#if UNITY_STANDALONE
        if (Input.GetKey(KeyCode.Escape))
        {
            Application.Quit();
        }
#endif
    }
}
