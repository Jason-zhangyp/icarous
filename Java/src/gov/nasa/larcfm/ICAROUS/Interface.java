/**
 * Interface
 * 
 * This is a class that will enable establishing a socket or
 * a serial interface between ICAROUS and any component that can
 * stream MAVLink messages
 *
 * Contact: Swee Balachandran (swee.balachandran@nianet.org)
 * 
 * 
 * Copyright (c) 2011-2016 United States Government as represented by
 * the National Aeronautics and Space Administration.  No copyright
 * is claimed in the United States under Title 17, U.S.Code. All Other
 * Rights Reserved.
 */
package gov.nasa.larcfm.ICAROUS;

import java.io.*;
import java.net.*;
import com.MAVLink.Parser;
import com.MAVLink.Messages.*;
import com.MAVLink.common.*;
import com.MAVLink.MAVLinkPacket;
import jssc.SerialPort;
import jssc.SerialPortException;
import jssc.SerialPortTimeoutException;

public class Interface{

    public static short PX4       = 1;
    public static short COM       = 2;
    public static short SAFEGUARD = 3;
    public static short BROADCAST = 4;

    public static short SOCKET    = 0;
    public static short SERIAL    = 1;
    
    public short interfaceType    = 0;
    private int udpReceivePort    = 0;
    private int udpSendPort       = 0;
    private String udpHost        = null;
    private String serialPortName = null;
    private InetAddress host      = null;
    private DatagramSocket sock   = null;
    private SerialPort serialPort = null;
    private Parser MsgParser      = new Parser();
    private MAVLinkMessages Inbox;
    private int Timeout;
    
    public Interface(int intType,String hostname,int recvPort,int sendPort,AircraftData acData){

	interfaceType   = (short) intType;
	udpReceivePort  = recvPort;
	udpSendPort     = sendPort;

	Inbox = null;
	if(acData != null){
	    Inbox           = acData.Inbox;
	}

	if(hostname != null){
	    try{
		host           = InetAddress.getByName(hostname);
	    }catch(UnknownHostException e){
		System.out.println(e);
	    }
	}
	
	InitSocketInterface();
	SetTimeout(100);
    }

    public Interface(int intType,String portName,int BAUDRATE,AircraftData acData){

	interfaceType  = (short) intType;
	serialPortName = portName;
	
	Inbox = null;
	if(acData != null){
	    Inbox           = acData.Inbox;
	}

	InitSerialInterface(BAUDRATE);
	SetTimeout(500);
    }

    public void InitSocketInterface(){
	
	try{
	    if(udpReceivePort != 0){
		sock  = new DatagramSocket(udpReceivePort);
	    }
	    else{
		sock  = new DatagramSocket();
	    }
	}
	catch(IOException e){
	    System.err.println(e);
	}
			
    }

    public void SetTimeout(int timeout){

	if(interfaceType == SOCKET){
	    try{
		sock.setSoTimeout(timeout);
	    }
	    catch(SocketException e){
		System.out.println(e);
	    }
	}
	else{
	    Timeout = timeout;
	}
    }
    
    public void InitSerialInterface(int BAUDRATE){

	serialPort = new SerialPort(serialPortName);

	try {
	    serialPort.openPort();
	    serialPort.setParams(BAUDRATE, 
				 SerialPort.DATABITS_8,
				 SerialPort.STOPBITS_1,
				 SerialPort.PARITY_NONE);
	
	}
	catch(SerialPortException ex){
	    System.out.println(ex);
	}

	System.out.println("Connected to serial port:"+serialPortName);

    }
	
    public synchronized byte[] UDPRead(){
	byte[] buffer = new byte[6+255+2];
	byte[] buffer_data = null;	    
	DatagramPacket input = new DatagramPacket(buffer, buffer.length);
	try{
	    sock.receive(input);
	    // Get port number on host to send data
	    if(udpSendPort == 0){
		udpSendPort = input.getPort();		
	    }
	    if(host == null){
		host = input.getAddress();
	    }	    
	    buffer_data   = input.getData();
	    	    	    	    	    
	}catch(SocketTimeoutException e){
	    
	}catch(IOException e){
	    System.out.println(e);
	}	
	return buffer_data;
    }

    public synchronized void UDPWrite(byte[] buffer){	
	try{
	    DatagramPacket  output = new DatagramPacket(buffer , buffer.length , host , udpSendPort);
	    sock.send(output);
	}
	catch(IOException e){
	    System.err.println(e);
	}

    }   

    public synchronized byte[] SerialRead(){
	byte[] buffer = null;		
	try{
	    buffer = serialPort.readBytes();
	}
	catch(SerialPortException e){
	    System.out.println(e);
	}
	//catch(SerialPortTimeoutException e){
	//  System.out.println(e);
	//}
	return buffer;
    }

    public synchronized void SerialWrite(byte[] buffer){
	try{
	    serialPort.writeBytes(buffer);
	}
	catch(SerialPortException e){
	    System.err.println(e);
	}	
    }
	
    public synchronized void Read(){

	byte[] buffer = null;
	if(interfaceType == SOCKET){
	    buffer = UDPRead();
	}
	else if(interfaceType == SERIAL){
	    buffer = SerialRead();
	}
	else{
	    System.out.println("Unknown interface");	    
	}

	MAVLinkPacket RcvdPacket = null;
	if(buffer != null){
	    for(int i=0;i<buffer.length;++i){
		RcvdPacket = MsgParser.mavlink_parse_char((int)0xFF & buffer[i]);
		Inbox.decode_message(RcvdPacket);	
	    }
	}	
    }
    
    public synchronized void Write(MAVLinkMessage msg2send){

	if(msg2send != null){
	    MAVLinkPacket raw_packet = msg2send.pack();
	    raw_packet.sysid  = msg2send.sysid;
	    raw_packet.compid = msg2send.compid;	    
	    byte[] buffer            = raw_packet.encodePacket();	    	    
	    if(interfaceType == SOCKET){
		this.UDPWrite(buffer);
	    }
	    else if (interfaceType == SERIAL){
		this.SerialWrite(buffer);
	    }
	}
	
    }

    public byte[] ReadBytes(){
	byte[] buffer = null;
	if(interfaceType == SOCKET){
	    buffer = UDPRead();
	}
	else if(interfaceType == SERIAL){
	    buffer = SerialRead();
	}
	else{
	    System.out.println("Unknown interface");	    
	}

	return buffer;
    }

    public void WriteBytes(byte[] buffer){

	if(buffer != null){
	    if(interfaceType == SOCKET){
		this.UDPWrite(buffer);
	    }
	    else if (interfaceType == SERIAL){
		this.SerialWrite(buffer);
	    }
	}
	
    }

    public static void PassThrough(Interface AP, Interface GS){
	byte[] AP_buffer = null;
	byte[] GS_buffer = null;

	AP.SetTimeout(1);
	GS.SetTimeout(1);
				

	// Read from AP
	AP_buffer = AP.ReadBytes();
	    
	// Write to GS
	GS.WriteBytes(AP_buffer);




	
	// Read from GS	
	GS_buffer = GS.ReadBytes();
	    	
	// Write to AP
	AP.WriteBytes(GS_buffer);
	
    }
        
}
