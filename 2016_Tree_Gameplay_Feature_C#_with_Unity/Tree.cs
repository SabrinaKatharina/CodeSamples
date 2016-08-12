using UnityEngine;
using System.Collections;

// This class is part of a gameplay feature in the game ShamanDance which was developed at globalgamejam.
// gamplay feature: grow a tree if a seed got water; the tree has a limited life time; if the life time is over, the tree turns to a seed again
public class Tree : MonoBehaviour {

    private Vector2 desiredColliderOffset;
    private Vector2 maxSize;
    private Vector2 maxLocalScale;
    private BoxCollider2D boxCollider;
    private float lifeTime;

    public Seed seedPrefab;  

	void Start () {

        boxCollider = this.GetComponent<BoxCollider2D>();
        desiredColliderOffset = boxCollider.offset;        
        maxSize = boxCollider.size;
        maxLocalScale = this.transform.localScale;
        lifeTime = 8f;

        // at the beginning set the box collider's height, size and y offset to almost zero
        boxCollider.offset = new Vector2(boxCollider.offset.x, 0.4f); // start y offset is 0.4f
        boxCollider.size = new Vector2(boxCollider.size.x, 0.01f);    // start size in y-axis is 0.01f
        this.transform.localScale = new Vector2(transform.localScale.x, 0.0001f);  // start local scale in y-axis is 0.0001f

	}
	
	// Update is called once per frame
	void Update () {

        boxCollider.offset = Vector2.Lerp(boxCollider.offset, desiredColliderOffset, Time.deltaTime);  // grow box collider to the desired offset
        boxCollider.size = Vector2.Lerp(boxCollider.size, maxSize, Time.deltaTime);  // grow box collider to the max size
        transform.localScale = Vector3.Lerp(transform.localScale, maxLocalScale, Time.deltaTime);  // lerp the local scale to the max local scale
       
       
        lifeTime -= Time.deltaTime; //reduce life time

        if (lifeTime < 0.0f)
        {
            // tree shrinks if life time is over
            boxCollider.offset = Vector2.Lerp(boxCollider.offset, new Vector2 (boxCollider.offset.x, 0.1f), Time.deltaTime);
            boxCollider.size = Vector2.Lerp(boxCollider.size, new Vector2 (boxCollider.size.x, 0.1f), Time.deltaTime);
            transform.localScale = Vector3.Lerp(transform.localScale, new Vector3 (1f, -1f, 1f), Time.deltaTime); // shrink tree's height

            if (transform.localScale.y  < 0.01f) // if tree has a low height again
            {

                Destroy(this.gameObject); // destroy tree
                GameObject seed = Instantiate(seedPrefab, transform.position, Quaternion.identity) as GameObject;   // instantiate new seed
            }
        }
	}
}
