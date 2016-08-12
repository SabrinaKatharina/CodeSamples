//import org.newdawn.slick.Color;
import org.newdawn.slick.GameContainer;
import org.newdawn.slick.Graphics;
import org.newdawn.slick.Image;
import org.newdawn.slick.Input;
import org.newdawn.slick.SlickException;
import org.newdawn.slick.geom.Shape;
import org.newdawn.slick.geom.Rectangle;

public class Player{

	protected Image avatar;
	protected Shape collisionShape;
	protected float posX, posY, imageWidth, imageHeight, halfWidth, halfHeight, startPositionX, startPositionY;
	protected int counter = 0;
	protected boolean dead = false; // false = alive; true = dead
	protected int life = 3; // how many times the player can revive

	public Player(float posX, float posY){
		
		this.posX = posX;
		this.posY = posY;
		this.startPositionX = posX;
		this.startPositionY = posY;
		
		}
	
	// getter methods
	public int getLife(){
		
		return life;
	}
	
	public float getImageWidth(){
		
		return imageWidth;
	}

	public float getImageHeight(){
		
		return imageHeight;
	}
		
	public Shape getShape(){
		
		return collisionShape;
	}
	
	public Image getImage(){
		
		return avatar;
		
	}
	
	public float getStartPositionX(){
		
		return startPositionX;
		
	}
	
	public float getStartPositionY(){
		
		return startPositionY;
		
	}
	
	public float getPosX(){
		
		return posX;
	}
	
	public float getPosY(){
		
		return posY;
	}

	// collision check
	public boolean collide(Shape other){
 		
		return (collisionShape.intersects(other) || collisionShape.contains(other));
	
 	}  
	
	// GameContainer methods
	public void init(GameContainer container) throws SlickException {
		
		avatar = new Image ("img/player.png ");
		
		imageWidth= avatar.getWidth();
		
		imageHeight = avatar.getHeight();
		
		halfWidth = imageWidth / 2;
		
		halfHeight = imageHeight/ 2;
		
		collisionShape = new Rectangle(posX,posY,imageWidth,imageHeight);
          
	}

	public void update(GameContainer container, int delta) throws SlickException{
		
        Input input = container.getInput();
		
        if(dead == false){ // player can move only if he is alive
        	
        	if (input.isKeyDown(Input.KEY_LEFT)){ // is the left arrow key pressed
        		if(posX > 0 && posX + imageWidth <= 1024){ // is player not crossing the screen border
        			
        			posX -= 110.0 * delta/ 500.0F;
        			
        		} else  
        			posX = 0; //player cannot cross left screen border
    		} 
			
        	else if (input.isKeyDown(Input.KEY_RIGHT)){ // is the right arrow key pressed
    		
        		if(posX + imageWidth < 1024){
     			
        			posX += 110.0 * delta/ 500.0F;
     			
        		} else
        			posX = 1024 - imageWidth; //player cannot cross right screen border
        	}
    		
		
        	else if (input.isKeyDown(Input.KEY_UP)){ // is the up arrow key pressed
        		
        		if(posY > 0 && posY + imageHeight <= 768){
    			
        			posY -= 130.0 * delta / 500.0F;
        			
        		} else 
        			posY = 0; // player cannot cross lower screen border
    		} 
        	
        	else if (input.isKeyDown(Input.KEY_DOWN)){ // is the down arrow key pressed
        		
        		if(posY + imageHeight < 768 ){
    			
        			posY += 70.0 * delta / 500.0F;
        			
        		} else 
        			posY = 768 - imageHeight; //player cannot cross upper screen border
        	}
    	}
        
        else { // if dead run a counter and then return to startPosition and reduce the life counter 
        	
        	posX = startPositionX;
        	posY = startPositionY;
    		
    			if (counter == 100 && life > 1){ // player has at least one more life
    			
    				life -= 1;
    				counter = 0;
    				dead = false;
    			
    			}
    			else {
    				counter++;
    			}
    			if (life == 1 && dead == true) //player died with last life
    				
    				life = 0; //game over
    	}
		
		collisionShape.setX(posX);
		collisionShape.setY(posY);

	}

	public void render(GameContainer container, Graphics g) throws SlickException{
		
		if (dead == false) // if player is not dead
	    avatar.draw(posX, posY);
		
	}	
}
