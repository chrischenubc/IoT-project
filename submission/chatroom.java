package com.led_on_off.led;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.ListView;
import android.widget.TextView;
import android.util.Log;



import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;
import com.google.android.gms.appindexing.Action;
import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.appindexing.Thing;
import com.google.android.gms.common.api.GoogleApiClient;

import java.lang.reflect.Array;
import java.net.URISyntaxException;
import java.util.ArrayList;


import org.json.JSONException;
import org.json.JSONObject;
/**
 * Created by linzi on 2017-03-27.
 */

public class chatroom extends ActionBarActivity {

    /*
    Used for chat room
     */
    String UserName = "Bob"; //default username
    String value;
    private Socket socket;  //define a IOsocket to communicate with Web Server
    EditText chatEditText;
    ArrayList<String> messages = new ArrayList<String>();
    ArrayAdapter<String> messageAdapter;

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */

    private GoogleApiClient client;

    {
        try {
            socket = IO.socket("http://38.88.74.87:9000");
        } catch (URISyntaxException e) {
            throw new RuntimeException(e);
        }
    }




    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.chatroom);
        socket.connect(); // connect socket
        socket.on("chat", chatwithserver); // IO socket will listen to chat event

        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();

        /*
         *define message list and display them on screen
         */
        ListView chatListView = (ListView)findViewById(R.id.charListView); //get text message by ID
        messageAdapter = new ArrayAdapter<String>(this,                    //this arrayAdapter linked to the ArrayList called "messages"
                android.R.layout.simple_list_item_1,
                messages);

        chatListView.setAdapter(messageAdapter);

         value = getIntent().getStringExtra("string_name");

    }

    private Emitter.Listener chatwithserver = new Emitter.Listener() {
        @Override

        public void call(final Object... args) {
            chatroom.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    String message = (String) args[0];
                    messages.add(message);
                    messageAdapter.notifyDataSetChanged();

                }
            });
        }
    };

    public void login(View v){
        Intent i = new Intent(this, login.class);
        startActivity(i);
    }
    /*
     *This method will send data to Web Server and update the message on the screen
     *@param v
     *
     */
    public void sendChat(View v){

        chatEditText = (EditText)findViewById(R.id.message);       //get EditText type message by finding ID
        String messageContent = chatEditText.getText().toString(); // Convert it to String type
        if(value != null){
            UserName = value;
        }
        socket.emit("message",UserName+": " + messageContent);      // send message to Web Server
        messages.add(UserName+": " + messageContent);               // add this message to arrayList "messages"
        messageAdapter.notifyDataSetChanged();                      // update the ArrayAdapter

    }

}
