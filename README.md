# tactsof-server-side

tactsof-server-side is a server-side application developed in C to test TMT actuator control & test-parameters, running on Embedded systems and interacts with one or multiple GUIs via the TCP/IP protocol.

## Getting started

For preparing the executable for the program an arm-embedded cross-compiler toolchain is required. The cross-compiler version will depend on the target embedded board.

## Working

The working of the application is fairly basic and comprises of the following components:
 * initialization of the GPIO libraries present in the embedded board & connection parameters
 * main function that opens a connection and listens for input from client-GUI for a switch-case statement
 * Depending on the input the program executes a certain function
 * And returns the resulting data from DAQ to the client-GUI

## License

None. Feel free to use as is or modified at your own risk.

