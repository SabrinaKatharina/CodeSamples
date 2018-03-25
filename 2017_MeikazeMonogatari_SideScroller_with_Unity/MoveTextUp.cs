﻿using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;
using UnityEngine.EventSystems;

[RequireComponent(typeof(Text))]
public class MoveTextUp : MonoBehaviour {

    private bool isMovingUp = false;
    private float length;
    private float deltaLength = 0f;
    private Vector2 startPosition;

    public delegate void FinishedMovingUp();
    public FinishedMovingUp finishedMovingUp;

    private void OnEnable()
    {
        startPosition = GetComponent<RectTransform>().anchoredPosition;
        length = GetComponent<Text>().preferredHeight;
        finishedMovingUp += ResetTextPosition;
    }

    private void OnDisable()
    {
        finishedMovingUp -= ResetTextPosition;
    }

    private void ResetTextPosition()
    {
        isMovingUp = false;
        GetComponent<RectTransform>().anchoredPosition = startPosition;
    }

    private void Update()
    {
        float animSpeedMultiplier = 1f;

#if UNITY_STANDALONE
        if (Input.GetMouseButton(0))
        {
            animSpeedMultiplier += 4f;
        }
#elif UNITY_ANDROID
        if (Input.touchCount > 0)
        {
            float touchPressure = Input.GetTouch(0).pressure;
            animSpeedMultiplier += touchPressure * 4f;
        }
#endif
        if (isMovingUp)
        {
            GetComponent<RectTransform>().anchoredPosition = Vector2.Lerp(startPosition, startPosition + Vector2.up * length, deltaLength);
            deltaLength += (Time.deltaTime / 30f) * animSpeedMultiplier;

            if (deltaLength >= 1f)
            {
                finishedMovingUp();               
            }
        }
    }

    public void StartMovingUp()
    {
        isMovingUp = true;
        deltaLength = 0f;
    }
}
