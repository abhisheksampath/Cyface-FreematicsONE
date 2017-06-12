Cyface Demo Server for Freematics OBD II Dongle V4

This software is a case study server developed as part of a blog post we published on our homepage. It is capable of handling data transmitted from a Freematics OBD (On-Board-Diagnostics) Dongle. This dongle is Open Hardware and may be purchased at http://freematics.com/store/index.php?route=product/product&path=20&product_id=82.

Cyface is in no way associated with Freematics.

Prerequisites

To use this server you need a FreematicsONE version 4 OBD 2 dongle with the ESP8266 Wifi module and some server hardware. For simple tests a common PC or Mac should suffice. The server was developed and tested on a Dell XPS 13 (9350) running Ubuntu Linux 16.04.
The dongle with the GSM module might work, but has not been tested. I guess if you want to use that one you need to make the server available on the internet. Beware that the server has not been developed with safety and security in mind!


Running the Server

The server is implemented using SpringBoot and thus based on the SpringFramework. Spring is a Java software stack, so you are going to need a Java installation on your system. Since GitHub contains only the sources of the server you need the Java Development Kit (JDK) and not the smaller Java Runtime Environment (JRE). Actually the JDK contains a JRE so you do not need to download a JRE separately. On Ubuntu you may simply install the OpenJDK via: sudo apt install default-jdk or follow the nice tutorial from WebUpd8 Team to get the Oracle JDK if you don't trust OpenJDK.

Clone the server repository from GitHub using Git. On Ubuntu Linux this works from the command line with: > git clone https://github.com/gogetoss4/Cyface-server.git. You will be asked for your GitHub credentials, so register a free account.

A successful clone places all the source files in your current directory. From there build and run the server using gradle, which thanks to the gradle wrapper you do not need to install :). > ./gradlew clean build check bootRun If all works well the build process should stop with a line telling you something like Started MessboxServerApplication in 2.929 seconds. To stop the server you can just close the window. This also means you should not close the window after you entered the command! ;) The server runs as long as the window is open on TCP port 8080. To test this you can enter http://localhost:8080 in your Web browsers address bar and should see a blank white screen. If you see an error message, something went wrong. Study the output you get from your command line/terminal to see what happened.

To prepare the server to receive data you need to setup a connection for the FreematicsONE. During development I used to setup my laptop as access point for the dongle. On Ubuntu Linux 16.04 this works via the network manager, for example like so. On older versions of Ubuntu it requires some hacking in some configuration files, for example like so. It should be possible on Windows too but my attempts have been unsuccessful so far. This might just be my inability to do anything more complex than starting a game on Windows. So if you are a Windows user I am sure you'll manage.


Running the OBD II Software

To submit data to the server you need the Arduino telelogger sketch for the FreematicsONE. It is available from the Freematics GitHub repository. However do not use that version at the moment. We created a fork and made some fixes to the code, that are mandatory for the telelogger to work. There is a pull request and as soon as that one has been merged you might also use the Freematics code directly. Until then checkout the FreematicsONE sketches from here using a command like: > git clone https://github.com/gogetoss4/Cyface-Logger.git. Install an Arduino IDE! Open the file firmware_v4/datalog/datalog.ino in your IDE. This should automatically open the files config.h and datalogger.ino in separate tabs. 

To run the datalogger on your FreematicsONE connect the dongle via USB to your PC! Click the upload button in the upper left corner of your Arduino IDE! As soon as the log window in the lower are of the IDE tells you “Done compiling.”, hit Ctrl+Shift+M to see the output from your dingle. Although, you probably need to be a little patient when you run the program at the first time as the GPS in the Freematics ONE take the while to connect successfully. Once that’s done, it should successfully start logging data into the SD card.

Once the logging’s done successfully, the data would reside in a file called “datalog.csv” in the SD card. You can open it open to check if it’s blank or has data in it just to reassure. The data that is logged in the csv file is of two types. The first being a # followed by a number, the number being the VIN ( Vehicle Identification Number).   As for the second type is a series of values separated by a comma or split into columns when viewed, where each of the column is as follows

Date, time, latitude, longitude, altitude, speed, satellites, Accelerometer X, Y, Z , flag(0,1) for whether the data has been sent successfully.

Now we need to switch the code in the Freematics stick by uploading a second file while can be found in firmware_v4/wifiLogger/wifiLogger.ino. In wifi.ino, right on top you can setup the data for you Wifi hotspot: SSID, password and server address. The default server address if you are running the hotspot is 10.41.0.1. If on windows, the address can be found by going to the command prompt and executing the code – “ipconfig”. Just note down the IPV4 address of your wifi hotspot. The server port of the server is 8080.

Just a heads up if you’re using windows you need to create an inbound rule for the port: 8080 in you windows firewall since the firewall denies you permission to connect to the server by default. Similar to the datalogger you can run this sketch by clicking the upload button on the top left corner followed up the Ctrl+Shift+M once uploading is done. After this the data would be transmitted to the server one by one. At the end of the trasmission. If it were completely successful, the serial monitor would print the word "completed", which signifies the end of the file.

Since the server stores everything in memory the time will come when your memory is full and the server will crash. So do not let it run forever. There will be errors! The connection will be reset from time to time and it might take some time at the beginning until the first data is transmitted. The server might also throw some error messages if it encounters not implemented entries.

License

The Cyface logger is available under MIT License. This means:
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Outlook

Over time and as we require new functionality this server might become more stable and support additional features. Since we are a small team we are also happy about pull requests providing fixes or new features.

