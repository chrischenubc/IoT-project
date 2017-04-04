package com.led_on_off.led;

import android.net.Uri;
import android.support.v7.app.ActionBarActivity;
import android.os.Bundle;
import android.text.TextUtils;
import android.view.Menu;
import android.view.MenuItem;

import android.bluetooth.BluetoothSocket;
import android.content.Intent;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.ImageButton;
import android.widget.TextView;
import android.widget.Toast;
import android.app.ProgressDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.os.AsyncTask;
import android.util.Log;

import java.io.IOException;
import java.text.DateFormat;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.UUID;
import android.os.Handler;

import org.w3c.dom.Text;

//import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;
import android.widget.TextView;

import com.github.nkzawa.emitter.Emitter;
import com.github.nkzawa.socketio.client.IO;
import com.github.nkzawa.socketio.client.Socket;
import com.google.android.gms.appindexing.Action;
import com.google.android.gms.appindexing.AppIndex;
import com.google.android.gms.appindexing.Thing;
import com.google.android.gms.common.api.GoogleApiClient;

import org.json.JSONException;
import org.json.JSONObject;

import java.net.URISyntaxException;


public class ledControl extends ActionBarActivity {

    // GUI Attributes
    ImageButton Discnt, chatRoomButton;   //BImage buttons
    TextView Temperature, Photosensor,Humidity, ultrasonicSensor;  //define 4 different reading textviews
    Button LED_Control, AlarmButton,ServoButton, AutoModelButton, ResetButton,TureOffButton; // define 6 different buttons

    //Used for bluetooth
    String address = null;
    private ProgressDialog progress;
    BluetoothAdapter myBluetooth = null;
    BluetoothSocket btSocket = null;
    private boolean isBtConnected = false;

    //Used by the thread that checks for incoming data from inputStream of btSocket
    boolean stopThread; //indicator to stop the thread or turn it on
    byte buffer[];      //if there is available info, store it here

    //SPP UUID. Look for it. Used to identify the bluetooth object to connect to
    static final UUID myUUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

    private Socket socket;
    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    private GoogleApiClient client;

