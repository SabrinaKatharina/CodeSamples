using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using System.Linq;

public class AutomaticGameObjectsNumbering : EditorWindow {

    private string newName;
    [MenuItem("Tools/AutomaticNumbering")]
    static void ShowWindow()
    {
        GetWindow(typeof(AutomaticGameObjectsNumbering),true,"Automatic Numbering");
    }

    private void OnGUI()
    {
        List<GameObject> numberingGameObjects = new List<GameObject>();
        // first, get a list with all objects that are selected
        for (int i = 0; i < Selection.gameObjects.Length; i++)
        {
            numberingGameObjects.Add(Selection.gameObjects[i]);
        }
        // sort objects by sibling index
        List<GameObject> sortedList = numberingGameObjects.OrderBy(go => go.transform.GetSiblingIndex()).ToList();
        numberingGameObjects = sortedList;
        
        GUILayout.Label("Selected GameObjects and their sibling index");
        for (int k = 0; k < numberingGameObjects.Count; k++)
        {
            GUILayout.Label(numberingGameObjects[k].name + " , " + numberingGameObjects[k].transform.GetSiblingIndex());
        }

        GUILayout.Label("New name for all selected GameObject(s) in the hierarchy view:");
        
        newName =  EditorGUILayout.TextField("New name: " , newName, GUILayout.Width(250));

        for(int i = 0; i < numberingGameObjects.Count; i++)
        {
            RenameSelectedGameObjects(numberingGameObjects[i], newName, i);
        }
    }

    void RenameSelectedGameObjects(GameObject selected, string name, int index)
    {
        Undo.RegisterCompleteObjectUndo(selected, "GameObject(s) name change");
        selected.name = name + " (" + index + ")";
    }
}
