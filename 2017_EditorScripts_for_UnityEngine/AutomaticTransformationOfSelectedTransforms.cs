using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class AutomaticTransformationOfSelectedTransforms : EditorWindow
{
    private Vector3 translatingVector;
    private Vector3 rotatingEulerAngles;

    [MenuItem("Tools/AutomaticTransformation")]
    static void ShowWindow()
    {
        GetWindow(typeof(AutomaticTransformationOfSelectedTransforms), true, "Automatic Transformation");
    }

    private void OnGUI()
    {
        GUILayout.Label("Translate all selected Transform(s) in the hierarchy view:");

        translatingVector = EditorGUILayout.Vector3Field("Translate vector", translatingVector);

        GUILayout.Label("Rotate all selected Transform(s) in the hierarchy view:");

        rotatingEulerAngles = EditorGUILayout.Vector3Field("Euler angles for rotation", rotatingEulerAngles);

        if (GUILayout.Button( "Transformate", GUILayout.Width(100)))
        {
            GameObject[] selectedGameObjects = Selection.gameObjects;
            for(int i = 0; i < selectedGameObjects.Length; i++)
            {
                if (selectedGameObjects[i].GetComponent<Transform>())
                {
                    DoTransformation(selectedGameObjects[i].GetComponent<Transform>());
                }
            }
        }
    }

    private void DoTransformation(Transform transform)
    {
        Undo.RecordObject(transform, "Transformate Object");
        transform.position = transform.position + translatingVector;
        transform.Rotate(rotatingEulerAngles);
    }
}