    {
        try {
            socket = IO.socket("http://38.88.74.87:9000");  // socket connects to 38.88.74.87:9000
        } catch (URISyntaxException e) {
            throw new RuntimeException(e);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        Intent newint = getIntent();
        address = newint.getStringExtra(DeviceList.EXTRA_ADDRESS); //receive the address of the bluetooth device

        //view of the ledControl
        setContentView(R.layout.activity_led_control);

        //call the widgets

        // connect each textView and Button by findViewById
        Discnt = (ImageButton) findViewById(R.id.discnt);
        chatRoomButton = (ImageButton) findViewById(R.id.chatRoomButton);
        Temperature = (TextView) findViewById(R.id.temperature);
        Humidity = (TextView) findViewById(R.id.humidity);
        ultrasonicSensor = (TextView) findViewById(R.id.ultrasonicsensor);
        Photosensor = (TextView)findViewById(R.id.photosensor);
        LED_Control = (Button) findViewById(R.id.led);
        AlarmButton = (Button) findViewById(R.id.alarmButton);
        ServoButton = (Button) findViewById(R.id.servobutton);
        AutoModelButton = (Button) findViewById(R.id.AutoModel);
        ResetButton = (Button) findViewById(R.id.Reset);
        TureOffButton = (Button) findViewById(R.id.TurnOfModel);

        new ConnectBT().execute(); //Call the class to connect

        //From here on until the method ends, all these codes attaches methods to buttons
        //That is, when a button is pressed, a method associated with the button activates


        Discnt.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Disconnect(); //close connection
            }
        });

        LED_Control.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                LEDOnOff(); //close connection
            }
        });

        AlarmButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                BuzzerAlarmOnOff(); //close connection
            }
        });
        ServoButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                turnServo2(); //close connection
            }
        });
        AutoModelButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Automodel(); //close connection
            }
        });

        ResetButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Reset(); //close connection
            }
        });

        TureOffButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                TurnOff(); //close connection
            }
        });



        socket.connect();
        socket.on("ledToAndroid", led_control); // socket listen on ledToAndroid event
        socket.on("piezoToAndroid", piezo_control); // socket listen on piezoToAndroid event
        socket.on("motorToAndroid",turnServo1);  // socket listen on motorToAndroid event
        socket.on("AutoModeAndroid",autoModel_control); //socket listen on AutoModeAndroid event
        socket.on("ResetAndroid",reset_control);  //socket listen on ResetAndroid event
        socket.on("TurnOffAndroid",turnOff_control);  //socket listen on TurnOffAndroid event
        //socket.on("TimeAndroid",time_control);
        socket.on("LCDAndroid",LCD_control);  //socket listen on LCDAndroid event


        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client = new GoogleApiClient.Builder(this).addApi(AppIndex.API).build();
    }





    private Emitter.Listener led_control = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            ledControl.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    LEDOnOff();
                }
            });
        }
    };  // led_control wiil call LEDOnOff

    private Emitter.Listener piezo_control = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            ledControl.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    BuzzerAlarmOnOff();
                }
            });
        }
    };  // piezo_control will call BuzzerAlarmOnOff
    private Emitter.Listener turnServo1 = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            ledControl.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    turnServo2();
                }
            });
        }
    };  // turnServo1 will call turnServo2 method
    private Emitter.Listener autoModel_control = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            ledControl.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Automodel();
                }
            });
        }
    };  // autoModel_control will call Autmodel method
    private Emitter.Listener reset_control = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            ledControl.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    Reset();
                }
            });
        }
    };  // reset_control will call Reset method
    private Emitter.Listener turnOff_control = new Emitter.Listener() {
        @Override
        public void call(final Object... args) {
            ledControl.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    TurnOff();
                }
            });
        }
    };  //turnOff_control will call TurnOff method


    /*
     *LCD_control will call this method directly, and this method will send data received
     * from Web Server, convert it to String and send it to LCD via Bluetooth
     *
     */
    private Emitter.Listener LCD_control = new Emitter.Listener() {
        @Override

        public void call(final Object... args) {
            ledControl.this.runOnUiThread(new Runnable() {
                @Override
                public void run() {
                    String LCD_message = (String) args[0]; // convert args[0] to string type
                    if (btSocket != null) {  // check btSocket is connected or not
                        try {
                            String data = "C" + LCD_message;  // send LCD_message to LCD via Bluetooth
                            btSocket.getOutputStream().write(data.getBytes());
                        } catch (IOException e) {
                            msg("Error from LCD_control ");
                        }
                    }

                }
            });
        }
    };

    /*
     * this method will direct send "S" to Arduino via Bluetooth
     */
    private void turnServo2() {
        if (btSocket != null) {  // check whether Bluetooth is connected or not
            try {
                btSocket.getOutputStream().write("S".toString().getBytes()); //send data to Bluetooth
            } catch (IOException e) {
                msg("Error from LEDOnOff ");
            }
        }
    }

    /*
    * this method will direct send "L" to Arduino via Bluetooth
    */
    private void LEDOnOff() {
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write("L".toString().getBytes());  // send data to Bluetooth
            } catch (IOException e) {
                msg("Error from LEDOnOff ");
            }
        }
    }
    /*
     *this method will get currentTime
     * @return string type time that format as HH:MM:SS
     */
    public static String getCurrentTime() {
        //date output format
        DateFormat dateFormat = new SimpleDateFormat("HH:mm:ss"); // get time format
        Calendar cal = Calendar.getInstance();  // receive current time
        return dateFormat.format(cal.getTime()); //return the time as format "HH:mm:ss"
    }

    /*
     * this method will direct send "P" to Arduino via Bluetooth
     */
    public void BuzzerAlarmOnOff(){
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write("P".toString().getBytes()); //send data to Bluetooth
            } catch (IOException e) {
                msg("Error from BuzzerAlarmOnOff" );
            }
        }
    }


    /*
     * this method will direct send "A" to Arduino via Bluetooth
     */
    public  void Automodel(){
        //TOBE:
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write("A".toString().getBytes()); //send data to Bluetooth
            } catch (IOException e) {
                msg("Error from Automodel method" );
            }
        }
    }

    /*
     * this method will direct send "R" to Arduino via Bluetooth
     */
    public void Reset(){
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write("R".toString().getBytes()); // send data to Bluetooth
            } catch (IOException e) {
                msg("Error from Reset" );
            }
        }
    }


    /*
     * this method will direct send "O" to Arduino via Bluetooth
     */
    public void TurnOff(){
        if (btSocket != null) {
            try {
                btSocket.getOutputStream().write("O".toString().getBytes()); // send datat to Bluetooth
            } catch (IOException e) {
                msg("Error from TurnOff method" );
            }
        }
    }

    public void Time(){
        if (btSocket != null) {
            try {
                String data = "T" + getCurrentTime();
                btSocket.getOutputStream().write(data.toString().getBytes());
            } catch (IOException e) {
                msg("Error from Time method" );
            }
        }
    }


    /**
     * If Disconnect button is pressed, close the socket and stop the thread that constantly checks for incoming Bluetooth data
     */
    private void Disconnect() {
        if (btSocket != null) //If the btSocket is busy
        {
            try {
                btSocket.close(); //close connection
                stopThread = true;  //stops the input Bluetooth thread
                socket.disconnect();
            } catch (IOException e) {
                msg("Error");
            }
        }
        finish(); //return to the first layout, that is, the DeviceList

    }

    // fast way to call Toast
    private void msg(String s) {
        Toast.makeText(getApplicationContext(), s, Toast.LENGTH_LONG).show();
    }

    /**
     * If About button is pressed, open a new intent, that is, a new page
     */
    public void about(View v) {
        if (v.getId() == R.id.chatRoomButton) { // check whether chatRoomButton is pressed
            Intent i = new Intent(this, AboutActivity.class); // switch to AboutActivity class
            startActivity(i);
        }
    }

    public void switch_to_chatroom(View v){
        Intent i = new Intent(this, chatroom.class);
        startActivity(i);
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_led_control, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }

    //
    //

    /**
     * This function has been copied and modified from
     * https://www.allaboutcircuits.com/projects/control-an-arduino-using-your-phone/
     * <p>
     * This function runs in the background to listen for incoming Bluetooth data and will change
     * the text
     */
    void beginListenForData() {
        final Handler handler = new Handler();
        stopThread = false;
        buffer = new byte[1024];
        Thread thread = new Thread(new Runnable() {
            public void run() {
                while (!Thread.currentThread().isInterrupted() && !stopThread) {
                    try {
                        int byteCount = btSocket.getInputStream().available();
                        if (byteCount > 0) {
                            byte[] rawBytes = new byte[byteCount];
                            int numBytes = btSocket.getInputStream().read(rawBytes);
                            final String string = new String(rawBytes);
                            handler.post(new Runnable() {
                                public void run() {
                                    int length = string.length();
                                    if (string.charAt(0) == '1') {
                                        Temperature.setText(string.substring(1, length));
                                        socket.emit("tempToServer", string.substring(1, length));
                                        // temperature will start with '1' in string, and this data will
                                        //send to Web Server as tempToServer event
                                    } else if(string.charAt(0) == '2') {
                                        Photosensor.setText(string.substring(1, length));
                                        socket.emit("lightToServer", string.substring(1, length));
                                        //photosensor will start with '2' in string, and this data will
                                        //send to WebServer as lightToServer event
                                    } else if (string.charAt(0) == '3') {
                                        ultrasonicSensor.setText(string.substring(1,length));
                                        //ultrasonicSensor will start with '3' in string
                                        socket.emit("distanceToServer", string.substring(1, length));
                                        //socket IO will send to Web Server as distanceToServer event
                                    } else if(string.charAt(0) == '4'){
                                        Humidity.setText(string.substring(1,length));
                                        //humidity is the last one
                                        socket.emit("humidityToServer", string.substring(1, length));
                                        //socket IO will send data to WebServer as humidityToServer event
                                    } else if(string.charAt(0) == '9') {
                                        //do nothing
                                    }

                                }
                            });

                        }
                    } catch (IOException ex) {
                        stopThread = true;
                        socket.disconnect();
                    }
                }
            }
        });

        thread.start();
    }

    /**
     * ATTENTION: This was auto-generated to implement the App Indexing API.
     * See https://g.co/AppIndexing/AndroidStudio for more information.
     */
    public Action getIndexApiAction() {
        Thing object = new Thing.Builder()
                .setName("ledControl Page") // TODO: Define a title for the content shown.
                // TODO: Make sure this auto-generated URL is correct.
                .setUrl(Uri.parse("http://[ENTER-YOUR-URL-HERE]"))
                .build();
        return new Action.Builder(Action.TYPE_VIEW)
                .setObject(object)
                .setActionStatus(Action.STATUS_TYPE_COMPLETED)
                .build();
    }

    @Override
    public void onStart() {
        super.onStart();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        client.connect();
        AppIndex.AppIndexApi.start(client, getIndexApiAction());
    }

    @Override
    public void onStop() {
        super.onStop();

        // ATTENTION: This was auto-generated to implement the App Indexing API.
        // See https://g.co/AppIndexing/AndroidStudio for more information.
        AppIndex.AppIndexApi.end(client, getIndexApiAction());
        client.disconnect();
    }


    public class ConnectBT extends AsyncTask<Void, Void, Void>  // UI thread
    {
        private boolean ConnectSuccess = true; //if it's here, it's almost connected

        @Override
        protected void onPreExecute() {
            progress = ProgressDialog.show(ledControl.this, "Connecting...", "Please wait!!!");  //show a progress dialog
        }

        @Override
        protected Void doInBackground(Void... devices) //while the progress dialog is shown, the connection is done in background
        {
            try {
                if (btSocket == null || !isBtConnected) {
                    myBluetooth = BluetoothAdapter.getDefaultAdapter();//get the mobile bluetooth device
                    BluetoothDevice dispositivo = myBluetooth.getRemoteDevice(address);//connects to the device's address and checks if it's available
                    btSocket = dispositivo.createInsecureRfcommSocketToServiceRecord(myUUID);//create a RFCOMM (SPP) connection
                    BluetoothAdapter.getDefaultAdapter().cancelDiscovery();
                    btSocket.connect();//start connection
                }
            } catch (IOException e) {
                ConnectSuccess = false;//if the try failed, you can check the exception here
            }
            return null;
        }

        @Override
        protected void onPostExecute(Void result) //after the doInBackground, it checks if everything went fine
        {
            super.onPostExecute(result);

            if (!ConnectSuccess) {
                msg("Connection Failed. Is it a SPP Bluetooth? Try again.");
                finish();
            } else {
                msg("Connected.");
                isBtConnected = true;
                beginListenForData();
            }
            progress.dismiss();
        }
    }
}
