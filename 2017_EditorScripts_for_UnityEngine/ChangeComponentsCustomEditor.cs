using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;
using UnityEngine.Networking;

/// <summary>
/// CustomEditor script can disable/enable components of type Behaviour, Collider and Renderer on Transform GameObjects. 
/// If Transform GameObject has child/children, it asks if the components of all children should be disabled/enabled, too.
/// </summary>
[CustomEditor(typeof(Transform))]
public class ChangeComponentsCustomEditor: Editor {

    private Transform transform; // target transform

    public override void OnInspectorGUI()
    {
        DrawDefaultInspector();
        transform = (Transform)target;
        EditorGUILayout.BeginHorizontal();
        //Add disable button
        if(GUILayout.Button("Disable all components", GUILayout.Width(150), GUILayout.Height(20)))
        {
            ChangeComponentsEnabledProperty(false);
        }
        //Add enable button
        if (GUILayout.Button("Enable all components", GUILayout.Width(150), GUILayout.Height(20)))
        {
            ChangeComponentsEnabledProperty(true);
        }
        EditorGUILayout.EndHorizontal();
    }

    public void ChangeComponentsEnabledProperty(bool enable)
    {
        EnableComponentsOnGameObject(enable, transform.gameObject);

        if (CheckIfHasChildren(transform.gameObject))
        {
            if (EditorUtility.DisplayDialog("Do you want to change component(s) on all children, too?", "Disable/Enable component(s) on all children of this GameObject", "Change Children", "Do not"))
            {
                // Enable/Disable components recursive so all children and their children (and so on) also change their components
                for (int i = 0; i < transform.transform.childCount; i++)
                {
                    ChangeComponentsEnabledRecursive(enable, transform.transform.GetChild(i).gameObject);
                }
            }
        }
    }

    bool CheckIfHasChildren(GameObject parent)
    {
        return (parent.transform.childCount > 0);
    }


    void ChangeComponentsEnabledRecursive(bool enable, GameObject parent)
    {
        if (parent.transform.childCount == 0) //leaf is reached
        {
            EnableComponentsOnGameObject(enable, parent);
        }
        else
        {
            EnableComponentsOnGameObject(enable, parent);
            for (int i = 0; i < parent.transform.childCount; i++)
            {
                ChangeComponentsEnabledRecursive(enable, parent.transform.GetChild(i).gameObject);
            }
        }
    }

    void EnableComponentsOnGameObject(bool enable, GameObject gameObject)
    {
        Component[] components = gameObject.GetComponents<Component>();

        for (int i = 0; i < components.Length; i++)
        {
            if (components[i] == null)
            {
                Debug.LogError("Component at " + i + " is null.");
                continue;
            }
            else
            {
                if (components[i] is Behaviour && !components[i].GetType().Equals(typeof(NetworkIdentity)))
                    (components[i] as Behaviour).enabled = enable;
                else if (components[i] is Collider)
                    (components[i] as Collider).enabled = enable;
                else if (components[i] is Renderer)
                    (components[i] as Renderer).enabled = enable;
            }
        }
    }
}
