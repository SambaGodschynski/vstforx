package frx.com.vstforxmobile.events;

import java.util.EventObject;

public class MessageEvent extends EventObject {
	protected String message;
	protected String request;
	
	public MessageEvent(Object source) {
		super(source);
	}
	
	public MessageEvent(Object source, String request, String message) {
		super(source);
		this.message = message;
		this.request = request;
	}

	/**
	 * @return the message
	 */
	public String getMessage() {
		return message;
	}

	/**
	 * @return the request
	 */
	public String getRequest() {
		return request;
	}

}
