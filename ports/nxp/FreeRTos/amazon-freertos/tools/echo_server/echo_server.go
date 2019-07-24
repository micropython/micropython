/*
 * Amazon FreeRTOS Echo Server V1.1.1
 * Copyright (C) 2018 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * http://aws.amazon.com/freertos
 * http://www.FreeRTOS.org
 */

package main

import (
	"io"
	"log"
	"net"
	"time"
)

//************** CONFIG SECTION *************//
//Folder were certs are put
const sUnSecureEchoPort string = "9001"
const xReadTimeoutSeconds = 300

func main() {
	// listen on all interfaces
	xUnsecureEchoServer, xStatus := net.Listen("tcp", ":"+sUnSecureEchoPort)
	if xStatus != nil {
		log.Printf("Error %s while trying to listen", xStatus)
	}
	log.Print("UnSecure server Listening to port " + sUnSecureEchoPort)

	startEchoServer(xUnsecureEchoServer)
}

func startEchoServer(xEchoServer net.Listener) {
	defer xEchoServer.Close()

	for {

		xConnection, xStatus := xEchoServer.Accept()
		if xStatus != nil {
			log.Printf("Error %s while trying to connect", xStatus)
		} else {
			go echoServerThread(xConnection)
		}
	}
}

func echoServerThread(xConnection net.Conn) {
	defer xConnection.Close()

	xDataBuffer := make([]byte, 4096)

	for {
		xConnection.SetReadDeadline(time.Now().Add(xReadTimeoutSeconds * time.Second))
		xNbBytes, xStatus := xConnection.Read(xDataBuffer)
		if xStatus != nil {
			//EOF mean end of connection
			if xStatus != io.EOF {
				//If not EOF then it is an error
			}
			break
		}

		xNbBytes, xStatus = xConnection.Write(xDataBuffer[:xNbBytes])
		if xStatus != nil {
			log.Printf("Error %s while sending data", xStatus)
			break
		}
	}
}
