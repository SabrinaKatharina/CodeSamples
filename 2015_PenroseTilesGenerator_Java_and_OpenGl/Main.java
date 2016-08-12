package penrose;

import static org.lwjgl.glfw.GLFW.*;
import static org.lwjgl.opengl.GL11.*;
import static org.lwjgl.opengl.GL20.*;
import static org.lwjgl.opengl.GL30.*;
import static org.lwjgl.opengl.GL15.*;
import static org.lwjgl.system.MemoryUtil.*;

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStreamReader;
import java.util.ArrayList;

import org.lwjgl.glfw.Callbacks;
import org.lwjgl.opengl.GLContext;

public class Main {

	private long window;
	private int program;
	private int vertexShader;
	private int fragmentShader;
	private int vbo_pos;  // vertex buffer object für position
	private int vbo_color; // vertex buffer object für color
	private int vao; // vertex array object
	private int vertexCount; // Anzahl aller Vertices
	private int colorSelection; // int-Index für die Farbauswahl der PenroseTiles
	private int count; // Anzahl subdivisions
	
	
	public static void main(String[] args) {
		Main m = new Main();
		m.run();
	}

	private void run() {
		// Einlesen
		System.out.println("Wie oft soll eine Subdivision ausgeführt werden?");
		BufferedReader input = null;
		input = new BufferedReader(new InputStreamReader(System.in));
		count = 0;
		colorSelection = 0;
		try {
			String line = input.readLine();
			count = Integer.parseInt(line);
			//Farbauswahl:
			System.out.println("Welche Farben sollen gewählt werden?\n"
					+"Rot/Grün  : Drücke 0\n"
					+"Rot/Blau  : Drücke 1\n"
					+"Rot/Gelb  : Drücke 2\n"
					+"Grün/Gelb : Drücke 3\n"
					+"Grün/Blau : Drücke 4\n"
					+"Falls > 4 oder < 0 gedrückt wird, ist Rot/Blau ausgewählt.");
			line = input.readLine();
			colorSelection = Integer.parseInt(line);
			
		} catch (IOException e) {
			e.printStackTrace();
		}
		
		// Initialisieren
		initFrame(); // initialsiere das Fenster
		GLContext context = GLContext.createFromCurrent();
		loadShaders(); //lade die Shader

		createGraphicData(); //erzeuge die Geometrien
		
		//Kernschleife
		while(glfwWindowShouldClose(window) == GL_FALSE){
			update(); 
			glfwSwapBuffers(window);
			glfwPollEvents();
			
		}
	}

	private void update() {
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(program);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}
	
