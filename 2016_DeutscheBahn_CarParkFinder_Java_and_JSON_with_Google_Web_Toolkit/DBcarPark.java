package de.dbCarPark.client;

import com.google.gwt.core.client.EntryPoint;
import com.google.gwt.core.client.JsonUtils;
import com.google.gwt.event.dom.client.ClickEvent;
import com.google.gwt.event.dom.client.ClickHandler;
import com.google.gwt.event.dom.client.KeyCodes;
import com.google.gwt.event.dom.client.KeyPressEvent;
import com.google.gwt.event.dom.client.KeyPressHandler;
import com.google.gwt.http.client.Request;
import com.google.gwt.http.client.RequestBuilder;
import com.google.gwt.http.client.RequestCallback;
import com.google.gwt.http.client.RequestException;
import com.google.gwt.http.client.Response;
import com.google.gwt.user.client.ui.Button;
import com.google.gwt.user.client.ui.FlexTable;
import com.google.gwt.user.client.ui.HorizontalPanel;
import com.google.gwt.user.client.ui.Label;
import com.google.gwt.user.client.ui.ListBox;
import com.google.gwt.user.client.ui.RootPanel;
import com.google.gwt.user.client.ui.TextBox;
import com.google.gwt.user.client.ui.VerticalPanel;

/**
 * Entry point classes define <code>onModuleLoad()</code>.
 */
public class DBcarPark implements EntryPoint {

	
	/* JSON URLs*/
	private static final String JSON_URL_Sites = "http://opendata.dbbahnpark.info/api/beta/sites";
	private static final String JSON_URL = "http://opendata.dbbahnpark.info/api/beta/occupancy";
	/* Objects that hold JSON Data*/
	private DBParkAllocationData allocationData;
	private DBParkData parkData;
	
	// show Error Message
	private Label errorMsgLabel = new Label();
	
	/* Car park location finder */
	private Label locationFinderLabel = new Label(); //only a text label
	private HorizontalPanel locationFinderPanel;   
	private VerticalPanel   locationMainPanel;
	private TextBox         locationSearchTextBox;
	private Button			findLocationButton;
	private FlexTable       locationResultTable;
	/* Car Park occupancy finder */
	private ListBox carparkList;
	private Button  findButton;
	private FlexTable resultTable;
	private Label carparkFinderLabel = new Label(); //only a text label
	private HorizontalPanel finderPanel;
	private VerticalPanel finderMainPanel;
		
	/**
	 * This is the entry point method.
	 */
	public void onModuleLoad() {
		
		createLocationFinder_UI();
		createCarParkFinder_UI();
	   
	    try {
	    	
	    	 RequestBuilder builder0 = new RequestBuilder(RequestBuilder.GET, JSON_URL_Sites);
	    	 builder0.sendRequest(null, new RequestCallback()
	    	 {	 
	    		  public void onError(Request request, Throwable exception) {
		    	      displayError("Couldn't retrieve JSON");
		    	  }

		    	  public void onResponseReceived(Request request, Response response)
		    	  {
		    	      if (200 == response.getStatusCode())
		    	      {
		    	    	  String json_string = response.getText();
		    	    	  parkData = JsonUtils.<DBParkData>safeEval(json_string);	
		    	   
		    	      } else {
		    	        displayError("Couldn't retrieve JSON (" + response.getStatusText()
		    	            + ")");
		    	      }
		    	    }
	    	  });
	    	
	    	
	    	 RequestBuilder builder1 = new RequestBuilder(RequestBuilder.GET, JSON_URL);
	    	 builder1.sendRequest(null, new RequestCallback() 
	    	 { 
	    	    public void onError(Request request, Throwable exception)
	    	    {
	    	      displayError("Couldn't retrieve JSON");
	    	    }

	    	    public void onResponseReceived(Request request, Response response)
	    	    {
	    	      if (200 == response.getStatusCode())
	    	      {
	    	    	  String json_string = response.getText();
	    	    	  allocationData = JsonUtils.<DBParkAllocationData>safeEval(json_string);	
	    	    	  updateListBox(allocationData);
	    	      } else {
	    	        displayError("Couldn't retrieve JSON (" + response.getStatusText()
	    	            + ")");
	    	      }
	    	    }
	    	  });
	    	} catch (RequestException e) {
	    	  displayError("Couldn't retrieve JSON");
	    	}	    
	}
	
