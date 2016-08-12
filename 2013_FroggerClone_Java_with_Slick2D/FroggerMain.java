import java.util.ArrayList;
import java.util.List;

import org.newdawn.slick.Color;
import org.newdawn.slick.GameContainer;
import org.newdawn.slick.Image;
import org.newdawn.slick.Input;
import org.newdawn.slick.SlickException;
import org.newdawn.slick.Graphics;
import org.newdawn.slick.state.*;
import org.newdawn.slick.Sound;

public class FroggerMain extends BasicGameState {

	protected Player player;
	protected Image background;
	protected Sound hit;
	protected List<Orca> orcas;
	protected List<Iceberg> icebergs;
	protected List<Polarbear> polarbears;
	protected List<Penguin> penguins;
	protected List<Fish> fish;
	protected Harpoon harpoon1, harpoon2;
	protected Goal flag;
	protected int points;
	protected int currentState;
	

	public FroggerMain(int state) {

		currentState = state;

	}
	
	public int getID(){
		
		return 1;
	}

	@Override
	public void init(GameContainer container, StateBasedGame sbg) throws SlickException {

		background = new Image("img/polarsea.png");
		
		hit = new Sound("res/bang-03-clean.WAV");
		
		player = new Player(560, 700 ); // with startPosition X=560 and Y=700
		
		player.init(container);
		
		flag = new Goal(container.getWidth()/2, 5, 0, 0);  // flag is nearby the upper screen border
		
		flag.init(container);
		
		// create  and initialize enemys
		orcas = new ArrayList<Orca>();

		for (int i = 0; i < 5; ++i) {
			orcas.add(new Orca(i * 220, 600, -120, 0));
		}

		for (Orca orca : orcas) {
			orca.init(container);
		}

		polarbears = new ArrayList<Polarbear>();

		for (int i = 0; i < 6; ++i) {
			polarbears.add(new Polarbear(i * 240, 500, 80, 0));

		}

		for (Polarbear polarbear : polarbears) {

			polarbear.init(container);
		}
		
		penguins = new ArrayList<Penguin>();

		for (int i = 0; i < 13; ++i) {
			penguins.add(new Penguin(i * 46, 335, 110, 0));

		}

		for (Penguin penguin : penguins) {

			penguin.init(container);
		}
		
		icebergs = new ArrayList<Iceberg>();
		
		for (int i = 0; i< 3; ++i){
			
			
			icebergs.add(new Iceberg(i* 590, 170, -60, 0));
		}
		
		for (Iceberg iceberg : icebergs){
			
			iceberg.init(container);
		}
		
		harpoon1 = new Harpoon(20, 115, 650, 0);
		
		harpoon1.init(container);
		
		harpoon2 = new Harpoon( container.getWidth()-90, 300, -650, 0);
		
		harpoon2.init(container);
		
		// create and initialize collectable fish objects 
		fish = new ArrayList<Fish>();
		
		for (int i = 0; i < 8; ++i){
			double randomX = Math.random() * 950 + 20;
			double randomY = Math.random() * 500 + 100;
			fische.add(new Fish((int) randomX,(int) randomY));
			
			
		}
		for (Fish fishObj : fish) {
			fishObj.init(container);
		}

	}
	
	

	@Override
	public void update(GameContainer container, StateBasedGame sbg, int delta) throws SlickException {

		Input input = container.getInput();

		if (input.isKeyDown(Input.KEY_ESCAPE))  // exits game with key ESC
			container.exit();

		// check if game is over
		if(player.getLife() == 1 && player.dead == true){
			
			sbg.enterState(3); // enter GameOver state
		}
		
		player.update(container, delta);     // call update function of player
		
		
		for (Orca orca : orcas) {
			orca.update(container, delta);
		}
		
		// Orcas collision check
		for (int i = 0; i < orcas.size(); i++) {

			Orca orca = orcas.get(i);

			if (player.collide(orca.getShape())) {
				
				hit.play();
				player.dead = true;
				
			}
		}

		for (Icebaer icebaer : icebaers) {

			icebaer.update(container, delta);
		}
		
		// Icebear collision check
		for (int i = 0; i < icebaers.size(); i++) {

			Icebear icebaer = icebaers.get(i);

			if (player.collide(icebaer.getShape())) {
				
				hit.play();
				player.dead = true;
			}
		}
		
		for (Penguin penguin : penguins) {

			penguin.update(container, delta);
		}
		
		// Penguin collision check
		for (int i = 0; i < penguins.size(); i++) {

			Penguin penguin = penguins.get(i);

			if (player.collide(penguin.getShape())) {
				
				hit.play();
				player.dead = true;
			}
		}
		
		for (Iceberg iceberg : icebergs){
			
			iceberg.update(container, delta);
		}
		
		// Iceberg collision
		for (int i = 0; i < icebergs.size(); i++) {

			Iceberg iceberg = icebergs.get(i);

			if (player.collide(iceberg.getShape())){
				
				hit.play();
				player.dead = true;
			}
		}
		
		harpoon1.update(container, delta);
		
		harpoon2.update(container, delta);
		
		// Harpoon collision check
	    if(player.collide(harpoon1.getShape())){
	    	
	    	hit.play();
	    	player.dead = true;
	    }
	    
	    if(player.collide(harpoon2.getShape())){
	    	
	    	hit.play();
	    	player.dead = true;
	    }

		flag.update(container, delta);
		
		// flag collision check
		if(player.collide(flag.getShape()))
			sbg.enterState(2); // enter Game Win state   
		
		for( Fish fishObj : fish) {
			
			fish.update(container, delta);
		}
		
		// Fishe collision check
		for( int i = 0; i< fish.size(); i++) {
			
			Fish fishObj = fish.get(i);
			
			if (player.collide(fishObj.getShape())){
				
				hit.play();
				fishObj.dead = true;
				fish.remove(fishObj);
			}
		}
		
		// reduce "Leftover fish" points
		for(int i = -1; i < fish.size(); i++){

			points = i+1;
		}
	
	}
	
	@Override
	public void render(GameContainer container, StateBasedGame sbg, Graphics g) throws SlickException {

		background.draw(0, 0);
		
        for( Fish fishObj : fish) {
			
			fishObj.render(container, g);
			
		}
	   
		for (Orca orca : orcas) {
			orca.render(container, g);
		}

		for (Eisbaer eisbaer : eisbaeren) {

			eisbaer.render(container, g);
		}
		
		for (Pinguin pinguin : pinguine) {

			pinguin.render(container, g);
		}
		
		for (Iceberg iceberg : icebergs){
			
			iceberg.render(container, g);
		}
		
		harpoon1.render(container, g);
		
		harpoon2.render(container, g);
		
		flag.render(container, g);
		
		player.render(container, g);
		
		g.setColor(Color.magenta);
		
		String lifeUI = String.format("Life: " + player.getLife());
		
		g.drawString(lifeUI, 20, 20);
		
		String scoreUI = String.format("Leftover fish:" + points);
		
    	g.drawString(scoreUI, 870, 20);
    	
    	if(player.getPosX() == 560 && player.getPosY() == 700)
		{ // if player is at his startposition
    	
    		String instruction = String.format("Collect all fish!"+ "\n"+ "Quit game with key Esc");
    		
    		g.drawString(instruction, container.getWidth()/2- 100, 100);
		}	
	}	
}
