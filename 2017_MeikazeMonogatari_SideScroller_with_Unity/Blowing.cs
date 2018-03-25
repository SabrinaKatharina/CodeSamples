using System.Collections;
using System.Collections.Generic;
using UnityEngine;


public class Blowing : MonoBehaviour {

    public float forceStrengthMultiplier;

    private float forceStrength;
    public float ForceStrength { get { return forceStrength; } }

    public GameObject directionArrow;

    public Color wrongDirectionColor;
    public Color rightDirectionColor;

    private Vector2 targetPosition;
    private float currentX;
    private bool isMoving;
    // Use this for initialization
    void Start()
    {
        currentX = transform.position.x;
        isMoving = GetComponent<Animator>().GetBool("isMoving");
        Cursor.lockState = CursorLockMode.Confined;
    }

    void FixedUpdate()
    {
        if (isMoving == false)
        {
            currentX = transform.position.x;
#if UNITY_STANDALONE
            if (Input.GetMouseButton(0))
            {
                targetPosition = Camera.main.ScreenToWorldPoint(Input.mousePosition);
#endif

#if UNITY_ANDROID
            if (Input.touchCount > 0)
            {
                Touch touch = Input.GetTouch(0);
                targetPosition = Camera.main.ScreenToWorldPoint(touch.position);
#endif
                if (directionArrow)
                {
                    DisplayDirectionArrow();
                }
                if (targetPosition.x > currentX)
                {
                    int index = 1;
                    if (targetPosition.y > transform.position.y + 1.5f)
                    {
                        index = 0;
                    }
                    if (targetPosition.y < transform.position.y - 2f)
                    {
                        index = 2;

                    }
                    LookInDirection(index);
                }
            }
        }
        // Limit movement in Y-Axis
        this.transform.position = new Vector3(transform.position.x, Mathf.Clamp(transform.position.y, -3.5f, 3f), transform.position.z);
    }

    void Update()
    {
#if UNITY_STANDALONE
        if (Input.GetMouseButtonUp(0))
        {
            OnInputRelease();
        }
#endif
#if UNITY_ANDROID
        if (Input.GetTouch(0).phase == TouchPhase.Ended)
        {
           OnInputRelease();
        }
#endif
    }

    void LookInDirection(int directionIndex)
    {
        GetComponent<Animator>().SetInteger("lookingDirectionIndex", directionIndex);
    }

    void AddForceToMove(Vector2 targetPosition)
    {
        isMoving = true;
        GetComponent<Animator>().SetBool("isMoving", isMoving);
        forceStrength = targetPosition.x - transform.position.x;
        Vector2 target = new Vector3(targetPosition.x - transform.position.x, targetPosition.y - transform.position.y);

        this.GetComponent<Rigidbody2D>().AddForce(target * forceStrength * forceStrengthMultiplier);
        StartCoroutine(MoveAndRest());
    }

    private IEnumerator MoveAndRest()
    {
        yield return new WaitForSeconds(1f);
        isMoving = false;
        transform.GetComponent<Rigidbody2D>().velocity = new Vector2(0.5f,0f) ;
        GetComponent<Animator>().SetBool("isMoving", isMoving);
    }

    private void DisplayDirectionArrow()
    {
        Vector2 directionVector = (Vector3)targetPosition - transform.position;

        if (directionVector.x < 0)
        {
            directionArrow.GetComponent<SpriteRenderer>().color = wrongDirectionColor;
        }
        else
        {
            directionArrow.GetComponent<SpriteRenderer>().color = rightDirectionColor;
        }

        directionArrow.transform.position = transform.position + ((Vector3)directionVector / 2f);

        float angle = Mathf.Atan2(directionVector.y, directionVector.x) * Mathf.Rad2Deg;
        Quaternion rotation = Quaternion.AngleAxis(angle, Vector3.forward);
        directionArrow.transform.rotation = rotation;

        Vector3 newScale = directionArrow.transform.localScale;
        newScale.x = Mathf.Clamp(directionVector.magnitude / 10f, 0.05f, 10f);
        newScale.y = Mathf.Clamp(directionVector.magnitude / 15f, 0.05f, 10f);
        directionArrow.transform.localScale = newScale;

        directionArrow.SetActive(true);
    }

    private void OnInputRelease()
    {
        if (targetPosition.x > currentX)
        {
            AddForceToMove(targetPosition);
        }
        HideDirectionArrow();
    }

    private void HideDirectionArrow()
    {
        directionArrow.SetActive(false);
    }
}
