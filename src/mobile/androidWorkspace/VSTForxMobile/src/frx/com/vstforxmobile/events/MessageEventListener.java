package frx.com.vstforxmobile.events;
import java.util.EventListener;

public interface MessageEventListener extends EventListener {
	void onMessage(MessageEvent ev);
}
