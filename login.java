package com.led_on_off.led;

import android.content.Intent;
import android.os.Bundle;
import android.support.v7.app.ActionBarActivity;
import android.view.View;
import android.widget.EditText;
import android.widget.Toast;
import android.util.Log;

/**
 * Created by linzi on 2017-03-27.
 */


public class login extends ActionBarActivity {

    protected void onCreate(Bundle savedInstanceState)
    {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.login);
    }

    String getusername = "Bob"; //default username

    /*
     *This method will receive the new EditTect as username when Signup button is pressed.
     * After receive the new user name we will transfer this string back to chatroom class and
     * jump back to chatroom class
     */
    public void setUserName(View v){
        EditText username = (EditText)findViewById(R.id.username); // link username to by find the id

         getusername = username.getText().toString();
        Log.i("testing               :",getusername);
        Toast.makeText(this, "username: " + getusername, Toast.LENGTH_SHORT).show();
        //
        Intent intent = new Intent(this, chatroom.class);
        intent.putExtra("string_name", getusername);
        startActivity(intent);

    }
}
