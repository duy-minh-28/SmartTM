console.log(" ------------------- Start NodeJS script ------------------- ");
var mqtt = require('mqtt'); //https://www.npmjs.com/package/mqtt
var Topic = 'TrashbinCapstone/#'; //subscribe to all topics

var Broker_URL = 'mqtt://127.0.0.1'; 
var Database_URL = '127.0.0.1';

var options = {
	clientId: `TrashbinCapstone_mqtt_${Math.random().toString(16).slice(3)}`, 
	port: 1883,
	keepalive: 60
	//username: 'mqtt_user',
	//password: 'mqtt_password'
};
 
var client = mqtt.connect(Broker_URL, options);
client.on('connect', mqtt_connect);
client.on('reconnect', mqtt_reconnect);
client.on('error', mqtt_error);
client.on('message', mqtt_messsageReceived);
client.on('close', mqtt_close);

// ---------------------------------------------------------- Function ----------------------------------------------------------
function mqtt_connect() {
    console.log("Connecting MQTT");
    client.subscribe(Topic, mqtt_subscribe);
};

function mqtt_subscribe(err, granted) {
    console.log("/Subscribed to " + Topic);
    if (err) {console.log(err);}
};

function mqtt_reconnect(err) {
    console.log("Reconnect MQTT");
    console.log("clientId: "+options.clientId);
    if (err) {console.log(err);}
	client  = mqtt.connect(Broker_URL, options);
};

function mqtt_error(err) {
    console.log("MQTT Error: ");
	if (err) {console.log(err);} 
};

function after_publish() {
	//do nothing
};

var topic_SlaveMsg = "TrashbinCapstone/Slave/MSG";
var topic_CheckSlaveMsg = "TrashbinCapstone/Slave/CheckMSG";
var topic_getSlaveNumber = "TrashbinCapstone/Slave/GetSlaveNumber";

// --- receive a message from MQTT broker ---
function mqtt_messsageReceived(topic, message, packet) {
	// console.log('\n\nTopic=' +  topic + '  Message=' + message);
	console.log('\n\n=========================================================================');

	var message_str = message.toString(); //convert byte array to string
	message_str = message_str.replace(/\n$/, ''); //remove new line
  
	var topic_str = topic.toString(); //convert byte array to string
	topic_str = topic_str.replace(/\n$/, ''); //remove new line

	// Case 1: Insert End Node message to DB
	if(topic_str === topic_SlaveMsg){ 
		console.log('GW Topic: ' +  topic + '  Message:' + message);
		insert_SlaveMsg(message_str, packet);
	}
	// Case 2: Checking msg of Gateway (just for debugging)
	if(topic_str === topic_CheckSlaveMsg){ 
		insert_CheckSlaveMsg(message_str, packet);
	}

};

function mqtt_close() {
	console.log("Close MQTT"); 
	console.log("~~~~~~~~");
};
 