	private void createLocationFinder_UI(){
		
		// Create the horizontal Panel, which hold the location name search
		locationFinderPanel = new HorizontalPanel();
		
		// Create the vertical Panel, that hold the result (google maps location, information) 
		locationMainPanel = new VerticalPanel();
		
		errorMsgLabel.setStyleName("errorMessage");
		errorMsgLabel.setVisible(false);
		
		locationFinderLabel.setText("Find your car park:");
		locationFinderLabel.setStyleName("carparkFinderLabel");
		
		locationSearchTextBox = new TextBox();
		locationSearchTextBox.setStyleName("carparkFinderLabel");
		locationSearchTextBox.setWidth("290px");
		// set placeholder in the text box
		locationSearchTextBox.getElement().setPropertyString("placeholder", " find train station");
		locationSearchTextBox.addKeyPressHandler(new KeyPressHandler(){
		@Override
		public void onKeyPress(KeyPressEvent event) {
			// show result when Enter key is pressed
			if (event.getCharCode() == KeyCodes.KEY_ENTER) {
				deleteSelectedItemInfo(); // delete previous information in the table
				locationResultTable.setVisible(true); // set table visible
				setSearchedItemInfo(parkData);	// set information
			}		
		}
		}
		
		);
		
		 // Make a find button
	    findLocationButton = new Button("Find");
	    findLocationButton.addStyleDependentName("find");
	    // Add a handler 
	 	findLocationButton.addClickHandler(new ClickHandler() {
	 			public void onClick(ClickEvent event) {
	 				deleteSelectedItemInfo(); // delete previous information in the table
					locationResultTable.setVisible(true); // set table visible
					setSearchedItemInfo(parkData);	// set information
	 			}
	 			
	 		});
		
		// add widgets to panel
		locationFinderPanel.add(locationFinderLabel);		
		locationFinderPanel.add(locationSearchTextBox);
		locationFinderPanel.add(findLocationButton);
		locationFinderPanel.addStyleName("searchBox");
		
		
		// create the location result table
		locationResultTable = new FlexTable();
		locationResultTable.setText(0,0, "Station name");
		locationResultTable.setText(0,1, "Car park name");
		locationResultTable.setText(0,2, "Car park type");
		locationResultTable.setText(0,3, "Car-parking space");
		locationResultTable.setText(0,4, "Is out of service");
		locationResultTable.setText(0,5, "Approach");
		locationResultTable.setText(0,6, "Opening times");
		locationResultTable.setText(0,7, "Max. parking time");
		locationResultTable.setText(0,8, "Occupancy");
		locationResultTable.setText(0, 9, "Show payment info");
	    
		locationResultTable.setVisible(false); // hide the result table
	    
	    //add styles for the table
		locationResultTable.addStyleName("resultTableLocation");
		locationResultTable.getRowFormatter().addStyleName(0, "resultTableHeader");
		
		 // sets the amount of padding (in pixels) to be added around cells
		locationResultTable.setCellPadding(10);
	    
		// add widgets to the main panel
		locationMainPanel.add(errorMsgLabel);
		locationMainPanel.add(locationFinderPanel);
		locationMainPanel.add(locationResultTable);
		
		// Add it to the root panel.
		RootPanel.get("locationFinder").add(locationMainPanel);
	}
	
	
	private void createCarParkFinder_UI(){
		
		 // Make the horizontal Panel, that hold the list box and the find button
		finderPanel = new HorizontalPanel();
		
		// Make the vertical Panel for displaying result
		finderMainPanel = new VerticalPanel();
		
		carparkFinderLabel.setText("Select the car park to get information about occupancy!");
		carparkFinderLabel.setStyleName("carparkFinderLabel");
			
		 // Make a new list box, adding a few items to it.
	    carparkList = new ListBox();
	    
	    // Make enough room for all five items (setting this value to 1 turns it
	    // into a drop-down list).
	    carparkList.setVisibleItemCount(1);

	    //create the hidden result label
	    resultTable = new FlexTable();
	    // set table header row
	    resultTable.setText(0,0, "Station name");
	    resultTable.setText(1,0, "Car park name");
	    resultTable.setText(2, 0, "Occupancy");
	    resultTable.setText(3, 0, "Timestamp");    
	    resultTable.setVisible(false);    
	    
	    //add styles
	    resultTable.addStyleName("resultTable");
	    resultTable.getColumnFormatter().addStyleName(0, "resultTableHeader");
	    
	    // sets the amount of padding (in pixels) to be added around cells
	     resultTable.setCellPadding(6);

	    // Make a find button
	    findButton = new Button("Find");
	    findButton.addStyleDependentName("find");
	    // Add a handler 
	 	findButton.addClickHandler(new ClickHandler() {
	 			public void onClick(ClickEvent event) {
	 				
	 				setSelectedItemInfo(allocationData);
	 				resultTable.setVisible(true);
	 			}
	 			
	 		});
	    
	    
	    //Add them to the finder panel.
	    finderPanel.add(carparkList);
	    finderPanel.add(findButton);
	    finderMainPanel.add(carparkFinderLabel);
	    finderMainPanel.add(finderPanel);
	    finderMainPanel.add(resultTable);
	   	    
	    // Add it to the root panel.
	    RootPanel.get("finder").add(finderMainPanel);		
	}
	
	
	private void updateListBox(DBParkAllocationData allocationData){
		for (int i = 0; i < allocationData.getLength(); i++) {
			carparkList.addItem(allocationData.getStationName(i) +" - " + allocationData.getSiteName(i));
	    }
		
		
	}
	
