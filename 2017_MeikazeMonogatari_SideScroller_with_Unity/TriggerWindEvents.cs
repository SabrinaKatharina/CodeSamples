using System.Collections;
using System.Collections.Generic;
using UnityEngine;

[RequireComponent(typeof(BoxCollider2D))]
public class TriggerWindEvents : MonoBehaviour {

    private Blowing blowingPlayer;
    public Animator parentEventAnimation;

    public AudioSource soundEffectClip;

    public bool isNegativeKarma = false;
    public float requiredStrengthForEvent;
    public float maxStengthForEvent;
       

	// Use this for initialization
	void Start ()
    {
        if (parentEventAnimation == null)
            parentEventAnimation = GetComponentInParent<Animator>();
        blowingPlayer = GameObject.FindGameObjectWithTag("Player").GetComponent<Blowing>();
    }

    private void OnTriggerEnter2D(Collider2D collision)
    {
        // check force strength of player
        if (blowingPlayer.ForceStrength >= requiredStrengthForEvent && blowingPlayer.ForceStrength <= maxStengthForEvent)
        { 
            StartCoroutine(ShowKarma.Instance.DisplayKarma(isNegativeKarma));
            parentEventAnimation.SetBool("isTriggered", true);
            if(soundEffectClip != null)
                soundEffectClip.Play();
            if (GetComponentInParent<TriggerAnimationByCondition>())
                GetComponentInParent<TriggerAnimationByCondition>().SetCondition(true);
        }
    }

    private void OnTriggerExit2D(Collider2D collision)
    {
        ShowKarma.Instance.FadeKarma();
        if (soundEffectClip != null)
            soundEffectClip.Stop();
    }
}
