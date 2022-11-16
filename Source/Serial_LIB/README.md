# Serial_LIB

Serial_LIB is a source library for communication with serial devices using the rs232 library. Contained within this repository
is SerialComm_LIB, the base library for serial communication, upon which device specific libraries can be developed. The device specific
libraries contained include the following:

* Ametek_LIB
* SMCActuators_LIB
* ZebraPrinter_LIB

# Definitions

  
#### Usage

This library is configured from an XML file that outlines the parameters for each communication device. This includes the following: 

* Device Name
    * The name for the device being configured
* Comport
    * The communication port the device is connected to
* BaudRate
	* The baud rate to configure the device with
* Parity
    * The parity to send with data transmition, options are:
		* None
		* Odd
		* Even
		* Mark
		* Space
* DataBits
    * The number of bits for messages
* StopBits
	* The ending bit to send at the end of every charachter
* CTSMode
	* Sets the hardware handshaking mode, options are:
		* On
		* Off
	* When you enable hardware handshaking, the CTS line is monitored to determine when it is safe to send characters to the receiver. Also, the RTS line is raised only when there is space in the input queue to receive additional characters. Optionally, you can tie the DTR line to the RTS line.
* XonXoff
	* Enables or disables software handshaking by enabling or disabling XON/XOFF sensitivity on transmission and reception of data, options are:
		* On
		* Off
* Timeout
	* The timeout limit for input/output operations.

Sample xml entry:
```xml
<Serial>
<DeviceName>Printer</DeviceName>
<Comport>COM1</Comport>
<BaudRate>9600</BaudRate>
<Parity>None</Parity>
<DataBits>8</DataBits>
<StopBits>1</StopBits>
<CTSMode>On</CTSMode>
<XonXoff>Off</XonXoff>
<Timeout>1</Timeout>
</Serial>
```

The SerialComm_LIB library can be used directly as a DLL or as a library in another project, however is meant to be wrapped with a higher level library
for device specific use.

There is a common workspace called Serial_LIB.cws which can be used to batch build the base library and any high level libraries included in Serial_LIB.

### Installation



### Resources

The following are resources for the library

| Resource | Link |
| ------ | ------ |
| RS-232 | [RS-232 Library Overview][PlXn] |\

### Contributors

Serial_LIB is currently managed by the following people

| Name | Email |
| ------ | ------ |
| Biye Chen | biye_chen@arxtron.com |
| Chao Zhang | chao_zhang@arxtron.com |
| Liang Chi | liang_chi@arxtron.com |

### Todos

 - 
 
### Untested Functions

The following are functions added to the library for additional functionality, however are not tested.

 - 

**Arxtron Technologies**

   [PlXn]: <https://zone.ni.com/reference/en-XX/help/370051AG-01/cvi/libref/cvirs232_library/>
