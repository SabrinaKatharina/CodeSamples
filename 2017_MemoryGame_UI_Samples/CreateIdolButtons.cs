using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.UI;

// creates all buttons with idols' names in the scroll list; if one button is clicked it shows the related idol's card collection
public class CreateIdolButtons : MonoBehaviour {

    public List<GameObject> idolButtons = new List<GameObject>();

    public GameObject parent;
    public GameObject idolButtonTemplate;

    public AlbumCollection albumCollection;

    private CardCollection cardCollection;


    private void OnEnable()
    {
        GameInformation.Instance.GetComponent<LoadScenes>().OnSceneHasLoaded += Setup;
    }

    private void OnDisable()
    {
        GameInformation.Instance.GetComponent<LoadScenes>().OnSceneHasLoaded -= Setup;
    }


    //initialization
    void Setup() {

        cardCollection = GameInformation.Instance.GetComponent<CardCollectionContainer>().cardCollection;
        int amountButtons = cardCollection.idolCardCollections.Count;

        for(int i = 0; i < amountButtons; i++)
        {
            GameObject newButton = Instantiate(idolButtonTemplate);

            // set the button as child of the scroll list
            newButton.transform.SetParent(parent.transform, false);

            // set button text to the idol's name
            string idolName = cardCollection.idolCardCollections[i].idolName;
            newButton.GetComponentInChildren<Text>().text = idolName;

            int temp = i;
            newButton.GetComponent<Button>().onClick.AddListener(() => { SetIdolIndex(temp); });
        }

	}

    void SetIdolIndex(int index)
    {
        albumCollection.SetIdolIndex(index);
    }
}
