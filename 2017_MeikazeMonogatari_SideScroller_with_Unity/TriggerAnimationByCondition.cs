using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(Animator), typeof(BoxCollider2D))]
public class TriggerAnimationByCondition : MonoBehaviour {

    private bool conditionFulfilled = false;

    public void SetCondition(bool isFulfilled)
    {
        conditionFulfilled = isFulfilled;
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        GetComponent<Animator>().SetTrigger("chooseNextAnimation");
        GetComponent<Animator>().SetBool("isConditionFulfilled", conditionFulfilled);
        if(conditionFulfilled == false)
            StartCoroutine(ShowKarma.Instance.DisplayKarma(true));
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        if (conditionFulfilled == false)
            ShowKarma.Instance.FadeKarma();
    }
}
