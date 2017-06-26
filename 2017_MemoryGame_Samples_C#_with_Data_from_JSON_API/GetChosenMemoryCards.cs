using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class GetChosenMemoryCards : MonoBehaviour {

    [HideInInspector]
    public List<MemoryCard> chosenMemoryCards = new List<MemoryCard>();

    public Transform discardPile;

    private Vector3 discardPileoffset = new Vector3(0.2f, 0f, -0.2f);

	public void AddChosenCard(MemoryCard card)
    {
        chosenMemoryCards.Add(card);
        if (chosenMemoryCards.Count >= 2)
            StartCoroutine(CheckIfMemoryCardsAreDuplicates());
    }

    private IEnumerator CheckIfMemoryCardsAreDuplicates()
    {
        while(chosenMemoryCards[1].GetIsRotating() == true)
        {
            yield return 0;
        }

        MemoryGame.Instance.turnCount++;
        if(chosenMemoryCards[0].duplicate == chosenMemoryCards[1])
        {
            MemoryGame.Instance.collectedPairsCount++;
            yield return new WaitForSeconds(0.5f);
            chosenMemoryCards[0].transform.position = discardPile.transform.position + MemoryGame.Instance.collectedPairsCount * discardPileoffset;
            chosenMemoryCards[1].transform.position = discardPile.transform.position + MemoryGame.Instance.collectedPairsCount * discardPileoffset;
            chosenMemoryCards.Clear();
        }
        else
        {
            StartCoroutine(FlipChosenCards());
        }
    }

    private IEnumerator FlipChosenCards()
    {
        while(chosenMemoryCards[0].GetIsRotating() || chosenMemoryCards[1].GetIsRotating())
        {
            yield return 0;
        }
        yield return new WaitForSeconds(0.5f);
        chosenMemoryCards[0].ReturnMemoryCard();
        chosenMemoryCards[1].ReturnMemoryCard();
        // clear chosen cards list
        chosenMemoryCards.Clear();
    }
}
