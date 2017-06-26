using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using UnityEditor;

public class CalculateDistance : EditorWindow {

    private Object object1;
    private Object object2;
    [MenuItem("Tools/CalculateDistance")]
    static void ShowWindow()
    {
        GetWindow(typeof(CalculateDistance), true, "Calculate Distance");
    }

    private void OnGUI()
    {
        GUILayout.Label("Drag&Drop two GameObjects to calculate distance from GameObject1 to GameObject2.");
        EditorGUILayout.BeginHorizontal();
        object1 = EditorGUILayout.ObjectField(object1, typeof(Transform), true);
        object2 = EditorGUILayout.ObjectField(object2, typeof(Transform), true);
        EditorGUILayout.EndHorizontal();
        Transform transform1 = (Transform)object1;
        Transform transform2 = (Transform)object2;
        if(object1 != null && object2 != null)
            EditorGUILayout.LabelField("Distance: " + GetDistance( transform1.position, transform2.position));
    }

    private float GetDistance(Vector3 position1, Vector3 position2)
    {
        return (position2 - position1).magnitude;
    }
}