function publishMessagePuback(topic, message) { 
	// console.log(`Sending Topic: ${topic}, Message: ${message}`);
	// let topic = "TrashbinCapstone/Slave/MSG-PUBACK";
    // let message = "Send data to trash_record successfully!";
	client.publish(topic, message, {
	  qos: 0,
	  retain: false,
	}); 
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////// MYSQL //////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

var mysql = require('mysql'); //https://www.npmjs.com/package/mysql
//Create Connection
var connection = mysql.createConnection({
	host: Database_URL,
	user: "root",
	// password: "123",
	database: "excellent_management"
});

connection.connect(function(err) {
	if (err) throw err;
	console.log("Database Connected!");
});

function isNumeric(num){
	return !isNaN(num)
}

const fs = require('fs'); 
const { Console } = require("console");
var logFile = "MQTT_logging.txt";

// ----------- insert a row into the tbl_messages table -----------
function insert_SlaveMsg(message_str, packet) {  
  	// ============================================== MSG PACKAGE ============================================== 
	// MSG Content: "MSGID(1), binID(2), VL53L0X Value(3), Voltage Value(4), Mac Address(5), FirmWare Version(6), reserved1(7), reserved2(8)"  
 
	let message_arr = extract_string(message_str); //split a string into an array
	let publishMsgID = message_arr[0];
	// MSG Content
	let SlaveMsgID = message_arr[1];
	let binID = message_arr[2];
	let distanceSensorValue = message_arr[3];
	let batteryVoltage = message_arr[4];
	let MacAdress = message_arr[5];
	let firmWareVersion = message_arr[6];
	let sleepPeriod = message_arr[7];
	let future3 = message_arr[8];
	let pckRSSI = message_arr[9];
	let pckSNR = message_arr[10];

	console.log("[SlaveMsgID: "+SlaveMsgID +"| binID: "+binID +"| DISTANCE VALUE: "+distanceSensorValue +"| batteryVoltage: "+batteryVoltage +"| sleepPeriod: "+sleepPeriod 
				+"| RSSI: "+pckRSSI +"| SNR: "+pckSNR+"]"); 

	// let checkBinID_int = checkBinID(binID);
	// console.log("check Bin ID: "+checkBinID(binID));
	let binID_int = parseInt(binID);
	
	// if (binID_int>0 && binID_int<50) {
	// if (checkBinID(binID_int)==1) {
		let Insert_SQL = "INSERT INTO ?? (??,??,??,??,??,??,??,??,??) VALUES (?,?,?,?,?,?,?,?,?)";
		let params = ['trash_record', 'binID', 'SlaveMsgID', 'emptyDistance','batteryVoltage', 'MacAdress', 'firmWareVersion', 'sleepPeriod', 'pckRSSI', 'pckSNR',
									binID, SlaveMsgID, distanceSensorValue, batteryVoltage, MacAdress, firmWareVersion, sleepPeriod, pckRSSI, pckSNR];

		Insert_SQL = mysql.format(Insert_SQL, params);	 
		connection.query(Insert_SQL, function (error, results) {
			// if (error) throw error;
			if (error){
				console.log("SQL error! BinID or something is wrong");
			}
			
		    // console.log("Send data to trash_record successfully!");
		    let topic = "SmartTM/MSG-PUBACK/";
		    let message = "Send data to trash_record successfully!";
		    publishMessagePuback(topic, message);
		});   
		
		// --- Logging ---
		getDate(); 
		let log = message_str + "\n";
		fs.appendFile(logFile, log, (err) => { 
			if (err) { 
				console.log(err); 
			} 
		}); 
		
	// }
	// else{
	// 	console.log("Bin ID not found!");
	// } 

}	

function insert_CheckSlaveMsg(message_str, packet) {  
	// DB Struture: checkID, binID, periodID, periodDate, periodPkgCount, status
    // MSG Struture: periodID, periodPkgCount 
	// INSERT INTO `slave_check`(`periodID`, `periodPkgCount`) VALUES ('1','5');
 
	console.log("insert_CheckSlaveMsg message: " + message_str);
	let message_arr = extract_string(message_str); //split a string into an array
	let periodID = message_arr[0];
	let periodPkgCount = message_arr[1]; 

	let sql = "INSERT INTO ?? (??,??) VALUES (?,?)";
	let params = ['slave_check', 'periodID', 'periodPkgCount', periodID, periodPkgCount];
	sql = mysql.format(sql, params);	
	connection.query(sql, function (error, results) {
		if (error) throw error;
	});    
}

function checkBinID(binID) {  
	console.log("SQL: SELECT binID FROM trash_bin WHERE binID "+binID);
	let sql_checkBinID = "SELECT binID FROM trash_bin WHERE binID = "+binID;
	sql_checkBinID = mysql.format(sql_checkBinID);	 
	
	connection.query(sql_checkBinID, function (error, result) {
		if (error) throw error;
		// console.log(result);

		if (binID == result[0].binID){
			console.log("Correct bin ID");
			// return 1;
		}

		else{
			console.log("Wrong bin ID");
			// return -1;
		}
	});   
 
}

function getSlaveNumber() { 
	let sql = "SELECT COUNT(binID) FROM trash_bin;";
	sql = mysql.format(sql, params);	
 
	connection.query(sql, function (error, results) {
		if (error) throw error;
	});   
	let binIDCount = results[0].binID;
	console.log("Bin ID Count: "+binIDCount);
} 

//split a string into an array of substrings
function extract_string(message_str) {
	let message_arr = message_str.split(","); //convert to array	
	return message_arr;
};	

//count number of delimiters in a string
var delimiter = ",";
function countInstances(message_str) {
	var substrings = message_str.split(delimiter);
	return substrings.length - 1;
};

function getDate(){
	let date_ob = new Date(); 
	// current date
	// adjust 0 before single digit date
	let date = ("0" + date_ob.getDate()).slice(-2); 
	// current month
	let month = ("0" + (date_ob.getMonth() + 1)).slice(-2); 
	// current year
	let year = date_ob.getFullYear(); 
	// current hours
	let hours = date_ob.getHours(); 
	// current minutes
	let minutes = date_ob.getMinutes(); 
	// current seconds
	let seconds = date_ob.getSeconds();

	// // prints date in YYYY-MM-DD format
	// console.log(year + "-" + month + "-" + date); 
	// // prints date & time in YYYY-MM-DD HH:MM:SS format
	// console.log(year + "-" + month + "-" + date + " " + hours + ":" + minutes + ":" + seconds); 
	// // prints time in HH:MM format
	// console.log(hours + ":" + minutes);

	fs.appendFile(logFile, "===================================================", (err) => { 
		if (err) { 
			console.log(err); 
		} 
	}); 
	var msg = "Current Date: " + year + "-" + month + "-" + date + " " + hours + ":" + minutes + ":" + seconds + "\n";
	fs.appendFile(logFile, msg, (err) => { 
		if (err) { 
			console.log(err); 
		} 
	}); 
}