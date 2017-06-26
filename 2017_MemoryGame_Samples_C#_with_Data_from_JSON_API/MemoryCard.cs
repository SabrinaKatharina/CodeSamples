using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class MemoryCard : MonoBehaviour {

    public MemoryCard duplicate;

    private Vector3 originPosition;

    private bool isRotating = false;
    private bool showFront = false;
    private Vector3 startRotation = Vector3.zero;
    private Vector3 destinationRotation = Vector3.zero;
    private float deltaRotation = 0f;
    private bool isChosen = false;

    private void Start()
    {
        originPosition = transform.position;
    }

    // Update is called once per frame
    private void Update () {
        if (isRotating)
        {
            transform.eulerAngles = Vector3.Lerp(startRotation, destinationRotation, deltaRotation);
            deltaRotation += Time.deltaTime * 5;
            if(deltaRotation >= 1f) // check if destination is reached
            {    
                isRotating = false; // reset
                deltaRotation = 0f;
                showFront = !showFront; // next time the card should rotate the other way around
            }
        }
	}

    public void FlipCard()
    {
        isRotating = true;
        startRotation = transform.eulerAngles;
        destinationRotation = showFront ? Vector3.zero : new Vector3(0f, 180f, 0f);
    }

    public bool GetIsRotating()
    {
        return isRotating;
    }

    public void ReturnMemoryCard()
    {
        isChosen = false;
        FlipCard();
    }

    private void OnMouseDown()
    {
        if (!isChosen && !isRotating && MemoryGame.Instance.GetComponent<GetChosenMemoryCards>().chosenMemoryCards.Count < 2)
        {
            FlipCard();
            MemoryGame.Instance.GetComponent<GetChosenMemoryCards>().AddChosenCard(this);
            isChosen = true;
        }
    }

    public void ResetMemoryCardToOriginalState()
    {
        transform.position = originPosition;
        transform.eulerAngles = Vector3.zero;
        deltaRotation = 0f;
        isChosen = false;
        isRotating = false;
        showFront = false;
    }
}
