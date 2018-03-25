using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

public class ShowKarma : MonoBehaviour {

    public Sprite goodKarma;
    public Sprite badKarma;

    public Image uiImage;

    public int karmaCounter = 0;

    private static ShowKarma _instance;
    public static ShowKarma Instance { get {
            return _instance;
                } set { _instance = value; }
    }

    private void Awake()
    {
        if (_instance)
            return;
        _instance = this;
    }

    public IEnumerator DisplayKarma(bool bad)
    {
        uiImage.enabled = true;
        if (bad)
        {
            uiImage.sprite = badKarma;
            karmaCounter--;
        }
        else
        {
            uiImage.sprite = goodKarma;
            karmaCounter++;
        }
        while (uiImage.color.a < 255)
        {
            Color color = uiImage.color;
            color.a += 0.01f;
            uiImage.color = color;
            
            yield return null;
        }
    }

    public void FadeKarma()
    {
        uiImage.enabled = false;
    }
}