	private void setSelectedItemInfo(DBParkAllocationData allocationData){
		
		String selectedItem = carparkList.getSelectedItemText(); // get selectedItem from list box
		int selectedItemIndex = -1;
		for (int i = 0; i < allocationData.getLength(); i++) {
			
			String currName = allocationData.getStationName(i) +" - " + allocationData.getSiteName(i);
			if(currName.equals(selectedItem))
			{
				selectedItemIndex = i;
			}
	    }
		
		resultTable.setText(0, 1, allocationData.getStationName(selectedItemIndex));
		resultTable.setText(1, 1, allocationData.getSiteName(selectedItemIndex));
		resultTable.setText(2, 1, allocationData.getOccupancyText(selectedItemIndex));
		resultTable.setText(3, 1, allocationData.getTimestamp(selectedItemIndex));
	}

	
	private void setSearchedItemInfo(DBParkData data){
		
		String searchedItem = locationSearchTextBox.getText().toUpperCase();
		String prefix = searchedItem.substring(0, 1);
		String suffix = searchedItem.substring(1).toLowerCase();
		
		searchedItem = prefix + suffix;
		boolean isSearchedItemExisting = false;
		int rowIndex = 1;
		for (int i = 0; i < data.getLength(); i++) {
			
			String currName = data.getCityName(i);
			if(currName.contains(searchedItem))
			{
				isSearchedItemExisting = true;
				locationResultTable.setText(rowIndex, 0, data.getCityName(i));
				
				if(!data.getParkraumName(i).isEmpty())
					locationResultTable.setText(rowIndex, 1, data.getParkraumName(i));
				else if(!data.getParkraumDisplayName(i).isEmpty())
					locationResultTable.setText(rowIndex, 1, data.getParkraumDisplayName(i));
				else
					locationResultTable.setText(rowIndex, 1, data.getCityName(i));
				locationResultTable.setText(rowIndex, 2, data.getParkraumParkTypName(i));
				locationResultTable.setText(rowIndex, 3, data.getParkraumStellplaetze(i));
				locationResultTable.setText(rowIndex, 4, data.getParkraumAusserBetrieb(i));
				locationResultTable.setText(rowIndex, 5, data.getParkraumZufahrt(i));
				locationResultTable.setText(rowIndex, 6, data.getParkraumOeffnungszeiten_en(i));
				locationResultTable.setText(rowIndex, 7, data.getTarifParkdauer_en(i));
				
				
				for(int j = 0; j < allocationData.getLength(); j++){
					if(data.getParkraumID(i) == allocationData.getID(j))
						locationResultTable.setText(rowIndex, 8, allocationData.getOccupancyText(j));
				
				}
				
				// create show info-button
				Button showInfoButton = new Button("Show");
				int index = i;
				showInfoButton.addStyleDependentName("find");
				// add click handler 
				showInfoButton.addClickHandler(new ClickHandler() {
				 			public void onClick(ClickEvent event) {
				 				// instantiate the dialog box and show it
				 				PaymentInfoDialog dialog = new PaymentInfoDialog(showInfoButton.getAbsoluteLeft(), showInfoButton.getAbsoluteTop(), data, index);
				 				dialog.show();
				 			}
				 			
				 		});
				locationResultTable.setWidget(rowIndex, 9, showInfoButton);
			
				rowIndex++;
			}
		
		}
		if(!isSearchedItemExisting ){
		
			locationResultTable.setVisible(false); // hide table
		}
	}
	
	private void deleteSelectedItemInfo(){
		while(locationResultTable.getRowCount() > 1)				
			locationResultTable.removeRow(1);
		
	}
	
	private void displayError(String error) {
		  errorMsgLabel.setText("Error: " + error);
		  errorMsgLabel.setVisible(true);
	}
	 

}
