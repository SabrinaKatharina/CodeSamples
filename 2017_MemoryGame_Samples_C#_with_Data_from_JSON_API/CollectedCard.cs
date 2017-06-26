using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System.IO;

public class CollectedCard : MonoBehaviour {

    // album card consits of an unidolized image and an idolized image
    public GameObject unidolizedImage;
    public GameObject idolizedImage;

    public Texture defaultMaterial;

    private string roundCardsCachedDirectory { get { return Path.Combine(Application.streamingAssetsPath, "Cache/AlbumCards"); } }

    public void SetImages(int cardId)
    {
        StartCoroutine(GetCollectedCardImages(cardId));
    }

    IEnumerator GetCollectedCardImages(int cardId)
    {
        if (GameInformation.Instance.cachingTexturesAllowed) // check if caching is allowed in the options
        {
            if (Directory.Exists(roundCardsCachedDirectory) == false) // check if the directory exists, if not create it
            {
                Directory.CreateDirectory(roundCardsCachedDirectory);
            }

            string possibleRoundCardCachedPath = Path.Combine(roundCardsCachedDirectory, cardId.ToString() + "_unidolized");
            if (File.Exists(possibleRoundCardCachedPath)) // check if card is already cached
            {
                // get the unidolized texture
                byte[] cachedTexture = File.ReadAllBytes(possibleRoundCardCachedPath);
                Texture2D NewTex = new Texture2D(1, 1);
                NewTex.LoadImage(cachedTexture);
                Renderer rendererUnidolized = unidolizedImage.GetComponent<Renderer>();
                rendererUnidolized.material.mainTexture = NewTex;

                // get the idolized texture
                string idolizedRoundCardCachedPath = Path.Combine(roundCardsCachedDirectory, cardId.ToString() + "_idolized");
                byte[] idolizedCachedTexture = File.ReadAllBytes(idolizedRoundCardCachedPath);
                Texture2D NewTex2 = new Texture2D(1, 1);
                NewTex2.LoadImage(idolizedCachedTexture);
                Renderer rendererIdolized = idolizedImage.GetComponent<Renderer>();
                rendererIdolized.material.mainTexture = NewTex2;
                yield break; // images are loaded, so break the coroutine
            }
        }
        // download the card image from API
        string json = "";
        WWW cardURL = new WWW("http://schoolido.lu/api/cards/" + cardId + "/");
        yield return cardURL; // wait until it is loaded
        if (cardURL.error == null) // check if there are no errors
        {
            json = cardURL.text;
        }
        else
        {
            Debug.Log("ERROR: " + cardURL.error);
        }
        CardItem card = JsonUtility.FromJson<CardItem>(json); // parse json to object Info
        if (card.round_card_image.Length > 0)
            StartCoroutine(LoadTexture(card.round_card_image, false, cardId) );
        else
            StartCoroutine(LoadTexture(card.round_card_idolized_image, false, cardId) ); // the card has no unidolized image, so get the idolized image instead
        StartCoroutine(LoadTexture(card.round_card_idolized_image, true, cardId) );
    }

    void TryCacheCurrentRoundImage(int cardId, bool idolized, Renderer objectRenderer)
    {
        string savePath;
        if (idolized)
        {
            savePath = Path.Combine(roundCardsCachedDirectory, cardId.ToString() + "_idolized");
        }
        else // round image is not idolized
        {
            savePath = Path.Combine(roundCardsCachedDirectory, cardId.ToString() + "_unidolized");
        }

        if (File.Exists(savePath) == false)
        {
            Texture2D textureToSave = objectRenderer.material.mainTexture as Texture2D;
            File.WriteAllBytes(savePath, textureToSave.EncodeToPNG());
        }
    }

    IEnumerator LoadTexture(string card_image_url, bool idolized, int cardId)
    {
        WWW myCardImageURL = new WWW("http:" + card_image_url);
        yield return myCardImageURL; // wait until it is loaded
        Renderer objectRenderer;
        if (idolized ? objectRenderer = idolizedImage.GetComponent<Renderer>() : objectRenderer = unidolizedImage.GetComponent<Renderer>())
        objectRenderer.material.mainTexture = myCardImageURL.texture; // set texture of this game object
        if (GameInformation.Instance.cachingTexturesAllowed) // if caching is allowed in options, try to cache the current image
        {
            TryCacheCurrentRoundImage(cardId, idolized, objectRenderer);
        }

    }

    public void ClearTextures()
    {
        unidolizedImage.GetComponent<Renderer>().material.mainTexture = defaultMaterial; // set texture of this game object to default
        idolizedImage.GetComponent<Renderer>().material.mainTexture = defaultMaterial; // set texture of this game object to default
    }
}
