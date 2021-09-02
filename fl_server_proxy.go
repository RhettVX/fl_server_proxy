package main

import (
	"log"
	"net"
)


var returnChannel = make(chan int)


func RecieveFromClient(clientConnection, serverConnection *net.UDPConn) {
	buffer := make([]byte, 1024)
	packetCount := 0
	for {
		dataLengthFromClient, clientAddr, err := clientConnection.ReadFromUDP(buffer)

		HandlePacket(buffer[:], dataLengthFromClient, clientAddr, packetCount, true)
		packetCount += 1

		dataLengthToServer, err := serverConnection.Write(buffer[:dataLengthFromClient])
		if err != nil {
			log.Fatalln("[X] Unable to forward message to server:", err.Error())
		}
		log.Printf("[*] Forwarded %d bytes to server\n", dataLengthToServer)
	}
	returnChannel <- 1337
}

func RecieveFromServer(clientConnection, serverConnection *net.UDPConn) {
	buffer := make([]byte, 1024)
	packetCount := 0
	for {
		dataLengthFromServer, serverAddr, err := serverConnection.ReadFromUDP(buffer)

		HandlePacket(buffer[:], dataLengthFromServer, serverAddr, packetCount, false)
		packetCount += 1

		dataLengthToClient, err := clientConnection.Write(buffer[:dataLengthFromServer])
		if err != nil {
			log.Fatalln("[X] Unable to forward message to client:", err.Error())
		}
		log.Printf("[*] Forwarded %d bytes to client\n", dataLengthToClient)
	}
	returnChannel <- 1337
}


/*
game[*] -> proxy[20042] -> proxy[*] -> server[20042]
game[*] <- proxy[20042] <- proxy[*] <- server[20042] 
*/
func main() {
	proxyIP  := net.UDPAddr{IP: []byte{127,0,0,1},     Port: 20042, Zone: ""}
	serverIP := net.UDPAddr{IP: []byte{64,37,174,149}, Port: 20042, Zone: ""}

	InitPackets()


	// NOTE(rhett): Listen for initial connection
	initialListener, err := net.ListenUDP("udp", &proxyIP)
	if err != nil {
		log.Fatalln("[X] Unable to listen:", err.Error())
	}
	defer initialListener.Close()
	log.Printf("[*] UDP proxy listening on %s\n", proxyIP.String())


	// NOTE(rhett): Dial server
	serverConnection, err := net.DialUDP("udp", nil, &serverIP)
	if err != nil {
		log.Fatalln("[X] Unable to dial server:", err.Error())
	}
	defer serverConnection.Close()
	log.Printf("[*] UDP dialed in to server (%s)\n", serverIP.String())


	// TODO(rhett): Listen for a message from the game client so we can maintain a connection with the client's address. This may not be the best way to handle this
	buffer := make([]byte, 1024)
	_, clientIP, err := initialListener.ReadFromUDP(buffer)
	if err != nil {
		log.Fatalln("[X] Failed to read:", err.Error())
	}
	log.Printf("[*] Recieved initial connection from %s. Closing listener and dialing client.\n", clientIP)
	initialListener.Close()


	// NOTE(rhett): Dial client
	clientConnection, err := net.DialUDP("udp", &proxyIP, clientIP)
	if err != nil {
		log.Fatalln("[X] Unable to dial client:", err.Error())
	}
	defer clientConnection.Close()
	log.Printf("[*] UDP dialed in to client (%s)\n", clientIP)


	go RecieveFromClient(clientConnection, serverConnection)
	go RecieveFromServer(clientConnection, serverConnection)

	returnValue := <- returnChannel
	log.Printf("[!] Returned %d\n", returnValue)
}
