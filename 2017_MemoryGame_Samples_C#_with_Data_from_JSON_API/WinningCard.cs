using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.Linq;

public class WinningCard : MonoBehaviour {

    private bool isRotating = false;
    private float deltaRotation = 0f;

    private bool isTurnedAround = false;

    private void OnDisable()
    {
        isTurnedAround = false; // reset the winning card, so it can be clicked again
    }

    // Update is called once per frame
    private void Update()
    {
        if (isRotating)
        {
            transform.eulerAngles = Vector3.Lerp(Vector3.zero, new Vector3(0f, 180f, 0f), deltaRotation);
            deltaRotation += Time.deltaTime * 2;
            if (deltaRotation >= 1f) // check if destination is reached
            {
                isRotating = false; // reset
                deltaRotation = 0f;
            }
        }
    }

    private void OnMouseDown()
    {
        if (!isRotating && !isTurnedAround)
        {
            isRotating = true;
            AddWinningCardInfo();
            isTurnedAround = true;
        }
    }

    private void AddWinningCardInfo()
    {
        string idolName = GetComponent<LoadCardImage>().cardItem.idol.name;
        int cardId = GetComponent<LoadCardImage>().cardItem.id;

        CardCollection collection = CardCollectionContainer.Instance.cardCollection;
        IdolCardCollection idolCardCollection = collection.idolCardCollections.Find(idolcards => idolcards.idolName.Equals(idolName)); // find the idol in the card collection

        if (idolCardCollection == null) // the idol exists not yet
        {
            idolCardCollection = new IdolCardCollection(idolName, new List<int>()); // add the idol to the idolCardCollection
            collection.idolCardCollections.Add(idolCardCollection);
            List<IdolCardCollection> orderdList = collection.idolCardCollections.OrderBy(idolcollection => idolcollection.idolName).ToList();
            collection.idolCardCollections = orderdList;
        }

        if (idolCardCollection.cardIds.Contains(cardId) == false) // check if the winning card id is already collected
        {
            idolCardCollection.cardIds.Add(cardId);
            Debug.Log("Add: " + idolName + " / " + cardId);
        }
        else
            Debug.Log("Card: " + idolName + " / " + cardId + " is already collected");
    }
}
