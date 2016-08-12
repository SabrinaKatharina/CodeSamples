package penrose;

import java.util.ArrayList;

public class PenTriangle {

	public Vertex[] vertices; // jedes Triangle hat 3 Vertices
	public boolean triangleCase; // false: für das spitze Dreieck, true: für das stumpfe Dreieck
	private PenTriangle[] subTriangles; //speichere seine subTriangles, falls es subdivided ist

	//Konstruktor
	public PenTriangle(Vertex[] v, boolean triCase) {
		vertices = v;
		triangleCase = triCase;
	}

	public void subdivide(int depth, ArrayList<PenTriangle> triList) {
		if(depth > 0){
			Vertex newPointP = new Vertex(0.0, 0.0, 0.0); // Hilfspunkt
			// falls es das spitze Dreieck ist
			if (!triangleCase) {
				subTriangles = new PenTriangle[2]; // das spitze Dreieck wird in 2 Dreiecke geteilt
				// newP = C + (A - C)/goldenRatio
				newPointP = add(vertices[2], subtract(vertices[0], vertices[2]));
				subTriangles[0] = new PenTriangle(new Vertex[] { newPointP, vertices[0], vertices[1] }, false);
				subTriangles[1] = new PenTriangle(new Vertex[] { vertices[1], vertices[2], newPointP }, true);
			} //falls es das stumpfe Dreieck ist
			else {
				Vertex newPointQ = new Vertex(0.0, 0.0, 0.0); //zweiter Hilfspunkt
				subTriangles = new PenTriangle[3]; // das stumpfe Dreieck wird in 3 Dreiecke geteilt
				// newP = A + (C - A) /goldenRatio
				newPointP = add(vertices[0], subtract(vertices[2], vertices[0]));
				// newQ = A + (B - A) /goldenRatio
				newPointQ = add(vertices[0], subtract(vertices[1], vertices[0]));
				subTriangles[0] = new PenTriangle(new Vertex[] {newPointQ, vertices[0], newPointP}, true);
				subTriangles[1] = new PenTriangle(new Vertex[] {newPointP, vertices[2], newPointQ}, false);
				subTriangles[2] = new PenTriangle(new Vertex[] {vertices[1], vertices[2], newPointQ}, true);
			}
		}
		if (--depth > 0) { // solange es noch weiter geteilt wird
			for (PenTriangle p : subTriangles) { // hole alle subTriangles
				p.subdivide(depth, triList); // Rekursion: teile weiter
			}
		}
		else if(depth == 0) { // Abbruchkriterium : depth == 0
			for (PenTriangle p : subTriangles) {
				triList.add(p); // füge die subTriangles der PenTriangle-ArrayListe hinzu
			}
		}
		else { // depth war 0, d.h. keine Subdivision wird ausgeführt , sondern das StartTriangle wird gezeichnet
			triList.add(this); // füge sich selbst hinzu
		}
	}

	//subtrahiere Vertex a von Vertex b und teile im Verhältnis des goldenen Schnitts
	public Vertex subtract(Vertex a, Vertex b) {

		Vertex r = new Vertex(0.0, 0.0, 0.0);
		r.setX(a.getX() - b.getX());
		r.setY(a.getY() - b.getY());
		r.divideByGoldenRation(); // teile im Verhältnis des goldenen Schnitts
		return r;
	}

	//addiere Vertex a mit Vertex b
	public Vertex add(Vertex a, Vertex b) {

		Vertex r = new Vertex(0.0, 0.0, 0.0);
		r.setX(a.getX() + b.getX());
		r.setY(a.getY() + b.getY());
		return r;
	}
}
