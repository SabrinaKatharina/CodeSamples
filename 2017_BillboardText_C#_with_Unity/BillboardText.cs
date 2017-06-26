using System.Collections;
using System.Collections.Generic;
using UnityEngine;

/* how to access / use:
 * Create Billboard Text:
 *      TextMesh myBillboardText = BillboardText.Instance.CreateBillboardText(text, position, showDuration);
 * or:
 *      BillboardText.Instance.CreateBillboardText(text, position, showDuration);
 * 
 * Access your billboard's text :
 * BillboardText.Instance.SetText(myBillboardText, newText);
 * 
 * Destroy your billboard:
 * BillboardText.Instance.DestroyTextImmediately(myBillboardText);
 * 
*/

public class BillboardText : MonoBehaviour
{

    public static BillboardText Instance;
    public TextMesh billboardTextTemplate; // prefab of 3D text

    private void Awake()
    {
        Instance = this; // create a static instance
    }

    private TextMesh CreateBillboardText(string text, Vector3 position, float showDuration)
    {
        TextMesh billboardText = Instantiate(billboardTextTemplate);
        billboardText.transform.rotation = Camera.main.transform.rotation;
        SetText(billboardText, text);

        if (showDuration > 0)
        {
            StartCoroutine(WaitSecondsAndDestroyBillboardText(showDuration, billboardText));
        }
        return billboardText;
    }

    private TextMesh CreateBillboardText(string text, Vector3 position, float showDuration, float characterSize, Color color)
    {
        TextMesh billboardText = Instantiate(billboardTextTemplate);
        billboardText.transform.rotation = Camera.main.transform.rotation;
        billboardText.characterSize = characterSize;
        billboardText.color = color;
        SetText(billboardText, text);
        
        if (showDuration > 0)
        {
            StartCoroutine(WaitSecondsAndDestroyBillboardText(showDuration, billboardText));
        }
        return billboardText;
    }

    private TextMesh CreateBillboardText(string text, Vector3 position, float showDuration, float characterSize, Color color, TextAlignment alignment, float lineSpacing)
    {
        TextMesh billboardText = Instantiate(billboardTextTemplate);
        billboardText.transform.rotation = Camera.main.transform.rotation;
        billboardText.characterSize = characterSize;
        billboardText.color = color;
        billboardText.alignment = alignment;
        billboardText.lineSpacing = lineSpacing;
        SetText(billboardText, text);

        if (showDuration > 0)
        {
            StartCoroutine(WaitSecondsAndDestroyBillboardText(showDuration, billboardText));
        }
        return billboardText;
    }

    private IEnumerator WaitSecondsAndDestroyBillboardText(float seconds, TextMesh text)
    {
        yield return new WaitForSeconds(seconds);
        Destroy(text.gameObject);
    }


    // --------- Access functions for billboard TextMesh -----------------
    private void SetText(TextMesh billboardText, string newText)
    {
        billboardText.text = newText;
    }

    private void DestroyTextImmediately(TextMesh billboardText)
    {
        Destroy(billboardText.gameObject);
    }
}
