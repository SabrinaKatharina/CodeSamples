using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEngine.EventSystems;
using System.IO;

public class StartMenu : MonoBehaviour {

    public UnityEngine.UI.Text startButtonText;
    public GameObject createNewGameButton;
    public GameObject credits;
    public GameObject startMenu;
    public GameObject optionsMenu;
    public UnityEngine.UI.Text optionMusicButtonText;
    public UnityEngine.UI.Text optionCacheButtonText;

    [Header("Changing Frame Color")]
    public GameObject memoryCardTemplate;
    public GameObject winningCardTemplate;
    public Material blackFrameMaterial;
    public Material whiteFrameMaterial;

    private bool isMusicPlaying;
    private bool isCachingActivated;

    private void Start()
    {
        // check if there are cards saved already, then the continue button displays
        if(CardCollectionContainer.Instance.cardCollection.idolCardCollections.Count > 0)
        {
            startButtonText.text = "CONTINUE";
            createNewGameButton.SetActive(true);
        }
        else
        {
            startButtonText.text = "CREATE NEW GAME";
        }
        isMusicPlaying = true;
        isCachingActivated = true;
    }

    private void OnEnable()
    {
        if (credits)
        {
            credits.GetComponentInChildren<MoveTextUp>().finishedMovingUp += DisableCredits;
        }
    }

    private void OnDisable()
    {
        if (credits)
        {
            credits.GetComponentInChildren<MoveTextUp>().finishedMovingUp -= DisableCredits;
        }
    }

    private void DisableCredits()
    {
        credits.SetActive(false);
        EventSystem.current.SetSelectedGameObject(null);
    }

    public void StartGame()
    {
        GameInformation.Instance.GetComponent<LoadScenes>().StartGame();
    }

    public void CreateNewGame()
    {
        CardCollectionContainer.Instance.cardCollection.idolCardCollections.Clear();
        CardCollectionContainer.Instance.SaveCardCollection();
    }

    public void ShowAlbum()
    {
        GameInformation.Instance.GetComponent<LoadScenes>().LoadAlbumScene();
    }

    public void ShowCredits()
    {
        credits.SetActive(true);
        credits.GetComponentInChildren<MoveTextUp>().StartMovingUp();
    }

    public void OpenOptionsMenu()
    {
        startMenu.SetActive(false);
        optionsMenu.SetActive(true);
    }

    public void HideOptionsMenu()
    {
        optionsMenu.SetActive(false);
        startMenu.SetActive(true);
    }

    public void SetFrameColor(string color)
    {
        if (color.Equals("Black"))
        {
            memoryCardTemplate.GetComponent<Renderer>().material = blackFrameMaterial;
            winningCardTemplate.GetComponent<Renderer>().material = blackFrameMaterial;
}
        else if (color.Equals("White"))
        {
            memoryCardTemplate.GetComponent<Renderer>().material = whiteFrameMaterial;
            winningCardTemplate.GetComponent<Renderer>().material = whiteFrameMaterial;
        }
        EventSystem.current.SetSelectedGameObject(null);
    }

    public void ToggleBackgroundMusic()
    {
        if (isMusicPlaying)
        {
            GameInformation.Instance.backgroundMusic.Stop();
            optionMusicButtonText.text = "ENABLE";
        }
        else
        {
            GameInformation.Instance.backgroundMusic.Play();
            optionMusicButtonText.text = "DISABLE";
        }
        isMusicPlaying = !isMusicPlaying;
        EventSystem.current.SetSelectedGameObject(null);
    }

    public void ToggleCaching()
    {
        if (isCachingActivated)
        {
            GameInformation.Instance.cachingTexturesAllowed = false;
            optionCacheButtonText.text = "ENABLE";
        }
        else
        {
            GameInformation.Instance.cachingTexturesAllowed = true;
            optionCacheButtonText.text = "DISABLE";
        }
        isCachingActivated = !isCachingActivated;
        EventSystem.current.SetSelectedGameObject(null);
    }

    public void ClearCache()
    {
        string path = Path.Combine(Application.streamingAssetsPath , "Cache");
        if (Directory.Exists(path))
        { 
            Directory.Delete(path, true);
        }
        EventSystem.current.SetSelectedGameObject(null);
    }

    public void Exit()
    {
        Application.Quit();
    }

}