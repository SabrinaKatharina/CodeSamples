package de.dbCarPark.client;

import com.google.gwt.core.client.JavaScriptObject;

public class DBParkAllocationData extends JavaScriptObject {
	
	protected DBParkAllocationData() {  }                                          // <span style="color:black;">**(2)**</span>

	public final native String getStationName(int index)  /*-{ return this.allocations[index].site.stationName; }-*/;
	public final native String getSiteName(int index)  /*-{ return this.allocations[index].site.siteName; }-*/;
	public final native String getOccupancyText(int index)  /*-{ return this.allocations[index].allocation.text; }-*/;
	public final native String getTimestamp(int index)  /*-{ return this.allocations[index].allocation.timestamp; }-*/;

	public final native String getID(int index)  /*-{ return this.allocations[index].site.id; }-*/;
	
	
	public final native int getLength() /*-{ return this.allocations.length}-*/;
}
