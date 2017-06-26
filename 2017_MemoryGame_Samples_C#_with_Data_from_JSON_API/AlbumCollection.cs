using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class AlbumCollection : MonoBehaviour
{
    private List<int> collectedCards = new List<int>();
    private List<GameObject> cardPool = new List<GameObject>();

    public GameObject collectedCardTemplate; // prefab of album card

    public GameObject selection; // drop down list with every selectable idol name; choose idol to show its card collection 
    public GameObject returnToSelectionButton; // button to return to the selection page

    private int columns;
    private int rows;

    private void OnEnable()
    {
        GameInformation.Instance.GetComponent<LoadScenes>().OnSceneHasLoaded += Setup;
    }

    private void OnDisable()
    {
        GameInformation.Instance.GetComponent<LoadScenes>().OnSceneHasLoaded -= Setup;
    }

    // for initialization
    void Setup()
    {

        CalculateLayout();

        Vector3 upperLeft = Camera.main.ViewportToWorldPoint(new Vector3(1f, 0.2f, -10));
        for (int i = 0; i < rows; i++)
        {
            for (int j = 0; j < columns; j++)
            {
                GameObject newCard = Instantiate(collectedCardTemplate, new Vector3(j * 2.5f + upperLeft.x + 1.5f, i * -1.5f + upperLeft.y + 1.5f), Quaternion.identity);
                cardPool.Add(newCard);
                newCard.SetActive(false);
            }
        }
    }

    // album card layout, starts from top left to bottom right
    void CalculateLayout()
    {
        // lossyScale: take the actual world scale (as much as it can)
        float objectWidth = collectedCardTemplate.transform.lossyScale.x;
        float objectHeight = collectedCardTemplate.transform.lossyScale.y;

        Vector2 lowerLeftPoint = Camera.main.WorldToScreenPoint(new Vector2(collectedCardTemplate.transform.position.x - objectWidth / 2, collectedCardTemplate.transform.position.y - objectHeight / 2));
        Vector2 upperRightPoint = Camera.main.WorldToScreenPoint(new Vector2(collectedCardTemplate.transform.position.x + objectWidth / 2, collectedCardTemplate.transform.position.y + objectHeight / 2));
        Vector2 upperLeftPoint = Camera.main.WorldToScreenPoint(new Vector2(collectedCardTemplate.transform.position.x - objectWidth / 2, collectedCardTemplate.transform.position.y + objectHeight / 2));

        float width = (upperRightPoint.x - upperLeftPoint.x) * 2.8f;
        float height = (upperLeftPoint.y - lowerLeftPoint.y) * 1.7f;

        columns = Mathf.FloorToInt(Screen.width / width);
        rows = Mathf.FloorToInt(Screen.height / height);
    }

    void SetImages()
    {
        int k = 0;
        while (k < cardPool.Count && k < collectedCards.Count)
        {
            cardPool[k].GetComponent<CollectedCard>().ClearTextures();
            cardPool[k].SetActive(true);
            cardPool[k].GetComponent<CollectedCard>().SetImages(collectedCards[k]); // set images with its parameter id
            ++k;
        }
    }

    void AddCardsToCollection(int idolCollectionIndex)
    {
        IdolCardCollection idolCollection = GameInformation.Instance.GetComponent<CardCollectionContainer>().cardCollection.idolCardCollections[idolCollectionIndex];
        foreach (int id in idolCollection.cardIds)
        {
            collectedCards.Add(id);
        }

        collectedCards.Reverse(); // at first the newest cardIds
    }

    public void SetIdolIndex(int index) // button listener
    {
        selection.SetActive(false); // hide the selection drop down list
        returnToSelectionButton.SetActive(true); // show the button to go back to the selection drop down list
        AddCardsToCollection(index);
        SetImages();
    }

    public void ReturnToSelection()
    {
        collectedCards.Clear(); // clear list
        foreach (GameObject card in cardPool)
        {
            card.SetActive(false); // hide card objects
        }
        selection.SetActive(true); // show selection drop down list with every selectable idol
        returnToSelectionButton.SetActive(false);
    }

    public void ReturnToStart()
    {
        GameInformation.Instance.GetComponent<LoadScenes>().ReturnToStartMenu("AlbumScene"); // change from AlbumScene to the start menu scene
    }
}