	private void createGraphicData() {
		// Vertex-Array mit den Vertices, die die äußeren Positionen des Dreieck-Rads beschreiben (11 äußere Positionen)
		Vertex v[] = new Vertex [] {
				new Vertex(0.0, 0.9, 0.0),
				new Vertex(0.529, 0.728, 0.0),
				new Vertex(0.856, 0.278, 0.0),
				new Vertex(0.856, -0.278, 0.0),
				new Vertex(0.529, -0.728, 0.0),
				new Vertex(-0.0, -0.9, 0.0),
				new Vertex(-0.529, -0.728, 0.0),
				new Vertex(-0.856, -0.278, 0.0),
				new Vertex(-0.856, 0.278, 0.0),
				new Vertex(-0.529, 0.728, 0.0),
				new Vertex(0, 0.9, 0.0)
		};
		
		ArrayList<PenTriangle> triangles = new ArrayList<PenTriangle>(); // speichert alle PenTriangles (auch alle subTriangles eines PenTriangle)
		PenTriangle startTriangles[] = new PenTriangle[11]; // speichert alle elf Start-Triangles
		for(int i = 0; i < 10; i+=2){
			//erzeuge zwei PenTriangle, die 36° in ihren Apex hat (= goldenes Dreieck)
			startTriangles[i] = new PenTriangle(new Vertex[]{
					v[i], 
					v[i+1],
					new Vertex (0,0,0)}, false); //false, da es ein spitzes Dreieck ist
			// spiegle das zweite Dreieck und gehe eine Vertex-Position weiter
			startTriangles[i+1] = new PenTriangle(new Vertex[]{
					v[i+2],
					v[i+1],
					new Vertex (0,0,0)}, false); 
			//subdivide die Start-Triangles mit der eingelesenen Anzahl
			startTriangles[i].subdivide(count, triangles);
			startTriangles[i+1].subdivide(count, triangles);
		}
		
		vertexCount = triangles.size()*3; // berechne die Anzahl der Gesamt-Vertices
		
		double[][] posArr = new double[vertexCount][4]; // erzeuge das Positions-Array für alle Vertices
		float[][] colorArr = new float[vertexCount][4]; // erzeuge das Color-Array für alle Vertices
		
		// float-Vektoren für jede mögliche Farbe
		float[] colorRED = new float[] {1.f, 0.f, 0.f, 1.f};
		float[] colorGREEN = new float[] {0.f, 1.f, 0.f, 1.f};
		float[] colorBLUE = new float[] {0.f, 0.f, 1.f, 1.f};
		float[] colorYELLOW = new float[]{1.f, 1.f, 0.f, 1.f};
		
		int triCount = 0; //Zähler für das aktuelle PenTriangle in der ArrayList
		for(int i = 0; i < vertexCount; i+=3) //laufe alle Vertices durch, aber nur jeden ersten eines PenTriangles => +=3
		{
			for(int j = 0; j < 3; ++j) // laufe jeden Vertices eines PenTriangles durch => 0 , 1, 2
			{					
				// Werte-Zuweisung für X, Y, Z und W für einen Vertex
				posArr[i+j][0] = triangles.get(triCount).vertices[j].getX(); 
				posArr[i+j][1] = triangles.get(triCount).vertices[j].getY();
				posArr[i+j][2] = triangles.get(triCount).vertices[j].getZ();
				posArr[i+j][3] = triangles.get(triCount).vertices[j].getW();
				
				//Farb-Zuweisung für jeden Vertex in einem PenTriangle
				if(triangles.get(triCount).triangleCase){ // falls es das stumpfe Dreieck ist
					// Farbe je nach eingelesenen Wert
					if(colorSelection == 0)
						colorArr[i+j] = colorGREEN;
					else if(colorSelection == 2 || colorSelection == 3)
						colorArr[i+j] = colorYELLOW;
					else 
						colorArr[i+j] = colorBLUE;
				}
				else{ //falls es das spitze Dreieck ist
					if(colorSelection == 3 || colorSelection == 4)
						colorArr[i+j] = colorGREEN;
					else
						colorArr[i+j] = colorRED;
				}
			}
			 triCount++; //gehe ein PenTriangle in der ArrayList weiter
		}
		
		// VBOs
		vbo_pos = glGenBuffers();
		glBindBuffer(GL_ARRAY_BUFFER,	vbo_pos);
		glBufferData(GL_ARRAY_BUFFER, BufferHelper.convert(posArr), GL_STATIC_DRAW);
						
		vbo_color = glGenBuffers();
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glBufferData(GL_ARRAY_BUFFER , BufferHelper.convert(colorArr), GL_STATIC_DRAW);
		
		//VAO
		vao = glGenVertexArrays();
		glBindVertexArray(vao);
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_pos);
		glVertexAttribPointer(1, 4, GL_DOUBLE, false, 32, 0L);
						
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
		glVertexAttribPointer(2, 4, GL_FLOAT, false, 16, 0L);
	}

	private void loadShaders() {
		program = glCreateProgram();
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, ""
				+ "#version 330\n"
				+ "in vec4 in_pos;\n"
				+ "in vec4 in_color;\n"
				+ "out vec4 var_color;\n"
				+ "void main(){"
				+ "gl_Position = in_pos;\n"
				+ "var_color = in_color;\n"
				+ "}"
				+ "");
		glCompileShader(vertexShader);
		glAttachShader(program, vertexShader);
		
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, ""
				+"#version 330\n"
				+"out vec4 out_color;\n"
				+"in vec4 var_color;\n"
				+ "void main(){"
				+ "out_color=var_color;\n"
				+ "}\n"
				+ "");
		glCompileShader(fragmentShader);
		glAttachShader(program, fragmentShader);
		
		glBindAttribLocation(program, 1, "in_pos"); 
		glBindAttribLocation(program, 2, "in_color"); 
		glBindFragDataLocation(program, 0, "out_color");
		
		glLinkProgram(program);
	}

	private void initFrame() {
		glfwSetErrorCallback(Callbacks.errorCallbackPrint());
		if(glfwInit() != GL_TRUE){
			
			throw new RuntimeException("Kann GLFW nicht initialisieren.");
		}
		glfwDefaultWindowHints();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_ANY_PROFILE);
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
		
		window = glfwCreateWindow(700,700,"Penrose-Tiles", NULL, NULL );
		
		if(window == NULL){
			
			throw new RuntimeException("Fehler bei der Fenstererstellung");
		}
		glfwMakeContextCurrent(window);
		
		glfwSwapInterval(1);
		glfwShowWindow(window);
		
	}

	// print-Hilfsfunktionen
	public void printArray(float[][] a){
		
		for(int i = 0; i < a.length; ++i){
			System.out.println("\nVertex: " + i);
			for(int j = 0; j < 4; ++j){
				
				System.out.print(a[i][j] + " ");
			}
		}
	}
	public void printArray(double[][] a){
		
		for(int i = 0; i < a.length; ++i){
			System.out.println("\nVertex: " + i);
			for(int j = 0; j < 4; ++j){
				
				System.out.print(a[i][j] + " ");
			}
		}
	}
}
