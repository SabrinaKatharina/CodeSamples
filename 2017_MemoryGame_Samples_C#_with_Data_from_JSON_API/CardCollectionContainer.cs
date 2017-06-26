using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Xml;
using System.Xml.Serialization;
using System.IO;


public class CardCollectionContainer : MonoBehaviour
{
    private static CardCollectionContainer _instance;
    public static CardCollectionContainer Instance { get { return _instance; } set { _instance = value; } }

    public CardCollection cardCollection;

    private string path { get { return Path.Combine(Application.streamingAssetsPath, "XML/cardCollection.xml"); } }
    
    private void Awake()
    {
        if (_instance != null)
            return;
        _instance = this;

        LoadCardCollection();
    }


    public void SaveCardCollection()
    {
        var serializer = new XmlSerializer(typeof(CardCollection));    
        var stream = new FileStream(path, FileMode.Create);
        serializer.Serialize(stream, cardCollection);
        stream.Close();
    }

    public void LoadCardCollection()
    {
        var serializer = new XmlSerializer(typeof(CardCollection));
        var stream = new FileStream(path, FileMode.Open);
        if(stream.Length > 0)
            cardCollection = serializer.Deserialize(stream) as CardCollection;
        stream.Close();
    }

    //Loads the xml directly from the given string. Useful in combination with www.text.
    public static CardCollection LoadFromText(string text)
    {
        var serializer = new XmlSerializer(typeof(CardCollection));
        return serializer.Deserialize(new StringReader(text)) as CardCollection;
    }
}

[XmlRoot("CardCollection")]
[Serializable]
public class CardCollection{

    [XmlArray("IdolCardCollections"), XmlArrayItem("IdolItem")]
    public List<IdolCardCollection> idolCardCollections = new List<IdolCardCollection>();

}

[Serializable]
public class IdolCardCollection
{
    [XmlElement("IdolName")]
    public string idolName;
    [XmlArray("CardIds"), XmlArrayItem("id")]
    public List<int> cardIds = new List<int>();

    public IdolCardCollection() { }

    public IdolCardCollection(string name, List<int> cardIdList)
    {
        idolName = name;
        cardIds = cardIdList;
    }
}
